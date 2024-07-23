
#include "SpdSts.h"

#include "SpdStsPoint.h"
#include "SpdStsGeoPar.h"
#include "SpdStsGeo.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "SpdDetectorList.h"
#include "SpdStack.h"

#include "FairGeoBuilder.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"    

#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdSts)

//_____________________________________________________________________________
SpdSts::SpdSts():

    FairDetector("SpdSts", kTRUE, kSpdSts),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fSpdStsPointCollection(new TClonesArray("SpdStsPoint"))
{
}

//_____________________________________________________________________________
SpdSts::SpdSts(const char* name, Bool_t active):

    FairDetector(name, active, kSpdSts),
    fTrackID(-1),
    fVolumeID(-1),
    fPos(),
    fMom(),
    fTime(-1.),
    fLength(-1.),
    fELoss(-1),
    fSpdStsPointCollection(new TClonesArray("SpdStsPoint"))
{
}

//_____________________________________________________________________________
SpdSts::~SpdSts()
{
  if (fSpdStsPointCollection) {
      fSpdStsPointCollection->Delete();
      delete fSpdStsPointCollection;
  }
}

//_____________________________________________________________________________
void SpdSts::Initialize()
{
  FairDetector::Initialize();
  FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
  SpdStsGeoPar* par = (SpdStsGeoPar*)(rtdb->getContainer("SpdStsGeoPar"));
}

//_____________________________________________________________________________
void SpdSts::Reset()
{
  fSpdStsPointCollection->Clear();
}

//_____________________________________________________________________________
void SpdSts::Register()
{

  /** This will create a branch in the output tree called
      SpdStsPoint, setting the last parameter to kFALSE means:
      this collection will not be written to the file, it will exist
      only during the simulation.
  */

  FairRootManager::Instance()->Register("SpdStsPoint", "SpdSts",
                                        fSpdStsPointCollection, kTRUE);

}

//_____________________________________________________________________________
Bool_t  SpdSts::ProcessHits(FairVolume* vol)
{
  /** This method is called from the MC stepping */
  
  //cout << "<SpdSts::ProcessHits> " << endl;

  //Set parameters at entrance of volume. Reset ELoss.
  if (gMC->IsTrackEntering()) {
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPos);
      gMC->TrackMomentum(fMom);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Create SpdStsPoint at exit of active volume
  if (gMC->IsTrackExiting()    ||
      gMC->IsTrackStop()       ||
      gMC->IsTrackDisappeared()) 
  {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    
    if (fELoss == 0.) { return kFALSE; }
    
    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
           TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength,
           fELoss);

    // Increment number of SpdSts det points in TParticle
    SpdStack* stack = (SpdStack*) gMC->GetStack();
    stack->AddPoint(kSpdSts);
  }

  return kTRUE;
}

//_____________________________________________________________________________
void SpdSts::EndOfEvent()
{
  fSpdStsPointCollection->Clear();
}


//_____________________________________________________________________________
TClonesArray* SpdSts::GetCollection(Int_t iColl) const
{
  return (iColl == 0) ? fSpdStsPointCollection : 0;
}


//_____________________________________________________________________________
void SpdSts::ConstructGeometry()
{
  
  cout << "\n*******************************************************************************" << endl;
  cout << "********************** SpdSts::ConstructGeometry ******************************" << endl;
  cout << "*******************************************************************************\n" << endl;
    
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  
  SpdStsGeo* stsgeo = new SpdStsGeo();
 
  stsgeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(stsgeo);
  Bool_t rc = geoFace->readSet(stsgeo);
  if (rc) { stsgeo->create(geoLoad->getGeoBuilder()); }
  
  FairRun* fRun = FairRun::Instance();
  
  if (!fRun) return;
    
  TList* volList = stsgeo->getListOfVolumes();
  
  // store geo parameter
 
  FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
  SpdStsGeoPar* par = (SpdStsGeoPar*)(rtdb->getContainer("SpdStsGeoPar"));
  
  TObjArray* fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray* fPassNodes = par->GetGeoPassiveNodes();
  
  TListIter iter(volList);
  FairGeoNode* node = 0;
  FairGeoVolume *aVol = 0;
  
  while((node = (FairGeoNode*)iter.Next())) 
  {
        aVol = dynamic_cast<FairGeoVolume*>(node);
        if (node->isSensitive()) fSensNodes->AddLast(aVol);
        else fPassNodes->AddLast(aVol);
  }
  
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);
  
  ProcessNodes(volList);
}

//_____________________________________________________________________________
TString SpdSts::FName(TString name) 
{   
  TString mname = GetName();
  mname += "1_" + name;
  return mname; 
}

//_____________________________________________________________________________
SpdStsPoint* SpdSts::AddHit(Int_t trackID, Int_t detID,
                            TVector3 pos, TVector3 mom,
                            Double_t time, Double_t length, Double_t eLoss)
{
  TClonesArray& clref = *fSpdStsPointCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) SpdStsPoint(trackID, detID, pos, mom, time, length, eLoss);
}


