// $Id$
// Author: artur   2021/08/18

//_____________________________________________________________________________
//
// SpdTofEC
//_____________________________________________________________________________

#include "SpdTofEC.h"

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
#include "FairGeoMedia.h"
#include "FairGeoBuilder.h"
#include "TMath.h"

#include "SpdTofECPoint.h"
#include "SpdTofECGeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdTofEC)

//_____________________________________________________________________________________
SpdTofEC::SpdTofEC(): 

  SpdDetector("TOF endcaps (qsl)", kSpdTofEC, kTRUE),
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
  SetParametersType("TofECParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdTofECPoint"; 
}

//_____________________________________________________________________________________
SpdTofEC::SpdTofEC(const char* name, Bool_t active):

  SpdDetector(name, kSpdTofEC, active),
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
  SetParametersType("TofECParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdTofECPoint";
}

//_____________________________________________________________________________________
SpdTofEC::~SpdTofEC()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdTofEC::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdTofEC::Initialize ********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();  

}

//_____________________________________________________________________________________
void SpdTofEC::Reset()
{
  fPointCollection->Delete();
  //fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdTofEC::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdTofEC", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdTofEC::ProcessHits(FairVolume* vol)
{
  
  //cout << "<SpdTofEC::ProcessHits> " << gMC->CurrentVolPath() << endl;
  
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

  // Create SpdTofECPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fSegmentLength = gMC->TrackLength() - fLength;
    fTimeOut   = gMC->TrackTime() * 1.e9;  // ns -> s
    
    fVolumeID   = kSpdTofEC;
    fVolumePath = gMC->CurrentVolPath();
    
    //     if (fVolumePath.Contains("zplus"))  fModuleID =  1;
    //else if (fVolumePath.Contains("zminus")) fModuleID = -1;
    //else fModuleID = 0;
    
         if (fPos.Z() >  1.e-6) fModuleID =  1;
    else if (fPos.Z() < -1.e-6) fModuleID = -1;
    else fModuleID = 0;
    
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
        
    //cout << "path = " << fVolumePath << endl;
   
    AddHit();
    
    //cout << "<SpdTofEC::AddHit> " << fPointCollection->GetEntriesFast() << endl;
    
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdTofEC);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdTofEC::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdTofECPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  SpdTofECPoint* point = new(clref[size]) SpdTofECPoint(fTrackID,fVolumeID,fModuleID,
                                          TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                                          TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                                          TVector3(fPosOut.X(),fPosOut.Y(),fPosOut.Z()),
                                          TVector3(fMomOut.Px(),fMomOut.Py(),fMomOut.Pz()),
                                          fTime, fTimeOut,
                                          fLength, fSegmentLength, fELoss);
  point->SetNodePath(fVolumePath);
}

//_____________________________________________________________________________________
void SpdTofEC::EndOfEvent()
{
   Reset();
}

