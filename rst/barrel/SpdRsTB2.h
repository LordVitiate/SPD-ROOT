
#ifndef __SPDRSTB2_H__
#define __SPDRSTB2_H__

#include <TLorentzVector.h>
#include <TString.h>

#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTB2                                                                   //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;

class SpdRsTB2: public SpdDetector { 

public:

    SpdRsTB2(const char* Name, Bool_t Active);
    SpdRsTB2();
  
    virtual ~SpdRsTB2();
  
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
    FairModule* CloneModule() const { return new SpdRsTB2(*this); }
 
    virtual void  Print(Option_t*) const;
      
private:
  
    void AddHit();
    
    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id (Detector Id)
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
    
   // SpdRsTB2(const SpdRsTB2&);
    SpdRsTB2& operator=(const SpdRsTB2&);
    
    /* GEOMETRY */
    
    TGeoVolume* fModule; //!
         
    TGeoVolume* BuildModule(const Char_t* name, TString mat, 
                Double_t length, Double_t hmin, Double_t hmax, Double_t lmin, Double_t lmax);

    TGeoVolume* CreateBottomLayer(Double_t rsSize, Double_t rsThickness, Double_t rsLength);
    TGeoVolume* CreateTopLayer(Int_t layerN, Double_t rsWidth, Double_t rsThickness, Double_t rsLength);
    TGeoVolume* CreateRegularLayer(Int_t layerN, Double_t rsSize, Double_t rsThickness, Double_t rsLength);
    TGeoVolume* CreateMDT(Double_t length);

    void ConstructDetector();
    
    TString GetMaterialName() const;
    
    Int_t   fBaseColor;   // (air)
    Int_t   fAbsorbColor; // (iron)
    
    ClassDef(SpdRsTB2,1)
};

#endif /* __SPDRSTB2_H__ */
