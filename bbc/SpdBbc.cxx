// $Id$
// Author: artur   2021/11/26

//_____________________________________________________________________________
//
// SpdBbc
//_____________________________________________________________________________

#include "SpdBbc.h"

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

#include "SpdBbcPoint.h"
#include "SpdBbcGeoMapper.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"
#include "SpdMCEventHeader.h"
#include "SpdCommonGeoMapper.h"
#include "SpdGeoFactory.h"

#include <iostream>

using namespace std;
using namespace TMath;

ClassImp(SpdBbc)

//_____________________________________________________________________________________
SpdBbc::SpdBbc(): 

  SpdDetector("B-B counter (qsl)", kSpdBbc, kTRUE),
  fTrackID(-1),
  fVolumeID(-1),
  fLocalModuleID(0),
  fRingID(0),
  fSectorID(0),
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
  SetParametersType("BbcParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdBbcPoint"; 
  
  fSaveEmptyHits = true; //FIXME!FIXME!FIXME!FIXME!FIXME
}

//_____________________________________________________________________________________
SpdBbc::SpdBbc(const char* name, Bool_t active):

  SpdDetector(name, kSpdBbc, active),
  fTrackID(-1),
  fVolumeID(-1),
  fLocalModuleID(0),
  fRingID(),
  fSectorID(),
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
  SetParametersType("BbcParSet");
   
  fNDataOut = 1;
  fOutDataPointObject = "SpdBbcPoint";
  
  fSaveEmptyHits = true; //FIXME!FIXME!FIXME!FIXME!FIXME
}

//_____________________________________________________________________________________
SpdBbc::~SpdBbc()
{
  if (fPointCollection) {
      fPointCollection->Delete();
      delete fPointCollection;
      fPointCollection = 0;
  }
}

//_____________________________________________________________________________________
void SpdBbc::Initialize()
{
  cout << "\n*******************************************************************************" << endl;
  cout << "************************* SpdBbc::Initialize **********************************" << endl;
  cout << "*******************************************************************************\n" << endl;
  
  // data collections
  fPointCollection = new TClonesArray(fOutDataPointObject);
  
  // Initialize module and fill parameters
  SpdDetector::Initialize(); 
 
  // SpdParSet* pars = GetParameters();
  // if (pars) pars->printParams();  

}

//_____________________________________________________________________________________
void SpdBbc::Reset()
{
  fPointCollection->Delete();
  //fPointCollection->Clear();
}

//_____________________________________________________________________________________
void SpdBbc::Register()
{
  if (fGeoMapper && fGeoMapper->GetGeoType() < 1) return;
    
  FairRootManager::Instance()->Register(fOutDataPointObject,"SpdBbc", 
                                        fPointCollection, kTRUE);
}

//_____________________________________________________________________________________
Bool_t SpdBbc::ProcessHits(FairVolume* vol)
{
  //cout << "<SpdBbc::ProcessHits> " << gMC->CurrentVolPath() << endl;
  
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

  // Create SpdBbcPoint at exit of active volume
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
    
    fNodePath = gMC->CurrentVolPath();
    SpdGeopathParser parser;
    parser.ParsePath(fNodePath);
    
    //cave: 1, Bbcmodule: 2, BBcRing:3, Bbcsector:4
    fVolumeID =  kSpdBbc;
    fLocalModuleID = parser.Num(2, true);
    fRingID   = parser.Num(3, true);
    if (fRingID==1) fSectorID =0;
    else fSectorID = parser.Num(4, true);


    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
        
    //cout << "path = " << fVolumePath << endl;
   
    AddHit();
    
    //cout << "<SpdBbc::AddHit> " << fPointCollection->GetEntriesFast() << endl;
    
    SpdStack* stack = (SpdStack*)gMC->GetStack();
    stack->AddPoint(kSpdBbc);
  } 
   
  return kTRUE;
}

