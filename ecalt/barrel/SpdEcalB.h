// $Id$
// Author: andre/artur   2020/07/31

#ifndef __SPDECALB_H__
#define __SPDECALB_H__

#include <TLorentzVector.h>
#include <TString.h>
#include "FairDetector.h"
#include "SpdDetector.h"
#include "TGeoManager.h"
#include "TMatrixD.h"
#include <vector>
#include <tuple>
#include <set>

using std::vector;
using std::set;
using std::tuple;
using std::pair;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalB                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalBPoint;

class FairVolume;
class TClonesArray;
class TGeoMedium;
class TParticle;


class SpdEcalB: public SpdDetector { 

public:

    SpdEcalB(const char* Name, Bool_t Active);
    SpdEcalB();
  
    virtual ~SpdEcalB();
  
    virtual void  Initialize();
    virtual void  Reset();
    virtual void  Register();
    
    virtual Bool_t ProcessHits(FairVolume* v = 0);
    
    virtual TString       GetDataOut(Int_t n) const;
    virtual TClonesArray* GetCollection(Int_t iColl) const;
        
    virtual SpdGeoMapper* GetMapper();

    virtual void  ConstructGeometry();
    void  ConstructGeometryTDR();
     
    virtual void  EndOfEvent();
    virtual void  FinishRun(); 
    
    virtual Bool_t LoadParsFrom(SpdParSet* params);
    FairModule* CloneModule() const { return new SpdEcalB(*this); }

    virtual void  Print(Option_t*) const;
    
    void EnableWritingPointsScint(Bool_t enable = true) { fEnableWritingPointsScint = enable; }
    void EnableWritingPointsAll(Bool_t enable = true)   { fEnableWritingPointsScint = enable;
                                                          fEnableWritingPointsAll = enable; }
        
    TGeoVolume* GetBasket() const { return fBasket; }
    TGeoVolume* GetModule() const { return fModule; }
    
private:
       
    void AddHit();
  
    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fTrackID;           //!  track index
    Int_t          fDetID;             //!  detector id
    Int_t          fBasketID;          //!  basket id
    Int_t          fModuleZID;         //!  module Zid
    Int_t          fModulePhiID;       //!  module phi id
    Int_t          fCellID;            //!  cell id
    Int_t          fLayerID;           //!  layer number
    TString        fMatID;             //!  material
    TLorentzVector fPos;               //!  position at entrance
    TLorentzVector fMom;               //!  momentum at entrance
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss
    TString        fNodePath;          //!

    Bool_t fEnableWritingPointsScint;
    Bool_t fEnableWritingPointsAll;

    
    /* containers for data  */
    
    TString        fOutDataPointObject; //!
    TClonesArray*  fPointCollection;    //!
    
   // SpdEcalB(const SpdEcalB&);
    SpdEcalB& operator=(const SpdEcalB&);
    
    /* GEOMETRY */
    
    void FillCell(TGeoVolume* cell);
    
    Double_t LinearInterp(Double_t x1, Double_t x2, Double_t t);

    TGeoVolume* fBasket;                
    TGeoVolume* fModule;               

    TString fAbsorberVolName;
    TString fScintVolName; 

    ClassDef(SpdEcalB,1)
};


#endif /* __SPDECALB_H__ */
