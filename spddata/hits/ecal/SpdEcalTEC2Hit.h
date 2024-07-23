// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTEC2HIT_H__
#define __SPDECALTEC2HIT_H__

#include "FairHit.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTEC2Hit                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTEC2Hit : public FairHit {

public:
   
    SpdEcalTEC2Hit();
    SpdEcalTEC2Hit(Int_t detID, Int_t endcapID, Int_t moduleID, Int_t cellID, TString nodePath);
   
    virtual ~SpdEcalTEC2Hit();

    void AddEnergy(Double_t edep) { fRawEnergy += edep; };
    void AddMCEnergy(Double_t edep) { fMCEnergy += edep; };
    
    void SetDetectorID(Int_t detID) { fDetectorID = detID; };
    void SetEndcapID(Int_t endcapID) { fEndcapID = endcapID; };
    void SetModuleID(Int_t moduleID) { fModuleID = moduleID; };
    void SetCellID(Int_t cellID) { fCellID = cellID; };
    void SetPosition(TVector3 pos) { fPos = pos; };
    void ScaleEnergy(Double_t coeff) { fRecoEnergy = fRawEnergy*coeff; };
    
    Double_t GetRawEnergy() { return fRawEnergy; };
    Double_t GetEnergy() { return fRecoEnergy; };
    Double_t GetMCEnergy() { return fMCEnergy; };
    Int_t GetEndcapID() { return fEndcapID; };
    Int_t GetModuleID() { return fModuleID; };
    Int_t GetCellID() { return fCellID; };
    TString GetNodePath() { return fNodePath; };
    TVector3 GetCellPosition() { return fPos; };
    
    void Print(const Option_t* opt = 0) const;
    
private:

    Int_t   fEndcapID;
    Int_t   fModuleID;
    Int_t   fCellID;
    
    Double_t fRawEnergy;
    Double_t fRecoEnergy;
    Double_t fMCEnergy;
    
    TString fNodePath;
    TVector3 fPos;
    
    ClassDef(SpdEcalTEC2Hit,1)
};

#endif  /* __SPDECALTEC2HIT_H__ */

