// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalTB
//_____________________________________________________________________________

#include "SpdEcalTB.h"

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
#include "TMath.h"

#include "SpdEcalTBPoint.h"
#include "SpdEcalTBGeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdEcalTB)

//_____________________________________________________________________________________
SpdEcalTB::SpdEcalTB(): 

  SpdDetector("Ecal barrel (tor)", kSpdEcalTB, kTRUE),
  fTrackID(-1),
  fVolumeID1(-1),fVolumeID2(-1),fVolumeID3(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fPointCollection(0)
{
   SetParametersType("EcalTBParSet");
   
   fNDataOut = 1;
   fOutDataPointObject = "SpdEcalTBPoint"; 
}

//_____________________________________________________________________________________
SpdEcalTB::SpdEcalTB(const char* name, Bool_t active):

  SpdDetector(name, kSpdEcalTB, active),
  fTrackID(-1),
  fVolumeID1(-1),fVolumeID2(-1),fVolumeID3(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1),
  fPointCollection(0)
{
  SetParametersType("EcalTBParSet");
  
  fNDataOut = 1;
  fOutDataPointObject = "SpdEcalTBPoint"; 
}

//_____________________________________________________________________________________
SpdEcalTB::~SpdEcalTB()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdEcalTB::Initialize()
{ 
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdEcalTB::Initialize *******************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();
}

//_____________________________________________________________________________________
void SpdEcalTB::Reset()
{
  fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdEcalTB::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdEcalTB", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdEcalTB::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdEcalTB::ProcessHits> " << endl;
  
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

  // Create SpdEcalTBPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    
    fVolumeID1 =  kSpdEcalTB;
    fVolumeID2 = -111111111;
    fVolumeID3 = -111111111;
    
    TString path = gMC->CurrentVolPath();
    
    static Int_t geo_type = fGeoMapper->GetGeoType();
    static Int_t NSectors = ((SpdEcalTBGeoMapper*)fGeoMapper)->GetNSectors();
  
    if (geo_type == 1) {

        TString vname3 = gMC->CurrentVolOffName(0);
        gMC->CurrentVolOffID(0,fVolumeID3); 
        
        if (path.Contains("ECALTbmodule11")) fVolumeID2 = 0;
        else {
             if (fVolumeID3 > NSectors) {
                 fVolumeID2 = -1;
                 fVolumeID3 -= NSectors;
             }
             else fVolumeID2 = 1;
        }
    }
    else if (geo_type == 2) {
        
        TString vname3 = gMC->CurrentVolOffName(0);
        gMC->CurrentVolOffID(0,fVolumeID3); 
        
        fVolumeID2 = 0;
    }
    
    //cout << "path = " << gMC->CurrentVolPath() << endl;
    //cout << geo_type << ": "<< fVolumeID1 << "/ " << fVolumeID2 << " " << fVolumeID3 << endl;
   
    AddHit();
    
    //cout << "<SpdEcalTB::AddHit> " << fPointCollection->GetEntriesFast() << endl;
    
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdEcalTB);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdEcalTB::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdEcalTBPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  new(clref[size]) SpdEcalTBPoint(fTrackID,fVolumeID1,fVolumeID2,fVolumeID3,
                                  TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                                  TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                                  fTime, fLength, fELoss);                                 
}

//_____________________________________________________________________________________
void SpdEcalTB::EndOfEvent()
{
  Reset();
}

//_____________________________________________________________________________________
void SpdEcalTB::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdEcalTB::ConstructGeometry()
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
  cout << "********************** SpdEcalTB::ConstructGeometry ***************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//________________________________________________________________________________
