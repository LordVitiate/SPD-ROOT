// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdTsSEC
//_____________________________________________________________________________

#include "SpdTsSEC.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairRun.h"
#include "FairRunSim.h"
#include "FairRuntimeDb.h"

#include "TRefArray.h"    
#include "TClonesArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TGeoPgon.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TMath.h"

#include "SpdTsSECPoint.h"
#include "SpdTsSECGeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdTsSEC)

//_____________________________________________________________________________________
SpdTsSEC::SpdTsSEC(): 

  SpdDetector("TS endcaps (tor)", kSpdTsSEC, kTRUE),
  fTrackID(-1),
  fVolumeID1(-1),fVolumeID2(-1),fVolumeID3(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fPointCollection(0)
{ 
  SetParametersType("TsSECParSet");
  
  fNDataOut = 1;
  fOutDataPointObject = "SpdTsSECPoint"; 
}

//_____________________________________________________________________________________
SpdTsSEC::SpdTsSEC(const char* name, Bool_t active):

  SpdDetector(name, kSpdTsSEC, active),
  fTrackID(-1),
  fVolumeID1(-1),fVolumeID2(-1),fVolumeID3(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fPointCollection(0)
{
  SetParametersType("TsSECParSet");
  
  fNDataOut = 1;
  fOutDataPointObject = "SpdTsSECPoint"; 
}

//_____________________________________________________________________________________
SpdTsSEC::~SpdTsSEC()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdTsSEC::Initialize()
{ 
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdTsSEC::Initialize ********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
  
// SpdParSet* pars = GetPars();
// if (pars) pars->printParams(); 
}

//_____________________________________________________________________________________
void SpdTsSEC::Reset()
{
  fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdTsSEC::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdTsSEC", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdTsSEC::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdTsSEC::ProcessHits> " << endl;
  
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

  // Create SpdTsSECPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
 
    fVolumeID1 =  kSpdTsSEC;
    fVolumeID2 = -111111111;
    fVolumeID3 = -111111111;
    
    TString path = gMC->CurrentVolPath();

    TString vname2 = gMC->CurrentVolOffName(0);
    gMC->CurrentVolOffID(0,fVolumeID2); 
    
    if (fVolumeID2 == 2) fVolumeID2 = -1;
 
    //cout << "path = " << gMC->CurrentVolPath() << endl;
    //cout << ": "<< fVolumeID1 << "/ " << fVolumeID2 << " " << fVolumeID3 << endl;
   
    AddHit();
    
    //cout << "<SpdTsSEC::AddHit> " << fPointCollection->GetEntriesFast() << endl;
   
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdTsSEC);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdTsSEC::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdTsSECPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  new(clref[size]) SpdTsSECPoint(fTrackID,fVolumeID1,fVolumeID2,fVolumeID3,
                                 TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                                 TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                                 fTime, fLength, fELoss);                                 
}

//_____________________________________________________________________________________
void SpdTsSEC::EndOfEvent()
{
  Reset();
}

//_____________________________________________________________________________________
void SpdTsSEC::FinishRun() 
{
  //SpdDetector::FinishRun();
  FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdTsSEC::ConstructGeometry()
{   
  // Create detector geometry
  
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
  if (!fMasterVolume) {  
      cout << "-E- <SpdTsTEC::ConstructGeometry> No MASTER volume " << endl;
      return;
  }

  if (!GetMapper()) return;
  
  if (!fGeoMapper->InitGeometry()) return;
  
  fGeoMapper->LockGeometry();
      
  //fGeoMapper->Print("");
  
  Int_t geo_type = fGeoMapper->GetGeoType();

  if (geo_type == 1) ConstructGeometry_1();

  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdTsSEC::ConstructGeometry ****************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//________________________________________________________________________________
void SpdTsSEC::ConstructGeometry_1()
{ 
  SpdParameter* par;
  
  if (!BuildCap()) return;
  
  Double_t thickness;
  par = fGeoMapper->GetParameter("TsSECThickness");
  if (!par) return;
  par->Value(thickness);
  
  Double_t mindist;
  par = fGeoMapper->GetParameter("TsSECMinDist");
  if (!par) return;
  par->Value(mindist);
  
  Double_t shift = mindist+0.5*thickness; // z-shift
  
  Double_t angle = ((SpdTsSECGeoMapper*)fGeoMapper)->GetSecAngleOver2();
  
  TGeoTranslation trans; 
  TGeoRotation rot;
  
  rot.RotateZ(angle);
  
  trans.SetTranslation(0,0,shift);
  fMasterVolume->AddNode(fCap.volume_,1,new TGeoCombiTrans(trans,rot));
  fCap.counts_++;
  
  trans.SetTranslation(0,0,-shift);
  fMasterVolume->AddNode(fCap.volume_,2,new TGeoCombiTrans(trans,rot));
  fCap.counts_++;
}

//_____________________________________________________________________________   
Bool_t SpdTsSEC::BuildCap()
{
   SpdParameter* par;
   par = fGeoMapper->GetParameter("TsSECBaseMaterial");
   
   if (!par) {
       cout << "-E- <SpdTsSEC::BuildCap> "
            << "Unknown material (1): TsSECBaseMaterial" << endl;
       return kFALSE;
   }
   
   TString mname(par->Value());
   TGeoMedium* material = FindMedium(mname,"");
   
   if (!material) {
       cout << "-E- <SpdTsSEC::BuildCap> "
            << "Unknown material (2): " << mname << endl;
       return kFALSE;
   }
   
   Double_t mlen, hmin, hmax; 
  
   par = fGeoMapper->GetParameter("TsSECThickness");
   if (!par) return kFALSE;
   par->Value(mlen);
   
   par = fGeoMapper->GetParameter("TsSECHmin");
   if (!par) return kFALSE;
   par->Value(hmin);
   
   par = fGeoMapper->GetParameter("TsSECSize");
   if (!par) return kFALSE;
   par->Value(hmax);
   
   Int_t nsectors = ((SpdTsSECGeoMapper*)fGeoMapper)->GetNSectors();
  
   TString volname = fGeoMapper->GetVolName(0,0);
   fCap.volume_ = gGeoManager->MakePgon(volname,material,0,360,nsectors,2);
   
   TGeoPgon* pgon = (TGeoPgon*)fCap.volume_->GetShape();
   
   pgon->DefineSection(0,-0.5*mlen, hmin, hmax);
   pgon->DefineSection(1, 0.5*mlen, hmin, hmax);
   
   fCap.capacity_ = fCap.volume_->Capacity();
   fCap.mass_ = fCap.capacity_ * fCap.volume_->GetMaterial()->GetDensity();
   
   fCap.volume_->SetFillColor(kYellow+2);
   fCap.volume_->SetLineColor(kYellow+2);
   fCap.volume_->SetTransparency(40);
   
   AddSensitiveVolume(fCap.volume_);
   
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdTsSEC::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper(fDetId);
  
  // create default mapper
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdTsSECGeoMapper");
     
  return fGeoMapper;
}

//_____________________________________________________________________________
Bool_t SpdTsSEC::LoadParsFrom(SpdParSet* params) 
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
TString SpdTsSEC::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}

//_____________________________________________________________________________________
TClonesArray* SpdTsSEC::GetCollection(Int_t iColl) const 
{ 
   return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdTsSEC::GetCapacity() const 
{ 
   return fCap.capacity_*fCap.counts_;
}
   
//_____________________________________________________________________________________
Double_t SpdTsSEC::GetMass() const 
{ 
   return fCap.mass_*fCap.counts_;
}

//_____________________________________________________________________________________
TString SpdTsSEC::GetBaseMaterialName() const // private
{ 
   return (fCap.volume_) ? fCap.volume_->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdTsSEC::Print(Option_t*) const
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
   
   cout << "\n";
}


