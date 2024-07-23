// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdTsTB
//_____________________________________________________________________________

#include "SpdTsTB.h"

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
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TMath.h"

#include "SpdTsTBPoint.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <map>
#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdTsTB)

//_____________________________________________________________________________________
SpdTsTB::SpdTsTB(): 

  SpdDetector("TS barrel (tor)", kSpdTsTB, kTRUE),
  fTrackID(-1),
  fVolumeID1(kSpdTsTB),fVolumeID2(-1),
  fPosIn(),fMomIn(),fTimeIn(-1.),
  fPosOut(),fMomOut(),fTimeOut(-1.),
  fLength(-1.),
  fSegmentLength(-1.),
  fELoss(-1),
  fPointCollection(0)
{
  SetParametersType("TsTBParSet");
  
  fNDataOut = 1;
  fOutDataPointObject = "SpdTsTBPoint"; 
}

//_____________________________________________________________________________________
SpdTsTB::SpdTsTB(const char* name, Bool_t active):

  SpdDetector(name, kSpdTsTB, active),
  fTrackID(-1),
  fVolumeID1(kSpdTsTB),fVolumeID2(-1),
  fPosIn(),fMomIn(),fTimeIn(-1.),
  fPosOut(),fMomOut(),fTimeOut(-1.),
  fLength(-1.),
  fSegmentLength(-1.),
  fELoss(-1),
  fPointCollection(0)
{
  SetParametersType("TsTBParSet");
  
  fNDataOut = 1;
  fOutDataPointObject = "SpdTsTBPoint"; 
}

//_____________________________________________________________________________________
SpdTsTB::~SpdTsTB()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdTsTB::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdTsTB::Initialize *********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
 
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
  
// SpdParSet* pars = GetPars();
// if (pars) pars->printParams(); 
}

//_____________________________________________________________________________________
void SpdTsTB::Reset()
{
  //Int_t nt = fPointCollection->GetEntriesFast();
  if (fPointCollection) fPointCollection->Delete();
  //if (fPointCollection) fPointCollection->Clear();
  //cout << "<SpdTsTB::Reset> " << nt << ", " << fPointCollection->GetEntriesFast() << endl;
}

//_____________________________________________________________________________________
void SpdTsTB::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdTsTB", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdTsTB::ProcessHits(FairVolume* vol)
{ 
  //cout << "<SpdTsTB::ProcessHits> " << endl;
  
  //Set parameters at entrance of volume. Reset ELoss.
  if (gMC->IsTrackEntering()) {
      fELoss  = 0.;
      fTimeIn = gMC->TrackTime() * 1.e9;  // ns -> s
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPosIn);
      gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create Point at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fSegmentLength = gMC->TrackLength() - fLength;
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    fTimeOut = gMC->TrackTime() * 1.e9;  // ns -> s
    
    fVolumeID2 = GetNodeId(gMC->CurrentVolPath());    
    
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);	 
     
    AddHit();  
    
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdTsTB);

//     static Int_t nerrors = 0;
///    TString path = gMC->CurrentVolPath();
//     
//     if (path != geotable->GetNodeGeoPath(fVolumeID2) && path != "cave_1") nerrors++;
//      
//     cout << "<SpdTsTB::ProcessHits>"
//          << " errors = " << nerrors 
//          << " path = " << path 
//          << " tid = " << fVolumeID2  
//          //<< " path(1) = " << geotable->GetNodeGeoPath(fVolumeID2) 
//          << endl;
   
    //cout << "<SpdTsTB::ProcessHits> " << fPointCollection->GetEntriesFast() << endl;
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdTsTB::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdTsTBPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
     
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  SpdTsTBPoint* point = new(clref[size]) SpdTsTBPoint(fTrackID,fVolumeID1,fVolumeID2,
                        TVector3(fPosIn.X(),fPosIn.Y(),fPosIn.Z()),
                        TVector3(fMomIn.Px(),fMomIn.Py(),fMomIn.Pz()),
                        fTimeIn,      
                        TVector3(fPosOut.X(),fPosOut.Y(),fPosOut.Z()),
                        TVector3(fMomOut.Px(),fMomOut.Py(),fMomOut.Pz()),			      
                        fTimeOut, 
                        fLength, fELoss); 
  
  point->SetSegmentLength(fSegmentLength);
  point->SetEventID(gMC->CurrentEvent());
  
  //point->Print("");
}

//_____________________________________________________________________________________
void SpdTsTB::EndOfEvent()
{
  Reset();
}