void SpdEcalTB::ConstructGeometry_1()
{ 
  SpdParameter* par;
  
  Int_t nsectors = ((SpdEcalTBGeoMapper*)fGeoMapper)->GetNSectors();
  
  Double_t hmin, hmax, lmin, lmax; 
  
  /*-----------------------------------------------------------*/
  
  Double_t mlen11;
  par = fGeoMapper->GetParameter("EcalTBLen11");
  if (!par) return;
  par->Value(mlen11);
  
  par = fGeoMapper->GetParameter("EcalTBLmin11");
  if (!par) return;
  par->Value(lmin);
  
  par = fGeoMapper->GetParameter("EcalTBLmax11");
  if (!par) return;
  par->Value(lmax);
  
  par = fGeoMapper->GetParameter("EcalTBHmin11");
  if (!par) return;
  par->Value(hmin);
  
  par = fGeoMapper->GetParameter("EcalTBSize11");
  if (!par) return;
  par->Value(hmax);
  
  TString volname;
  
  volname = fGeoMapper->GetVolName(0,0);
  
  fLargeModule[0].volume_ = BuildModule(volname, "EcalTBBaseMaterial", 
                                        0.5*mlen11, hmin, hmax, 0.5*lmin, 0.5*lmax);
  
  if (!fLargeModule[0].volume_) {
      cout << "-E- <SpdEcalTB::ConstructGeometry_1> "
           << "Volume " << volname << " cannot be created " << endl;
      return;
  }
    
  fLargeModule[0].capacity_ = fLargeModule[0].volume_->Capacity();
  
  fLargeModule[0].volume_->SetFillColor(kCyan);
  fLargeModule[0].volume_->SetLineColor(kCyan);
  fLargeModule[0].volume_->SetTransparency(30);
    
  /*-----------------------------------------------------------*/
  
  Double_t mlen12;
  par = fGeoMapper->GetParameter("EcalTBLen12");
  if (!par) return;
  par->Value(mlen12);
  
  par = fGeoMapper->GetParameter("EcalTBLmin12");
  if (!par) return;
  par->Value(lmin);
  
  par = fGeoMapper->GetParameter("EcalTBLmax12");
  if (!par) return;
  par->Value(lmax);
  
  par = fGeoMapper->GetParameter("EcalTBHmin12");
  if (!par) return;
  par->Value(hmin);
  
  par = fGeoMapper->GetParameter("EcalTBSize12");
  if (!par) return;
  par->Value(hmax);
  
  volname = fGeoMapper->GetVolName(0,1);
  
  fLargeModule[1].volume_ = BuildModule(volname, "EcalTBBaseMaterial",
                                         0.5*mlen12, hmin, hmax, 0.5*lmin, 0.5*lmax);
  
  if (!fLargeModule[1].volume_) {
      cout << "-E- <SpdEcalTB::ConstructGeometry_1> "
           << "Volume " << volname << " cannot be created " << endl;
      return;
  }
  
  fLargeModule[1].capacity_ = fLargeModule[1].volume_->Capacity();
  
  fLargeModule[1].volume_->SetFillColor(kCyan+3);
  fLargeModule[1].volume_->SetLineColor(kCyan+3);
  fLargeModule[1].volume_->SetTransparency(30);
  
  /*-----------------------------------------------------------*/
 
  Double_t angle = ((SpdEcalTBGeoMapper*)fGeoMapper)->GetSecAngle();
  
  TGeoTranslation trans; 
  TGeoRotation rot;
  
  rot.RotateZ(-90.); 
  
  for (int i(1); i<=nsectors; i++) {
      
       trans.SetTranslation(0, 0, 0); 
       fMasterVolume->AddNode(fLargeModule[0].volume_,i,new TGeoCombiTrans(trans,rot));
       fLargeModule[0].counts_++;
       
       trans.SetTranslation(0, 0, 0.5*(mlen11+mlen12)); 
       fMasterVolume->AddNode(fLargeModule[1].volume_,i,new TGeoCombiTrans(trans,rot));
       fLargeModule[1].counts_++;
       
       trans.SetTranslation(0, 0,-0.5*(mlen11+mlen12)); 
       fMasterVolume->AddNode(fLargeModule[1].volume_,i+nsectors,new TGeoCombiTrans(trans,rot));
       fLargeModule[1].counts_++;
     
       rot.RotateZ(angle); 
  }
}

//_____________________________________________________________________________________
void SpdEcalTB::ConstructGeometry_2()
{  
  SpdParameter* par;
  
  Int_t nsectors = ((SpdEcalTBGeoMapper*)fGeoMapper)->GetNSectors();
  
  Double_t mlen, hmin, hmax, lmin, lmax; 
  
  par = fGeoMapper->GetParameter("EcalTBLen2");
  if (!par) return;
  par->Value(mlen);
  
  par = fGeoMapper->GetParameter("EcalTBLmin2");
  if (!par) return;
  par->Value(lmin);
  
  par = fGeoMapper->GetParameter("EcalTBLmax2");
  if (!par) return;
  par->Value(lmax);
  
  par = fGeoMapper->GetParameter("EcalTBHmin2");
  if (!par) return;
  par->Value(hmin);
  
  par = fGeoMapper->GetParameter("EcalTBSize2");
  if (!par) return;
  par->Value(hmax);
  
  TString volname = fGeoMapper->GetVolName(0,0);
  
  fLargeModule[0].volume_ = BuildModule(volname, "EcalTBBaseMaterial",
                                        0.5*mlen, hmin, hmax, 0.5*lmin, 0.5*lmax);
  
  if (!fLargeModule[0].volume_) {
      cout << "-E- <SpdEcalTB::ConstructGeometry_2> "
           << "Volume " << volname << " cannot be created " << endl;
      return;
  }
    
  fLargeModule[0].capacity_ = fLargeModule[0].volume_->Capacity();
  
  fLargeModule[0].volume_->SetFillColor(kCyan);
  fLargeModule[0].volume_->SetLineColor(kCyan);
  fLargeModule[0].volume_->SetTransparency(30);  
  
  /*-----------------------------------------------------------*/
  
  Double_t angle = ((SpdEcalTBGeoMapper*)fGeoMapper)->GetSecAngle();
  
  TGeoTranslation trans(0,0,0); 
  TGeoRotation rot;
  
  for (int i(1); i<=nsectors; i++) {
      
       fMasterVolume->AddNode(fLargeModule[0].volume_,i,new TGeoCombiTrans(trans,rot));   
       fLargeModule[0].counts_++;
       
       rot.RotateZ(angle); 
  }
}

