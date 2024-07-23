// $Id$
// Author: artur   2021/12/21

#ifndef __SPDTRACKSTATE2_H__
#define __SPDTRACKSTATE2_H__

#include "SpdTrackState.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTrackState2                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTrackState;

class SpdTrackState2: public TObject {

public:

    SpdTrackState2():fMomentum(0),fPosition(0),fCovariance(0),fTime(-1),fPointId(-1) {}
    SpdTrackState2(const SpdTrackState2& state);
    
    virtual ~SpdTrackState2();
    
    void   ClearState();
    void   ClearMomentum()    { if (fMomentum)   { delete fMomentum;   fMomentum   = 0; } }
    void   ClearPosition()    { if (fPosition)   { delete fPosition;   fPosition   = 0; } }
    void   ClearCovariance()  { if (fCovariance) { delete fCovariance; fCovariance = 0; } }
    
    SpdTrackState* CreateTrackState();

    // Setters
    void   SetMomentum(TVector3 mom);
    void   SetPosition(TVector3 pos);
    void   SetMomPos(TVector3 mom, TVector3 pos) { SetMomentum(mom); SetPosition(pos); }
    
    void   SetCovariance(const TMatrixDSym& c /*6x6*/); 
    
    void   SetTime(Double_t t)  { fTime  = t;    }
    void   SetPointId(Int_t id) { fPointId = id; }
      
    SpdTrackState2& operator=(const SpdTrackState2& state);
     
    // Getters
    const TVector3*       Momentum()   const { return fMomentum;   }
    const TVector3*       Position()   const { return fPosition;   }
    const TMatrixDSym*    Covariance() const { return fCovariance; }
    
    const TVector3*    GetMomentum()   const { return fMomentum;   }
    const TVector3*    GetPosition()   const { return fPosition;   }
    const TMatrixDSym* GetCovariance() const { return fCovariance; }
    
    Bool_t  GetMomentum(TVector3& mom) const { if (fMomentum) { mom = *fMomentum; return true; } return false; }
    Bool_t  GetPosition(TVector3& pos) const { if (fPosition) { pos = *fPosition; return true; } return false; }
    Bool_t  GetCovariance(TMatrixDSym& c /*6x6*/) const;
    
    Double_t           GetTime()       const { return fTime;    }
    Int_t              GetPointId()    const { return fPointId; }
    
    Int_t              GetCovSize()    const { return (fCovariance) ? fCovariance->GetNrows() : 0; } 
    Bool_t             IsCovDefined()  const { return (fCovariance && fCovariance->GetMatrixArray()) ? true : false; }
    
    Double_t           GetP()          const { return (fMomentum) ? fMomentum->Mag() : 0; }
    
    // Print
    virtual void Print(Option_t* option = "") const;

private:
    
    TVector3*          fMomentum;    // track momentum [GeV/c]
    TVector3*          fPosition;    // track position [cm]
    TMatrixDSym*       fCovariance;  // covariation matrix of size (6x6) for vector (x,p)  
    Double_t           fTime;        // track time [ns] 
    Int_t              fPointId;     // the number of corresponding track point
    
    ClassDef(SpdTrackState2,1)
};
 
//------------------------------------------------------------------------
inline void SpdTrackState2::SetMomentum(TVector3 mom) 
{ 
    if (!fMomentum) fMomentum = new TVector3(mom);
    else *fMomentum = mom; 
}

//------------------------------------------------------------------------
inline void  SpdTrackState2::SetPosition(TVector3 pos) 
{ 
    if (!fPosition) fPosition = new TVector3(pos); 
    else *fPosition = pos; 
}

//------------------------------------------------------------------------
inline void SpdTrackState2::SetCovariance(const TMatrixDSym& c /*6x6*/) 
{ 
    if (!fCovariance) fCovariance = new TMatrixDSym(c); 
    else {
        fCovariance->ResizeTo(c); 
       *fCovariance = c;
    }
} 
    
//------------------------------------------------------------------------
inline Bool_t SpdTrackState2::GetCovariance(TMatrixDSym& c /*6x6*/) const 
{
    if (!IsCovDefined()) return false; 
    c.ResizeTo(*fCovariance); 
    c = *fCovariance;
    return true;
}

//------------------------------------------------------------------------
inline SpdTrackState2& SpdTrackState2::operator=(const SpdTrackState2& state)
{
    if (state.fMomentum) 
    {
        if (!fMomentum) fMomentum = new TVector3(*state.fMomentum);
        else *fMomentum = *state.fMomentum;
    }
    else ClearMomentum();
    
    if (state.fPosition) 
    {
        if (!fPosition) fPosition = new TVector3(*state.fPosition);
        else *fPosition = *state.fPosition;
    }
    else ClearPosition();
   
    if (state.fCovariance) 
    {
        if (!fCovariance) fCovariance = new TMatrixDSym(*state.fCovariance); 
        else {
            fCovariance->ResizeTo(*state.fCovariance); 
           *fCovariance = *state.fCovariance;
        }
    }
    else ClearCovariance();
   
    fTime    = state.fTime;
    fPointId = state.fPointId;
    
    return *this;
}

#endif  /* __SPDTRACKSTATE2_H__ */

