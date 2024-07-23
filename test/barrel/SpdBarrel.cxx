/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#include "SpdBarrel.h"

#include "SpdBarrelGeoPar.h"

#include "FairVolume.h"
#include "FairRootManager.h"

#include "SpdDetectorList.h"
#include "SpdStack.h"

#include "FairGeoBuilder.h"
#include "FairGeoMedia.h"

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
//#include "TGeoCombiTrans.h"
#include "TGeoTube.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"

#include "TMath.h"
using namespace TMath;

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdBarrel)

//_____________________________________________________________________________
SpdBarrel::SpdBarrel():FairDetector("SpdBarrel", kTRUE, kSpdBarrel),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fSpdBarrelPointCollection(0)
    //fSpdBarrelPointCollection(new TClonesArray("SpdBarrelPoint"))
{
    fBuildGeoOption = "";
    fMasterVol = 0;
    fMasterMedium = 0;
}

//_____________________________________________________________________________
SpdBarrel::SpdBarrel(const char* name, Bool_t active):FairDetector(name,active,kSpdBarrel),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fSpdBarrelPointCollection(0)
    //fSpdBarrelPointCollection(new TClonesArray("SpdBarrelPoint"))
{
    fBuildGeoOption = "";
    fMasterVol = 0;
    fMasterMedium = 0;
}

//_____________________________________________________________________________
SpdBarrel::~SpdBarrel()
{
  if (fSpdBarrelPointCollection) {
      fSpdBarrelPointCollection->Delete();
      delete fSpdBarrelPointCollection;
  }
}

//_____________________________________________________________________________
void SpdBarrel::SetBuildGeoOption(TString option)
{
   if (option == "toroid" || 
       option == "solenoid")  fBuildGeoOption = option;
   else fBuildGeoOption = "";
}

//_____________________________________________________________________________
void SpdBarrel::Initialize()
{
  FairDetector::Initialize();
  FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
  SpdBarrelGeoPar* par = (SpdBarrelGeoPar*)(rtdb->getContainer("SpdBarrelGeoPar"));
  
  FairRun* run = FairRun::Instance();
  if (!run) return;
  par->setChanged();
  par->setInputVersion(run->GetRunId(),1);
}

//_____________________________________________________________________________
Bool_t SpdBarrel::ProcessHits(FairVolume* vol)
{
  if (gMC->IsTrackEntering()) {
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
  }

  fELoss += gMC->Edep();

  if (gMC->IsTrackExiting()    ||
      gMC->IsTrackStop()       ||
      gMC->IsTrackDisappeared()   ) {
    
      fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
      fVolumeID = vol->getMCid();
   
      if (fELoss == 0.) { return kFALSE; }
   
      //SpdStack* stack = (SpdStack*) gMC->GetStack();
      //stack->AddPoint(kSpdBarrel);
  }

  return kTRUE;
}

//_______________________________________________________________________________________
void SpdBarrel::EndOfEvent()
{
  if (fSpdBarrelPointCollection) fSpdBarrelPointCollection->Clear();
}

//_______________________________________________________________________________________
void SpdBarrel::Register()
{
  if (fSpdBarrelPointCollection) 
  {
      FairRootManager::Instance()->Register("SpdBarrelPoint", "SpdBarrel",
                                            fSpdBarrelPointCollection, kTRUE);
  }
}

//_______________________________________________________________________________________
void SpdBarrel::Reset()
{
  if (fSpdBarrelPointCollection) fSpdBarrelPointCollection->Clear();
}

//_______________________________________________________________________________________
void SpdBarrel::ConstructGeometry()
{
    if (!gGeoManager) gGeoManager = new TGeoManager("SPD prototype geometry",
                                                    "SPD prototype geometry");
      
    FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    FairGeoBuilder*  geobuild = geoLoad->getGeoBuilder(); 
    FairGeoMedia*       Media = geoFace->getMedia();
    
    TGeoVolume *top = gGeoManager->GetTopVolume(); 
    fMasterVol = top;
    
    Int_t nmed;  
     
    //------------------------
    // HIGH PRECISION STEPPING
    //------------------------
    TGeoMedium* Air = gGeoManager->GetMedium("vacuum2");
    if (!Air) {
        nmed = geobuild->createMedium(Media->getMedium("vacuum2"));
        Air = gGeoManager->GetMedium(nmed);
    }
    fMasterMedium = Air;
    
    TGeoVolume *Cone = gGeoManager->MakeCone("Cone",fMasterMedium,400,0,350,0,350); // cm
    Cone->SetFillColor(35);
    Cone->SetLineColor(35);
    Cone->SetTransparency(50);
    
   
    if (fBuildGeoOption == "toroid") ConstructToroidalGeo();
    else if (fBuildGeoOption == "solenoid") ConstructSolenoidalGeo();
    
    fMasterVol->AddNodeOverlap(Cone,1,new TGeoTranslation(0,0,0));
    
//     TGeoVolume *Pipe = gGeoManager->MakeTube("Pipe",fMasterMedium,4.0,5.0,350);
//     Pipe->SetFillColor(kBlack);
//     Pipe->SetLineColor(kBlack);
//     Pipe->SetTransparency(30);
//   
//     fMasterVol->AddNodeOverlap(Pipe,1,new TGeoTranslation(0,0,0));
    
    gGeoManager->CloseGeometry();
}

