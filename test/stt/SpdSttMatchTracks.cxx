

#include <iostream>

#include "TClonesArray.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "SpdSttMatchTracks.h"
#include "SpdSttHit.h"
#include "SpdSttTrackMatch.h"
#include "SpdTrackCand.h"

#include "TMath.h"
#include <vector>

using namespace std;

// -----   Default constructor   -------------------------------------------
SpdSttMatchTracks::SpdSttMatchTracks() 
  : FairTask("STT track match") {
  fMatches    = NULL;
  fVerbose    = 1;
  fCollectionsComplete = kFALSE;
  fPersistence = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Constructor with verbosity level   ------------------------------
SpdSttMatchTracks::SpdSttMatchTracks(Int_t verbose) 
  : FairTask("STT track match") {
  fMatches    = NULL;
  fVerbose    = verbose;
  fCollectionsComplete = kFALSE;
  fPersistence = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Constructor with name, title and verbosity  ---------------------
SpdSttMatchTracks::SpdSttMatchTracks(const char* name, const char* title,
				     Int_t verbose) 
  : FairTask(name) {
  fMatches    = NULL;
  fVerbose    = verbose;
  fCollectionsComplete = kFALSE;
  fPersistence = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
SpdSttMatchTracks::~SpdSttMatchTracks() 
{
  fHitCollectionNames.clear();  
  fPointCollectionNames.clear();
}
// -------------------------------------------------------------------------




// -----   Public method Init   --------------------------------------------
InitStatus SpdSttMatchTracks::Init() {
  
  // Get FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- SpdSttMatchTracks::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get TrackCand Array 
  fTrackCandidates = (TClonesArray*) ioman->GetObject("STTTrackCand");
  if ( ! fTrackCandidates ) {
    cout << "-E- SpdSttMatchTracks::Init: No STTTrackCand array!" << endl;
    return kERROR;
  }
  
  // Create and register SttTrackMatch array
  fMatches = new TClonesArray("SpdSttTrackMatch",100);
  ioman->Register("STTTrackMatch", "STT", fMatches, fPersistence);

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void SpdSttMatchTracks::Exec(Option_t* opt) 
{
  AddAllCollections();
  
  if (fHitCollectionList.GetEntries() == 0)
  {
      cout << "-E- SpdSttTrackFinderIdeal::DoFind: "
	   << "No hit arrays present, call AddHitCollection() first (at least once)! " << endl;
  }

  if (fPointCollectionList.GetEntries() == 0)
  {
      cout << "-E- SpdSttTrackFinderIdeal::DoFind: "
	   << "No point arrays present, call AddHitCollection() first (at least once)! " << endl;
  }

  // Clear output array
  fMatches->Delete();

  // Create some pointers and variables
  SpdTrackCand*     trackCand = NULL; 
  SpdSttHit*        mHit  = NULL;
  FairMCPoint*       point = NULL;

  Int_t nHits    = 0;
  Int_t nMCTracks = 0;
  Int_t iPoint    = 0;
  Int_t iFlag     = 0;
  Int_t iMCTrack  = 0;
  Int_t nAll      = 0;
  Int_t nTrue     = 0;
  Int_t nWrong    = 0;
  Int_t nFake     = 0;
  Int_t nHitSum     = 0;
  Int_t nTrueSum    = 0;
  Int_t nWrongSum   = 0;
  Int_t nFakeSum    = 0;
  Int_t nMCTrackSum = 0;

   
  // Loop over TracksCand 
  Int_t nTracks = fTrackCandidates->GetEntriesFast();


  for (Int_t iTrack=0; iTrack<nTracks; iTrack++) 
    {
      trackCand = (SpdTrackCand*) fTrackCandidates->At(iTrack); 
 
      if ( ! trackCand)  
	{
	  cout << "-W- SpdSttMatchTracks::Exec: Empty STTTrackCand at "
	       << iTrack << endl;
	  continue;
	}
 
      nHits = trackCand->GetNHits(); 
      
      nAll = nTrue = nWrong = nFake = nMCTracks = 0;

      fMatchMap.clear();
      if (fVerbose > 2) cout << endl << "Track " << iTrack << ", Hits " 
			     << nHits << endl;

      // Loop over Hits of track
      for (Int_t iMHit=0; iMHit<nHits; iMHit++) 
	{

	  SpdTrackCandHit candhit = trackCand->GetSortedHit(iMHit);

	  // alter here
	  mHit = GetHitFromCollections(candhit.GetHitId()); 

 	

	  if ( ! mHit ) 
	    {
	      cout << "-E- SpdSttMatchTracks::Exec: "
		   << "No Hit " << iMHit << " for track " << iTrack << endl;
	      continue;
	    }


	  iPoint = mHit->GetRefIndex();
	  
	  if ( iPoint < 0 ) 
	    {        
	      nFake++;
	      continue;
	    }
	  
	  // alter here
	  point = GetPointFromCollections(candhit.GetHitId()); 
	  
	  
	  if ( ! point ) 
	    {
	      cout << "-E- SpdSttMatchTracks::Exec: "
		   << "Empty MCPoint " << iPoint << " from Hit " << iMHit
		   << " (track " << iTrack << ")" << endl;
	      continue;
	    }
	  
	  iMCTrack = point->GetTrackID();
	  
	  if ( fVerbose > 2 ) cout << "Track " << iTrack << ", hit "
				   << candhit.GetHitId() 
				   << ", STTPoint " << iPoint << ", MCTrack "
				   << iMCTrack << endl;
	  fMatchMap[iMCTrack]++;
	}
    
      // Search for best matching MCTrack
      iMCTrack = -1;

      map<Int_t, Int_t>::iterator it;
      for (it=fMatchMap.begin(); it!=fMatchMap.end(); it++) 
	{
	  if (fVerbose > 2) cout << it->second 
				 << " common points wth MCtrack " 
				 << it->first << endl;
	  nMCTracks++;
	  nAll += it->second;
	  if ( it->second > nTrue ) 
	    {
	      iMCTrack = it->first;
	      nTrue    = it->second;	
	    }
	}
    
      nWrong = nAll - nTrue;
      if (fVerbose>1) cout << "-I- SpdSttMatchTracks: STTTrack " << iTrack 
			   << ", MCTrack " << iMCTrack << ", true " 
			   << nTrue << ", wrong " << nWrong << ", fake " 
			   << nFake << ", #MCTracks " << nMCTracks << endl;
      
      // Create SttTrackMatch
      new ((*fMatches)[iTrack]) SpdSttTrackMatch(iMCTrack, nTrue, 
						 nWrong, nFake, 
						 nMCTracks);

      // Some statistics
      nHitSum     += nHits;
      nTrueSum    += nTrue;
      nWrongSum   += nWrong;
      nFakeSum    += nFake;
      nMCTrackSum += nMCTracks;

    } // Track loop

  // Event statistics

  Double_t qTrue = 0.;
  if ( nHitSum) 
      qTrue  = Double_t(nTrueSum)  / Double_t(nHitSum) * 100.;

  if (fVerbose == 3) 
  {
      Double_t 
	  qWrong = 0.,
	  qFake = 0.,
	  qMC = 0.;
      
      if (nHitSum)
      {
	  qWrong = Double_t(nWrongSum) / Double_t(nHitSum) * 100.;
	  qFake  = Double_t(nFakeSum)  / Double_t(nHitSum) * 100.;
      }
      if (nTracks)
	  qMC = Double_t(nMCTrackSum) / Double_t(nTracks);

    cout << endl;
    cout << "-------------------------------------------------------" 
	 << endl;
    cout << "-I-              STT Track Matching                 -I-"
	 << endl;
    cout << "Reconstructed STTTracks : " << nTracks << endl;;
    cout << "True  hit assignments   : " << qTrue  << " %" << endl;
    cout << "Wrong hit assignments   : " << qWrong << " %" << endl;
    cout << "Fake  hit assignments   : " << qFake  << " %" << endl;
    cout << "MCTracks per STTTrack   : " << qMC << endl;
    cout << "--------------------------------------------------------" 
	 << endl;
  }
  else  if (fVerbose) cout << "-I- SpdSttMatchTracks: rec. " << nTracks << ", quota "
			   << qTrue << " % " << endl;

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void SpdSttMatchTracks::AddHitCollectionName(char *hitCollectionName, char *pointCollectionName)
{
    string
	newPointName(pointCollectionName),
	newHitName(hitCollectionName);

    fHitCollectionNames.push_back(newHitName);
    fPointCollectionNames.push_back(newPointName);
}

void SpdSttMatchTracks::AddHitCollection(char const *hitCollectionName, char const *pointCollectionName)
{
    // Get and check FairRootManager
    FairRootManager
	*ioman = FairRootManager::Instance();
    
    if (!ioman) 
    {
	cout << "-E- SpdSttFindTracks::AddHitCollection: "
	     << "RootManager not instantised!" << endl;
    }
    
    // Get hit Array
    TClonesArray
	*fHitArray = (TClonesArray*) ioman->GetObject(hitCollectionName);

    if (!fHitArray) 
    {
	cout << "-W- SpdSttFindTracks::AddHitCollection: No " << hitCollectionName << " STT hit array!"
	     << endl;
    }

    // Get point Array
    TClonesArray
	*fPointArray = (TClonesArray*) ioman->GetObject(pointCollectionName);

    if (!fPointArray) 
    {
	cout << "-W- SpdSttFindTracks::AddHitCollection: No " << pointCollectionName << " STT hit array!"
	     << endl;
    }

    fHitCollectionList.Add(fHitArray); 
    fPointCollectionList.Add(fPointArray);
}

void SpdSttMatchTracks::AddAllCollections() 
{
    if (!fCollectionsComplete)
    {
	for (int counter = 0; counter < fHitCollectionNames.size(); counter++)
	{
	    AddHitCollection(fHitCollectionNames[counter].c_str(), fPointCollectionNames[counter].c_str());
	}
	fCollectionsComplete = kTRUE;
    }
}

SpdSttHit* SpdSttMatchTracks::GetHitFromCollections(Int_t hitCounter)
{
    SpdSttHit
	*retval = NULL;
 
    Int_t
	relativeCounter = hitCounter;

    for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++)
    {
	Int_t
	    size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

	if (relativeCounter < size)
	{
	    retval = (SpdSttHit*) ((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter);
	    break;
	}
	else
	{
	    relativeCounter -= size;
	}
    }
    return retval;
}

FairMCPoint* SpdSttMatchTracks::GetPointFromCollections(Int_t hitCounter)
{
    FairMCPoint
	*retval = NULL;
 
    Int_t
	relativeCounter = hitCounter;

    for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++)
    {
	Int_t
	    size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

	if (relativeCounter < size)
	{
	    Int_t
		tmpHit = ((SpdSttHit*) ((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter))->GetRefIndex();
	    
	    retval = (FairMCPoint*) ((TClonesArray *)fPointCollectionList.At(collectionCounter))->At(tmpHit);
	    
	    break;
	}
	else
	{
	    relativeCounter -= size;
	}
    }
    return retval;
}

// -----   Public method Finish   ------------------------------------------
void SpdSttMatchTracks::Finish() { }
// -------------------------------------------------------------------------


    
ClassImp(SpdSttMatchTracks)


      

      
