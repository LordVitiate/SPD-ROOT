
#ifndef __SPDTOFEC_H__
#define __SPDTOFEC_H__

#include <TLorentzVector.h>
#include <TString.h>

#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTofEC                                                                   //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;

class SpdTofEC: public SpdDetector { 

public:

    SpdTofEC(const char* Name, Bool_t Active);
    SpdTofEC();
  
    virtual ~SpdTofEC();
  
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
    
    FairModule* CloneModule() const { return new SpdTofEC(*this); }
 
    virtual void  Print(Option_t*) const;
      
private:
  
    void AddHit();
    
    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id (Detector Id)
    Int_t          fModuleID;          //!  module id (Endcap z+, z-)
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
    
    // SpdTofEC(const SpdTofEC&);
    SpdTofEC& operator=(const SpdTofEC&);
    
    /* GEOMETRY */
    TGeoVolume* fModule;  //!
    TGeoVolume* fModule1; //! z+ endcap
    TGeoVolume* fModule2; //! z- endcap
    
    void ConstructDetector();
    void ConstructDetector_MRPC_Hybrid();
    
    TGeoVolume* BuildMRPCModule(Double_t mwidth1, Double_t mwidth2, Double_t mlength, Double_t msize);

    TString GetMaterialName() const;
    
    ClassDef(SpdTofEC,1)
};

#endif /* __SPDTOFEC_H__ */
