// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTB2HIT_H__
#define __SPDECALTB2HIT_H__

#include "FairHit.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTB2Hit                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTB2Hit : public FairHit {

public:

    SpdEcalTB2Hit();
    SpdEcalTB2Hit(Int_t detID, Int_t basketID, Int_t moduleZID, Int_t modulePhiID, Int_t cellID, TString nodePath);
    
    virtual ~SpdEcalTB2Hit();

    void AddEnergy(Double_t edep) { fRawEnergy += edep; };
    void AddMCEnergy(Double_t edep) { fMCEnergy += edep; };

    void SetDetectorID(Int_t detID) { fDetectorID = detID; };
    void SetBasketID(Int_t basketID) { fBasketID = basketID; };
    void SetModuleZID(Int_t moduleZID) { fModuleZID = moduleZID; };
    void SetModulePhiID(Int_t modulePhiID) { fModulePhiID = modulePhiID; };
    void SetCellID(Int_t cellID) { fCellID = cellID; };
    void SetPosition(TVector3 pos) { fPos = pos; };
    void ScaleEnergy(Double_t coeff) { fRecoEnergy = fRawEnergy*coeff; };
    
    
    Double_t GetRawEnergy() { return fRawEnergy; };
    Double_t GetEnergy() { return fRecoEnergy; };
    Double_t GetMCEnergy() { return fMCEnergy; };
    Int_t GetBasketID() { return fBasketID; };
    Int_t GetModuleZID() { return fModuleZID; };
    Int_t GetModulePhiID() { return fModulePhiID; };
    Int_t GetCellID() { return fCellID; };
    TString GetNodePath() { return fNodePath; };
    TVector3 GetCellPosition() { return fPos; };
    
    void Print(const Option_t* opt = 0) const;
    
private:

    Int_t   fBasketID; 
    Int_t   fModuleZID; 
    Int_t   fModulePhiID; 
    Int_t   fCellID;
    
    Double_t fRawEnergy;
    Double_t fRecoEnergy;
    Double_t fMCEnergy;
    
    TString fNodePath;
    TVector3 fPos;
    
    ClassDef(SpdEcalTB2Hit,1)
};

#endif  /* __SPDECALTB2HIT_H__ */

