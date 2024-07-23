// $Id$
// Author: andre/artur   2020/07/31

#ifndef __SPDECALTEC2_H__
#define __SPDECALTEC2_H__

#include <TLorentzVector.h>
#include <TString.h>
#include "FairDetector.h"
#include "SpdDetector.h"
#include "TGeoManager.h"
#include "TMatrixD.h"
#include <vector>

using std::vector;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTEC2                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTEC2Point;
class SpdEcalTEC2Hits;

class FairVolume;
class TClonesArray;
class TGeoMedium;
class TParticle;

class SpdEcalTEC2: public SpdDetector { 

public:

    SpdEcalTEC2(const char* Name, Bool_t Active);
    SpdEcalTEC2();
  
    virtual ~SpdEcalTEC2();
  
    virtual void  Initialize();
    virtual void  Reset();
    virtual void  Register();
    
    virtual Bool_t ProcessHits(FairVolume* v = 0);
    
    virtual TString       GetDataOut(Int_t n) const;
    virtual TClonesArray* GetCollection(Int_t iColl) const;
    
    virtual Double_t GetCapacity() const;  // cm^3
    virtual Double_t GetMass()     const;  // g
    virtual Double_t GetDensity()  const;  // g/cm^3
     
    virtual SpdGeoMapper* GetMapper();

    virtual void  ConstructGeometry();
     
    virtual void  EndOfEvent();
    virtual void  FinishRun(); 
    
    virtual Bool_t LoadParsFrom(SpdParSet* params);
    FairModule* CloneModule() const { return new SpdEcalTEC2(*this); }
 
    virtual void  Print(Option_t*) const;
    
    void EnableWritingPointsScint(Bool_t enable = true) { fEnableWritingPointsScint = enable; }
    void EnableWritingPointsAll(Bool_t enable = true)   { fEnableWritingPointsScint = enable; 
                                                          fEnableWritingPointsAll   = enable; }
    
    TGeoVolume* GetEndcap() const { return fEndcap; } 
    TGeoVolume* GetModule() const { return fModule; }
    TGeoVolume* GetCell()   const { return fCell;   }
    
private:
       
    void AddHit();
  
    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fTrackID;           //!  track index
    Int_t          fDetID;             //!  detector ID
    Int_t          fEndcapID;          //!  endcap ID (+1 for +Z, -1 for -Z)
    Int_t          fModuleID;          //!  module ID
    Int_t          fCellID;            //!  cell ID
    Int_t          fLayerID;           //!  layer ID
    TString        fMatID;             //!  material name ("LEAD" etc. - if writing points in absorber is enabled or "SCINT" etc.)
    TLorentzVector fPos;               //!  position at entrance
    TLorentzVector fMom;               //!  momentum at entrance
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss
    TString        fNodePath;          //!

    Bool_t  fEnableWritingPointsScint;  
    Bool_t  fEnableWritingPointsAll;
    
    /* containers for data  */
    
    TString        fOutDataPointObject; //!
    TClonesArray*  fPointCollection;    //!
    
    //SpdEcalTEC2(const SpdEcalTEC2&);
    SpdEcalTEC2& operator=(const SpdEcalTEC2&);
    
    /* GEOMETRY */
    
    void BuildGeometry();
    
    void BuildEndcap();
    void BuildModule();
    void BuildCell();

    TGeoVolume* fEndcap;  
    TGeoVolume* fModule; 
    TGeoVolume* fCell;  

    TString fAbsorberVolName;
    TString fScintVolName; 

    ClassDef(SpdEcalTEC2,1)
};

#endif /* __SPDECALTEC2_H__ */
