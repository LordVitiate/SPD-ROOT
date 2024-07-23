
#ifndef __SPDTSSB_H__
#define __SPDTSSB_H__

#include <TLorentzVector.h>
#include <TString.h>

#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsSB                                                                    //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;

class SpdTsSB: public SpdDetector { 

public:

    SpdTsSB(const char* Name, Bool_t Active);
    SpdTsSB();
  
    virtual ~SpdTsSB();
  
    virtual void  Initialize();
    virtual void  Reset();
    virtual void  Register();
    
    virtual Bool_t ProcessHits(FairVolume* v = 0);
    
    virtual TString GetDataOut(Int_t n) const;
    virtual TClonesArray* GetCollection(Int_t iColl) const;
    
    virtual Double_t GetCapacity() const;  // cm^3
    virtual Double_t GetMass()     const;  // g
    virtual Double_t GetDensity()  const;  // g/cm^3
    
    virtual SpdGeoMapper*  GetMapper();
    virtual SpdGeoBuilder* GetBuilder();
    
    virtual void ConstructGeometry();
     
    virtual void  EndOfEvent();
    virtual void  FinishRun();
    
    virtual Bool_t LoadParsFrom(SpdParSet* params);
    FairModule* CloneModule() const { return new SpdTsSB(*this); }
    
    virtual void  Print(Option_t*) const;
        
private:
  
    void AddHit();
    
    /* GEOMETRY */
    
    Double_t GetCapacity(Int_t nmodule) const;
    Double_t GetMass(Int_t nmodule) const;
    Double_t GetDensity(Int_t nmodule) const;
    TString  GetMaterialName(Int_t nmodule) const;    
       
    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID1;         //!  volume id1
    Long_t         fVolumeID2;         //!  volume id2
    TLorentzVector fPosIn;             //!  position at entrance
    TLorentzVector fMomIn;             //!  momentum at entrance
    Double32_t     fTimeIn;            //!  time
    TLorentzVector fPosOut;            //!  position at exit
    TLorentzVector fMomOut;            //!  momentum at exit
    Double32_t     fTimeOut;           //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss

    /* containers for data */

    TString        fOutDataPointObject; //!
    TClonesArray*  fPointCollection;    //!
    
   // SpdTsSB(const SpdTsSB&);
    SpdTsSB& operator=(const SpdTsSB&);
    
    ClassDef(SpdTsSB,1)
};

#endif /* __SPDTSSB_H__ */
