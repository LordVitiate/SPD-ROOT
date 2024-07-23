// $Id$
// Author: artur   2021/11/26

//_____________________________________________________________________________
//
// SpdAeg
//_____________________________________________________________________________

#include "SpdAeg.h"

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

#include "SpdAegPoint.h"
#include "SpdAegGeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdAeg)

//_____________________________________________________________________________________
SpdAeg::SpdAeg(): 

  SpdDetector("Aerogel (qsl)", kSpdAeg, kTRUE),
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
  SetParametersType("AegParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdAegPoint"; 
  
  fSaveEmptyHits = true; //FIXME!FIXME!FIXME!FIXME!FIXME
}

//_____________________________________________________________________________________
SpdAeg::SpdAeg(const char* name, Bool_t active):

  SpdDetector(name, kSpdAeg, active),
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
  SetParametersType("AegParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdAegPoint";
  
  fSaveEmptyHits = true; //FIXME!FIXME!FIXME!FIXME!FIXME
}

//_____________________________________________________________________________________
SpdAeg::~SpdAeg()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdAeg::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdAeg::Initialize **********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();  

}

//_____________________________________________________________________________________
void SpdAeg::Reset()
{
  fPointCollection->Delete();
  //fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdAeg::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdAeg", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdAeg::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdAeg::ProcessHits> " << gMC->CurrentVolPath() << endl;
  
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

  // Create SpdAegPoint at exit of active volume
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
    
    fVolumeID   = kSpdAeg;
    fVolumePath = gMC->CurrentVolPath();
    
         if (fVolumePath.Contains("zplus"))  fModuleID =  1;
    else if (fVolumePath.Contains("zminus")) fModuleID = -1;
    else fModuleID = -10; //???
    
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
        
    //cout << "path = " << fVolumePath << endl;
   
    AddHit();
    
    //cout << "<SpdAeg::AddHit> " << fPointCollection->GetEntriesFast() << endl;
    
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdAeg);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdAeg::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdAegPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  new(clref[size]) SpdAegPoint(fTrackID,fVolumeID,fModuleID,
                               TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                               TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                               TVector3(fPosOut.X(),fPosOut.Y(),fPosOut.Z()),
                               TVector3(fMomOut.Px(),fMomOut.Py(),fMomOut.Pz()),
                               fTime, fTimeOut,
                               fLength, fSegmentLength, fELoss);
}

//_____________________________________________________________________________________
void SpdAeg::EndOfEvent()
{
   Reset();
}

//_____________________________________________________________________________________
void SpdAeg::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdAeg::ConstructGeometry()
{   
  // Create detector geometry
  
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
  if (!fMasterVolume) {  
      cout << "-E- <SpdAeg::ConstructGeometry> No MASTER volume " << endl;
      return;
  }
  
  if (!GetMapper()) return;
  
  if (!fGeoMapper->InitGeometry()) return;
      
  //fGeoMapper->Print("");
  
  Int_t geo_type = fGeoMapper->GetGeoType();

  if (geo_type == 1) ConstructDetector();
  
  fGeoMapper->LockGeometry();
      
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdAeg::ConstructGeometry ******************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//_____________________________________________________________________________________
void SpdAeg::ConstructDetector()
{  
  SpdParameter* par;
  
  SpdAegGeoMapper* mapper = dynamic_cast<SpdAegGeoMapper*>(fGeoMapper);
  if (!mapper) return;
  
  TString mat = mapper->GetBaseMaterial();
  TGeoMedium* material = FindMedium(mat,"air");
  

  Double_t a;        // Mass of a mole in g/mole
  Double_t z;        // Atomic number
  Double_t density;  // Material density in g/cm3

 
  TGeoElement* elO  = new TGeoElement("Oxygen"  , "O", z= 8., a= 16.00);
  TGeoElement* elSi = new TGeoElement("Silicon", "Si", z= 14., a= 28.09);

  // ----------
 TGeoMixture* matSiO2
    = new TGeoMixture("quartz", 2, density=2.200);
  matSiO2->AddElement(elSi, 1);
  matSiO2->AddElement(elO, 2);

  TGeoElement* elH  = new TGeoElement("Hydrogen", "H", z= 1,  a= 1.01);

 TGeoMixture* matH2O
    = new TGeoMixture("Water", 2, density=1.000);
  matH2O->AddElement(elH, 2);
  matH2O->AddElement(elO, 1);

 TGeoElement* elC  = new TGeoElement("Carbon"  , "C", z= 6., a= 12.01);

  TGeoMixture* matAerog
    = new TGeoMixture("Aerogel", 3, density=0.200);
  matAerog->AddElement(matSiO2, 0.625);
  matAerog->AddElement(matH2O,  0.374);
  matAerog->AddElement(elC,     0.001);


  TGeoMedium* medAerog = new TGeoMedium("medAerog", 0, matAerog);



 // -----------
  if (!material) {
      cout << "-E- <SpdAeg::BuildModule> Unknown material: " << mat << endl;
      return;
  }
  


  Double_t mthick  = mapper->GetModuleThickness();
  Double_t msize   = mapper->GetModuleSize();
  Double_t mwidth  = mapper->GetModuleWidth();
  
  fModule1 = gGeoManager->MakeTube(mapper->AddPrefix("zplus"), medAerog , msize-mwidth,msize,0.5*mthick);
  
  if (!fModule1) {
      cout << "-E- <SpdAeg::ConstructGeometry> Module (1) cannot be created " << endl;
      return;
  }
  
  fModule2 = gGeoManager->MakeTube(mapper->AddPrefix("zminus"), medAerog, msize-mwidth,msize,0.5*mthick);
  
  if (!fModule2) {
      cout << "-E- <SpdAeg::ConstructGeometry> Module (2) cannot be created " << endl;
      return;
  }
    
  fModule1->SetFillColor(kGreen);
  fModule1->SetLineColor(kGreen);
  fModule1->SetTransparency(30);  
  
  fModule2->SetFillColor(kGreen);
  fModule2->SetLineColor(kGreen);
  fModule2->SetTransparency(30);  
  
  AddSensitiveVolume(fModule1); //ATTENTION FIXME ATTENTION
  AddSensitiveVolume(fModule2); //ATTENTION FIXME ATTENTION
  
  Double_t shift = mapper->GetMinDistance() + 0.5*mthick;
  
  fMasterVolume->AddNode(fModule1,1,new TGeoTranslation(0,0, shift));
  fMasterVolume->AddNode(fModule2,1,new TGeoTranslation(0,0,-shift));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdAeg::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper("SpdAegGeoMapper");
  
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdAegGeoMapper");

  return fGeoMapper; 
}

//_____________________________________________________________________________
Bool_t SpdAeg::LoadParsFrom(SpdParSet* params) 
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
TString SpdAeg::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}


//_____________________________________________________________________________________
TClonesArray* SpdAeg::GetCollection(Int_t iColl) const 
{ 
  return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdAeg::GetCapacity() const 
{ 
  return (fModule1) ? fModule1->GetShape()->Capacity() : 0;
}
   
//_____________________________________________________________________________________
Double_t SpdAeg::GetMass() const 
{ 
  return (fModule1) ? GetCapacity()*GetDensity() : 0;
}

//_____________________________________________________________________________________
Double_t SpdAeg::GetDensity() const 
{ 
  return (fModule1) ? fModule1->GetMaterial()->GetDensity() : 0;
}

//_____________________________________________________________________________________
TString SpdAeg::GetMaterialName() const // private
{ 
  return (fModule1) ? fModule1->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdAeg::Print(Option_t*) const
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


