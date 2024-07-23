// $Id$
// Author: andre/artur   2020/07/31

#ifndef __SPDECALTB2_H__
#define __SPDECALTB2_H__

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
// SpdEcalTB2                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTB2Point;
class SpdEcalTB2Hits;

class FairVolume;
class TClonesArray;
class TGeoMedium;
class TParticle;

class SpdEcalTB2: public SpdDetector { 

public:

    SpdEcalTB2(const char* Name, Bool_t Active);
    SpdEcalTB2();
  
    virtual ~SpdEcalTB2();
  
    virtual void  Initialize();
    virtual void  Reset();
    virtual void  Register();
    
    virtual Bool_t ProcessHits(FairVolume* v = 0);
    
    virtual TString       GetDataOut(Int_t n) const;
    virtual TClonesArray* GetCollection(Int_t iColl) const;
        
    virtual SpdGeoMapper* GetMapper();

    virtual void  ConstructGeometry();
     
    virtual void  EndOfEvent();
    virtual void  FinishRun(); 
    
    virtual Bool_t LoadParsFrom(SpdParSet* params);
    FairModule* CloneModule() const { return new SpdEcalTB2(*this); }

    virtual void  Print(Option_t*) const;
    
    void EnableWritingPointsScint(Bool_t enable = true) { fEnableWritingPointsScint = enable; }
    void EnableWritingPointsAll(Bool_t enable = true)   { fEnableWritingPointsScint = enable; 
                                                          fEnableWritingPointsAll   = enable; }
        
    TGeoVolume* GetBasket() const { return fBasket; }
    TGeoVolume* GetModule(Int_t i) const;
    
    Int_t GetNModules() { return fUniqueModules.size(); }
      
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
    
   // SpdEcalTB2(const SpdEcalTB2&);
    SpdEcalTB2& operator=(const SpdEcalTB2&);
    
    /* GEOMETRY */
    
    void     ConstructGeometry_1();
        
    TMatrixD GetMatrixToRotateVectorTriplet(TVector3 initialVector1, TVector3 initialVector2, TVector3 initialVector3, 
                                            TVector3 finalVector1,   TVector3 finalVector2,   TVector3 finalVector3);
    
    TMatrixD GetMatrixToEmplaceModule(Double_t ax, Double_t ay, Bool_t rotateClockwise, TGeoVolume* moduleVolume);
    
    void     GetEmplacedModulePosition(TGeoVolume* moduleVolume, 
                                       Double_t ax, Double_t ay, Double_t arhs, 
                                       Double_t ax2, Double_t ay2, Double_t arhs2, 
                                       Double_t & xMiddle, Double_t & yMiddle, 
                                       Double_t & ax_new, Double_t & ay_new, Double_t & arhs_new, 
                                       Double_t & xPlane2Bottom, Double_t & yPlane2Bottom, 
                                       Double_t & xPlane2Top, Double_t & yPlane2Top, 
                                       Int_t rotateZdirection, Int_t deltaXdirection, Int_t deltaYdirection);
    
    void FillBasketFixedCells(TGeoVolume* basketVolume, TGeoVolume* moduleVolume);
    void FillBasketTightly(TGeoVolume* basketVolume);
    
    pair <TGeoCombiTrans*, TGeoVolume*> StackModuleInSector(TGeoVolume* basketVolume, 
                                                            Double_t angleModuleCenter, Double_t angleModulePeriph, 
                                                            Double_t moduleInnerSizeTheta, Int_t imod);

    TGeoVolume* BuildModule();
    
    void FillCell(TGeoVolume* cell);
    
    void BuildModule(TGeoVolume* module);
    void BuildBasket();
    void BuildBarrel();
    
    Double_t LinearInterp(Double_t x1, Double_t x2, Double_t t);

    TGeoVolume* fBasket;                
    TGeoVolume* fModule;               
    vector<TGeoVolume*> fUniqueModules;

    TString fAbsorberVolName;
    TString fScintVolName; 

    ClassDef(SpdEcalTB2,1)
};


#endif /* __SPDECALTB2_H__ */