//_____________________________________________________________________________________
void SpdBbc::AddHit() 
{
  static Bool_t addhit = (fOutDataPointObject == "SpdBbcPoint") ? kTRUE : kFALSE;
  
  if (!addhit) return;
  
  TClonesArray& clref = *fPointCollection;
  Int_t size = clref.GetEntriesFast();
  
  new(clref[size]) SpdBbcPoint(fTrackID,fVolumeID,fLocalModuleID,fRingID,fSectorID,
                               TVector3(fPos.X(),fPos.Y(),fPos.Z()),
                               TVector3(fMom.Px(),fMom.Py(),fMom.Pz()),
                               TVector3(fPosOut.X(),fPosOut.Y(),fPosOut.Z()),
                               TVector3(fMomOut.Px(),fMomOut.Py(),fMomOut.Pz()),
                               fTime, fTimeOut,
                               fLength, fSegmentLength, fELoss);
}

//_____________________________________________________________________________________
void SpdBbc::EndOfEvent()
{
   Reset();
}

//_____________________________________________________________________________________
void SpdBbc::FinishRun() 
{
   //SpdDetector::FinishRun();
   FillNodesTableIn(GetParameters());
}

//_____________________________________________________________________________________
void SpdBbc::ConstructGeometry()
{   
  // Create detector geometry
  
  fMasterVolume = SpdCommonGeoMapper::Instance()->GetMasterVolume();
  
  if (!fMasterVolume) {  
      cout << "-E- <SpdBbc::ConstructGeometry> No MASTER volume " << endl;
      return;
  }
  
  if (!GetMapper()) return;
  
  if (!fGeoMapper->InitGeometry()) return;
      
  //fGeoMapper->Print("");
  
  Int_t geo_type = fGeoMapper->GetGeoType();

  ConstructDetector();
  
  fGeoMapper->LockGeometry();
      
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdBbc::ConstructGeometry ******************************" << endl;
  cout << "********************** GEOMETRY TYPE: " << geo_type 
       << " ***************************************" << endl;
  cout << "*******************************************************************************\n" << endl;
}

