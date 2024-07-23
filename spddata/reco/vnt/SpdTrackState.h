// $Id$
// Author: artur   2021/02/10

#ifndef __SPDTRACKSTATE_H__
#define __SPDTRACKSTATE_H__

#include <TObject.h>
#include <TVector3.h>
#include <vector>
#include <TMatrixDSym.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTrackState                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTrackState: public TObject {

public:

    SpdTrackState():fTime(-1),fMomSigma(-1),fPosDelta(-1) {}
    SpdTrackState(const SpdTrackState& state);
    
    virtual ~SpdTrackState() {}
    
    void   ClearState();
    void   ClearCovariance() { fCovariance.ResizeTo(0,0); }

    // Setters
    void   SetMomPos(TVector3 mom, TVector3 pos) { fMomentum = mom; fPosition = pos; }
    void   SetMomentum(TVector3 mom) { fMomentum = mom; }
    void   SetPosition(TVector3 pos) { fPosition = pos; }
    void   SetTime(Double_t t)       { fTime = t; }
    void   SetCovariance(const TMatrixDSym& c /*6x6*/); 
    
    void   SetMomSigma(Double_t s)   { fMomSigma = s; }
    void   SetPosDelta(Double_t d)   { fPosDelta = d; }

    SpdTrackState& operator=(const SpdTrackState& state);
     
    // Getters
    TVector3           GetMomentum()   const { return fMomentum;   }
    TVector3           GetPosition()   const { return fPosition;   }
    Double_t           GetTime()       const { return fTime;       }
    const TMatrixDSym& GetCovariance() const { return fCovariance; }
    
    Int_t              GetCovSize()    const { return fCovariance.GetNrows();       } 
    Bool_t             IsCovDefined()  const { return (fCovariance.GetMatrixArray()) ? true : false; } 
    
    const TVector3*    Momentum()      const { return &fMomentum;   }
    const TVector3*    Position()      const { return &fPosition;   }
    const TMatrixDSym* Covariance()    const { return &fCovariance; }
    
    void      GetMomentum(TVector3& mom) const { mom = fMomentum; }
    void      GetPosition(TVector3& pos) const { pos = fPosition; }
    void      GetMomPos(TVector3& mom, TVector3& pos) const { mom = fMomentum, pos = fPosition; }
    Bool_t    GetCovariance(TMatrixDSym& c /*6x6*/) const;
    
    Double_t  GetMomSigma() const { return fMomSigma; }
    Double_t  GetPosDelta() const { return fPosDelta; }

    // Print
    virtual void Print(Option_t* option = "") const;

private:

    TVector3              fMomentum;    // track momentum [GeV/c]
    TVector3              fPosition;    // track position [cm]
    Double_t              fTime;        // track time [ns] 
    TMatrixDSym           fCovariance;  // covariation matrix of size (6x6) for vector (x,p)  
    
    Double_t              fMomSigma;    // sqrt(var) for track momentum
    Double_t              fPosDelta;    // fit position quality parameter
    
    ClassDef(SpdTrackState,1)
};

//------------------------------------------------------------------------
inline void SpdTrackState::SetCovariance(const TMatrixDSym& c /*6x6*/) 
{ 
    fCovariance.ResizeTo(c); 
    fCovariance = c;
} 
    
//------------------------------------------------------------------------
inline Bool_t SpdTrackState::GetCovariance(TMatrixDSym& c /*6x6*/) const 
{
    if (!IsCovDefined()) return false; 
    c.ResizeTo(fCovariance); 
    c = fCovariance;
    return true;
}

//------------------------------------------------------------------------
inline SpdTrackState& SpdTrackState::operator=(const SpdTrackState& state)
{
    fMomentum = state.fMomentum;
    fPosition = state.fPosition;
    fTime = state.fTime;
    fMomSigma = state.fMomSigma;
    fPosDelta = state.fPosDelta;
    
    fCovariance.ResizeTo(state.fCovariance); 
    fCovariance = state.fCovariance;
    
    return *this;
}

#endif  /* __SPDTRACKSTATE_H__ */

