// $Id$
// Author: artur   2020/10/01

#ifndef __SPDTRACKFITPAR_H__
#define __SPDTRACKFITPAR_H__

#include <TObject.h>
#include "SpdTrackState.h"
#include "SpdTrackState2.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTrackFitPar                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

enum SpdTrackFitErrorFlag 
{
   kSpdTrUndefined = -1, 
   kSpdTrNoFatalErrors = 0, 
   kSpdTrEmpty,
   kSpdTrNotFitted,
   kSpdTrNoStatus,
   kSpdTrBadFitStatus,
   kSpdTrNoFirstState,
   kSpdTrNoLastState,
   kSpdTrTurnedOut
};

class SpdTrackFitPar: public TObject {

public:

    SpdTrackFitPar();
    virtual ~SpdTrackFitPar();

    virtual void ClearImStates(); 
    virtual void ClearPars();    
    virtual void Clear(Option_t* option = "");
    
    virtual void RemoveImStates(); 
    
    // ---------- Setters ----------
    
    void SetTrackId(Int_t id)          { fTrackId = id;         }
    void SetTrackPdgCode(Int_t pdg)    { fTrackPdgCode = pdg;   }
    void SetNPoints(Int_t npoints)     { fNPoints = npoints;    }
    
    void SetFirstPointId(Int_t n)      { fFirstPointId = n;     }
    void SetLastPointId(Int_t n)       { fLastPointId = n;      }
    
    void SetTrackLength(Double_t len)  { fTrackLength = len;    }
    
    void SetMaterialEffects(Int_t f)   { fMaterialEffects = f;  }     
    void SetChi2(Double_t chi2)        { fChi2 = chi2;          }      
    void SetNDF(Int_t ndf)             { fNDF = ndf;            }  
    void SetDChi2(Double_t dchi2)      { fDChi2 = dchi2;        }
    
    void SetErrorFlag(Int_t eflag)     { fErrorFlag = eflag;    }
    void SetErrorMesg(TString emsg)    { fErrorMesg = emsg;     }
    void SetNFailedHits(Int_t nhits)   { fNFailedHits = nhits;  } 
    void SetConvergency(Int_t conv)    { fConvergencyGF = conv; }
    
    void ChangeTrackCharge()           { fTrackPdgCode = -fTrackPdgCode;  
                                         fIsTrackChargeChanged = true ; }
    
    // ---------- Getters ----------
    
    Int_t      GetTrackId()           const { return fTrackId;         }
    Int_t      GetTrackPdgCode()      const { return fTrackPdgCode;    }
    
    Int_t      GetNPoints()           const { return fNPoints;         }
   
    Int_t      GetNStates()           const { return (fImStates) ? fImStates->size()+2 : 2; }
    Int_t      GetNImStates()         const { return (fImStates) ? fImStates->size(): 0; }
   
    Int_t      GetFirstPointId()      const { return fFirstPointId; }
    Int_t      GetLastPointId()       const { return fLastPointId;  }
    
    Double_t   GetTrackLength()       const { return fTrackLength;   }
    Bool_t     IsTrackChargeChanged() const { return fIsTrackChargeChanged; } 
    
    Int_t      GetMaterialEffects()   const { return fMaterialEffects; } 
    Double_t   GetChi2()              const { return fChi2;  }
    Int_t      GetNDF()               const { return fNDF;   } 
    Double_t   GetDChi2()             const { return fDChi2; }  
    
    Double_t   GetChi2overNDF()       const { return (fNDF > 0) ? fChi2/fNDF : 0; }
   
    Bool_t     HasErrorMesg()         const { return !fErrorMesg.IsWhitespace(); }
    Int_t      GetErrorFlag()         const { return fErrorFlag;     }
    TString    GetErrorMesg()         const { return fErrorMesg;     }
    Int_t      GetNFailedHits()       const { return fNFailedHits;   } 
    Int_t      GetConvergency()       const { return fConvergencyGF; }
   
    Bool_t     GetIsFittedOk()   const;
    Bool_t     GetIsGood()       const;
    Bool_t     GetIsAcceptable() const;
   
    Double_t   GetDPos() const;
    
    //--------------------------------------------------------------------------------
    // shortcuts to fit parameters 
    //
    // state_id = 0:                                      returns first state pars.
    // state_id > 0 && state_id <= im. states array size: returns intermediate state pars.
    // state_id > im. states array size:                  returns last state pars.
    //--------------------------------------------------------------------------------
    
    inline const TVector3*    GetMomentum(Int_t state_id)   const;
    inline const TVector3*    GetPosition(Int_t state_id)   const; 
    inline const TMatrixDSym* GetCovariance(Int_t state_id) const; 
    inline Double_t           GetTime(Int_t state_id)       const; 
    inline Int_t              GetPointId(Int_t state_id)    const;  // returns track point id 
    
    // fit parameters
        
    SpdTrackState*  GetInitialState()   { return &fInitialState; }
    
    SpdTrackState*  GetFirstState()     { return fFirstState; }
    SpdTrackState*  GetLastState()      { return fLastState;  }
    
    SpdTrackState*  GetFinalState()     { return fFinalState; }
     
