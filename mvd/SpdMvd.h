// $Id$
// Author: artur   2021/11/12

#ifndef __SPDMVD_H__
#define __SPDMVD_H__

#include <TLorentzVector.h>
#include <TString.h>

#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMvd                                                                     //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;

class SpdMvd: public SpdDetector { 

public:

    SpdMvd(const char* Name, Bool_t Active);
    SpdMvd();
  
    virtual ~SpdMvd();
  
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
    FairModule* CloneModule() const { return new SpdMvd(*this); }
 
    virtual void  Print(Option_t*) const;
    
    virtual void  PrintGeometry(Option_t*);
       
private:
  
    void AddHit();
    
    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id (Detector Id)
    Int_t          fSubVolID;          //!  detector part number (0: barrel; -1: endcapZ-; +1 endcapZ+)     
    Int_t          fLayerID;           //!  layer number   
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
    
    // SpdMvd(const SpdMvd&);
    SpdMvd& operator=(const SpdMvd&);
    
    /* GEOMETRY */
    
    TGeoVolume* fModule; //!

    void ConstructBarrel();
    void ConstructEndcaps();
    
    TString GetMaterialName() const;
    
    TGeoMedium* MediumStrictly(TString mat);
    
    Double_t Density(TGeoMedium* medium);
    Double_t X0(TGeoMedium* medium);  // g/cm^3
    Double_t X0cm(TGeoMedium* medium);  // cm
    Double_t XdivX0(TGeoMedium* medium, Double_t x);  // X/X0
    
    ClassDef(SpdMvd,1)
};

#endif /* __SPDMVD_H__ */
