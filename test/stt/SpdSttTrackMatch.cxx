

#include "SpdSttTrackMatch.h"


// -----   Default constructor   -------------------------------------------
SpdSttTrackMatch::SpdSttTrackMatch() {
  fMCTrackID    = -1;
  fNofTrueHits  =  0;
  fNofWrongHits =  0;
  fNofFakeHits  =  0;
  fNofMCTracks  =  0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
SpdSttTrackMatch::SpdSttTrackMatch(Int_t mcTrackID, Int_t nTrue,
				   Int_t nWrong, Int_t nFake, 
				   Int_t nTracks) {
  fMCTrackID    = mcTrackID;
  fNofTrueHits  = nTrue;
  fNofWrongHits = nWrong;
  fNofFakeHits  = nFake;
  fNofMCTracks  = nTracks;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
SpdSttTrackMatch::~SpdSttTrackMatch() {}
// -------------------------------------------------------------------------


ClassImp(SpdSttTrackMatch)
