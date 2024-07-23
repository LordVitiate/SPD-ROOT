// $Id$
// Author: artur   2021/29/11

#ifndef __SPDBBCMCHIT_H__
#define __SPDBBCMCHIT_H__

#include "SpdHit.h"
#include "SpdHitMCTruth.h"
#include "SpdDetectorList.h"
#include <TVector3.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdBbcMCHit                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdBbcMCHit: public SpdHit {

public:

    SpdBbcMCHit();
    virtual ~SpdBbcMCHit();
    
    virtual void Reset();
    
    void      RemoveMCTruth();  
    
    // ----- Setters -----
    
    /* MC-PARTICLES INFO */
    Int_t     AddMCTruth(Int_t MotherId);
    
    // void      SetDetId(Int_t id)               { fDetId = id;        }
    void      SetLocalModId(Int_t id)          { fLocalModId = id;        }
    void      SetRingId(Int_t id)              { fRingId = id;       }
    void      SetSectorId(Int_t id)            { fSectorId = id;     }

    void      SetHitPos(const TVector3& pos)   { fHitPos   = pos;    }
    void      SetTrackMom(const TVector3& mom) { fTrackMom = mom;    }
    void      SetTrackLen(Double_t len)        { fTrackLen = len;    }
    void      SetTrackSegLen(Double_t len)     { fTrackSegLen = len; }
    void      SetResp(Double_t resp)           { fResp    = resp;    }
    
    void      SetHitPos(Double_t x, Double_t y, Double_t z)      { fHitPos.SetXYZ(x,y,z); }
    void      SetTrackMom(Double_t px, Double_t py, Double_t pz) { fTrackMom.SetXYZ(px,py,pz); }
    
    // ----- Getters -----
    
    // Int_t     GetDetId()        const { return fDetId;       }
    Int_t     GetLocalModId()   const { return fLocalModId;       }
    Int_t     GetRingId()       const { return fRingId;      } 
    Int_t     GetSectorId()     const { return fSectorId;    } 
    TVector3  GetHitPos()       const { return fHitPos;      } 
    TVector3  GetTrackMom()     const { return fTrackMom;    } 
    Double_t  GetTrackLen()     const { return fTrackLen;    } 
    Double_t  GetTrackSegLen()  const { return fTrackSegLen; } 
    Double_t  GetResp()         const { return fResp;        }
    
    /* MC-PARTICLES INFO */
    SpdHitMCTruth* GetMCTruth() const { return fMCTruth; }
   
    // Shortcuts to mc-truth 
    Bool_t    IsSingleParticleHit()           const { return (fMCTruth) ? (fMCTruth->GetN() == 1) :  0; } 
    Int_t     GetParticleId(Int_t i = 0)      const { return (fMCTruth) ? fMCTruth->GetMother(i)  : -1; }
    Int_t     GetParticleEntries(Int_t i = 0) const { return (fMCTruth) ? fMCTruth->GetEntries(i) : -1; }
    Int_t     GetNEntryParticles()            const { return (fMCTruth) ? fMCTruth->GetN() : 0;         } 
   
    // Print
    virtual void Print(Option_t* option = "") const;
    
private:
    
    // Int_t       fDetId;        // detector id (-1 = z- detector, +1 = z+ detector)
    Int_t       fLocalModId;
    Int_t       fRingId;
    Int_t       fSectorId;
    TVector3    fHitPos;       // hit position (mc-track entrance point), cm
    TVector3    fTrackMom;     // mc-track momentum at entrance, GeV/c
    Double_t    fTrackLen;     // mc-track length at entrance since creation, cm
    Double_t    fTrackSegLen;  // length of mc-track segment inside the detector, cm
    Double_t    fResp;         // detector response value energy, GeV    
       
    //-------------------------------------------------------------------
    // list of pairs [particle id, number of entering],
    // which made a contribution to this hit
    
    SpdHitMCTruth*  fMCTruth; 
    
    ClassDef(SpdBbcMCHit,1)
};

#endif  /* __SPDBBCMCHIT_H__ */
