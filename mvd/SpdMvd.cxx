// $Id$
// Author: artur   2021/11/12

//_____________________________________________________________________________
//
// SpdMvd
//_____________________________________________________________________________

#include "SpdMvd.h"

#include "TRefArray.h"    
#include "TClonesArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoArb8.h"

#include "TMath.h"

#include "SpdMvdPoint.h"
#include "SpdMvdGeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdMvd)

//_____________________________________________________________________________________
SpdMvd::SpdMvd(): 

  SpdDetector("Micromegas VD", kSpdIts, kTRUE),
  fTrackID(-1),
  fVolumeID(-1),fSubVolID(0),fLayerID(0),
  fPos(),fMom(),
  fPosOut(),fMomOut(),
  fTime(-1.),fTimeOut(-1.),
  fLength(-1.),
  fSegmentLength(-1.),
  fELoss(-1.),
  fPointCollection(0),
  fModule(0)
{
  SetParametersType("MvdParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdMvdPoint"; 
}

//_____________________________________________________________________________________
SpdMvd::SpdMvd(const char* name, Bool_t active):

  SpdDetector(name, kSpdIts, active),
  fTrackID(-1),
  fVolumeID(-1),fSubVolID(0),fLayerID(0),
  fPos(),fMom(),
  fPosOut(),fMomOut(),
  fTime(-1.),fTimeOut(-1.),
  fLength(-1.),
  fSegmentLength(-1.),
  fELoss(-1.),
  fPointCollection(0),
  fModule(0)
{
  SetParametersType("MvdParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdMvdPoint";
}

//_____________________________________________________________________________________
SpdMvd::~SpdMvd()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdMvd::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdMvd::Initialize **********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();  

}

//_____________________________________________________________________________________
void SpdMvd::Reset()
{
  fPointCollection->Delete();
  //fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdMvd::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdMvd", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdMvd::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdMvd::ProcessHits> " << gMC->CurrentVolPath() << endl;
  
  //Set parameters at entrance of volume. Reset ELoss.
  if (gMC->IsTrackEntering()) {
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.e9;  // s -> ns
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create SpdMvdPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fSegmentLength = gMC->TrackLength() - fLength;
    fTimeOut   = gMC->TrackTime() * 1.e9;  // ns -> s
    
    fVolumeID   = kSpdIts;

    fVolumePath = gMC->CurrentVolPath();
    
    fSubVolID = -1; 
    fLayerID  = -1;
    
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
        
    //cout << "path = " << fVolumePath << endl;
    
    SpdGeopathParser parser;
    parser.ParsePath(fVolumePath);
    //parser.Print("");
    
    if (parser.Name(2).Contains("Barrel")) fSubVolID = 0;
    fLayerID = parser.VNum(3);
    
    //cout << "SubVolID = " << fSubVolID << " LayerID = " << fLayerID << " SublayerID = " << parser.VNum(4) << endl;
   
    AddHit();
    
    //cout << "<SpdMvd::AddHit> " << fPointCollection->GetEntriesFast() << endl;
    
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdIts);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdMvd::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdMvdPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  new(clref[size]) SpdMvdPoint(fTrackID,fVolumeID,fSubVolID,fLayerID,
                               TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                               TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                               TVector3(fPosOut.X(),fPosOut.Y(),fPosOut.Z()),
                               TVector3(fMomOut.Px(),fMomOut.Py(),fMomOut.Pz()),
                               fTime, fTimeOut,
                               fLength, fSegmentLength, fELoss);
}

//_____________________________________________________________________________________
void SpdMvd::EndOfEvent()
{
   Reset();
}

//_____________________________________________________________________________________
void SpdMvd::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdMvd::ConstructGeometry()
{   
  // Create detector geometry
    
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
  if (!fMasterVolume) {  
      cout << "-E- <SpdMvd::ConstructGeometry> No MASTER volume " << endl;
      return;
  }
  
  if (!GetMapper()) return;
  if (!fGeoMapper->InitGeometry()) return;
  
  //fGeoMapper->Print("");
  
  Int_t geo_type = fGeoMapper->GetGeoType();

  ConstructBarrel();
  //ConstructEndcaps();
  
  fGeoMapper->LockGeometry();
      
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdMvd::ConstructGeometry ******************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//_____________________________________________________________________________________
void SpdMvd::ConstructBarrel()
{  
  cout << "-I- <SpdMvd::ConstructBarrel> " << endl;
  
  SpdParameter* par;
  
  SpdMvdGeoMapper* mapper = dynamic_cast<SpdMvdGeoMapper*>(fGeoMapper);
  if (!mapper) return;
  
  TString mat = mapper->GetBaseMaterial();
  TGeoMedium* material = FindMedium(mat,"air");
  
  if (!material) {
      cout << "-E- <SpdMvd::ConstructBarrel> Unknown material: " << mat << endl;
      return;
  }
  
  Double_t mLength = mapper->GetMaxLength();
  Double_t mRmax   = mapper->GetMaxRadius();
  Double_t mRmin   = mapper->GetMinRadius();
  
  fModule = gGeoManager->MakeTube(mapper->AddPrefix("Barrel"),material,mRmin,mRmax,0.5*mLength);
  
  if (!fModule) {
      cout << "-E- <SpdMvd::ConstructBarrel> Module cannot be created " << endl;
      return;
  }
    
  fModule->SetFillColor(kOrange+8);
  fModule->SetLineColor(kOrange+8);
  fModule->SetTransparency(30);  
  
  //----------------------------------------
  // Build layers
  
  Int_t nl = mapper->GetNLayers();
  
  if (nl < 1) {
      cout << "-F- <SpdMvd::ConstructBarrel> No layers inside the barrel " << endl;
      exit(1);
      //return;
  }
  
  TString active_mat = mapper->GetActiveMaterial();
    
  Int_t nsl; 
  
  Double_t L, Rmin, Rmax, rmin, rmax, w;
  Bool_t is_active;
  
  TGeoMedium* medium;
  TGeoVolume *vol, *svol; 
  
  for (Int_t i(1); i<=nl; i++) 
  {
       L    = mapper->GetLayerLength(i);
       Rmin = mapper->GetLayerRmin(i);
       Rmax = mapper->GetLayerRmax(i);
       
       is_active = mapper->IsLayerActive(i);
       
       mat = mapper->GetLayerBaseMaterial(i);
       medium = MediumStrictly(mat);  //ATTENTION
       
       vol = gGeoManager->MakeTube(Form("MvdLayer%d",i),medium,Rmin,Rmax,0.5*L);
   
       nsl  = mapper->GetNSublayers(i);
      
       for (Int_t j(1); j <= nsl; j++) {
            
            mat = mapper->GetSublayerMaterial(i,j); 
            medium = MediumStrictly(mat);  //ATTENTION
            
            rmin = mapper->GetSublayerRmin(i,j);
            rmax = rmin + mapper->GetSublayerWidth(i,j);
            
            svol = gGeoManager->MakeTube(Form("MvdLayer%dSublayer%d",i,j),medium,rmin,rmax,0.5*L);
            
            svol->SetFillColor(kYellow+1);
            svol->SetLineColor(kYellow+1);
            svol->SetTransparency(50);  
            
            if (is_active && mat == active_mat) AddSensitiveVolume(svol); //ATTENTION 
            
            vol->AddNode(svol,1);
       }
       
       vol->SetFillColor(kOrange+7);
       vol->SetLineColor(kOrange+7);
       vol->SetTransparency(30);  
       
       fModule->AddNode(vol,1);
  }
  
  fMasterVolume->AddNode(fModule,1);
}

//_____________________________________________________________________________________
void SpdMvd::ConstructEndcaps()
{  
  cout << "-I- <SpdMvd::ConstructEndcap> " << endl;
    
  SpdParameter* par;
  
  SpdMvdGeoMapper* mapper = dynamic_cast<SpdMvdGeoMapper*>(fGeoMapper);
  if (!mapper) return;
  
  TString mat = mapper->GetBaseMaterial();
  TGeoMedium* material = FindMedium(mat,"air");
  
  if (!material) {
      cout << "-E- <SpdMvd::ConstructEndcaps> Unknown material: " << mat << endl;
      return;
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdMvd::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper("SpdMvdGeoMapper");
  
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdMvdGeoMapper");

  return fGeoMapper; 
}

//_____________________________________________________________________________
Bool_t SpdMvd::LoadParsFrom(SpdParSet* params) 
{
  if (!params) return kFALSE;
 
  if (!SpdDetector::LoadParsFrom(params)) return kFALSE;
  
  TString mapper;
  params->GetParameter("Mapper",mapper); 
  fGeoMapper = SpdGeoFactory::Instance()->Mapper(mapper);  
  if (fGeoMapper) fGeoMapper->LoadParametersFrom(params);
   
  fOutDataPointObject = "unknown";

  if (fNDataOut < 1) return kTRUE;

  params->GetParameter("Detector/NOutData_1",fOutDataPointObject);
  
  return kTRUE;
}

//_____________________________________________________________________________________
TString SpdMvd::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}

//_____________________________________________________________________________________
TClonesArray* SpdMvd::GetCollection(Int_t iColl) const 
{ 
  return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdMvd::GetCapacity() const 
{ 
  return (fModule) ? fModule->GetShape()->Capacity() : 0;
}
   
//_____________________________________________________________________________________
Double_t SpdMvd::GetMass() const 
{ 
  return (fModule) ? GetCapacity()*GetDensity() : 0;
}

//_____________________________________________________________________________________
Double_t SpdMvd::GetDensity() const 
{ 
  return (fModule) ? fModule->GetMaterial()->GetDensity() : 0;
}

//_____________________________________________________________________________________
TString SpdMvd::GetMaterialName() const // private
{ 
  return (fModule) ? fModule->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdMvd::Print(Option_t*) const
{
   SpdDetector::Print("");
   
   if (!fGeoMapper) return;
   
   TString divider('-',150);
   
   cout << "\n";

   fGeoMapper->Print("");

   cout << "\n";
   cout <<"\tCapacity (total):    " << GetCapacity()*1e-6 << " [m^3] " << endl;
   cout <<"\tMass (total):        " << GetMass()*1e-3 << " [kg] " << endl;
   cout <<"\tDensity (averaged):  " << GetDensity() << " [g/cm^3] " << endl;
   cout << "\n";
   
   cout << "\n" << divider.Data() << "\n";
   
   printf("%6s %4s %15s %14s %14s %14s \n\n",
          "Type","N","Material","Dens [g/cm^3]","Volume [m^3]","Mass [kg]"
         );
   
   if (!fModule) {
       cout << "\n";
       return;
   }
   
   printf("%6d %4d %15s %14.6f %14.6f %14.6f \n",
           1, 1, GetMaterialName().Data(),
           GetDensity(), GetCapacity()*1e-6, GetMass()*1e-3);
   
   cout << divider.Data() << "\n";
   
   printf("%6s %35s %14.3e %14.3e \n","TOTAL:","",GetCapacity()*1e-6,GetMass()*1e-3);
    
   cout << divider.Data() << "\n";
   
   cout << "\n";
}

//_____________________________________________________________________________
void SpdMvd::PrintGeometry(Option_t*) 
{
   SpdMvdGeoMapper* mapper = dynamic_cast<SpdMvdGeoMapper*>(fGeoMapper);
   if (!mapper) return;
   
   cout << "-I- <SpdMvd::PrintGeometry> " << endl;
   
   Int_t nl = mapper->GetNLayers();
   if (nl < 1) return;
   
   Int_t nsl; 
   
   printf("\n\t------------------------------------------------------------------------------------------------\n");
   printf("\t%5s %3s %10s %3s  %10s %10s %10s %15s %10s %10s \n","L","A",
          "Len, cm","l","R_min, cm","R_max, cm","Width, cm","Material","phi, deg","Z0, cm");

   Double_t Rmin, Rmax, W, rmin, rmax, w;
   TString mat, ss;
   TGeoMedium* medium;
   Double_t xdivx0, xdivx0_sum;
   
   for (Int_t i(1); i<=nl; i++) 
   {
        printf("\t------------------------------------------------------------------------------------------------\n");
        nsl  = mapper->GetNSublayers(i);
        Rmin = mapper->GetLayerRmin(i);
        Rmax = mapper->GetLayerRmax(i);
        W = Rmax-Rmin;
        printf("\t%5d %3d %10.3f %3d: %10.5f %10.5f %10.5f %15s %10.3f %10.3f\n",i,
               mapper->IsLayerActive(i),mapper->GetLayerLength(i),nsl,Rmin,Rmax,W,
               mapper->GetLayerBaseMaterial(i).Data(),mapper->GetLayerPhi(i),mapper->GetLayerZpos(i));
        
        printf("\t%20s----------------------------------------------------------------------------\n","");
        printf("\n\t%74s %12s %12s %12s %12s %12s\n\n","","D[g/cm^3]","X0[g/cm^2]","X0[cm]","X/X0","Sum(X/X0)"); 
        
        xdivx0_sum = 0;
        for (Int_t j(1); j <= nsl; j++) {
             
             mat = mapper->GetSublayerMaterial(i,j); 
             medium = MediumStrictly(mat);  //ATTENTION
             
             w = mapper->GetSublayerWidth(i,j);
             
             xdivx0 = XdivX0(medium,w);
             xdivx0_sum += xdivx0;
             
             ss = Form("%12.4f %12.4f %12.4f %12.7f %12.7f",
                       Density(medium),X0(medium),X0cm(medium),xdivx0,xdivx0_sum);
             
             rmin = mapper->GetSublayerRmin(i,j);
             
             rmax = rmin + w;
             printf("\t%20s %3d  %10.5f %10.5f %10.5f %15s %s\n","",j,
                    rmin,rmax,w,mat.Data(),ss.Data());    
        }
        printf("\n");
   }
   
   cout << endl;
   
}

//_____________________________________________________________________________
TGeoMedium* SpdMvd::MediumStrictly(TString mat) 
{ 
   TGeoMedium* medium = FindMedium(mat,"air");
   if (!medium) {
       cout << "-F- <SpdMvd::MaterialStrictly> Unknown material: " << mat << endl;
       exit(1);
   }
   return medium;
}

//_____________________________________________________________________________
Double_t SpdMvd::Density(TGeoMedium* medium) 
{ 
   if (!medium) return 0.; 
   TGeoMaterial* material = medium->GetMaterial();
   if (!material) return 0;
   return  material->GetDensity();
}

//_____________________________________________________________________________
Double_t SpdMvd::X0(TGeoMedium* medium)  // g/cm^3
{
   if (!medium) return 0.; 
   TGeoMaterial* material = medium->GetMaterial();
   if (!material) return 0;
   return material->GetDensity()*material->GetRadLen();
}

//_____________________________________________________________________________
Double_t SpdMvd::X0cm(TGeoMedium* medium)  // cm
{
   if (!medium) return 0.; 
   TGeoMaterial* material = medium->GetMaterial();
   if (!material) return 0; 
   return material->GetRadLen();
}

//_____________________________________________________________________________
Double_t SpdMvd::XdivX0(TGeoMedium* medium, Double_t x) // X/X0
{
   if (!medium) return 0.; 
   TGeoMaterial* material = medium->GetMaterial();
   if (!material) return 0;
   return x/material->GetRadLen();
}


