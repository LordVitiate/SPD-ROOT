
#ifndef __SPDSTTTRACKFINDER_H__
#define __SPDSTTTRACKFINDER_H__

#include "TObject.h"

class TClonesArray;

class SpdSttTrackFinder : public TObject {

public:

  SpdSttTrackFinder() : fVerbose(0) { }; 
  virtual ~SpdSttTrackFinder() {};
  
  virtual void Init() {};

  virtual Int_t DoFind(TClonesArray* trackArray, TClonesArray* trackCandArray) = 0; 
  virtual void  Finish() = 0;
  
  virtual void  AddHitCollection(TClonesArray* mHitArray, TClonesArray* mPointArray) = 0;
  virtual void  SetTubeArray(TClonesArray *tubeArray) = 0;

  void SetVerbose(Int_t verbose) { fVerbose = verbose; }
 
protected:
   
  Int_t fVerbose;      // Verbosity level

  ClassDef(SpdSttTrackFinder,1);
};

#endif
