// $Id$
// Author: artur   2019/20/02

#ifndef __SPDITS_H__
#define __SPDITS_H__

#include <TLorentzVector.h>
#include <TString.h>

#include <vector>

#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdIts                                                                     //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;
class SpdGeoVVolumeBox2D;
class SpdItsPoint;

class SpdIts: public SpdDetector { 

public:

    SpdIts(const char* Name, Bool_t Active);
    SpdIts();
  
    virtual ~SpdIts();
  
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
    virtual SpdGeoBuilder* GetBuilder();
    
    virtual void ConstructGeometry();
     
    virtual void  EndOfEvent();
    virtual void  FinishRun(); 
    
    virtual Bool_t LoadParsFrom(SpdParSet* params);
    FairModule* CloneModule() const { return new SpdIts(*this); }
 
    virtual void  Print(Option_t*) const;
      
private:
  
    void AddHit();
    
    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID1;         //!  volume id1
    TLorentzVector fPos;               //!  position at entrance
    TLorentzVector fMom;               //!  momentum at entrance
    TLorentzVector fPosOut;            //!  position at exit
    TLorentzVector fMomOut;            //!  momentum at exit
    Double_t       fTime;              //!  time
    Double_t       fLength;            //!  length
    Double_t       fELoss;             //!  energy loss
    
    SpdGeoVVolumeBox2D* fVVolume;      //!  current v-volume (if exists) 
    
    std::vector<Int_t>    fVid;        //!  set of v-detectors
    std::vector<Double_t> fVed;        //!  set of v-deposits
 
    /* containers for data */

    TString        fOutDataPointObject; //!
    TClonesArray*  fPointCollection;    //!
    
    //SpdIts(const SpdIts&);
    SpdIts& operator=(const SpdIts&);
    
    /* GEOMETRY */
          
    Int_t    GetNLayers() const;
    
    Double_t GetCapacity(Int_t nlayer) const;
    Double_t GetMass(Int_t nlayer) const;
    Double_t GetDensity(Int_t nlayer) const;
    
    ClassDef(SpdIts,1)
};

#endif /* __SPDITS_H__ */