//_____________________________________________________________________________________
void SpdTsTB::FinishRun() 
{
  //SpdDetector::FinishRun();
  FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdTsTB::ConstructGeometry()
{   
  // Create detector geometry
  
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
 
  if (!fMasterVolume) {  
      cout << "-E- <SpdTsTB::ConstructGeometry> No MASTER volume " << endl;
      return;
  }
   
  if (GetMapper()) 
  {
      if (!fGeoMapper->InitGeometry()) return;   
      fGeoMapper->LockGeometry(); 
      //fGeoMapper->Print("");
  }
  
  RegisterMediums();
  
  if (GetBuilder()) 
  {
     fGeoBuilder->SetMasterVolume(fMasterVolume);
     fGeoBuilder->SetMapper(fGeoMapper);
     if (!fGeoBuilder->ConstructGeometry()) return;
     fGeoBuilder->LockGeometry();
     //fGeoBuilder->Print("");
  }
 
  RegisterActiveVolumes(); 
    
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdTsTB::ConstructGeometry *****************************" << endl;
  if (fGeoMapper) cout << "********************** GEOMETRY TYPE (MAPPER): " 
                       << fGeoMapper->GetGeoType() << " ******************************" << endl;
  cout << "*******************************************************************************\n" << endl;  
}
    
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdTsTB::GetMapper() 
{ 
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper(fDetId);
  
  // create default mapper
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdTsTBGeoMapper");
     
  return fGeoMapper;
}

//_____________________________________________________________________________________
SpdGeoBuilder* SpdTsTB::GetBuilder() 
{
  if (fGeoBuilder) return fGeoBuilder;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoBuilder = factory->SearchForBuilder(fDetId);
  
  // create default mapper
  if (!fGeoBuilder) fGeoBuilder = factory->Builder("SpdTsTBGeoBuilder");
      
  return fGeoBuilder;
}

//_____________________________________________________________________________
Bool_t SpdTsTB::LoadParsFrom(SpdParSet* params) 
{
  if (!params) return kFALSE;
 
  if (!SpdDetector::LoadParsFrom(params)) return kFALSE;
  
  TString mapper;
  params->GetParameter("Mapper",mapper); 
  fGeoMapper = SpdGeoFactory::Instance()->Mapper(mapper);  
  if (fGeoMapper) fGeoMapper->LoadParametersFrom(params);
  
  TString builder;
  params->GetParameter("Builder",builder); 
  fGeoBuilder = SpdGeoFactory::Instance()->Builder(builder);
  if (fGeoBuilder) fGeoBuilder->LoadParametersFrom(params);
 
  fOutDataPointObject = "unknown";

  if (fNDataOut < 1) return kTRUE;

  params->GetParameter("Detector/NOutData_1",fOutDataPointObject);
  
  return kTRUE;
}

//_____________________________________________________________________________________
TString SpdTsTB::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}

//_____________________________________________________________________________________
TClonesArray* SpdTsTB::GetCollection(Int_t iColl) const 
{ 
  return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdTsTB::GetCapacity() const 
{ 
  return 0;
}
   
//_____________________________________________________________________________________
Double_t SpdTsTB::GetMass() const 
{ 
  return 0; 
}

//_____________________________________________________________________________
Double_t SpdTsTB::GetDensity() const 
{ 
  return SpdDetector::GetDensity();
}

//_____________________________________________________________________________________
Double_t SpdTsTB::GetCapacity(Int_t nmodule) const // private
{ 
  return 0;
}
   
//_____________________________________________________________________________________
Double_t SpdTsTB::GetMass(Int_t nmodule) const // private
{ 
  return 0;
}

//_____________________________________________________________________________________
Double_t SpdTsTB::GetDensity(Int_t nmodule) const // private
{ 
  return 0;
}

//_____________________________________________________________________________________
TString SpdTsTB::GetMaterialName(Int_t nmodule) const // private
{ 
  return "mixture";
}

//_____________________________________________________________________________
void SpdTsTB::Print(Option_t*) const
{
  SpdDetector::Print("");
  
  if (!fGeoMapper) return;
  
  TString divider('-',150);
  
  cout << "\n";

  fGeoMapper->Print("");
  
  return; // FIXME

  cout << "\n";
  cout <<"\tCapacity (total):    " << GetCapacity()*1e-6 << " [m^3] " << endl;
  cout <<"\tMass (total):        " << GetMass()*1e-3 << " [kg] " << endl;
  cout <<"\tDensity (averaged):  " << GetDensity() << " [g/cm^3] " << endl;
  cout << "\n";
  
  cout << "\n" << divider.Data() << "\n";
  
  printf("%6s %4s %15s %14s %14s %14s \n\n",
         "Type","N","Material","Dens [g/cm^3]","Volume [m^3]","Mass [kg]"
        );
  
  for (int i(0); i<3; i++) {
  
      printf("%6d %4d %15s %14.6f %14.6f %14.6f \n",
              i, 8, GetMaterialName(i).Data(),
              GetDensity(i), GetCapacity(i)*1e-6, GetMass(i)*1e-3);
  }
  
  cout << divider.Data() << "\n";
  
  printf("%6s %35s %14.3e %14.3e \n","TOTAL:","",GetCapacity()*1e-6,GetMass()*1e-3);
   
  cout << divider.Data() << "\n";
  
  cout << "\n";
}


