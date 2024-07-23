// $Id$
// Author: artur   2021/11/26

#ifndef __SPDBBC_H__
#define __SPDBBC_H__

#include <TLorentzVector.h>
#include <TString.h>

#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdBbc                                                                     //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;

class SpdBbc: public SpdDetector { 

public:

    SpdBbc(const char* Name, Bool_t Active);
    SpdBbc();
  
    virtual ~SpdBbc();
  
    virtual void  Initialize();
    virtual void  Reset();
    virtual void  Register();
    
    virtual Bool_t ProcessHits(FairVolume* v = 0);
    
    virtual TString GetDataOut(Int_t n) const;
    virtual TClonesArray* GetCollection(Int_t iColl) const;
    
    virtual Double_t GetCapacity() const;  // cm^3
    virtual Double_t GetMass()     const;  // g
    virtual Double_t GetDensity()  const;  // g/cm^3
    
    virtual SpdGeoMapper* GetMapper();
    
    virtual void ConstructGeometry();
     
    virtual void  EndOfEvent();
    virtual void  FinishRun(); 
    
    virtual Bool_t LoadParsFrom(SpdParSet* params);
    
    FairModule* CloneModule() const { return new SpdBbc(*this); }
 
    virtual void  Print(Option_t*) const;
      
private:
  
    void AddHit();
    
    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id (Detector Id)
    Int_t          fLocalModuleID;     //!  module id (z+, z-)
    Int_t          fRingID;
    Int_t          fSectorID;
    TString        fNodePath;
    TString        fVolumePath;        //!  full path to current volume
    TLorentzVector fPos;               //!  position at entrance
    TLorentzVector fMom;               //!  momentum at entrance
    TLorentzVector fPosOut;            //!  position at exit
    TLorentzVector fMomOut;            //!  momentum at exit
    Double32_t     fTime;              //!  time
    Double32_t     fTimeOut;           //!  time at exit
    Double32_t     fLength;            //!  length
    Double32_t     fSegmentLength;     //!  length of segment
    Double32_t     fELoss;             //!  energy loss

    /* containers for data */

    TString        fOutDataPointObject; //!
    TClonesArray*  fPointCollection;    //!
    
    // SpdBbc(const SpdBbc&);
    SpdBbc& operator=(const SpdBbc&);
    
    /* GEOMETRY */
    
    TGeoVolume* fModule1; //! z+ module
    TGeoVolume* fModule2; //! z- module
    
    void ConstructDetector();
    
    TString GetMaterialName() const;
    
    ClassDef(SpdBbc,1)
};

#endif /* __SPDBBC_H__ */
