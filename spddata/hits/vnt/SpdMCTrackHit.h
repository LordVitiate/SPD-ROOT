// $Id$
// Author: artur   2021/02/03

#ifndef __SPDMCTRACKHIT_H__
#define __SPDMCTRACKHIT_H__

#include "SpdHit.h"
#include "SpdHitMCTruth.h"
#include "SpdDetectorList.h"
#include <TVector3.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCTrackHit                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCTrackHit: public SpdHit {

public:

    SpdMCTrackHit();
    virtual ~SpdMCTrackHit();
   
    virtual void Reset();
    
    virtual void RemoveMCTruth();
     
    // ======= Setters =======

    void      SetDetId(Int_t i, Long_t id);
    void      SetSpecifity(Char_t spec)    { fSpecifity = spec; }
    void      SetResp(Double_t resp)       { fResp = resp;      }    
    void      SetMCPointId(Int_t n)        { fMCPointId = n;    }
    void      SetMCPoint(TVector3 point)   { fMCPoint = point;  }
    
    void      SetMCPoint(Double_t x, Double_t y, Double_t z) { fMCPoint.SetXYZ(x,y,z); }

    /* MC PARTICLE INFO */
    Int_t     AddMCTruth(Int_t MotherId);
    
    // ======= Getters =======
    
    Long_t    GetDetId(Int_t i) const;
    Char_t    GetSpecifity()    const { return fSpecifity; }
    Double_t  GetResp()         const { return fResp;      }
    Int_t     GetMCPointId()    const { return fMCPointId; }
    TVector3  GetMCPoint()      const { return fMCPoint;   }
    
    void      GetMCPoint(TVector3& point) const { point = fMCPoint; }
   
    Double_t  GetPointX() const { return fMCPoint.X(); }
    Double_t  GetPointY() const { return fMCPoint.Y(); }
    Double_t  GetPointZ() const { return fMCPoint.Z(); }
   
    /* MC PARTICLE INFO */
    SpdHitMCTruth* GetMCTruth() const { return fMCTruth; }
    
    Bool_t    IsSingleParticleHit()           const { return (fMCTruth) ? (fMCTruth->GetN() == 1) :  0; } 
    Int_t     GetParticleId(Int_t i = 0)      const { return (fMCTruth) ? fMCTruth->GetMother(i)  : -1; }
    Int_t     GetParticleEntries(Int_t i = 0) const { return (fMCTruth) ? fMCTruth->GetEntries(i) : -1; }
    Int_t     GetNEntryParticles()            const { return (fMCTruth) ? fMCTruth->GetN() : 0;         }
    
    // Print
    virtual void Print(Option_t* option = "") const;
    
protected:

    std::vector<Long_t> fDetId;      // detector id, subdectector id, submodule id, etc. 
    Double_t            fResp;       // detector response value, GeV    
    TVector3            fMCPoint;    // corresponding mc-point coordinates (in global S.C.)
    
    Int_t               fMCPointId;  // mother mc-point number
    
    //----------------------------------------------------------------
    // Additional parameter "specifity" (hit type), for example: 
    // 'p'(pointlike), 'n'(undefined), ... 
    // for silicon detectors: "m" = MAPS, "s" = DSSD(strip);
    // for straw detectors: "v" = 1D-straw(R), "w" = 2D-starw(RZ);
    // ...
    Char_t              fSpecifity; 
    
    //-------------------------------------------------------------------
    // list of pairs [particle id, number of entering],
    // which made a contribution to this hit
    
    SpdHitMCTruth*      fMCTruth; 
    
    ClassDef(SpdMCTrackHit,1)
};


#endif  /* __SPDMCTRACKHIT_H__ */

