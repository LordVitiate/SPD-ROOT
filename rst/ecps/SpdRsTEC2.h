
#ifndef __SPDRSTEC2_H__
#define __SPDRSTEC2_H__

#include <TLorentzVector.h>
#include <TString.h>

#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTEC2                                                                   //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;

class SpdRsTEC2: public SpdDetector { 

public:

    SpdRsTEC2(const char* Name, Bool_t Active);
    SpdRsTEC2();
  
    virtual ~SpdRsTEC2();
  
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
    FairModule* CloneModule() const { return new SpdRsTEC2(*this); }
 
    virtual void  Print(Option_t*) const;
      
private:
  
    void AddHit();
    
    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id (detector id)
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
    
    //SpdRsTEC2(const SpdRsTEC2&);
    SpdRsTEC2& operator=(const SpdRsTEC2&);
    
    /* GEOMETRY */

    TGeoVolume* fModule; //!  
    
    TGeoVolume* BuildModule(const char* name, TString mat,
                            Int_t nsectors, Double_t rsInnerRadius, Double_t rsOuterRadius, Double_t rsThickness);
    TGeoVolume* CreateFirstLayer(Double_t rsInnerRadius, Double_t rsOuterRadius, Double_t rsThickness);
    TGeoVolume* CreateRegularLayer(Int_t layerN, Double_t rsInnerRadius, Double_t rsOuterRadius, Double_t rsThickness);
    TGeoVolume* CreateLastLayer(Int_t layerN, Double_t rsInnerRadius, Double_t rsOuterRadius, Double_t rsThickness);
    TGeoVolume* CreateMDT(Double_t length);
     
    void ConstructDetector();
    
    TString  GetMaterialName() const;

    Int_t   fBaseColor;   // (air)     
    Int_t   fAbsorbColor; // (iron)

    ClassDef(SpdRsTEC2,1)
};

#endif /* __SPDRSTEC2_H__ */
