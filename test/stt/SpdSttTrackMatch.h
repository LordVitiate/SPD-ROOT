
#ifndef __SPDSTTTRACKMATCH_H__
#define __SPDSTTTRACKMATCH_H__

#include "TObject.h"

class SpdSttTrackMatch : public TObject
{

 public:

  /** Default constructor **/
  SpdSttTrackMatch();


  /** Standard constructor 
  *@param mcTrackID   Index of matched MCTrack
  *@param nTrue       Good hits from matched MCTrack
  *@param nWrong      Good Hits from other MCTracks
  *@param nFake       Fake Hits
  *@param nTracks     Number of MCTracks with common points
  **/
  SpdSttTrackMatch(Int_t mcTrackID, Int_t nTrue, Int_t nWrong, 
		   Int_t nFake, Int_t nTracks);


  /** Destructor **/
  virtual ~SpdSttTrackMatch();


  /** Accessors **/
  Int_t GetMCTrackID()    { return fMCTrackID;    };
  Int_t GetNofTrueHits()  { return fNofTrueHits;  };
  Int_t GetNofWrongHits() { return fNofWrongHits; };
  Int_t GetNofFakeHits()  { return fNofFakeHits;  };
  Int_t GetNofMCTracks()  { return fNofMCTracks;  };


 private:

  /** Best matching SpdMCTrack  **/
  Int_t fMCTrackID;

  /** Number of good hits belonging to the matched MCTrack **/
  Int_t fNofTrueHits;

  /** Number of good hits belonging to other MCTracks **/
  Int_t fNofWrongHits;

  /** Number of fake hits **/
  Int_t fNofFakeHits;

  /** Number of MCTracks with common points **/
  Int_t fNofMCTracks;


  ClassDef(SpdSttTrackMatch,1);

};


#endif
				 