//_____________________________________________________________________________________
void SpdBbc::ConstructDetector()
{  
  SpdParameter* par;
  
  SpdBbcGeoMapper* mapper = dynamic_cast<SpdBbcGeoMapper*>(fGeoMapper);
  if (!mapper) return;
  
  TString mat = mapper->GetBaseMaterial();
  TGeoMedium* material = FindMedium(mat,"air");
  
  if (!material) {
      cout << "-E- <SpdBbc::BuildModule> Unknown material: " << mat << endl;
      return;
  }



  // Double_t mthick  = mapper->GetModuleThickness();
  // Double_t msize   = mapper->GetModuleSize();
  // Double_t mwidth  = mapper->GetModuleWidth();

  
  Double_t bbcthickness =  mapper->GetModuleThickness(); //cm
//**************
  TGeoVolume *fRing1 = new TGeoVolumeAssembly(mapper->AddPrefix("Ring"));

   TGeoVolume *fSector1 = gGeoManager->MakeTube(mapper->AddPrefix("Sector"),material,4.25,9.5,0.5*bbcthickness);

  fSector1->SetFillColor(kMagenta);
  fSector1->SetLineColor(kMagenta);
  fSector1->SetTransparency(15);

  TGeoVolume *fRing2 = new TGeoVolumeAssembly(mapper->AddPrefix("Ring"));


  TGeoVolume *fSector2  = gGeoManager->MakeTrd1(mapper->AddPrefix("Sector"), material, 3.5/2, 8.9/2 ,0.5*bbcthickness, 13.7/2 );
  fSector2->SetFillColor(kMagenta);
  fSector2->SetLineColor(kMagenta);
  fSector2->SetTransparency(15);

  TGeoVolume *fRing3 = new TGeoVolumeAssembly(mapper->AddPrefix("Ring"));

  TGeoVolume *fSector3  = gGeoManager->MakeTrd1(mapper->AddPrefix("Sector"), material, 9.3/2, 14.8/2 ,0.5*bbcthickness, 13.7/2 );
  fSector3->SetFillColor(kMagenta);
  fSector3->SetLineColor(kMagenta);
  fSector3->SetTransparency(15);

  TGeoVolume *fRing4 = new TGeoVolumeAssembly(mapper->AddPrefix("Ring"));

  TGeoVolume *fSector4  = gGeoManager->MakeTrd1(mapper->AddPrefix("Sector"), material, 15.2/2, 20.6/2 ,0.5*bbcthickness, 13.7/2 );
  fSector4->SetFillColor(kMagenta);
  fSector4->SetLineColor(kMagenta);
  fSector4->SetTransparency(15);

  TGeoVolume *fRing5 = new TGeoVolumeAssembly(mapper->AddPrefix("Ring"));

  TGeoVolume *fSector5  = gGeoManager->MakeTrd1(mapper->AddPrefix("Sector"), material,  21./2, 26.5/2 ,0.5*bbcthickness, 13.7/2);
  fSector5->SetFillColor(kMagenta);
  fSector5->SetLineColor(kMagenta);
  fSector5->SetTransparency(15);

  TGeoVolume *fRing6 = new TGeoVolumeAssembly(mapper->AddPrefix("Ring"));

  TGeoVolume *fSector6  = gGeoManager->MakeTrd1(mapper->AddPrefix("Sector"), material, 26.9/2, 32.3/2 ,0.5*bbcthickness, 13.7/2 );
  fSector6->SetFillColor(kMagenta);
  fSector6->SetLineColor(kMagenta);
  fSector6->SetTransparency(15);


  AddSensitiveVolume(fSector1); //ATTENTION FIXME ATTENTION
  AddSensitiveVolume(fSector2); //ATTENTION FIXME ATTENTION
  AddSensitiveVolume(fSector3); //ATTENTION FIXME ATTENTION
  AddSensitiveVolume(fSector4); //ATTENTION FIXME ATTENTION
  AddSensitiveVolume(fSector5); //ATTENTION FIXME ATTENTION
  AddSensitiveVolume(fSector6); //ATTENTION FIXME ATTENTION

  for (int j=0; j<6; j++){
    Int_t inode = 0;

    if (j==0){
        
        TGeoRotation rot0;
        TGeoTranslation trans0;
        trans0.SetTranslation(0,0,0);

        fRing1->AddNode(fSector1,   1, new TGeoCombiTrans(trans0, rot0));
    }
    else{

        for (int i = 0; i<16; i++){

        TGeoRotation rot1;
        TGeoTranslation trans1;
        rot1.RotateX(-90);
        Double_t angle = 22.5/2+i*22.5;
        rot1.RotateZ(-angle);
        Double_t h = 13.7;
        Double_t rho = 10.0+13.7/2 + (j-1)*(h+1.);
        // Double_t rho = 10.0+13.7/2 + 0*(h+1.);
        Double_t xd = rho*sin(angle*TMath::Pi()/180.);
        Double_t yd = rho*cos(angle*TMath::Pi()/180.);

        trans1.SetTranslation(xd, yd, 0);
        
        if(j==1) fRing2->AddNode(fSector2,   ++inode, new TGeoCombiTrans(trans1, rot1));
        if(j==2) fRing3->AddNode(fSector3,   ++inode, new TGeoCombiTrans(trans1, rot1));
        if(j==3) fRing4->AddNode(fSector4,   ++inode, new TGeoCombiTrans(trans1, rot1));
        if(j==4) fRing5->AddNode(fSector5,   ++inode, new TGeoCombiTrans(trans1, rot1));
        if(j==5) fRing6->AddNode(fSector6,   ++inode, new TGeoCombiTrans(trans1, rot1));

    }

    }
    
  }


  
  TGeoRotation rot0;
  TGeoTranslation trans0;
  trans0.SetTranslation(0,0,0);

//  Double_t shift = 171.6;
  Double_t shift = mapper->GetMinDistance();

  //TGeoVolume *
  fModule1 = new TGeoVolumeAssembly(mapper->AddPrefix("zplus"));
  
  fModule1->AddNode(fRing1,   1, new TGeoCombiTrans(trans0, rot0));
  fModule1->AddNode(fRing2,   2, new TGeoCombiTrans(trans0, rot0));
  fModule1->AddNode(fRing3,   3, new TGeoCombiTrans(trans0, rot0));
  fModule1->AddNode(fRing4,   4, new TGeoCombiTrans(trans0, rot0));
  fModule1->AddNode(fRing5,   5, new TGeoCombiTrans(trans0, rot0));
  fModule1->AddNode(fRing6,   6, new TGeoCombiTrans(trans0, rot0));
   
  TGeoRotation rot1;
  // rot1.RotateX(0);
  TGeoTranslation trans1;
  trans1.SetTranslation(0,0,shift);


  fMasterVolume->AddNode(fModule1, 1, new TGeoCombiTrans(trans1, rot1));

  // TGeoVolume *
  fModule2 = new TGeoVolumeAssembly(mapper->AddPrefix("zminus"));
 
  fModule2->AddNode(fRing1,   1, new TGeoCombiTrans(trans0, rot0));
  fModule2->AddNode(fRing2,   2, new TGeoCombiTrans(trans0, rot0));
  fModule2->AddNode(fRing3,   3, new TGeoCombiTrans(trans0, rot0));
  fModule2->AddNode(fRing4,   4, new TGeoCombiTrans(trans0, rot0));
  fModule2->AddNode(fRing5,   5, new TGeoCombiTrans(trans0, rot0));
  fModule2->AddNode(fRing6,   6, new TGeoCombiTrans(trans0, rot0));
  
  TGeoRotation rot2;
  // rot1.RotateX(0);
  TGeoTranslation trans2;
  trans2.SetTranslation(0,0,-shift);
  fMasterVolume->AddNode(fModule2, 2, new TGeoCombiTrans(trans2, rot2));



  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
SpdGeoMapper* SpdBbc::GetMapper() 
{
  if (fGeoMapper) return fGeoMapper;
  
  SpdGeoFactory* factory = SpdGeoFactory::Instance();
 
  // search for mapper
  fGeoMapper = factory->SearchForMapper("SpdBbcGeoMapper");
  
  if (!fGeoMapper) fGeoMapper = factory->Mapper("SpdBbcGeoMapper");

  return fGeoMapper; 
}

//_____________________________________________________________________________
Bool_t SpdBbc::LoadParsFrom(SpdParSet* params) 
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
TString SpdBbc::GetDataOut(Int_t n) const 
{ 
  if (n < 0 || n >= fNDataOut) return "unknown";
  return fOutDataPointObject;
}


//_____________________________________________________________________________________
TClonesArray* SpdBbc::GetCollection(Int_t iColl) const 
{ 
  return (iColl == 0) ? fPointCollection : 0; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Double_t SpdBbc::GetCapacity() const 
{ 
  return (fModule1) ? fModule1->GetShape()->Capacity() : 0;
}
   
//_____________________________________________________________________________________
Double_t SpdBbc::GetMass() const 
{ 
  return (fModule1) ? GetCapacity()*GetDensity() : 0;
}

//_____________________________________________________________________________________
Double_t SpdBbc::GetDensity() const 
{ 
  return (fModule1) ? fModule1->GetMaterial()->GetDensity() : 0;
}

//_____________________________________________________________________________________
TString SpdBbc::GetMaterialName() const // private
{ 
  return (fModule1) ? fModule1->GetMaterial()->GetName() : "";
}

//_____________________________________________________________________________
void SpdBbc::Print(Option_t*) const
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


