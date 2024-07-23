// $Id$
// Author: artur   2018/08/27


//_____________________________________________________________________________
//
// SpdMCTracks
//_____________________________________________________________________________

#include <TClonesArray.h>

#include "SpdMCTracks.h"
#include "SpdFairEventManager.h"
#include <TEveManager.h>  
#include <TEveTrack.h>
#include <TGeoTrack.h> 
#include <TParticle.h>   
#include <TMathBase.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdMCTracks)

//_____________________________________________________________________________
SpdMCTracks::SpdMCTracks(const char* name, Int_t iVerbose):FairMCTracks(name,iVerbose)
{
}

//_____________________________________________________________________________
SpdMCTracks::~SpdMCTracks()
{
}

//_____________________________________________________________________________
InitStatus SpdMCTracks::Init()
{
  cout << "<SpdMCTracks::Init>" << endl;
  
  FairRootManager* fManager = FairRootManager::Instance();
  fTrackList = static_cast<TClonesArray*>(fManager->GetObject(fName));
  
  fEventManager = SpdFairEventManager::Instance();
  
  if (!fEventManager) { 
      cout << "-E- <SpdFairMCTracks::Init> No Event Manager." << endl;
      return kERROR;
  }
  
  if (fTrackList) return kSUCCESS;
      
  cout << "-W- <SpdFairMCTracks::Init> Branch " << fName << " not found [FairRootManager]. " << endl;
  
  TTree* tree = fManager->GetInTree();
  
  TBranch* branch = tree->GetBranch("GeoTracks");
  if (!branch) {
      cout << "-W- <SpdFairMCTracks::Init> Branch " << fName << " not found. " << endl;
      SetActive(kFALSE);
      return kERROR;
  }

  int entries = branch->GetEntries();
  
  if (entries == 0) {
      cout << "-I- <SpdFairMCTracks::Init> Branch " << fName << " is empty. " << endl;
      SetActive(kFALSE);   
      return kERROR;
  }

  branch->SetAddress(&fTrackList);
  
  return kSUCCESS;
}

//_____________________________________________________________________________
void SpdMCTracks::Exec(Option_t* /*option*/)
{
  if (!IsActive()) return;
  
  ResetTracks();
  
  //cout << "-I- <SpdFairMCTracks::Exec> " << endl; 
 
  TGeoTrack* ptrack;
  TParticle* part;
  const Double_t* point;
  Int_t pdg;
  
  for (Int_t i(0); i < fTrackList->GetEntriesFast(); i++)  {
      
       //cout << "<SpdMCTracks::Exec> itrack = " << i << endl; 
      
       ptrack = static_cast<TGeoTrack*>(fTrackList->At(i));
       
       if (!AcceptTrack(ptrack)) continue;
       
       part = static_cast<TParticle*>(ptrack->GetParticle());
       
       PEnergy = part->Energy();
       pdg = ptrack->GetPDG();
       
       if ( fEventManager->IsPriOnly() && (part->GetMother(0) > -1) ) continue;
       
       if ( (fEventManager->GetCurrentPDG() != 0) && 
            (fEventManager->GetCurrentPDG() != pdg) ) continue;

       Int_t Np = ptrack->GetNpoints();
       
       if (Np == 0) continue;      
       
       fTrList = GetTracksGroup(part);
       
       TEveTrack* track = new TEveTrack(part, pdg, fTrList->GetPropagator());
       
       track->SetLineColor(fEventManager->Color(pdg));
       
       for (Int_t n(0); n < Np; n++) {
           
            point = ptrack->GetPoint(n);
            track->SetPoint(n,point[0],point[1],point[2]);
         
//             TEvePathMark* path = new TEvePathMark();
//             path->fV.Set(point[0], point[1],point[2]);
//             path->fTime = point[3];
//             if (n == 0) path->fP.Set(part->Px(),part->Py(),part->Pz());
//             track->AddPathMark(*path);
//             delete path;
       }
      
       fTrList->AddElement(track);
  }
   
  gEve->Redraw3D(kFALSE);

}

//_____________________________________________________________________________
Bool_t SpdMCTracks::AcceptTrack(TGeoTrack* track)
{
   if (!track) return kFALSE;
   
//    Int_t pdg = track->GetPDG();
//    
//    if (TMath::Abs(pdg) == 12) return kFALSE;  // nu_e
//    if (TMath::Abs(pdg) == 14) return kFALSE;  // nu_mu
    
   return kTRUE;    
}

//_____________________________________________________________________________
Bool_t SpdMCTracks::DeactivateTrack(TParticle* particle)
{
   if (!particle) return kFALSE;
   
   Int_t pdg = particle->GetPdgCode();
   
   if (TMath::Abs(pdg) == 12) return kTRUE;  // nu_e
   if (TMath::Abs(pdg) == 14) return kTRUE;  // nu_mu
   
   return kFALSE;
}

//_____________________________________________________________________________
void SpdMCTracks::ResetTracks()
{
  for (Int_t i(0); i < fEveTrList->GetEntriesFast(); i++) {
       TEveTrackList* elem = static_cast<TEveTrackList*>(fEveTrList->At(i));
       gEve->RemoveElement(elem,fEventManager);
  }
  
  fEveTrList->Clear();
}

//_____________________________________________________________________________
TEveTrackList* SpdMCTracks::GetTracksGroup(TParticle* P) 
{
  fTrList = 0;
  
  for (Int_t i(0); i < fEveTrList->GetEntriesFast(); i++) {
     
       TEveTrackList* TrListIn = static_cast<TEveTrackList*>(fEveTrList->At(i));
       
       if (strcmp(TrListIn->GetName(),P->GetName()) == 0) {
           fTrList = TrListIn;
           break;
       }
  }
  
  if (fTrList == 0) {
     
      fTrPr = new TEveTrackPropagator();
      fTrList = new TEveTrackList(P->GetName(),fTrPr);

      fTrList->SetMainColor(fEventManager->Color(P->GetPdgCode()));
      
      fEveTrList->Add(fTrList);
      gEve->AddElement(fTrList,fEventManager);
      
      fTrList->SetRnrLine(kTRUE);
    
      if (DeactivateTrack(P)) {
          fTrList->SetRnrSelf(kFALSE);      // deactivate track selection
          fTrList->SetRnrChildren(kFALSE);  // deactivate track's child selection
      }
  }
  
  return fTrList;
}
