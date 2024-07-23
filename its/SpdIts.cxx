// $Id$
// Author: artur   2019/20/02

//_____________________________________________________________________________
//
// SpdIts
//_____________________________________________________________________________

#include "SpdIts.h"

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

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"
#include "SpdItsPoint.h"
#include "SpdGeoVVolume.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdIts)

//_____________________________________________________________________________________
SpdIts::SpdIts(): 

  SpdDetector("Inner tracker system", kSpdIts, kTRUE),
  fTrackID(-1),
  fVolumeID1(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fPosOut(),
  fMomOut(),
  fLength(-1.),
  fELoss(-1),
  fVVolume(0),
  fPointCollection(0)
{
  SetParametersType("ItsParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdItsPoint"; 
}

//_____________________________________________________________________________________
SpdIts::SpdIts(const char* name, Bool_t active):

  SpdDetector(name, kSpdIts, active),
  fTrackID(-1),
  fVolumeID1(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fPosOut(),
  fMomOut(),
  fLength(-1.),
  fELoss(-1),
  fVVolume(0),
  fPointCollection(0)
{
  SetParametersType("ItsParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdItsPoint"; 
}

//_____________________________________________________________________________________
SpdIts::~SpdIts()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdIts::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdIts::Initialize **********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();  
}

//_____________________________________________________________________________________
void SpdIts::Reset()
{
  if (fPointCollection) fPointCollection->Delete();
  //if (fPointCollection) fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdIts::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdIts", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdIts::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdIts::ProcessHits> " << endl;
    
  // Set parameters at entrance of volume
  
  if (gMC->IsTrackEntering()) {
    
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.e9;  // ns -> s
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
      
      fVVolume = 0;
      if (fGeoMapper) {
          fVVolume = (SpdGeoVVolumeBox2D*)fGeoMapper->GetVVolume(gMC->CurrentVolName());
          if (fVVolume) {
              gMC->SetMaxStep(fVVolume->GetMaxStep());
              //fVVolume->PrintVolume();
          }
      }
      
      fVid.clear();
      fVed.clear();
  }
  
  if (fVVolume) 
  {
      static Double_t xm[3], xd[3];
      gMC->TrackPosition(xm[0],xm[1],xm[2]);
      gMC->Gmtod(xm,xd,1);
      Int_t ii = fVVolume->GetCellId(xd[0],xd[1],xd[2]);
      
//       Int_t i1, i2;
//       fVVolume->CellId(ii,i1,i2);
//   
//       cout << " Track: " << gMC->GetStack()->GetCurrentTrackNumber() 
//            << " volume: " << gMC->CurrentVolName() << " de = " << gMC->Edep()*1e9 
//            << "  de(sum) = " << fELoss*1e9  << " step = " << gMC->TrackStep()*1e4 << " mkm " 
//            << " (x,y,z) = " << xd[0] << " " << xd[1] << " " << xd[2] 
//            << " id = " << ii << " (" << i1 << " " << i2 << ") "<< endl;
//            
//       Double_t xc[3];
//       fVVolume->GetCellPos(ii,xc[0],xc[1],xc[2]);     
//       
//       cout << "\tcell position: " << xc[0] << " " << xc[1] << " " << xc[2] << endl;
      
      Int_t ndet = fVid.size();
      if (ndet > 0) {
          if (fVid[ndet-1] == ii) {
              fVed[ndet-1] += gMC->Edep();
          }
          else {       
              fVid.push_back(ii);
              fVed.push_back(gMC->Edep());         
          }
      }
      else {      
          fVid.push_back(ii);
          fVed.push_back(gMC->Edep());        
      }  
  }
       
  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();
  
  // Create SpdItsPoint at exit of active volume
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) 
  {
    
    if (!fSaveEmptyHits && fELoss == 0.) { return kFALSE; }
    
    fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
    
    fVolumeID1 = GetNodeId(gMC->CurrentVolPath());  
    
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
    
    AddHit();
    
    //cout << "<SpdIts::AddHit> " << fPointCollection->GetEntriesFast() << endl;
    
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdIts);
    
    //----------------------------------------------------------------------------------
    
//     if (!(fELoss > 0)) return kTRUE;
//       
//     TString path = gMC->CurrentVolPath();
//     
//     Double_t dL = (gMC->TrackLength()-fLength)*1e4;
//     TGeoMedium* cmed = gGeoManager->GetMedium(gMC->CurrentMedium()); 
//     
//     Double_t edsum = 0;     
//     for (Int_t i(0); i < fVid.size(); i++) {
//         Int_t i1, i2;
//         fVVolume->CellId(fVid[i],i1,i2);
//         cout << i <<  " id = " << fVid[i] <<  " ed = " << fVed[i]*1e9  
//              << " (" << i1 << " " << i2 << ") " << endl;  
//              edsum += fVed[i];
//     }
//     cout << "edsum = " << edsum*1e9 << " eV " << endl;
//     
//     SpdGeoVVolume* vvol = fGeoMapper->GetVVolume(fVolumeID1);
//    
//     cout << "path(1) = " << gMC->CurrentVolPath() 
//          << "\npath(2) = "<< fGeoMapper->GetGeoTable()->GetNodeGeoPath(fVolumeID1)
//          << "\nvolume: " <<  SpdGeopathParser::GetVolumeName(path) 
//          << " = " << vvol->GetTitle() << " vvol: " << vvol->GetName() 
//          << endl;
//     cout << "ID: "<< fDetId << "/" << fVolumeID1 << endl;
//     cout << "Total eloss = " << fELoss*1e9 << " eV " 
//          << " dL = " << dL << " mkm " 
//          << " medium = " << cmed->GetName()
//          << endl;
//     cout << "-----------------------------------------------------------------" << "\n\n";
     
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdIts::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdItsPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  if (fPointCollection) 
  {
      TClonesArray& clref = *fPointCollection;
      Int_t size = clref.GetEntriesFast();
  
      SpdItsPoint* point = new(clref[size]) SpdItsPoint(fTrackID,fDetId,
                               TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                               TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                               fTime, fLength, fELoss); 
      
      point->SetNodeId(fVolumeID1);
   
      point->SetPositionOut(fPosOut.X(),fPosOut.Y(),fPosOut.Z());
      point->SetMomentumOut(fMomOut.Px(),fMomOut.Py(),fMomOut.Pz());
      point->SetVData(fVid,fVed);
      
      //if (!(fELoss > 0)) return;
      //point->Print("");
  }
}

//_____________________________________________________________________________________
void SpdIts::EndOfEvent()
{
   Reset();
}

//_____________________________________________________________________________________
void SpdIts::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdIts::ConstructGeometry()
{   
  // Create detector geometry
  
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
  if (!fMasterVolume) {  
      cout << "-E- <SpdIts::ConstructGeometry> No MASTER volume " << endl;
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
  cout << "********************** SpdIts::ConstructGeometry ******************************" << endl;
  if (fGeoMapper) cout << "********************** GEOMETRY TYPE (MAPPER): " 
                       << fGeoMapper->GetGeoType() << " ******************************" << endl;
  cout << "*******************************************************************************\n" << endl;  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdIts::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper(fDetId);
  
  // create default mapper
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdItsGeoMapperX");
     
  return fGeoMapper; 
}

//_____________________________________________________________________________________
SpdGeoBuilder* SpdIts::GetBuilder() 
{
  if (fGeoBuilder) return fGeoBuilder;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoBuilder = factory->SearchForBuilder(fDetId);
  
  // create default mapper
  if (!fGeoBuilder) fGeoBuilder = factory->Builder("SpdItsGeoBuilder");
      
  return fGeoBuilder;
}

//_____________________________________________________________________________
Bool_t SpdIts::LoadParsFrom(SpdParSet* params) 
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
TString SpdIts::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}


//_____________________________________________________________________________________
TClonesArray* SpdIts::GetCollection(Int_t iColl) const 
{ 
  return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "SpdItsGeoMapperX.h"

//_____________________________________________________________________________________
Int_t SpdIts::GetNLayers() const
{
   if (!fGeoMapper) return 0;
   SpdItsGeoMapperX* mapper = dynamic_cast<SpdItsGeoMapperX*>(fGeoMapper); 
   return mapper ? mapper->GetNLayers() : 0;
}

//_____________________________________________________________________________________
Double_t SpdIts::GetCapacity() const 
{  
   Double_t capacity = 0; 
   for (Int_t i(0); i<GetNLayers(); i++) capacity += GetCapacity(i);
   return capacity;
}
   
//_____________________________________________________________________________________
Double_t SpdIts::GetMass() const 
{ 
   Double_t mass = 0;
   for (Int_t i(0); i<GetNLayers(); i++) mass += GetMass(i);
   return mass; 
}

//_____________________________________________________________________________
Double_t SpdIts::GetDensity() const 
{ 
  return SpdDetector::GetDensity();
}

//_____________________________________________________________________________________
Double_t SpdIts::GetCapacity(Int_t nlayer) const // private
{ 
  return 0;
}
   
//_____________________________________________________________________________________
Double_t SpdIts::GetMass(Int_t nlayer) const // private
{ 
  return 0;
}

//_____________________________________________________________________________________
Double_t SpdIts::GetDensity(Int_t nlayer) const // private
{ 
  return 0;
}

//_____________________________________________________________________________
void SpdIts::Print(Option_t*) const
{
   SpdDetector::Print("");
   
   if (!fGeoMapper) return;
   
   TString difVider('-',150);
   
   cout << "\n";

   fGeoMapper->Print("");
   
   return; // FIXME

   cout << "\n";
   cout <<"\tCapacity (total):    " << GetCapacity()*1e-6 << " [m^3] " << endl;
   cout <<"\tMass (total):        " << GetMass()*1e-3 << " [kg] " << endl;
   cout <<"\tDensity (averaged):  " << GetDensity() << " [g/cm^3] " << endl;
   cout << "\n";
   
   cout << "\n" << difVider.Data() << "\n";
   
   printf("%6s %14s %14s %14s \n\n",
          "Layer","Dens [g/cm^3]","Volume [m^3]","Mass [kg]"
         );
   
   for (int i(0); i<GetNLayers(); i++) {
    
       printf("%6d %14.6f %14.6f %14.6f \n",
               i, GetDensity(i), GetCapacity(i)*1e-6, GetMass(i)*1e-3);
   }
   
   cout << difVider.Data() << "\n";
   
   printf("%6s %35s %14.3e %14.3e \n","TOTAL:","",GetCapacity()*1e-6,GetMass()*1e-3);
    
   cout << difVider.Data() << "\n";
   
   cout << "\n";
}


