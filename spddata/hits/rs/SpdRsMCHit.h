// $Id$
// Author: artur   2021/03/02

#ifndef __SPDRSMCHIT_H__
#define __SPDRSMCHIT_H__

#include "SpdHit.h"
#include "SpdHitMCTruth.h"
#include "SpdDetectorList.h"
#include "SpdRsTID.h"
#include <TVector3.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsHitMCTruth                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsHitMCTruth: public SpdHitMCTruth {
    
public:

    SpdRsHitMCTruth():SpdHitMCTruth() {}
    virtual ~SpdRsHitMCTruth() {}
    
    virtual void clear();
    virtual void print() const;
   
protected:
    
    ClassDef(SpdRsHitMCTruth,1)
};

///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
// SpdRsMCHit                                                                    //
//                                                                               //
// RS hit's parameters, hits are produced from mc-points                         //
// Every wire hit produses a strip hit                                           //
// Some fields:                                                                  //
// fDetectorId    - detector id, subdectector id, submodule id, etc.             //
//                  (or strip id in case of strip-hit)                           //
// fNPoints       - number of mc-points which made a contribution to this hit    //
// fResp          - detector response value energy, GeV                          //
// fHitPos        - hit position in the frame (center of wire or strip-detector) //
// fHitType       - type of hit (wire or strip)                                  //
// fAssociatedHit - associated hit wire <-> strip                                //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////

class SpdRsMCHit: public SpdHit {

public:

    SpdRsMCHit();
    virtual ~SpdRsMCHit();
    
    virtual void Reset();
    
    void      RemoveMCTruth();  
    
    // ----- Setters -----
    
    Int_t     AddMCTruth(Int_t MotherId);

    void      SetHitType(Char_t hitType)    { fHitType = hitType;  }
    
    void      SetDetectorId(Int_t detId)    { fDetectorId = detId; }  
    void      SetNPoints(Double_t n)        { fNPoints = n;        }
    void      SetResp(Double_t resp)        { fResp = resp;        }
    void      SetHitPos(TVector3 pos)       { fHitPos = pos;       }

    void      SetHitPos(Double_t x, Double_t y, Double_t z) { fHitPos.SetXYZ(x,y,z); }
    
    void      AddResp(Double_t resp)        { fResp += resp; }
    void      AddPoint()                    { fNPoints++;    }

    void      SetAssociatedHit(Int_t hitId) { fAssociatedHit = hitId; }
    
    // ----- Getters -----
    
    Char_t    GetHitType()    const { return fHitType;    }

    Int_t     GetDetectorId() const { return fDetectorId; }  
    Int_t     GetNPoints()    const { return fNPoints;    }
    Double_t  GetResp()       const { return fResp;       }
    TVector3  GetHitPos()     const { return fHitPos;     } 

    Bool_t    IsBarrelHit()   const { return (fModId == kSpdRsTB);  } 
    Bool_t    IsEndcapHit()   const { return (fModId == kSpdRsTEC); } 
    
    Bool_t    IsWireHit()     const { return (fHitType == 'w') ? true : false; } 
    Bool_t    IsStripHit()    const { return (fHitType == 's') ? true : false; } 
    
    //>>> wire ID
    Int_t     GetSector()     const { return (fHitType == 'w') ? SpdRsTID::Sector(fDetectorId) : -1; }  // Barrel = 1..8, EC = 10 || 20
    Int_t     GetLayer()      const { return (fHitType == 'w') ? SpdRsTID::Layer(fDetectorId)  : -1; }  // Layers = 1..20
    Int_t     GetMDT()        const { return (fHitType == 'w') ? SpdRsTID::MDT(fDetectorId)    : -1; }  // MDT = 1..?
    Int_t     GetCell()       const { return (fHitType == 'w') ? SpdRsTID::Cell(fDetectorId)   : -1; }  // Cell = 1..8
    
    //>>> strip ID
    Int_t     GetStrip()      const { return (fHitType == 's') ? fDetectorId : -1; }

    Int_t     GetAssociatedHit() const { return fAssociatedHit; }
    
    /* MC-PARTICLES INFO */
    SpdRsHitMCTruth* GetMCTruth() const { return fMCTruth; }
   
    // Shortcuts to mc-truth 
    Bool_t    IsSingleParticleHit()           const { return (fMCTruth) ? (fMCTruth->GetN() == 1) :  0; } 
    Int_t     GetParticleId(Int_t i = 0)      const { return (fMCTruth) ? fMCTruth->GetMother(i)  : -1; }
    Int_t     GetParticleEntries(Int_t i = 0) const { return (fMCTruth) ? fMCTruth->GetEntries(i) : -1; }
    Int_t     GetNEntryParticles()            const { return (fMCTruth) ? fMCTruth->GetN() : 0;         } 
   
    // Print
    virtual void Print(Option_t* option = "") const;
    
private:
    
    Char_t      fHitType;       // 'w' (wire), 's' (strip), 'n' (undefined)
    
    Int_t       fDetectorId;    // detector id, subdectector id, submodule id, etc. (or strip id in case of strip-hit) 
    Int_t       fNPoints;       // a number of mc-points which made a contribution to this hit
    Double_t    fResp;          // detector response value energy, GeV    
    TVector3    fHitPos;        // hit position in the frame
    
    Int_t       fAssociatedHit; // associated hit: wire <-> strip
    
    //-------------------------------------------------------------------
    // list of pairs [particle id, number of entering],
    // which made a contribution to this hit
    
    SpdRsHitMCTruth*  fMCTruth; 
    
    ClassDef(SpdRsMCHit,1)
};

#endif  /* __SPDRSMCHIT_H__ */

