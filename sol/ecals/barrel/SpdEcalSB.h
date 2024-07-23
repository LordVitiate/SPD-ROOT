
#ifndef __SPDECALSB_H__
#define __SPDECALSB_H__

#include <TLorentzVector.h>
#include <TString.h>

#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalSB                                                                  //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;

class SpdEcalSB: public SpdDetector { 

public:
    
    SpdEcalSB(const char* Name, Bool_t Active);
    SpdEcalSB();
  
    virtual ~SpdEcalSB();
  
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
    FairModule* CloneModule() const { return new SpdEcalSB(*this); }
 
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
    
    //SpdEcalSB(const SpdEcalSB&);
    SpdEcalSB& operator=(const SpdEcalSB&);
    
    /* GEOMETRY */
    
    DetVolume   fSection;
         
    void ConstructGeometry_1();
    void ConstructGeometry_2();
    
    Bool_t BuildSection();
    
    TString  GetBaseMaterialName() const;

    ClassDef(SpdEcalSB,1)
};

#endif /* __SPDECALSB_H__ */
