
#ifndef __SPDSTTHITINFO_H__
#define __SPDSTTHITINFO_H__ 

#include "TObject.h"

class SpdSttHitInfo: public TObject {

public:

  SpdSttHitInfo();
  SpdSttHitInfo(Int_t fileId, Int_t eventID, Int_t trackID,
		Int_t pointID, Bool_t isFake = kFALSE);

  virtual ~SpdSttHitInfo() { }
  
  virtual void Clear();
  virtual void Print(const Option_t* opt = 0) const;
   
  inline void SetFileID(Int_t id)  { fFileID  = id; }
  inline void SetEventID(Int_t id) { fEventID = id; }
  inline void SetTrackID(Int_t id) { fTrackID = id; }
  inline void SetPointID(Int_t id) { fPointID = id; }  
 
  inline void SetFake(Bool_t fake = kTRUE) { fIsFake = fake; }

  inline Int_t  FileNumber()  const { return fFileID;  }
  inline Int_t  EventNumber() const { return fEventID; }
  inline Int_t  TrackID()     const { return fTrackID; } 
  inline Int_t  PointID()     const { return fPointID; }
  inline Bool_t IsFake()      const { return fIsFake;  }

private:

  Int_t  fFileID;      // -1 (no file), 0 (data file), 1 (pileup)
  Int_t  fEventID;     // -1 (fake hit), n = 0,... current event number in data file or pileup
  Int_t  fTrackID;     // number of mc-track 
  Int_t  fPointID;     // number of mc-point 
  Bool_t fIsFake;      // indicator for fake hit

  ClassDef(SpdSttHitInfo,1);

};


#endif
