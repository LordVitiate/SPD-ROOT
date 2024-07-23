

#ifndef __SPDSTS_H__
#define __SPDSTS_H__

#include "FairDetector.h"

#include "TVector3.h"
#include "TLorentzVector.h"

class SpdStsPoint;
class FairVolume;
class TClonesArray;

class SpdSts: public FairDetector {

public:

    SpdSts();
    SpdSts(const char* Name, Bool_t Active);

    virtual ~SpdSts();

    virtual void   Initialize();
    virtual void   Reset();
    virtual void   Register();
 
 
    virtual Bool_t ProcessHits( FairVolume* v = 0);

    
    virtual TClonesArray* GetCollection(Int_t iColl) const ;
    
   
    void ConstructGeometry();


    SpdStsPoint* AddHit(Int_t trackID, Int_t detID,
                        TVector3 pos, TVector3 mom,
                        Double_t time, Double_t length,
                        Double_t eLoss);

 
    virtual void   CopyClones(TClonesArray* cl1,  TClonesArray* cl2 , Int_t offset) {}
    
    virtual void   SetSpecialPhysicsCuts() {}
   
    virtual void   FinishPrimary() {}
    virtual void   FinishRun()     {}
    virtual void   BeginPrimary()  {}
    virtual void   PostTrack()     {}
    virtual void   PreTrack()      {}
    virtual void   BeginEvent()    {}
    
    virtual void   EndOfEvent();

    TString FName(TString name);
  
private:
 
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id
    TLorentzVector fPos;               //!  position at entrance
    TLorentzVector fMom;               //!  momentum at entrance
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss

    /** container for data points */

    TClonesArray*  fSpdStsPointCollection;  //!

    SpdSts(const SpdSts&);
    SpdSts& operator=(const SpdSts&);

    ClassDef(SpdSts,1)
};

#endif //__SPDSTS_H__
