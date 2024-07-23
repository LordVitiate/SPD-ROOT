/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#include "SpdStack.h"
#include "SpdModulesMap.h"

#include "FairDetector.h" 
#include "FairLink.h"    
#include "FairMCPoint.h"     
#include "FairRootManager.h"  

#include <iosfwd>
#include "TClonesArray.h" 
#include "TIterator.h"  
#include "TLorentzVector.h"  
#include "TParticle.h"      
#include "TRefArray.h"  

#include "TGeoManager.h"

#include <stddef.h> 
#include <iostream> 

#include "SpdMCTrack.h"  

using std::cout;
using std::endl;
using std::pair;

ClassImp(SpdStack)

//_____________________________________________________________________________________
SpdStack::SpdStack(Int_t size) : FairGenericStack(),
    fStack(),
    fParticles(new TClonesArray("TParticle", size)),
    fTracks(new TClonesArray("SpdMCTrack", size)),
    fStoreMap(),
    fStoreIter(),
    fIndexMap(),
    fIndexIter(),
    fPointsMap(),
    fCurrentTrack(-1),
    fNPrimaries(0),
    fNParticles(0),
    fNTracks(0),
    fIndex(0),
    fStoreSecondaries(kTRUE),
    fMinPoints(1),
    fEnergyCut(0.),
    fStoreMothers(kTRUE)
{
}

SpdStack::SpdStack(const SpdStack& orig): FairGenericStack(),
    fStack(),
    fParticles(new TClonesArray("TParticle", orig.fParticles->GetEntries())),
    fTracks(new TClonesArray("SpdMCTrack", orig.fParticles->GetEntries())),
    fStoreMap(),
    fStoreIter(),
    fIndexMap(),
    fIndexIter(),
    fPointsMap(),
    fCurrentTrack(-1),
    fNPrimaries(0),
    fNParticles(0),
    fNTracks(0),
    fIndex(0),
    fStoreSecondaries(kTRUE),
    fMinPoints(1),
    fEnergyCut(0.),
    fStoreMothers(kTRUE)
{
}

//_____________________________________________________________________________________
SpdStack::~SpdStack()
{
  if (fParticles) {
      fParticles->Delete();
      delete fParticles;
  }
  if (fTracks) {
      fTracks->Delete();
      delete fTracks;
  }
}

//_____________________________________________________________________________________
void SpdStack::Reset()
{
  //cout << "<SpdStack::Reset>" << endl;
  
  fIndex = 0;
  fCurrentTrack = -1;
  fNPrimaries = 0;
  fNParticles = 0;
  fNTracks = 0;
  
  while (!fStack.empty() ) { fStack.pop(); }
  
  Int_t nt = fTracks->GetEntriesFast();
  
  fParticles->Delete();
  fTracks->Delete();
  fPointsMap.clear();
  
  //cout << "<SpdStack::Reset> " << nt << ", " << fTracks->GetEntriesFast() << endl;
}

//_____________________________________________________________________________________
void SpdStack::Register()
{
  FairRootManager::Instance()->Register("SpdMCTrack", "Stack", fTracks, kTRUE);
}

//_____________________________________________________________________________________
void SpdStack::PushTrack(Int_t toBeDone, Int_t parentId, Int_t pdgCode,
                         Double_t px, Double_t py, Double_t pz,
                         Double_t e, Double_t vx, Double_t vy, Double_t vz,
                         Double_t time, Double_t polx, Double_t poly,
                         Double_t polz, TMCProcess proc, Int_t& ntr,
                         Double_t weight, Int_t is)
{
  PushTrack(toBeDone, parentId, pdgCode,
            px,  py,  pz, e,  vx,  vy,  vz,
            time,  polx,  poly, polz, proc, 
            ntr, weight, is, -1);
}

