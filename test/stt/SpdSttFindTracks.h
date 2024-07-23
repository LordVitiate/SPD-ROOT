
#ifndef __SPDSTTFINDTRACKS_H__
#define __SPDSTTFINDTRACKS_H__

#include "FairTask.h"
#include "SpdSttGeoPar.h"

#include <string>
#include <vector>

class SpdSttTrackFinder;
class TClonesArray;

class SpdSttFindTracks : public FairTask
{

 public:

  /** Default constructor **/
  SpdSttFindTracks();


  /** Standard constructor
   *@param finder   Pointer to STT track finder concrete class
   *@param verbose  Verbosity level
   **/
  SpdSttFindTracks(SpdSttTrackFinder* finder, Int_t verbose = 1);


  /** Constructor with name and title
   *@param name     Name of class
   *@param title    Task title
   *@param finder   Pointer to STT track finder concrete class
   *@param verbose  Verbosity level
   **/
  SpdSttFindTracks(const char* name, const char* title = "FairTask", 
		   SpdSttTrackFinder* finder = NULL, Int_t verbose = 1);


  /** Destructor **/
  virtual ~SpdSttFindTracks();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();

  void SetParContainers();

  /** Accessors **/
  SpdSttTrackFinder* GetFinder() { return fFinder; };
  Int_t GetNofTracks()           { return fNofTracks; };


  /** Set concrete track finder **/
  void UseFinder(SpdSttTrackFinder* finder) { fFinder = finder; };

  /** Add an hit collection to perform trackfinding on */
  void AddHitCollectionName(char *hitCollectionName, char *pointCollectionName);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

  /** set the helix hit production flag true or false **/
  void SetHelixHitProduction(Bool_t hhprod = kTRUE) { fHelixHitProduction = hhprod; }
   

 private:
  void AddAllCollections(); 
  void AddHitCollection(char const *collectionName, char const *pointCollectionName);

  SpdSttTrackFinder* fFinder;    // Pointer to TrackFinder concrete class
  TClonesArray* fTrackCandArray; // Output array of SpdTrackCand
  TClonesArray* fTrackArray;     // Output array of SpdTrack

  TClonesArray* fHelixHitArray;  // Output array of SpdSttHelixHit

  Int_t fNofTracks;              // Number of tracks created
  Int_t fVerbose;                // Verbosity level

  std::vector<std::string> fHitCollectionNames;
  std::vector<std::string> fPointCollectionNames;

  Bool_t fCollectionsComplete;

  /** object persistence **/
  Bool_t  fPersistence; //!

  /** production to file of helix hit from PR or not **/
  Bool_t fHelixHitProduction; //!

  Int_t fEventCounter; //!

  /** from parameters array of SpdSttTube **/  //  CHECK added
  TClonesArray* fTubeArray;
  
  SpdSttGeoPar *fSttParameters;  //  CHECK added
  
  ClassDef(SpdSttFindTracks,1);
};

#endif
