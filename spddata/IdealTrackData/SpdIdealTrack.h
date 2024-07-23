// $Id$
// Author: artur   2019/09/06

#ifndef __SPDIDEALTRACK_H__
#define __SPDIDEALTRACK_H__

#include <TObject.h>
#include <TObjArray.h>
#include <TVector3.h>

class SpdIdealHit;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdIdealTrack                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

enum SpdTrackSelectionMark // unsigned integer marks
{ 
    kDefined = 0,
    kAccepted,
    kVertexParticle, 
    kIdentified,         
    kCharged,
    kMomentumCut,
    kBetaGammaCut,
    kVDHitsLim,
    kTHitsLim,
    kTBHitsLim,
    kTECHitsLim,
    kFullySpecified
};

class SpdIdealTrack: public TObject {

public:

    SpdIdealTrack();
    virtual ~SpdIdealTrack();
    
    void     ClearData();
    
    Int_t    AddHit(SpdIdealHit* hit);
    
    void     SetTrackId(Int_t id)        { fTrackId = id;       }
    void     SetMother1TrackId(Int_t id) { fMother1TrackId = id; }
    void     SetMother2TrackId(Int_t id) { fMother2TrackId = id; }
    
    void     SetPdgCode(Int_t pdg)    { fPdgCode = pdg; }
    void     SetCharge(Double_t q)    { fCharge  = q;   }
    void     SetVProcess(Int_t vp)    { fVProcess = vp; }
    void     SetStartTime(Double_t t) { fStartTime = t; }
    
    void     SetPrimVertex(TVector3 v)    { fPrimVertex = v; }
    void     SetStartMomentum(TVector3 p) { fStartMom   = p; }
    
    void     SetNPoints(Int_t npoints)  { fNPoints = npoints; }
    void     SetErrorFlag(Int_t error)  { fErrorFlag = error; }
    
    Int_t    GetTrackId()        const { return fTrackId;        }
    Int_t    GetMother1TrackId() const { return fMother1TrackId; }
    Int_t    GetMother2TrackId() const { return fMother2TrackId; }
    
    Int_t    GetPdgCode()        const { return fPdgCode;   }
    Double_t GetCharge()         const { return fCharge;    } 
    Int_t    GetVProcess()       const { return fVProcess;  } 
    Double_t GetStartTime()      const { return fStartTime; }
    
    TVector3 GetPrimVertex()     const { return fPrimVertex; }
    TVector3 GetStartMomentum()  const { return fStartMom;   }
    
    Double_t GetStartMomentumMag() const { return fStartMom.Mag();   }
    Double_t GetPrimVertexDist()   const { return fPrimVertex.Mag(); }
    
    Int_t    GetNPoints()       const { return fNPoints; }
    Int_t    GetErrorFlag()     const { return fErrorFlag; }
      
    Int_t        GetNHits()      const { return fHits->GetEntriesFast(); }
    TObjArray*   GetHits()       const { return fHits; }
    SpdIdealHit* GetHit(Int_t i);
    
    Double_t GetHitsEdep() const;
    
    static const Char_t* GetTrackSelectionMarkStr(UInt_t i); 
    static const Char_t* GetTrackSelectionMarkStr(SpdTrackSelectionMark mark); 
    
//     static void   SetMark(UInt_t& selection, SpdTrackSelectionMark mark);
//     static void   SetMark(UInt_t& selection, UInt_t mark);
//     static Bool_t GetMark(const UInt_t selection, SpdTrackSelectionMark mark);
//     static Bool_t GetMark(const UInt_t selection, UInt_t mark);
//     static Bool_t IsDefined(const UInt_t selection);
//     static Bool_t IsAccepted(const UInt_t selection);
//     static Bool_t IsFullySpecified(const UInt_t selection);
    
    virtual void  PrintTrack(Bool_t print_hits = kFALSE) const;

private:

    Int_t      fTrackId;        // MC-track ID
    Int_t      fMother1TrackId; // First mother mc-track ID
    Int_t      fMother2TrackId; // Second mother mc-track ID
    
    Int_t      fPdgCode;        // PDG particle code
    Double_t   fCharge;         // Particle charge
    Int_t      fVProcess;       // Initial process
    TVector3   fPrimVertex;     // Start vertex [cm]
    TVector3   fStartMom;       // Start momentum [GeV]
    Double_t   fStartTime;      // Start time [ns]
    
    Int_t      fNPoints;        // Number of mc-points
    Int_t      fErrorFlag;      // Error token (=0: no errors) 
    
    TObjArray* fHits;
 
    ClassDef(SpdIdealTrack,1)
};

#endif  /* __SPDIDEALTRACK_H__ */

