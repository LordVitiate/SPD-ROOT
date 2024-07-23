// $Id$
// Author: artur   2021/09/21

//_____________________________________________________________________________
//
// SpdZdc
//_____________________________________________________________________________

#include "SpdZdc.h"

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

#include "SpdZdcPoint.h"
#include "SpdZdcGeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdZdc)

//_____________________________________________________________________________________
SpdZdc::SpdZdc(): 

  SpdDetector("Zdc (qsl)", kSpdZdc, kTRUE),
  fTrackID(-1),
  fVolumeID(-1),
  fModuleID(0),
  fPos(),fMom(),
  fPosOut(),fMomOut(),
  fTime(-1.),fTimeOut(-1.),
  fLength(-1.),
  fSegmentLength(-1.),
  fELoss(-1.),
  fPointCollection(0),
  fModule1(0),
  fModule2(0)
{
  SetParametersType("ZdcParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdZdcPoint"; 
  
  fSaveEmptyHits = true; //FIXME!FIXME!FIXME!FIXME!FIXME
}

//_____________________________________________________________________________________
SpdZdc::SpdZdc(const char* name, Bool_t active):

  SpdDetector(name, kSpdZdc, active),
  fTrackID(-1),
  fVolumeID(-1),
  fModuleID(0),
  fPos(),fMom(),
  fPosOut(),fMomOut(),
  fTime(-1.),fTimeOut(-1.),
  fLength(-1.),
  fSegmentLength(-1.),
  fELoss(-1.),
  fPointCollection(0),
  fModule1(0),
  fModule2(0)
{
  SetParametersType("ZdcParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdZdcPoint";
  
  fSaveEmptyHits = true; //FIXME!FIXME!FIXME!FIXME!FIXME
}

//_____________________________________________________________________________________
SpdZdc::~SpdZdc()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdZdc::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdZdc::Initialize **********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();  

}

//_____________________________________________________________________________________
void SpdZdc::Reset()
{
  fPointCollection->Delete();
  //fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdZdc::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdZdc", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdZdc::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdZdc::ProcessHits> " << gMC->CurrentVolPath() << endl;
  
  //Set parameters at entrance of volume. Reset ELoss.
  if (gMC->IsTrackEntering()) {
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.e9;  // ns -> s
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create SpdZdcPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    //cout << "path = " << gMC->CurrentVolPath() 
    //     << " eloss = " << fELoss*1e9  
    //     << " save empty: " << fSaveEmptyHits 
    //     << endl;
      
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fSegmentLength = gMC->TrackLength() - fLength;
    fTimeOut   = gMC->TrackTime() * 1.e9;  // s -> ns
    
    fVolumeID   = kSpdZdc;
    fVolumePath = gMC->CurrentVolPath();
    
         if (fVolumePath.Contains("zplus"))  fModuleID =  1;
    else if (fVolumePath.Contains("zminus")) fModuleID = -1;
    else fModuleID = -10; //???
    
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
        
    //cout << "path = " << fVolumePath << endl;
   
    AddHit();
    
    //cout << "<SpdZdc::AddHit> " << fPointCollection->GetEntriesFast() << endl;
    
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdZdc);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdZdc::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdZdcPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  new(clref[size]) SpdZdcPoint(fTrackID,fVolumeID,fModuleID,
                               TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                               TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                               TVector3(fPosOut.X(),fPosOut.Y(),fPosOut.Z()),
                               TVector3(fMomOut.Px(),fMomOut.Py(),fMomOut.Pz()),
                               fTime, fTimeOut,
                               fLength, fSegmentLength, fELoss);
}

//_____________________________________________________________________________________
void SpdZdc::EndOfEvent()
{
   Reset();
}

//_____________________________________________________________________________________
void SpdZdc::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdZdc::ConstructGeometry()
{   
  // Create detector geometry
  
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
  if (!fMasterVolume) {  
      cout << "-E- <SpdTsTEC::ConstructGeometry> No MASTER volume " << endl;
      return;
  }
  
  if (!GetMapper()) return;
  
  if (!fGeoMapper->InitGeometry()) return;
      
  //fGeoMapper->Print("");
  
  Int_t geo_type = fGeoMapper->GetGeoType();

  ConstructDetector();
  
  fGeoMapper->LockGeometry();
      
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdZdc::ConstructGeometry ******************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//_____________________________________________________________________________________
void SpdZdc::ConstructDetector()
{  
  SpdParameter* par;
  
  SpdZdcGeoMapper* mapper = dynamic_cast<SpdZdcGeoMapper*>(fGeoMapper);
  if (!mapper) return;
  
  TString mat = mapper->GetBaseMaterial();
  TGeoMedium* material = FindMedium(mat,"air");
  
  if (!material) {
      cout << "-E- <SpdTofB::BuildModule> Unknown material: " << mat << endl;
      return;
  }

  Double_t h1  = 0.5*mapper->GetModuleHmin();
  Double_t h2  = 0.5*mapper->GetModuleHmax();
  Double_t w   = 0.5*mapper->GetModuleWidth();
  Double_t l   = 0.5*mapper->GetModuleLength();
  
  Double_t v[8][2];
  
  v[0][0] =  w; v[0][1] =  h1;
  v[1][0] =  w; v[1][1] = -h1;
  v[2][0] = -w; v[2][1] = -h1;
  v[3][0] = -w; v[3][1] =  h1;
  
  v[4][0] =  w; v[4][1] =  h2;
  v[5][0] =  w; v[5][1] = -h2;
  v[6][0] = -w; v[6][1] = -h2;
  v[7][0] = -w; v[7][1] =  h2;

  Int_t n;
  Double_t vert[16];
  
  n = 0;
  for (Int_t i(0); i<8; i++) 
       for (Int_t j(0); j<2; j++) vert[n++] = v[i][j];   
  
  fModule1 = gGeoManager->MakeArb8(mapper->AddPrefix("zplus"),material,l,vert);
  
  if (!fModule1) {
      cout << "-E- <SpdZdc::ConstructGeometry> Module (1) cannot be created " << endl;
      return;
  }
  
  n = 0;
  for (Int_t i(4); i<8; i++) 
       for (Int_t j(0); j<2; j++) vert[n++] = v[i][j];  
  for (Int_t i(0); i<4; i++) 
       for (Int_t j(0); j<2; j++) vert[n++] = v[i][j];     
       
  fModule2 = gGeoManager->MakeArb8(mapper->AddPrefix("zminus"),material,l,vert);
  
  if (!fModule2) {
      cout << "-E- <SpdZdc::ConstructGeometry> Module (2) cannot be created " << endl;
      return;
  }
        
  fModule1->SetFillColor(kMagenta);
  fModule1->SetLineColor(kMagenta);
  fModule1->SetTransparency(30);  
  
  fModule2->SetFillColor(kMagenta);
  fModule2->SetLineColor(kMagenta);
  fModule2->SetTransparency(30);  
  
  AddSensitiveVolume(fModule1); //ATTENTION FIXME ATTENTION
  AddSensitiveVolume(fModule2); //ATTENTION FIXME ATTENTION
  
  Double_t shift = mapper->GetMinDistance() + l;
  
  fMasterVolume->AddNode(fModule1,1,new TGeoTranslation(0,0, shift));
  fMasterVolume->AddNode(fModule2,1,new TGeoTranslation(0,0,-shift));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdZdc::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper("SpdZdcGeoMapper");
  
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdZdcGeoMapper");

  return fGeoMapper; 
}

//_____________________________________________________________________________
Bool_t SpdZdc::LoadParsFrom(SpdParSet* params) 
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
TString SpdZdc::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}


//_____________________________________________________________________________________
TClonesArray* SpdZdc::GetCollection(Int_t iColl) const 
{ 
  return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdZdc::GetCapacity() const 
{ 
  return (fModule1) ? fModule1->GetShape()->Capacity() : 0;
}
   
//_____________________________________________________________________________________
Double_t SpdZdc::GetMass() const 
{ 
  return (fModule1) ? GetCapacity()*GetDensity() : 0;
}

//_____________________________________________________________________________________
Double_t SpdZdc::GetDensity() const 
{ 
  return (fModule1) ? fModule1->GetMaterial()->GetDensity() : 0;
}

//_____________________________________________________________________________________
TString SpdZdc::GetMaterialName() const // private
{ 
  return (fModule1) ? fModule1->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdZdc::Print(Option_t*) const
{
   SpdDetector::Print("");
   
   if (!fGeoMapper) return;
   
   TString divider('-',150);
   
   cout << "\n";

   fGeoMapper->Print("");

   cout << "\n";
   cout <<"\tCapacity (total):    " << GetCapacity()*1e-6 << " [m^3] " << endl;
   cout <<"\tMass (total):        " << 2*GetMass()*1e-3 << " [kg] " << endl;
   cout <<"\tDensity (averaged):  " << GetDensity() << " [g/cm^3] " << endl;
   cout << "\n";
   
   cout << "\n" << divider.Data() << "\n";
   
   printf("%6s %4s %15s %14s %14s %14s \n\n",
          "Type","N","Material","Dens [g/cm^3]","Volume [m^3]","Mass [kg]"
         );
   
   if (!fModule1 || !fModule2) {
       cout << "\n";
       return;
   }
   
   printf("%6d %4d %15s %14.6f %14.6f %14.6f \n",
           1, 1, GetMaterialName().Data(),
           GetDensity(), GetCapacity()*1e-6, GetMass()*1e-3);
   
   printf("%6d %4d %15s %14.6f %14.6f %14.6f \n",
           1, 2, GetMaterialName().Data(),
           GetDensity(), GetCapacity()*1e-6, GetMass()*1e-3);
   cout << divider.Data() << "\n";
   
   printf("%6s %35s %14.3e %14.3e \n","TOTAL:","",2*GetCapacity()*1e-6,2*GetMass()*1e-3);
    
   cout << divider.Data() << "\n";
   
   cout << "\n";
}


