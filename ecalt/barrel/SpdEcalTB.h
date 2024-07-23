
#ifndef __SPDECALTB_H__
#define __SPDECALTB_H__

#include <TLorentzVector.h>
#include <TString.h>

#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTB                                                                  //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;

class SpdEcalTB: public SpdDetector { 

public:

    SpdEcalTB(const char* Name, Bool_t Active);
    SpdEcalTB();
  
    virtual ~SpdEcalTB();
  
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
    FairModule* CloneModule() const { return new SpdEcalTB(*this); }
 
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
    
   // SpdEcalTB(const SpdEcalTB&);
    SpdEcalTB& operator=(const SpdEcalTB&);
    
    /* GEOMETRY */
    DetVolume   fLargeModule[2];
         
    TGeoVolume* BuildModule(const Char_t* name, const char* mat, 
                Double_t length, Double_t hmin, Double_t hmax, Double_t lmin, Double_t lmax);

    void ConstructGeometry_1();
    void ConstructGeometry_2();
    
    Double_t GetCapacity(Int_t nmodule) const;
    Double_t GetMass(Int_t nmodule) const;
    Double_t GetDensity(Int_t nmodule) const;
    TString  GetMaterialName(Int_t nmodule) const;

    ClassDef(SpdEcalTB,1)
};

#endif /* __SPDECALTB_H__ */
