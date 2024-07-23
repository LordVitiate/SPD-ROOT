// $Id$
// Author: artur   2021/03/04

#ifndef __SPDECALMCHIT_H__
#define __SPDECALMCHIT_H__

#include "SpdHit.h"
#include "SpdHitMCTruth.h"
#include "SpdDetectorList.h"
#include <TVector3.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalHitMCTruth                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalHitMCTruth: public SpdHitMCTruth {
    
public:

    SpdEcalHitMCTruth():SpdHitMCTruth() {}
    virtual ~SpdEcalHitMCTruth() {}
    
    virtual void clear();
    virtual void print() const;
    
    // return particle local number
    virtual Int_t Add(Int_t MotherId);                
    virtual Int_t Add(Int_t MotherId, Double_t edep);  
    virtual Int_t Add(Int_t MotherId, Bool_t& is_new);   
    virtual Int_t Add(Int_t MotherId, Double_t edep, Bool_t& is_new); 
   
    virtual Int_t GetEdep(Int_t i)  const { return (i < fEdep.size()) ? fEdep[i] : 0.0; } 
    
    const std::vector<Int_t>&    Parts() const { return fTruth; }
    const std::vector<Double_t>& Edep()  const { return fEdep;  }
    
    void GetDataFast(const Int_t& i, Int_t& id, Double_t& ed) { id = fTruth[i*2]; ed = fEdep[i]; }
   
protected:
    
    std::vector<Double_t> fEdep; // particle energy deposit, GeV
    
    ClassDef(SpdEcalHitMCTruth,1)
};

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalMCHit                                                               //
//                                                                            //
// Describes a cell, the smallest "unit of information" in ECAL.              //
// Contains information on energy deposition and the position of the cell.    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalMCHit: public SpdHit {

public:

    SpdEcalMCHit();
    virtual ~SpdEcalMCHit();
    
    virtual void Reset();
    
    void      RemoveMCTruth();  
    
    // ----- Setters -----
    virtual void SetModId(Int_t id);
     
    void      SetEnergy(Double_t e)      { fRCEnergy = e; }
    void      SetMCEnergy(Double_t e)    { fMCEnergy = e; }
    void      SetPosition(TVector3 pos)  { fPosition = pos; }
    void      SetNPoints(Int_t n)        { fNPoints = n;  }
    
    void      SetPosition(Double_t x, Double_t y, Double_t z) { fPosition.SetXYZ(x,y,z); }
    
    void      AddMCEnergy(Double_t edep) { fMCEnergy += edep; }
    void      AddEnergy(Double_t edep)   { fRCEnergy += edep; }
    void      AddPoint()                 { fNPoints++; }
    
              /* --- DETECTOR  --- */
                        
    void      SetBasketID(Int_t id)      { if (fModId == kSpdEcalTB)  fDetId[0] = id; } // barrel 
    void      SetEndcapID(Int_t id)      { if (fModId == kSpdEcalTEC) fDetId[0] = id; } // endcap
    void      SetModuleZID(Int_t id)     { if (fModId == kSpdEcalTB)  fDetId[1] = id; } // barrel
    void      SetModulePhiID(Int_t id)   { if (fModId == kSpdEcalTB)  fDetId[2] = id; } // barrel
    void      SetModuleID(Int_t id)      { if (fModId == kSpdEcalTEC) fDetId[1] = id; } // endcap
    void      SetCellID(Int_t id)        {      if (fModId == kSpdEcalTB)  fDetId[3] = id; 
                                           else if (fModId == kSpdEcalTEC) fDetId[2] = id; }  // barrel, endcap 
    
    /* MC-PARTICLES INFO */
    Int_t     AddMCTruth(Int_t MotherId, Double_t edep);
    
    // ----- Getters -----
    Double_t  GetEnergy()      const { return fRCEnergy; }
    Double_t  GetMCEnergy()    const { return fMCEnergy; }
    TVector3  GetPosition()    const { return fPosition; } 
    Int_t     GetNPoints()     const { return fNPoints;  }
    
    Double_t  GetPositionR()   const { return fPosition.Perp(); }
    Double_t  GetPositionZ()   const { return fPosition.Z();    }
    
              /* --- DETECTOR  --- */
          
    Bool_t    IsBarrelHit()    const { return (fModId == kSpdEcalTB);  } 
    Bool_t    IsEndcapHit()    const { return (fModId == kSpdEcalTEC); }           
    
    Int_t     GetBasketID()    const { return (fModId == kSpdEcalTB)  ? fDetId[0] : -1; } // barrel 
    Int_t     GetEndcapID()    const { return (fModId == kSpdEcalTEC) ? fDetId[0] : -1; } // endcap
    Int_t     GetModuleZID()   const { return (fModId == kSpdEcalTB)  ? fDetId[1] : -1; } // barrel 
    Int_t     GetModulePhiID() const { return (fModId == kSpdEcalTB)  ? fDetId[2] : -1; } // barrel 
    Int_t     GetModuleID()    const { return (fModId == kSpdEcalTEC) ? fDetId[1] : -1; } // endcap
    Int_t     GetCellID()      const { if (fModId == kSpdEcalTB)  return fDetId[3]; 
                                       if (fModId == kSpdEcalTEC) return fDetId[2]; 
                                       return -1; }  // barrel, endcap 
    /* MC-PARTICLES INFO */
    SpdEcalHitMCTruth* GetMCTruth() const { return fMCTruth; }
    
    // Shortcuts to mc-truth 
    Bool_t    IsSingleParticleHit()           const { return (fMCTruth) ? (fMCTruth->GetN() == 1) :  0; } 
    Int_t     GetParticleId(Int_t i = 0)      const { return (fMCTruth) ? fMCTruth->GetMother(i)  : -1; }
    Int_t     GetParticleEntries(Int_t i = 0) const { return (fMCTruth) ? fMCTruth->GetEntries(i) : -1; }
    Int_t     GetNEntryParticles()            const { return (fMCTruth) ? fMCTruth->GetN() : 0;         } 
   
    // Print
    void Print(const Option_t* opt = 0) const;

private:
    
    Double_t  fMCEnergy;    // total energy deposit (is taken from MC)    
    
    Double_t  fRCEnergy;    // total energy deposit (corrected)
    TVector3  fPosition;    // hit position in the global CS
    Int_t     fNPoints;     // a number of mc-point which made a contribution to this hit
   
    std::vector<Int_t> fDetId; // detector id, subdectector id, submodule id, etc. 
    
    //-------------------------------------------------------------------
    // list of pairs [particle id, number of entering],
    // which made a contribution to this hit
    
    SpdEcalHitMCTruth*  fMCTruth; 
    
    ClassDef(SpdEcalMCHit,1)
};


 
#endif  /* __SPDECALMCHIT_H__ */

