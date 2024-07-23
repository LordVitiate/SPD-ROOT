
#ifndef __TESTDETECTOR_H__
#define __TESTDETECTOR_H__

#include "FairDetector.h"

#include <TString.h>
#include <TLorentzVector.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TestDetector                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;
class TGeoVolume;
class TGeoMedium;

class TestDetector: public FairDetector {
  
public: 

    TestDetector();
    TestDetector(const char* Name, Bool_t Active);
    
    virtual ~TestDetector();

    virtual void  Initialize();
    virtual void  Reset();
    virtual void  Register();
    
    // Create geometry
    virtual void ConstructGeometry();
 
    // Simulations
    virtual Bool_t ProcessHits(FairVolume* v = 0);
    
    virtual void EndOfEvent();
  
    // Access methods
    virtual TClonesArray* GetCollection(Int_t iColl) const 
            { return (iColl == 0) ? fTestDetectorPointCollection : NULL; }
            
 
    // --- NOT USED ---
    virtual void   CopyClones( TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {}
    virtual void   SetSpecialPhysicsCuts() {}
   
    virtual void   FinishPrimary() {}
    virtual void   FinishRun()     {}
    virtual void   BeginPrimary()  {}
    virtual void   PostTrack()     {}
    virtual void   PreTrack()      {}
    virtual void   BeginEvent()    {}

protected:
    
    Int_t          fTrackID;           //  track index
    Int_t          fVolumeID;          //  volume id
    TLorentzVector fPos;               //  position at entrance
    TLorentzVector fMom;               //  momentum at entrance
    Double32_t     fTime;              //  time
    Double32_t     fLength;            //  length
    Double32_t     fELoss;             //  energy loss

    TClonesArray*  fTestDetectorPointCollection;  //!
    
    TGeoVolume*    fMasterVol;
    TGeoMedium*    fMasterMedium;

    TestDetector(const TestDetector&);
    TestDetector& operator=(const TestDetector&);

    ClassDef(TestDetector,1)
};


#endif /*__TESTDETECTOR_H__ */
