
#ifndef __SPDECALSEC_H__
#define __SPDECALSEC_H__

#include <TLorentzVector.h>
#include <TString.h>

#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalSEC                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;

class SpdEcalSEC: public SpdDetector { 

public:

    SpdEcalSEC(const char* Name, Bool_t Active);
    SpdEcalSEC();
  
    virtual ~SpdEcalSEC();
  
    virtual void  Initialize();
    virtual void  Reset();
    virtual void  Register();
    
    virtual Bool_t ProcessHits(FairVolume* v = 0);
    
    virtual TString GetDataOut(Int_t n) const;
    virtual TClonesArray* GetCollection(Int_t iColl) const;
    
    virtual Double_t GetCapacity() const;  // cm^3
    virtual Double_t GetMass()     const;  // g
    
    virtual SpdGeoMapper* GetMapper();

    virtual void ConstructGeometry();
     
    virtual void  EndOfEvent();
    virtual void  FinishRun(); 
    
    virtual Bool_t LoadParsFrom(SpdParSet* params);
    FairModule* CloneModule() const { return new SpdEcalSEC(*this); }
 
    virtual void  Print(Option_t*) const;
      
private:
       
    void AddHit();
  
    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID1;         //!  volume id1
    Int_t          fVolumeID2;         //!  volume id2
    Int_t          fVolumeID3;         //!  volume id3
    TLorentzVector fPos;               //!  position at entrance
    TLorentzVector fMom;               //!  momentum at entrance
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss

    /* containers for data  */
    
    TString        fOutDataPointObject; //!
    TClonesArray*  fPointCollection;    //!

    
    //SpdEcalSEC(const SpdEcalSEC&);
    SpdEcalSEC& operator=(const SpdEcalSEC&);
    
    /* GEOMETRY */
    
    DetVolume   fCap;
         
    void ConstructGeometry_1();
    
    Bool_t BuildCap();
    
    TString  GetBaseMaterialName() const;

    ClassDef(SpdEcalSEC,1)
};

#endif /* __SPDECALSEC_H__ */