//_____________________________________________________________________________________
void SpdStack::PushTrack(Int_t toBeDone, Int_t parentId, Int_t pdgCode,
                         Double_t px, Double_t py, Double_t pz,
                         Double_t e, Double_t vx, Double_t vy, Double_t vz,
                         Double_t time, Double_t polx, Double_t poly,
                         Double_t polz, TMCProcess proc, Int_t& ntr,
                         Double_t weight, Int_t is, Int_t secondparentID)
{
  // Get TParticle array
  TClonesArray& partArray = *fParticles;

  // Create new TParticle and add it to the TParticle array
  Int_t trackId = fNParticles;
  
  Int_t daughter1Id = -1;
  Int_t daughter2Id = -1;
  
  TParticle* particle = 
             new(partArray[fNParticles++]) TParticle(pdgCode, trackId, parentId, secondparentID, 
                                                     daughter1Id, daughter2Id, px, py, pz, e,
                                                     vx, vy, vz, time);

  particle->SetPolarisation(polx, poly, polz);
  particle->SetWeight(weight);
  particle->SetUniqueID(proc);

  // Increment counter
  if (parentId < 0) { fNPrimaries++; }

  // Set argument variable
  ntr = trackId;

  // Push particle on the stack if toBeDone is set
  if (toBeDone == 1) { fStack.push(particle); }
}

//_____________________________________________________________________________________
TParticle* SpdStack::PopNextTrack(Int_t& iTrack)
{
  //cout << "<SpdStack::PopNextTrack> iTrack: " << iTrack << endl;
  
  // If end of stack: Return empty pointer
  if (fStack.empty()) {
      iTrack = -1;
      return NULL;
  }

  // If not, get next particle from stack
  TParticle* thisParticle = fStack.top();
  fStack.pop();

  if (!thisParticle) {
      iTrack = 0;
      return NULL;
  }

  fCurrentTrack = thisParticle->GetStatusCode();
  iTrack = fCurrentTrack;

  return thisParticle;
}

//_____________________________________________________________________________________
TParticle* SpdStack::PopPrimaryForTracking(Int_t iPrim)
{
  // Get the iPrimth particle from the fStack TClonesArray. 
  // This should be a primary (if the index is correct).

  //cout << "<SpdStack::PopPrimaryForTracking> iPrim = " << iPrim << endl;
  
  // Test for index
  if (iPrim < 0 || iPrim >= fNPrimaries) {
      Fatal("SpdStack::PopPrimaryForTracking", "Index out of range");
  }
  
  // Return the iPrim-th TParticle from the fParticle array. This should be a primary.
  TParticle* part = (TParticle*)fParticles->At(iPrim);
  
  if (!(part->GetMother(0) < 0)) {
      Fatal("SpdStack::PopPrimaryForTracking", "Not a primary track");
  }
  
//  cout << "<SpdStack::PopPrimaryForTracking> " << iPrim << "/" << fNPrimaries 
//       << " PDG: " << part->GetPdgCode() << endl;
  
  return part;
}

//_____________________________________________________________________________________
TParticle* SpdStack::GetCurrentTrack() const
{
  TParticle* currentPart = GetParticle(fCurrentTrack);
  if (!currentPart) {
      Warning("SpdStack::GetCurrentTrack", "Track not found in stack");
  }
  return currentPart;
}

//_____________________________________________________________________________________
Int_t SpdStack::GetCurrentParentTrackNumber() const
{
  TParticle* currentPart = GetCurrentTrack();
  if (currentPart) { return currentPart->GetFirstMother(); }
  else return -1;
}

//_____________________________________________________________________________________
TParticle* SpdStack::GetParticle(Int_t trackID) const
{
  if (trackID < 0 || trackID >= fNParticles) {
      //cout << trackID << "/" << fNParticles << endl;
      Fatal("GetParticle", "Index out of range");
  }
  return (TParticle*)fParticles->At(trackID);
}

// //_____________________________________________________________________________________
// void SpdStack::AddParticle(TParticle* oldPart)
// {
//   TClonesArray& array = *fParticles;
//   TParticle* newPart = new(array[fIndex]) TParticle(*oldPart);
//   newPart->SetWeight(oldPart->GetWeight());
//   newPart->SetUniqueID(oldPart->GetUniqueID());
//   fIndex++;
// }


//_____________________________________________________________________________________
void SpdStack::AddPoint(DetectorId detId)
{
  //
  // for current track
  //
  Int_t iDet = detId;
  pair<Int_t, Int_t> a(fCurrentTrack, iDet);
  if (fPointsMap.find(a) == fPointsMap.end()) fPointsMap[a] = 1;
  else fPointsMap[a]++;
}