//_____________________________________________________________________________________
void SpdTofEC::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdTofEC::ConstructGeometry()
{   
  // Create detector geometry
  
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
  if (!fMasterVolume) {  
      cout << "-E- <SpdTofEC::ConstructGeometry> No MASTER volume " << endl;
      return;
  }
  
  if (!GetMapper()) return;
  
  if (!fGeoMapper->InitGeometry()) return;
      
  //fGeoMapper->Print("");
  
  Int_t geo_type = fGeoMapper->GetGeoType();

       if (geo_type == 1) ConstructDetector();
  else if (geo_type == 2) ConstructDetector_MRPC_Hybrid();

  fGeoMapper->LockGeometry();
      
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdTofEC::ConstructGeometry ****************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//_____________________________________________________________________________________
void SpdTofEC::ConstructDetector()
{  
  SpdParameter* par;
  
  SpdTofECGeoMapper* mapper = dynamic_cast<SpdTofECGeoMapper*>(fGeoMapper);
  if (!mapper) return;
  
  TString mat = mapper->GetBaseMaterial();
  TGeoMedium* material = FindMedium(mat,"air");
  
  if (!material) {
      cout << "-E- <SpdTofEC::BuildModule> Unknown material: " << mat << endl;
      return;
  }
  
  Double_t mthick  = mapper->GetModuleThickness();
  Double_t msize   = mapper->GetModuleSize();
  Double_t mwidth  = mapper->GetModuleWidth();
  
  fModule1 = gGeoManager->MakeTube(mapper->AddPrefix("zplus"),material,msize-mwidth,msize,0.5*mthick);
  
  if (!fModule1) {
      cout << "-E- <SpdTofEC::ConstructGeometry> Module (1) cannot be created " << endl;
      return;
  }
  
  fModule2 = gGeoManager->MakeTube(mapper->AddPrefix("zminus"),material,msize-mwidth,msize,0.5*mthick);
  
  if (!fModule2) {
      cout << "-E- <SpdTofEC::ConstructGeometry> Module (2) cannot be created " << endl;
      return;
  }
    
  fModule1->SetFillColor(kYellow);
  fModule1->SetLineColor(kYellow);
  fModule1->SetTransparency(30);  
  
  fModule2->SetFillColor(kYellow);
  fModule2->SetLineColor(kYellow);
  fModule2->SetTransparency(30);  
  
  AddSensitiveVolume(fModule1); //ATTENTION FIXME ATTENTION
  AddSensitiveVolume(fModule2); //ATTENTION FIXME ATTENTION
  
  Double_t shift = mapper->GetMinDistance() + 0.5*mthick;
  
  fMasterVolume->AddNode(fModule1,1,new TGeoTranslation(0,0, shift));
  fMasterVolume->AddNode(fModule2,1,new TGeoTranslation(0,0,-shift));
}

//_____________________________________________________________________________________
void SpdTofEC::ConstructDetector_MRPC_Hybrid()
{
  SpdTofECGeoMapper* mapper = dynamic_cast<SpdTofECGeoMapper*>(fGeoMapper);
  if (!mapper) return;

  Double_t mlength  = mapper->GetTofECLengthModuleMRPCTsinghua();
  Double_t msize    = mapper->GetTofECSizeModuleMRPCTsinghua();
  Double_t mwidth1  = mapper->GetTofECWidth1ModuleMRPCTsinghua();
  Double_t mwidth2  = mapper->GetTofECWidth2ModuleMRPCTsinghua();
  Double_t innerRadius  = mapper->GetModuleSize() - mapper->GetModuleWidth();

  fModule = BuildMRPCModule(mwidth1, mwidth2, msize, mlength);
  
  if (!fModule) {
      cout << "-E- <SpdTofEC::ConstructDetector_MRPC_Hybrid> Module cannot be created " << endl;
      return;
  }


  Double_t RmaxTof   = mapper->GetModuleSize();
 // Double_t RwidthTof = mapper->GetTofECThickness();
 // Double_t RminTof = RmaxTof - RwidthTof ;

  Double_t shift_z = mapper->GetMinDistance() ;
  Double_t Rmean =  RmaxTof - 10. ;

  Double_t step_phi = 45.;
  Double_t phi_second_layer = 45. / 2.;
  Double_t shift_r_y = 2.5;

  Double_t shift_between_layer_z = 1.0;

  Int_t max_phi = 360;

  Double_t phi, r, x_dec, y_dec, z_dec;
  
  Int_t inode = 0;
  
  for (int idet = -1; idet <= 1; idet += 2)
  {
    for (int iphi = 0, istep = 0; iphi < max_phi; iphi += step_phi, istep++)
    {
      TGeoRotation rot1, rot2;
      TGeoTranslation trans1, trans2;
      rot1.RotateX(90);
      rot2.RotateX(90);

      // --- first layer ----
      r = innerRadius + mlength / 2.;
      phi = (90 + iphi) * (TMath::Pi() / 180.);
      x_dec = r * cos(phi);
      y_dec = r * sin(phi);

      rot1.RotateZ(iphi);

      trans1.SetTranslation(x_dec, y_dec, idet * (shift_z+msize/2.));
      fMasterVolume->AddNode(fModule, ++inode, new TGeoCombiTrans(trans1, rot1));

      // --- second layer ----
      r = innerRadius + mlength / 2.;
      phi = (90 + iphi + step_phi / 2.) * (TMath::Pi() / 180.);
      x_dec = r * cos(phi);
      y_dec = r * sin(phi);

      rot2.RotateZ(iphi + phi_second_layer);

      trans2.SetTranslation(x_dec, y_dec, idet * (shift_z + shift_between_layer_z +1.5*msize));
      fMasterVolume->AddNode(fModule, ++inode, new TGeoCombiTrans(trans2, rot2));
    }
  }
}

//________________________________________________________________________________
TGeoVolume* SpdTofEC::BuildMRPCModule(Double_t mwidth1, Double_t mwidth2,  Double_t msize, Double_t mlength)
{
  SpdTofECGeoMapper* mapper = dynamic_cast<SpdTofECGeoMapper*>(fGeoMapper);
  if (!mapper) return 0;

  TString mat = mapper->GetBaseMaterial();
  TGeoMedium* material = FindMedium(mat, "air");

   
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  FairGeoBuilder*  geobuild = geoLoad->getGeoBuilder(); 
  FairGeoMedia*       geoMedia = geoFace->getMedia();
    

  FairGeoMedium* mRPCglass = geoMedia->getMedium("PMTglass");
  if ( ! mRPCglass ) Fatal("Main", "FairMedium RPCglass not found");
  geobuild->createMedium(mRPCglass);
  TGeoMedium* pMedRPCglass = gGeoManager->GetMedium("PMTglass");
  if ( ! pMedRPCglass ) Fatal("Main", "Medium RPCglass not found");



  if (!material) {
      cout << "-E- <SpdTofEC::BuildMRPCModule> Unknown material: " << mat << endl;
      return 0;
  }

  double thickness = 11 * 0.033; // 11 glass plates with 0.33 mm thick 
  TGeoVolume *vol = new TGeoVolumeAssembly(mapper->AddPrefix("module"));

  TString name_vol_air = "air" +  mapper->AddPrefix("module");
  TGeoVolume *vol_air  = gGeoManager->MakeTrd1(name_vol_air, material, 
                                          mwidth1 * 0.5, mwidth2 * 0.5,
                                          (msize-thickness) * 0.5, mlength * 0.5);

  TString name_vol_glass = "glass" +  mapper->AddPrefix("module");
  TGeoVolume *vol_glass = gGeoManager->MakeTrd1(name_vol_glass, pMedRPCglass, 
                                          mwidth1 * 0.5, mwidth2 * 0.5,
                                          thickness * 0.5, mlength * 0.5);        
   

  vol->AddNode(vol_air,   1, new TGeoTranslation(0,(msize-thickness)/2.,0));
  vol->AddNode(vol_glass, 2, new TGeoTranslation(0,-thickness/2.,0));


   vol_air->SetFillColor(kGreen);
   vol_air->SetLineColor(kGreen);
   vol_air->SetTransparency(30);

   vol_glass->SetFillColor(kYellow);
   vol_glass->SetLineColor(kYellow);
   vol_glass->SetTransparency(30);

                            

   AddSensitiveVolume(vol_air);
   AddSensitiveVolume(vol_glass);

  
  return vol;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdTofEC::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper("SpdTofECGeoMapper");
  
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdTofECGeoMapper");

  return fGeoMapper; 
}

//_____________________________________________________________________________
Bool_t SpdTofEC::LoadParsFrom(SpdParSet* params) 
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
TString SpdTofEC::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}

//_____________________________________________________________________________________
TClonesArray* SpdTofEC::GetCollection(Int_t iColl) const 
{ 
  return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdTofEC::GetCapacity() const 
{ 
   return (fModule1) ? fModule1->GetShape()->Capacity() : 0;
}
   
//_____________________________________________________________________________________
Double_t SpdTofEC::GetMass() const 
{ 
   return (fModule1) ? GetCapacity()*GetDensity() : 0;
}

//_____________________________________________________________________________________
Double_t SpdTofEC::GetDensity() const 
{ 
   return (fModule1) ? fModule1->GetMaterial()->GetDensity() : 0;
}

//_____________________________________________________________________________________
TString SpdTofEC::GetMaterialName() const // private
{ 
   return (fModule1) ? fModule1->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdTofEC::Print(Option_t*) const
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


