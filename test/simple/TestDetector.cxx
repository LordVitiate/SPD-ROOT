
#include "TestDetector.h"

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

ClassImp(TestDetector)

//_____________________________________________________________________________
TestDetector::TestDetector():FairDetector("TestDetector", kTRUE, kTestDetector),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fTestDetectorPointCollection(0)
    //fTestDetectorPointCollection(new TClonesArray("TestDetectorPoint"))
{
    fMasterVol = 0;
    fMasterMedium = 0;
}

//_____________________________________________________________________________
TestDetector::TestDetector(const char* name, Bool_t active):FairDetector(name,active,kTestDetector),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fTestDetectorPointCollection(0)
    //fTestDetectorPointCollection(new TClonesArray("TestDetectorPoint"))
{
    fMasterVol = 0;
    fMasterMedium = 0;
}

//_____________________________________________________________________________
TestDetector::~TestDetector()
{
  if (fTestDetectorPointCollection) {
      fTestDetectorPointCollection->Delete();
      delete fTestDetectorPointCollection;
  }
}

//_____________________________________________________________________________
void TestDetector::Initialize()
{
  FairDetector::Initialize();
  /*
  
  FairDetector::Initialize();
  FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
  //SpdBarrelGeoPar* par = (SpdBarrelGeoPar*)(rtdb->getContainer("SpdBarrelGeoPar"));
  
  FairRun* run = FairRun::Instance();
  if (!run) return;*/
}

//_____________________________________________________________________________
Bool_t TestDetector::ProcessHits(FairVolume* vol)
{
  if (gMC->IsTrackEntering()) {
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
  }
  
  cout << gMC->CurrentVolPath() << endl;

  fELoss += gMC->Edep();

  if (gMC->IsTrackExiting()    ||
      gMC->IsTrackStop()       ||
      gMC->IsTrackDisappeared()   ) {
    
      fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
      fVolumeID = vol->getMCid();
   
      if (fELoss == 0.) { return kFALSE; }
   
      //SpdStack* stack = (SpdStack*) gMC->GetStack();
      //stack->AddPoint(kTestDetector);
  }

  return kTRUE;
}

//_______________________________________________________________________________________
void TestDetector::EndOfEvent()
{
   Reset();
}

//_______________________________________________________________________________________
void TestDetector::Register()
{
  if (fTestDetectorPointCollection) 
  {
      FairRootManager::Instance()->Register("TestDetectorPoint", "TestDetector",
                                            fTestDetectorPointCollection, kTRUE);
  }
}

//_______________________________________________________________________________________
void TestDetector::Reset()
{
  if (fTestDetectorPointCollection) fTestDetectorPointCollection->Clear();
}

//_______________________________________________________________________________________
void TestDetector::ConstructGeometry()
{
   // if (!gGeoManager) gGeoManager = new TGeoManager("test detector geometry",
   //                                                 "test detector geometry");
  
    FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    FairGeoBuilder*  geobuild = geoLoad->getGeoBuilder(); 
    FairGeoMedia*       Media = geoFace->getMedia();

    fMasterVol = gGeoManager->GetTopVolume(); 
  
    Int_t nmed;  
    
    //------------------------
    // HIGH PRECISION STEPPING
    //------------------------
    fMasterMedium = gGeoManager->GetMedium("iron");
   
    if (!fMasterMedium) {
        nmed = geobuild->createMedium(Media->getMedium("iron"));
        fMasterMedium = gGeoManager->GetMedium(nmed);
    }
   
    TGeoVolume *Cone = gGeoManager->MakeCone("Cone",fMasterMedium,400,0,350,0,350); // cm
    
    Cone->SetFillColor(35);
    Cone->SetLineColor(35);
    Cone->SetTransparency(50);
    
    AddSensitiveVolume(Cone);
    
    fMasterVol->AddNode(Cone,1,new TGeoTranslation(0,0,0));
    
//     TGeoVolume *Pipe = gGeoManager->MakeTube("Pipe",fMasterMedium,4.0,5.0,350);
//     Pipe->SetFillColor(kBlack);
//     Pipe->SetLineColor(kBlack);
//     Pipe->SetTransparency(30);
//   
//     fMasterVol->AddNodeOverlap(Pipe,1,new TGeoTranslation(0,0,0));
    
    //gGeoManager->CloseGeometry();
}


