// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsSB
//_____________________________________________________________________________

#include "SpdRsSB.h"

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

#include "SpdRsSBPoint.h"
#include "SpdRsSBGeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdRsSB)

//_____________________________________________________________________________________
SpdRsSB::SpdRsSB(): 

  SpdDetector("RS barrel (tor)", kSpdRsSB, kTRUE),
  fTrackID(-1),
  fVolumeID1(-1),fVolumeID2(-1),fVolumeID3(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fPointCollection(0)
{
  SetParametersType("RsSBParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdRsSBPoint"; 
}

//_____________________________________________________________________________________
SpdRsSB::SpdRsSB(const char* name, Bool_t active):

  SpdDetector(name, kSpdRsSB, active),
  fTrackID(-1),
  fVolumeID1(-1),fVolumeID2(-1),fVolumeID3(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fPointCollection(0)
{
  SetParametersType("RsSBParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdRsSBPoint"; 
}

//_____________________________________________________________________________________
SpdRsSB::~SpdRsSB()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdRsSB::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdRsSB::Initialize *********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();  

}

//_____________________________________________________________________________________
void SpdRsSB::Reset()
{
  fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdRsSB::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdRsSB", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdRsSB::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdRsSB::ProcessHits> " << endl;
  
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

  // Create SpdRsSBPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        
    fVolumeID1 =  kSpdRsSB;
    fVolumeID2 = -111111111;
    fVolumeID3 = -111111111;
    
    TString path = gMC->CurrentVolPath();
    
    static Int_t geo_type = fGeoMapper->GetGeoType();
  
    if (geo_type == 1) {
    
        fVolumeID2 = 1;
    }
    else if (geo_type == 2) {
        
        TString vname2 = gMC->CurrentVolOffName(0);
        gMC->CurrentVolOffID(0,fVolumeID2); 
    }
   
    //cout << "path = " << gMC->CurrentVolPath() << endl;
    //cout << ": "<< fVolumeID1 << "/ " << fVolumeID2 << " " << fVolumeID3 << endl;
   
    AddHit();
    
    //cout << "<SpdRsSB::AddHit> " << fSpdRsSBPointCollection->GetEntriesFast() << endl;
   
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdRsSB);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdRsSB::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdRsSBPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  new(clref[size]) SpdRsSBPoint(fTrackID,fVolumeID1,fVolumeID2,fVolumeID3,
                                TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                                TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                                fTime, fLength, fELoss);                                 
}

//_____________________________________________________________________________________
void SpdRsSB::EndOfEvent()
{
   Reset();
}

//_____________________________________________________________________________________
void SpdRsSB::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdRsSB::ConstructGeometry()
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
  
       if (geo_type == 1) ConstructGeometry_1();
  else if (geo_type == 2) ConstructGeometry_2();
  
  fGeoMapper->LockGeometry();
             
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdRsSB::ConstructGeometry *****************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//________________________________________________________________________________
void SpdRsSB::ConstructGeometry_1()
{ 
  SpdParameter* par;
  
  if (!BuildSection()) return;
  
  TGeoRotation* rot = new TGeoRotation();
  rot->RegisterYourself();
  
  Double_t angle = ((SpdRsSBGeoMapper*)fGeoMapper)->GetSecAngleOver2();
 
  rot->RotateZ(angle);
   
  fMasterVolume->AddNode(fSection.volume_,1,rot);
  fSection.counts_++;
}

//_____________________________________________________________________________________
void SpdRsSB::ConstructGeometry_2()
{  
  SpdParameter* par;
  
  if (!BuildSection()) return;
  
  Double_t len;
  par = fGeoMapper->GetParameter("RsSBSectionLen");
  if (!par) return;
  par->Value(len);
 
  TGeoTranslation trans; 
  TGeoRotation rot;
  
  Double_t angle = ((SpdRsSBGeoMapper*)fGeoMapper)->GetSecAngleOver2();
  
  Int_t nsections;
  par = fGeoMapper->GetParameter("RsSBNSections2");
  if (!par) return;
  par->Value(nsections);
  
  rot.RotateZ(angle);
  
  Double_t shift = -0.5*(nsections-1)*len;
   
  for (Int_t i(0); i<nsections; i++) {
       trans.SetTranslation(0,0,shift);
       fMasterVolume->AddNode(fSection.volume_,i+1,new TGeoCombiTrans(trans,rot));
       shift += len;
       fSection.counts_++;
  }
}

//_____________________________________________________________________________   
Bool_t SpdRsSB::BuildSection()
{
   SpdParameter* par;
   par = fGeoMapper->GetParameter("RsSBBaseMaterial");
   
   if (!par) {
       cout << "-E- <SpdRsSB::BuildCap> "
            << "Unknown material (1): RsSBBaseMaterial" << endl;
       return kFALSE;
   }
   
   TString mname(par->Value());
   TGeoMedium* material = FindMedium(mname,"");
   
   if (!material) {
       cout << "-E- <SpdRsSB::BuildCap> "
            << "Unknown material (2): " << mname << endl;
       return kFALSE;
   }
   
   Double_t mlen, hmin, hmax; 
  
   par = fGeoMapper->GetParameter("RsSBSectionLen");
   if (!par) return kFALSE;
   par->Value(mlen);
   
   par = fGeoMapper->GetParameter("RsSBHmin");
   if (!par) return kFALSE;
   par->Value(hmin);
   
   par = fGeoMapper->GetParameter("RsSBSize");
   if (!par) return kFALSE;
   par->Value(hmax);
   
   Int_t nsectors = ((SpdRsSBGeoMapper*)fGeoMapper)->GetNSectors();
  
   TString volname = fGeoMapper->GetVolName(0,0);
   fSection.volume_ = gGeoManager->MakePgon(volname,material,0,360,nsectors,2);
   
   TGeoPgon* pgon = (TGeoPgon*)fSection.volume_->GetShape();
   
   pgon->DefineSection(0,-0.5*mlen, hmin, hmax);
   pgon->DefineSection(1, 0.5*mlen, hmin, hmax);
   
   fSection.volume_->SetFillColor(kSpring);
   fSection.volume_->SetLineColor(kSpring);
   fSection.volume_->SetTransparency(30);
   
   fSection.capacity_ = fSection.volume_->Capacity();
   fSection.mass_ = fSection.capacity_ * fSection.volume_->GetMaterial()->GetDensity();
   
   AddSensitiveVolume(fSection.volume_);
   
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdRsSB::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper(fDetId);
  
  // create default mapper
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdRsSBGeoMapper");
     
  return fGeoMapper; 
}

//_____________________________________________________________________________
Bool_t SpdRsSB::LoadParsFrom(SpdParSet* params) 
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
TString SpdRsSB::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}

//_____________________________________________________________________________________
TClonesArray* SpdRsSB::GetCollection(Int_t iColl) const 
{ 
  return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdRsSB::GetCapacity() const 
{ 
   return fSection.capacity_*fSection.counts_;
}
   
//_____________________________________________________________________________________
Double_t SpdRsSB::GetMass() const 
{ 
   return fSection.mass_*fSection.counts_;
}

//_____________________________________________________________________________
Double_t SpdRsSB::GetDensity() const 
{ 
  return SpdDetector::GetDensity();
}

//_____________________________________________________________________________________
TString SpdRsSB::GetBaseMaterialName() const // private
{ 
   return (fSection.volume_) ? fSection.volume_->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdRsSB::Print(Option_t*) const
{
   cout << "<SpdRsSB::Print> " << endl; 
   
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


