

#ifndef __SPDSTTMATCHTRACKS_H__
#define __SPDSTTMATCHTRACKS_H__

#include <vector>
#include <map>
#include "FairTask.h"
#include "SpdSttHit.h"
#include "FairMCPoint.h"

class TClonesArray;



class SpdSttMatchTracks : public FairTask
{

 public:

  /** Default constructor **/
  SpdSttMatchTracks();


  /** Constructor with verbosity level **/
  SpdSttMatchTracks(Int_t verbose);


  /** Constructor with name, title and verbosity
   **
   *@param name     Name of taks
   *@param title    Title of task   (default FairTask)
   *@param verbose  Verbosity level (default 1)
   **/
  SpdSttMatchTracks(const char* name, const char* title = "FairTask",
		    Int_t verbose = 1);


  /** Destructor **/
  virtual ~SpdSttMatchTracks();


  /** Intialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Execution **/
  virtual void Exec(Option_t* opt);


  /** Finishing */
  virtual void Finish();

  /** Add an hit collection to perform trackfinding on */
  void AddHitCollectionName(char *hitCollectionName, char *pointCollectionName);
  SpdSttHit* GetHitFromCollections(Int_t hitCounter);
  FairMCPoint* GetPointFromCollections(Int_t hitCounter);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

 private:
  void AddAllCollections(); 
  void AddHitCollection(char const *collectionName, char const *pointCollectionName);

  TClonesArray* fTrackCandidates;       // Array of SpdTrackCand 
  TClonesArray* fMatches;      // Array of SpdSttTrackMatch 

  /** Map from MCTrackID to number of common hits **/
  std::map<Int_t, Int_t> fMatchMap;

  /** Verbosity level **/
  Int_t fVerbose;

  Bool_t fCollectionsComplete;
 
  std::vector<std::string> fHitCollectionNames;
  std::vector<std::string> fPointCollectionNames;
  TList fHitCollectionList;
  TList fPointCollectionList;
 
  /** object persistence **/
  Bool_t  fPersistence; //!

  SpdSttMatchTracks(const  SpdSttMatchTracks& L);
  SpdSttMatchTracks& operator= (const  SpdSttMatchTracks&) {return *this;};

  ClassDef(SpdSttMatchTracks,1);
};

#endif