//________________________________________________________________________________
TGeoVolume* SpdEcalTB::BuildModule(const char* name, const char* mat, Double_t length, 
                                   Double_t hmin, Double_t hmax, Double_t lmin, Double_t lmax)
{
  //cout << "-I- <SpdEcalTB::BuildModule> " << length << "/" 
  //     << hmin << "/" << hmax  << "/" <<  lmin << "/" << lmax << endl;
  
  Double_t v[16];
  
  v[0] =  lmin; v[1] = hmin;
  v[2] = -lmin; v[3] = hmin;
  v[4] = -lmax; v[5] = hmax;
  v[6] =  lmax; v[7] = hmax;
  
  v[8]  =  lmin; v[9]  = hmin;
  v[10] = -lmin; v[11] = hmin;
  v[12] = -lmax; v[13] = hmax;
  v[14] =  lmax; v[15] = hmax;

  SpdParameter* par = fGeoMapper->GetParameter(mat);
  
  if (!par) {
      cout << "-E- <SpdEcalTB::BuildModule> "
           << "Unknown material (1): " << mat << endl;
      return 0;
  }
  
  mat = par->Value();
 
  TString mname(mat);
  TGeoMedium* material = FindMedium(mname,"");
  
  if (!material) {
      cout << "-E- <SpdEcalTB::BuildModule> "
            << "Unknown material (2): " << mname << endl;
      return 0;
  }
  
  TGeoVolume* vol = gGeoManager->MakeArb8(name,material,length,v);
  AddSensitiveVolume(vol);
  
  return vol;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdEcalTB::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper(fDetId);
  
  // create default mapper
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdEcalTBGeoMapper");
     
  return fGeoMapper;
}

//_____________________________________________________________________________
Bool_t SpdEcalTB::LoadParsFrom(SpdParSet* params) 
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
TString SpdEcalTB::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}

//_____________________________________________________________________________________
TClonesArray* SpdEcalTB::GetCollection(Int_t iColl) const 
{ 
   return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdEcalTB::GetCapacity() const 
{ 
   Double_t capacity = 0; 
   for (Int_t i(0); i<2; i++) capacity += GetCapacity(i)*fLargeModule[i].counts_;
   return capacity;
}
   
//_____________________________________________________________________________________
Double_t SpdEcalTB::GetMass() const 
{ 
   Double_t mass = 0;
   for (Int_t i(0); i<2; i++) mass += GetMass(i)*fLargeModule[i].counts_;
   return mass; 
}

//_____________________________________________________________________________
Double_t SpdEcalTB::GetDensity() const 
{ 
   return SpdDetector::GetDensity();
}

//_____________________________________________________________________________________
Double_t SpdEcalTB::GetCapacity(Int_t nmodule) const // private
{ 
   return (fLargeModule[nmodule].volume_) ? fLargeModule[nmodule].capacity_ : 0;
}
   
//_____________________________________________________________________________________
Double_t SpdEcalTB::GetMass(Int_t nmodule) const // private
{ 
   return (fLargeModule[nmodule].volume_) ? GetCapacity(nmodule)*GetDensity(nmodule) : 0;
}

//_____________________________________________________________________________________
Double_t SpdEcalTB::GetDensity(Int_t nmodule) const // private
{ 
   TGeoVolume* vol = fLargeModule[nmodule].volume_;
   return (vol) ? vol->GetMaterial()->GetDensity() : 0;
}

//_____________________________________________________________________________________
TString SpdEcalTB::GetMaterialName(Int_t nmodule) const // private
{ 
   TGeoVolume* vol = fLargeModule[nmodule].volume_;
   return (vol) ? vol->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdEcalTB::Print(Option_t*) const
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
   
   for (int i(0); i<2; i++) {
    
       if (!fLargeModule[i].volume_) continue;
       
       printf("%6d %4d %15s %14.6f %14.6f %14.6f \n",
               i, fLargeModule[i].counts_, GetMaterialName(i).Data(),
               GetDensity(i), GetCapacity(i)*1e-6, GetMass(i)*1e-3);
   }
   
   cout << divider.Data() << "\n";
   
   printf("%6s %35s %14.3e %14.3e \n","TOTAL:","",GetCapacity()*1e-6,GetMass()*1e-3);
    
   cout << divider.Data() << "\n";
   
   cout << "\n";
}