    SpdTrackState2* GetImState(Int_t i) { return (i > -1 && i <fImStates->size()) ? fImStates->at(i) : 0; }
    SpdTrackState2* FindImState(Int_t point_id);
      
    std::vector<SpdTrackState2*>* GetImStates() { return fImStates; }
     
    // get or create new state (or array of intermediate states)
    
    SpdTrackState*  FirstState();
    SpdTrackState*  LastState();
    SpdTrackState*  FinalState();
    
    std::vector<SpdTrackState2*>& ImStates();
       
    // Print
    virtual void Print(Option_t* option = "") const; // option = "", "imstates0", "imstates"

private:

    // States
    SpdTrackState  fInitialState;            // initial track state data
    
    SpdTrackState* fFirstState;              // first track point fit data
    SpdTrackState* fLastState;               // last track point fit data
    
    SpdTrackState* fFinalState;              // final fitted track state (extrapolation, optionally)
    
    std::vector<SpdTrackState2*>* fImStates; // fitted track ordered list of intermediate states (optionally)
    
    Int_t          fFirstPointId;            // track first point id (<0: unknown)
    Int_t          fLastPointId;             // track last point id (<0: unknown)
    
    Double_t       fTrackLength;             // track length [cm]
    Bool_t         fIsTrackChargeChanged;    // indicates for track charge changing during the fit procedure
    
    // Track
    Int_t          fTrackId;                 // track unique id
    Int_t          fTrackPdgCode;            // track pdg (hypothesis)
    Int_t          fNPoints;                 // total number of points in the track to fit
    
    // Fit characteristics 
    Int_t          fMaterialEffects;         // material effects flag (<0: off; > 0: on)
    Double_t       fChi2;                    // chisquare
    Int_t          fNDF;                     // degrees of freedom (chisquare)
    Double_t       fDChi2;                   // forward/backward chisquare deviation (parcents,%)
    
    // Errors & fit GF-convergency
    Int_t          fErrorFlag;       // see SpdTrackFitErrorFlags above 
    TString        fErrorMesg;       // error summary massage 
    Int_t          fNFailedHits;     // number of hits rejected in the fit process
    Int_t          fConvergencyGF;   // 0 (not converged), -1 (partially converged), 1(fully converged)
    
    ClassDef(SpdTrackFitPar,1)
};

//-----------------------------------------------------------------------------
inline Bool_t SpdTrackFitPar::GetIsFittedOk() const
{
    if (fErrorFlag != 0) return false;
    if (!fFirstState) return false;
    if (!fLastState) return false;
    return true;
}

//-----------------------------------------------------------------------------
inline Bool_t SpdTrackFitPar::GetIsGood() const
{
    if (fErrorFlag != 0) return false;
    if (HasErrorMesg()) return false;
    //if (fNFailedHits > 0) return false;
    if (fConvergencyGF != 1) return false;
    return true;
}

//-----------------------------------------------------------------------------
inline Bool_t SpdTrackFitPar::GetIsAcceptable() const
{
    if (fErrorFlag != 0) return false;
    if (HasErrorMesg()) return false;
    //if (fNFailedHits > 0) return false;
    if (fNDF < 3) return false;
    if (GetChi2overNDF() < 2) return true;
    return false;
}

//------------------------------
// shortcuts to fit parameters
//

//-----------------------------------------------------------------------------
inline const TVector3* SpdTrackFitPar::GetMomentum(Int_t state_id) const
{
    if (state_id < 1) return (fFirstState) ? fFirstState->Momentum() : 0;
    if (!(state_id > fImStates->size())) return fImStates->at(state_id-1)->Momentum(); 
    return (fLastState) ? fLastState->Momentum() : 0;
}

//-----------------------------------------------------------------------------
inline const TVector3* SpdTrackFitPar::GetPosition(Int_t state_id) const
{
    if (state_id < 1) return (fFirstState) ? fFirstState->Position() : 0;
    if (!(state_id > fImStates->size())) return fImStates->at(state_id-1)->Position(); 
    return (fLastState) ? fLastState->Position() : 0;
}

//-----------------------------------------------------------------------------
inline const TMatrixDSym* SpdTrackFitPar::GetCovariance(Int_t state_id) const
{
    if (state_id < 1) return (fFirstState) ? fFirstState->Covariance() : 0;
    if (!(state_id > fImStates->size())) return fImStates->at(state_id-1)->Covariance(); 
    return (fLastState) ? fLastState->Covariance() : 0;
}

//-----------------------------------------------------------------------------
inline Double_t SpdTrackFitPar::GetTime(Int_t state_id) const
{
    if (state_id < 1) return (fFirstState) ? fFirstState->GetTime() : -1;
    if (!(state_id > fImStates->size())) return fImStates->at(state_id-1)->GetTime(); 
    return (fLastState) ? fLastState->GetTime() : -1;
}

//-----------------------------------------------------------------------------
inline Int_t SpdTrackFitPar::GetPointId(Int_t state_id) const
{
    if (state_id < 1) return fFirstPointId;
    if (!(state_id > fImStates->size())) return fImStates->at(state_id-1)->GetPointId(); 
    return fLastPointId;
}


#endif  /* __SPDTRACKFITPAR_H__ */

