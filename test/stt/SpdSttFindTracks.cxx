
#include "SpdSttFindTracks.h"

#include "SpdSttTrackFinder.h"
#include "SpdTrackCand.h"
#include "SpdTrack.h"
#include "SpdSttHelixHit.h"
#include "SpdSttTube.h"
#include "SpdSttMapCreator.h"

#include "FairHit.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

// -----   Default constructor   -------------------------------------------
SpdSttFindTracks::SpdSttFindTracks() 
  : FairTask("STT Find Tracks") 
{
  fFinder      = NULL;
  fTrackCandArray  = NULL; 
  fHelixHitArray  = NULL; 
  fNofTracks   = 0;
  fVerbose     = 1;
  fCollectionsComplete = kFALSE;
  fPersistence = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
SpdSttFindTracks::SpdSttFindTracks(SpdSttTrackFinder* finder, 
				   Int_t verbose)
  : FairTask("STT Find Tracks") 
{
  fFinder      = finder;
  fTrackCandArray  = NULL; 
  fHelixHitArray  = NULL; 
  fNofTracks   = 0;
  fVerbose     = verbose;
  fCollectionsComplete = kFALSE;
  fPersistence = kTRUE;
  fHelixHitProduction = kFALSE;
}
// -------------------------------------------------------------------------



// -----   Constructor with name and title   -------------------------------
SpdSttFindTracks::SpdSttFindTracks(const char* name, const char* title, 
				   SpdSttTrackFinder* finder,
				   Int_t verbose) 
  : FairTask(name) 
{
  fFinder      = finder;
  fTrackCandArray  = NULL; 
  fHelixHitArray  = NULL; 
  fNofTracks   = 0;
  fVerbose     = verbose;
  fCollectionsComplete = kFALSE;
  fPersistence = kTRUE;
  fHelixHitProduction = kFALSE;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
SpdSttFindTracks::~SpdSttFindTracks() 
{
  fTrackCandArray->Delete(); 
  fHelixHitArray ->Delete();
  fHitCollectionNames.clear();  
  fPointCollectionNames.clear();
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus SpdSttFindTracks::Init() 
{
  fEventCounter = 0;

  cout << "+++++++++++++++++ InitStatus SpdSttFindTracks::Init()  +++++++++++++++++ " << endl;
  // Check for Track finder
  if (! fFinder) {
    cout << "-E- SpdSttFindTracks::Init: No track finder selected!" << endl;
    return kERROR;
  }

  FairRootManager
      *ioman = FairRootManager::Instance();
  
  if (!ioman) 
  {
      cout << "-E- SpdSttFindTracks::AddHitCollection: "
	   << "RootManager not instantised!" << endl;
      return kFATAL;
  }

  fTrackCandArray = new TClonesArray("SpdTrackCand",100); 
  ioman->Register("STTTrackCand", "STT", fTrackCandArray, fPersistence); 

  fTrackArray = new TClonesArray("SpdTrack",100); 
  ioman->Register("STTFoundTrack", "STT", fTrackArray, fPersistence); 
  
  fHelixHitArray = new TClonesArray("SpdSttHelixHit",100); 
  ioman->Register("STTPRHelixHit", "STT", fHelixHitArray, fHelixHitProduction);
  

  // Set verbosity of track finder
  fFinder->SetVerbose(fVerbose);
  
  cout << " +++++++++++++++++ SpdSttFindTracks::Init()  fHelixHitProduction =  " <<  fHelixHitProduction << endl;
  

  // helix hits? 
  fFinder->SetHelixHitProduction(fHelixHitProduction);
   
  // CHECK added 
  SpdSttMapCreator mapper(fSttParameters);
  fTubeArray = mapper.FillTubeArray();
  
  // Call the Init method of the track finder
  fFinder->Init();
  fFinder->SetTubeArray(fTubeArray);


  return kSUCCESS;
}

// CHECK added 
void SpdSttFindTracks::SetParContainers() {
  FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (SpdSttGeoPar*) rtdb->getContainer("SpdSttGeoPar");


}

// -------------------------------------------------------------------------
void SpdSttFindTracks::AddHitCollectionName(char *hitCollectionName, char *pointCollectionName)
{
//    string
//	newPointName(pointCollectionName),
//	newHitName(hitCollectionName);

//    fHitCollectionNames.push_back(newHitName);
//    fPointCollectionNames.push_back(newPointName);
    fHitCollectionNames.push_back(hitCollectionName);
    fPointCollectionNames.push_back(pointCollectionName);

  fFinder->SetInputBranchName(hitCollectionName); // this passes the HitBranch Name to
  						  //  SpdSttTrackFinderReal.

}

void SpdSttFindTracks::AddHitCollection(char const *hitCollectionName, char const *pointCollectionName)
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

    fFinder->AddHitCollection(fHitArray, fPointArray);
}

void SpdSttFindTracks::AddAllCollections() 
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

// -----   Public method Exec   --------------------------------------------
void SpdSttFindTracks::Exec(Option_t* opt) 
{
  // CHECK !!! IMPORTANT: event 0 (the first) 

  if(fVerbose) cout << "******* event # " << fEventCounter << "*******" << endl;
  fEventCounter++;
  
  AddAllCollections();
  
  fTrackCandArray->Delete(); 
  fTrackArray->Delete(); 
  fHelixHitArray->Delete(); 
  
  fNofTracks = fFinder->DoFind(fTrackCandArray, fTrackArray, fHelixHitArray); 
  
  
  for (Int_t iTrack=0; iTrack < fTrackCandArray->GetEntriesFast(); iTrack++) 
    { 
      SpdTrackCand* trackCand = (SpdTrackCand*) fTrackCandArray->At(iTrack);
      trackCand->Sort();
   }
}
// -------------------------------------------------------------------------



// -----   Public method Finish   ------------------------------------------
void SpdSttFindTracks::Finish() 
{
  fFinder->Finish();
}
// -------------------------------------------------------------------------




ClassImp(SpdSttFindTracks)