//_____________________________________________________________________________________
void SpdStack::AddPoint(DetectorId detId, Int_t iTrack)
{
  // 
  // for arbitrary track
  //
  if (iTrack < 0) return;
  Int_t iDet = detId;
  pair<Int_t, Int_t> a(iTrack, iDet);
  if (fPointsMap.find(a) == fPointsMap.end()) fPointsMap[a] = 1;
  else fPointsMap[a]++;
}

//_____________________________________________________________________________________
void SpdStack::FillTrackArray()
{
 // Print();
  // Reset index map and number of output tracks
  fIndexMap.clear();
  fNTracks = 0;

  // Check tracks for selection criteria
  SelectTracks();
  
  TGeoNavigator* seeker = 0;
  if (gGeoManager) seeker = gGeoManager->GetCurrentNavigator();
  
  SpdModulesMap* modmap = 0;
  
  if (seeker) {
      modmap = SpdModulesMap::Instance();
      if (modmap->Empty()) modmap = 0;
  }
  
  TParticle* part;
  Int_t ID;
  TString path;
  
  // Loop over fParticles array and copy selected tracks
  for (Int_t iPart = 0; iPart<fNParticles; iPart++) {

       fStoreIter = fStoreMap.find(iPart);
       
       if (fStoreIter == fStoreMap.end()) {
           Fatal("FillTrackArray","Particle not found in storage map.");
       }
       
       Bool_t store = (*fStoreIter).second;

       if (store) {
         
           part = GetParticle(iPart);
     
           SpdMCTrack* track = new((*fTracks)[fNTracks]) SpdMCTrack(part);
           fIndexMap[iPart] = fNTracks;
           
           // Set the number of points in the detectors for this track
           for (Int_t iDet = kSpdEcalTB; iDet<kSTOPHERE; iDet++) {
                pair<Int_t, Int_t> a(iPart, iDet);
                if (fPointsMap[a] > 0) track->SetNPoints(iDet, fPointsMap[a]);
           }
           
           // Searching for primary vertex detector ID
           if (modmap) 
           {
               if (seeker->FindNode(part->Vx(),part->Vy(),part->Vz())) {   
                              
                   path = seeker->GetPath();            
                   ID = modmap->SearchForId(path);  
                   track->SetVModule(ID);                
                   track->SetVNodeName(modmap->GetFirstNodeName(path));         
                
                   //if (ID == kSpdTop) cout << iPart << " PATH: " << path << " ID: " << ID << endl;
                   //cout << iPart << " PATH: " << path << " ID: " << ID << endl;
               }     
           }
           fNTracks++;
       } 
       else {
           fIndexMap[iPart] = -2;
       }
  }
  
 // if (modmap) modmap->Print();

  // Map index for primary mothers
  fIndexMap[-1] = -1;
}

//_____________________________________________________________________________________
void SpdStack::UpdateTrackIndex(TRefArray* detList)
{
  // First update mother ID in MCTracks
  for (Int_t i = 0; i<fNTracks; i++) {
    
       SpdMCTrack* track = (SpdMCTrack*)fTracks->At(i);
       Int_t iMotherOld = track->GetMotherId();
       
       fIndexIter = fIndexMap.find(iMotherOld);
       
       if (fIndexIter == fIndexMap.end()) {
           Fatal("UpdateTrackIndex", "Particle index not found in map");
       }
       
       track->SetMotherId((*fIndexIter).second);
  }

  // Now iterate through all active detectors
  if (fDetList == 0) fDetIter = detList->MakeIterator();
  fDetIter->Reset();
  
  Int_t nColl = 0;
  FairDetector* det = NULL;
  
  while((det = (FairDetector*)fDetIter->Next())) {

     // Get hit collections from detector
     Int_t iColl = 0;
     TClonesArray* hitArray;
     
     while ( (hitArray = det->GetCollection(iColl++)) ) {
         
        nColl++;
        Int_t nPoints = hitArray->GetEntriesFast();

        // Update track index for all MCPoints in the collection
        for (Int_t iPoint = 0; iPoint<nPoints; iPoint++) {
             
             FairMCPoint* point = (FairMCPoint*)hitArray->At(iPoint);
             Int_t iTrack = point->GetTrackID();
             
             fIndexIter = fIndexMap.find(iTrack);
             
             if (fIndexIter == fIndexMap.end()) {
                 Fatal("UpdateTrackIndex", "Particle index not found in map");
             }
             
             point->SetTrackID((*fIndexIter).second);
             point->SetLink(FairLink("MCTrack",(*fIndexIter).second));
        }
     }   
  } 
}

