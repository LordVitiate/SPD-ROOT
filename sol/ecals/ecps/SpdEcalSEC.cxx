// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalSEC
//_____________________________________________________________________________

#include "SpdEcalSEC.h"

#include "TRefArray.h"    
#include "TClonesArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TGeoPgon.h"
#include "TGeoCompositeShape.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TMath.h"

#include "SpdEcalSECPoint.h"
#include "SpdEcalSECGeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdEcalSEC)

//_____________________________________________________________________________________
SpdEcalSEC::SpdEcalSEC(): 

  SpdDetector("Ecal endcaps (tor)", kSpdEcalSEC, kTRUE),
  fTrackID(-1),
  fVolumeID1(-1),fVolumeID2(-1),fVolumeID3(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fPointCollection(0)
{
   SetParametersType("EcalSECParSet");
   
   fNDataOut = 1;
   fOutDataPointObject = "SpdEcalSECPoint"; 
}

//_____________________________________________________________________________________
SpdEcalSEC::SpdEcalSEC(const char* name, Bool_t active):

  SpdDetector(name, kSpdEcalSEC, active),
  fTrackID(-1),
  fVolumeID1(-1),fVolumeID2(-1),fVolumeID3(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fPointCollection(0)
{
   SetParametersType("EcalSECParSet");
   
   fNDataOut = 1;
   fOutDataPointObject = "SpdEcalSECPoint"; 
}

//_____________________________________________________________________________________
SpdEcalSEC::~SpdEcalSEC()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdEcalSEC::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdEcalSEC::Initialize ******************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();
}

//_____________________________________________________________________________________
void SpdEcalSEC::Reset()
{
  fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdEcalSEC::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdEcalSEC", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdEcalSEC::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdEcalSEC::ProcessHits> " << endl;
  
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

  // Create SpdEcalSECPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
  
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    
    fVolumeID1 =  kSpdEcalSEC;
    fVolumeID2 = -111111111;
    fVolumeID3 = -111111111;
    
    TString path = gMC->CurrentVolPath();

    TString vname2 = gMC->CurrentVolOffName(0);
    gMC->CurrentVolOffID(0,fVolumeID2); 
    
    if (fVolumeID2 == 2) fVolumeID2 = -1;
 
    //cout << "path = " << gMC->CurrentVolPath() << endl;
    //cout << ": "<< fVolumeID1 << "/ " << fVolumeID2 << " " << fVolumeID3 << endl;
   
    AddHit();
    
    //cout << "<SpdEcalSEC::AddHit> " << fSpdEcalSECPointCollection->GetEntriesFast() << endl;
   
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdEcalSEC);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdEcalSEC::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdEcalSECPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  new(clref[size]) SpdEcalSECPoint(fTrackID,fVolumeID1,fVolumeID2,fVolumeID3,
                                   TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                                   TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                                   fTime, fLength, fELoss);                                 
}

//_____________________________________________________________________________________
void SpdEcalSEC::EndOfEvent()
{
  Reset();
}

//_____________________________________________________________________________________
void SpdEcalSEC::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdEcalSEC::ConstructGeometry()
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
  cout << "********************** SpdEcalSEC::ConstructGeometry **************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//________________________________________________________________________________
void SpdEcalSEC::ConstructGeometry_1()
{ 
  SpdParameter* par;
  
  if (!BuildCap()) return;
  
  Double_t thickness;
  par = fGeoMapper->GetParameter("EcalSECThickness");
  if (!par) return;
  par->Value(thickness);
  
  Double_t mindist;
  par = fGeoMapper->GetParameter("EcalSECMinDist");
  if (!par) return;
  par->Value(mindist);
  
  Double_t shift = mindist+0.5*thickness; // z-shift
  
  Double_t angle = ((SpdEcalSECGeoMapper*)fGeoMapper)->GetSecAngleOver2();
  
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
Bool_t SpdEcalSEC::BuildCap()
{
   SpdParameter* par;
   par = fGeoMapper->GetParameter("EcalSECBaseMaterial");
   
   if (!par) {
       cout << "-E- <SpdEcalSEC::BuildCap> "
            << "Unknown material (1): EcalSECBaseMaterial" << endl;
       return kFALSE;
   }
   
   TString mname(par->Value());
   TGeoMedium* material = FindMedium(mname,"");
   
   if (!material) {
       cout << "-E- <SpdEcalSEC::BuildCap> "
            << "Unknown material (2): " << mname << endl;
       return kFALSE;
   }
   
   Double_t mlen, hmin, hmax; 
  
   par = fGeoMapper->GetParameter("EcalSECThickness");
   if (!par) return kFALSE;
   par->Value(mlen);
   
   par = fGeoMapper->GetParameter("EcalSECHmin");
   if (!par) return kFALSE;
   par->Value(hmin);
   
   par = fGeoMapper->GetParameter("EcalSECSize");
   if (!par) return kFALSE;
   par->Value(hmax);
   
   Int_t nsectors = ((SpdEcalSECGeoMapper*)fGeoMapper)->GetNSectors();
  
   TString volname = fGeoMapper->GetVolName(0,0);
   fCap.volume_ = gGeoManager->MakePgon(volname,material,0,360,nsectors,2);
   
   TGeoPgon* pgon = (TGeoPgon*)fCap.volume_->GetShape();
   
   pgon->DefineSection(0,-0.5*mlen, hmin, hmax);
   pgon->DefineSection(1, 0.5*mlen, hmin, hmax);
   
   fCap.capacity_ = fCap.volume_->Capacity();
   fCap.mass_ = fCap.capacity_ * fCap.volume_->GetMaterial()->GetDensity();
   
   fCap.volume_->SetFillColor(kBlue+2);
   fCap.volume_->SetLineColor(kBlue+2);
   fCap.volume_->SetTransparency(40);
   
   AddSensitiveVolume(fCap.volume_);
   
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdEcalSEC::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper(fDetId);
  
  // create default mapper
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdEcalSECGeoMapper");
     
  return fGeoMapper;
}

//_____________________________________________________________________________
Bool_t SpdEcalSEC::LoadParsFrom(SpdParSet* params) 
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
TString SpdEcalSEC::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}

//_____________________________________________________________________________________
TClonesArray* SpdEcalSEC::GetCollection(Int_t iColl) const 
{ 
   return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdEcalSEC::GetCapacity() const 
{ 
   return fCap.capacity_*fCap.counts_;
}
   
//_____________________________________________________________________________________
Double_t SpdEcalSEC::GetMass() const 
{ 
   return fCap.mass_*fCap.counts_;
}

//_____________________________________________________________________________________
TString SpdEcalSEC::GetBaseMaterialName() const // private
{ 
   return (fCap.volume_) ? fCap.volume_->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdEcalSEC::Print(Option_t*) const
{
   cout << "<SpdEcalSEC::Print> " << endl;
   
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


