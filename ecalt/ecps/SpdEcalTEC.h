
#ifndef __SPDECALTEC_H__
#define __SPDECALTEC_H__

#include <TLorentzVector.h>
#include <TString.h>

#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTEC                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;

class SpdEcalTEC: public SpdDetector { 

public:

    SpdEcalTEC(const char* Name, Bool_t Active);
    SpdEcalTEC();
  
    virtual ~SpdEcalTEC();
  
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
    FairModule* CloneModule() const { return new SpdEcalTEC(*this); }
 
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
    
    //SpdEcalTEC(const SpdEcalTEC&);
    SpdEcalTEC& operator=(const SpdEcalTEC&);
    
    /* GEOMETRY */
    
    DetVolume   fCap;
         
    void ConstructGeometry_1();
    void ConstructGeometry_2();
    
    Bool_t BuildCap();
    
    TString  GetBaseMaterialName() const;

    ClassDef(SpdEcalTEC,1)
};

#endif /* __SPDECALTEC_H__ */