//_____________________________________________________________________________________
void SpdStack::SelectTracks()
{
  // Clear storage map
  fStoreMap.clear();

  // Check particles in the fParticle array
  for (Int_t i = 0; i<fNParticles; i++) {

       TParticle* thisPart = GetParticle(i);
       
       Bool_t store = kTRUE;

       // Get track parameters
       Int_t iMother = thisPart->GetMother(0);
       TLorentzVector p;
       thisPart->Momentum(p);
       Double_t energy = p.E();
       Double_t mass   = p.M();
       Double_t eKin = energy - mass;
       
       /* Calculate number of points */
       Int_t nPoints = 0;
       for (Int_t iDet = 0; iDet < kSTOPHERE; iDet++) {
           pair<Int_t, Int_t> a(i, iDet);
           if (fPointsMap.find(a) != fPointsMap.end()) nPoints += fPointsMap[a];
       }

       // Check for cuts (store primaries in any case)
       if (iMother < 0) store = kTRUE;
       else {
           if (!fStoreSecondaries)   store = kFALSE;
           if (nPoints < fMinPoints) store = kFALSE;
           //if (eKin < fEnergyCut)    store = kFALSE;
       }
       
       
       /* Set storage flag */
       fStoreMap[i] = store;
            
       //if (i < fNPrimaries) {
       //    cout << thisPart->GetStatusCode() << " " << i << " " << iMother << " " << thisPart->GetPdgCode() 
       //         << " " << p.Px() << " " << p.Py()<< " " << p.Pz() << endl;
       //}
       
       //if (!store)   
       //    cout << store << " " << i << " " << iMother << " " << nPoints << " /// " 
       //       << fMinPoints << " " << fEnergyCut << " " << fStoreSecondaries << endl;
       //}
  }

  // If flag is set, flag recursively mothers of selected tracks
  if (fStoreMothers) {
      for (Int_t i = 0; i<fNParticles; i++) {
           if (fStoreMap[i]) {
               Int_t iMother = GetParticle(i)->GetMother(0);
               while (iMother >= 0) {
                  fStoreMap[iMother] = kTRUE;
                  iMother = GetParticle(iMother)->GetMother(0);
               }
           }
      }
  }
}

//_____________________________________________________________________________________
void SpdStack::Print(Int_t iVerbose) const
{
  cout << "<SpdStack::Print> Primaries = " << fNPrimaries 
       << " Total particles  = " << fNParticles 
       << " Tracks in output = " << fNTracks << endl;
  
  if (iVerbose) {
      for (Int_t iTrack = 0; iTrack<fNTracks; iTrack++) {
          ((SpdMCTrack*)fTracks->At(iTrack))->Print(iTrack);
     }
  }
}

//_____________________________________________________________________________________
void SpdStack::PrintPrimaries() const
{
  cout << "<SpdStack::Print> Primaries = " << fNPrimaries << endl;
  
  if (fNPrimaries < 1 || !fParticles) return;
  
  cout << "\n";
  
  TParticle* p;
  
  printf("  %-5s  [%10s %10s]  %12s  [%4s %4s] (%10s %10s %10s) (%10s %10s %10s %10s) \n\n",
         "N","FM","LM","PDG","FD","LD","Px","Py","Pz","Vx","Vy","Vz","T");
  
  for (Int_t i(0); i<fNPrimaries; i++) {
    
       p = (TParticle*)fParticles->At(i);
       if (!p) continue;
       
       printf("  %-5d  [%10d %10d]  %12d  [%4d %4d] (%10.6f %10.6f %10.6f) (%10.3e %10.3e %10.3e %10.3e) \n",
                 i, 
                 p->GetFirstMother(), p->GetSecondMother() ,
                 p->GetPdgCode(), 
                 p->GetFirstDaughter(), p->GetLastDaughter(), 
                 p->Px(), p->Py(), p->Pz(), 
                 p->Vx(),  p->Vy(),  p->Vz(), p->T()
                );
  }
  cout << "\n";
}