//_______________________________________________________________________________________
void SpdBarrel::ConstructToroidalGeo()
{
  cout << "-I- <SpdBarrel::ConstructToroidalGeo> " << endl;
  
  int nseg = 8;
  Double_t coff= 10.;
  
  MakeBarrel_2("Tracker_c","Tracker_s",  nseg, 75, 50, 25, 150, coff, kRed);
  MakeBarrel_1("Calorimeter", nseg, 75, 160, 250, coff, kBlue);
  MakeBarrel_1("RangeSystem", nseg, 75, 260, 300, coff, kGreen);
}
   
//_______________________________________________________________________________________
void SpdBarrel::ConstructSolenoidalGeo()
{
  cout << "-I- <SpdBarrel::ConstructSolenoidalGeo> " << endl;
}

//________________________________________________________________________________
TGeoVolume* SpdBarrel::MakeArb(const char* name, 
                       double dz, double angle, double hmin, double hmax, 
                       double cutoff)
{
  angle *= 0.5*DegToRad();
  
  double v[16];
  
  double a = hmin*Tan(angle)-.5*cutoff/Cos(angle);  // < 0 ?
  double b = hmax*Tan(angle)-.5*cutoff/Cos(angle);  // < 0 ?
  
  cout << " 2*a = " << 2*a << " b = " << 2*b << "  " << 0.5*cutoff/Cos(angle) << endl;  
  
  v[0] =  a; v[1] = hmin;
  v[2] = -a; v[3] = hmin;
  v[4] = -b; v[5] = hmax;
  v[6] =  b; v[7] = hmax;
  
  v[8]  =  a; v[9]  = hmin;
  v[10] = -a; v[11] = hmin;
  v[12] = -b; v[13] = hmax;
  v[14] =  b; v[15] = hmax;

  TGeoVolume* vol = gGeoManager->MakeArb8(name,fMasterMedium,dz,v);
  
  return vol;
}

//________________________________________________________________________________
void SpdBarrel::MakeBarrel_1(const char* name, 
                int nsec, double dz, double hmin, double hmax, 
                double cutoff, int col)
{
  if (nsec < 3) return;
  
  double angle = 360./nsec;
 
  TGeoVolume *Arb = MakeArb(name,dz,angle,hmin,hmax,cutoff);
  
  Arb->SetFillColor(col);
  Arb->SetLineColor(col);
  Arb->SetTransparency(30);
  
  TGeoRotation* rot;
  int n(0), n1, n2;
  
  for (int i(0); i<nsec; i++) {
       n++;
       rot = new TGeoRotation(Form("t%d",n),i*angle,0,0);
       fMasterVol->AddNodeOverlap(Arb,n,rot);
       
       n1 = n+nsec;
       rot = new TGeoRotation(Form("t%d",n1),i*angle,0,0);
       fMasterVol->AddNodeOverlap(Arb,n1,new TGeoCombiTrans(0,0,-160,rot));
       
       n2 = n+2*nsec;
       rot = new TGeoRotation(Form("t%d",n2),i*angle,0,0);
       fMasterVol->AddNodeOverlap(Arb,n2,new TGeoCombiTrans(0,0,160,rot));
  }
}

//________________________________________________________________________________
void SpdBarrel::MakeBarrel_2(const char* name_c, const char* name_s, 
                int nsec, double dz, double hmin_c, double hmin_s, double hmax, 
                double cutoff, int col)
{
  if (nsec < 3) return;
  
  double angle = 360./nsec;
 
  TGeoVolume *Arb_c = MakeArb(name_c,dz,angle,hmin_c,hmax,cutoff);
  
  Arb_c->SetFillColor(col);
  Arb_c->SetLineColor(col);
  Arb_c->SetTransparency(30);
  
  TGeoVolume *Arb_s = MakeArb(name_s,dz,angle,hmin_s,hmax,cutoff);
  
  Arb_s->SetFillColor(col);
  Arb_s->SetLineColor(col);
  Arb_s->SetTransparency(30);
  
  TGeoRotation* rot;
  int n(0), n1, n2;
  
  for (int i(0); i<nsec; i++) {
       n++;
       rot = new TGeoRotation(Form("t%d",n),i*angle,0,0);
       fMasterVol->AddNodeOverlap(Arb_c,n,rot);
       
       n1 = n + nsec;
       rot = new TGeoRotation(Form("t%d",n1),i*angle,0,0);
       fMasterVol->AddNodeOverlap(Arb_s,n1,new TGeoCombiTrans(0,0,-160,rot));
       
       n2 = n + 2*nsec;
       rot = new TGeoRotation(Form("t%d",n2),i*angle,0,0);
       fMasterVol->AddNodeOverlap(Arb_s,n2,new TGeoCombiTrans(0,0,160,rot));
  }
}


