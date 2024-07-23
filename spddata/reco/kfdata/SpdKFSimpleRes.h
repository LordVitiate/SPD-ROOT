// $Id$
// Author: artur   2019/09/18

#ifndef __SPDKFSIMPLERES_H__
#define __SPDKFSIMPLERES_H__

#include <TNamed.h>
#include <TMath.h>
#include <TVector3.h>
#include <TMatrixDSym.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdKFSimpleRes                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdKFSimpleResTrack: public TObject {
    
public:
    
    SpdKFSimpleResTrack(): 
                        TrackNum(-1),TrackPdg(0),NVhits(0),NTBhits(0),NTEChits(0),
                        IsFitted(false),MaterialEffects(false),
                        Chi2(0),NDF(-1),dChi2(0),
                        Convergency(0),NFailedHits(0),ErrorFlag(-1) { }
                        
    SpdKFSimpleResTrack(Int_t n, Int_t pdg): 
                        TrackNum(n),TrackPdg(pdg),NVhits(0),NTBhits(0),NTEChits(0),
                        IsFitted(false),MaterialEffects(false),
                        Chi2(0),NDF(-1),dChi2(0),
                        Convergency(0),NFailedHits(0),ErrorFlag(0) { }                     
                        
    SpdKFSimpleResTrack(Int_t n, Int_t pdg, TVector3& mom, TVector3& mom_rec): 
                        TrackNum(n),TrackPdg(pdg),
                        MomentumMC(mom),MomentumREC(mom_rec),
                        NVhits(0),NTBhits(0),NTEChits(0),
                        IsFitted(false),MaterialEffects(false),
                        Chi2(0),NDF(-1),dChi2(0),
                        Convergency(0),NFailedHits(0),ErrorFlag(0) { }
                        
    virtual ~SpdKFSimpleResTrack() {}
    
    Int_t       TrackNum;        // MC-track number
    Int_t       TrackPdg;        // track PDG identifier
    TVector3    MomentumMC;      // MC-track momentum value
    TVector3    MomentumREC;     // reconstructed track momentum
    
    Int_t       NVhits;          // number of vertex detector hits
    Int_t       NTBhits;         // number of tracker barrel hits 
    Int_t       NTEChits;        // number of tracker endcaps hits 
    
    Double_t    Chi2;            // chisquare
    Int_t       NDF;             // degrees of freedom (chisquare)
    Double_t    dChi2;           // forward/backward chisquare deviation
    TMatrixDSym CovMom;          // covariation matrix for momentum (3x3)
    
    Bool_t      IsFitted;        // indicates that fit has been finished
    Bool_t      MaterialEffects; // if = true, material effects were taken into account
    
    Int_t       Convergency;     // 0 (fit is not converged), -1 (partially converged), 1(fully converged)
    Int_t       NFailedHits;     // number of hits rejected in the fit process  
     
    Int_t       ErrorFlag;       // -1 (empty data), 0(no errors), 1(errors occurred, see ErrorMessage for details)
    TString     ErrorMessage;    // error summary massage 
    
    TVector3    SeedMomentum;    // start track vertex momentum [GeV/c]
    TVector3    SeedVertex;      // start track vertex position [cm]
    TMatrixDSym SeedCov;         // covariation matrix of size (6x6) => (x,y,z,px,py,pz)

private:
    
    ClassDef(SpdKFSimpleResTrack,1)
};


class SpdKFSimpleRes: public TNamed {

public:

    SpdKFSimpleRes();
    virtual ~SpdKFSimpleRes();
    
    void  ClearData();
    void  ClearVertexData();
    
    /* ===================== SETTERS ===================== */
    
    void     SetRunId(UInt_t id) { fRunId = id; }
    void     SetEventId(Int_t id) { fEventId = id; }
    void     SetNPrimTracks(Int_t nt) { fNVertexTracks = nt; }
    void     SetNSelectedTracks(Int_t nt) { fNSelectedTracks = nt; }
    
    void     SetVertex(TVector3& Vmc, TVector3& Vrec) { fVertexMC = Vmc; fVertexREC = Vrec; }
    
    Int_t    AddTrack(Int_t ntrack, Int_t pdg);
    Int_t    AddTrack(Int_t ntrack, Int_t pdg, TVector3& Pmc, TVector3& Prec);

    void     SetPmc(Int_t i, TVector3& Pmc)   { if (FindTrack(i)) fTracks[i].MomentumMC = Pmc; }
    void     SetPrec(Int_t i, TVector3& Prec) { if (FindTrack(i)) fTracks[i].MomentumREC = Prec; }
     
    void     SetNVhits(Int_t i, Int_t nhits)   { if (FindTrack(i)) fTracks[i].NVhits = nhits; }
    void     SetNTBhits(Int_t i, Int_t nhits)  { if (FindTrack(i)) fTracks[i].NTBhits = nhits; } 
    void     SetNTEChits(Int_t i, Int_t nhits) { if (FindTrack(i)) fTracks[i].NTEChits = nhits; } 
    
    void     SetChi2(Int_t i, Double_t chi2)   { if (FindTrack(i)) fTracks[i].Chi2 = chi2; }
    void     SetNDF(Int_t i, Int_t ndf)        { if (FindTrack(i)) fTracks[i].NDF = ndf; }
    void     SetChi2Delta(Int_t i, Double_t dchi2) { if (FindTrack(i)) fTracks[i].dChi2 = dchi2; }
    void     SetMomCov(Int_t i, const TMatrixDSym& c) { if (FindTrack(i)) { fTracks[i].CovMom.ResizeTo(c); fTracks[i].CovMom = c; } }
    
    void     SetIsFitted(Int_t i, Bool_t fitted = true) { if (FindTrack(i)) fTracks[i].IsFitted = fitted; }
    void     SetMaterialEffects(Int_t i, Bool_t m = true) { if (FindTrack(i)) fTracks[i].MaterialEffects = m; }
    void     SetConvergency(Int_t i, Int_t cnv)  { if (FindTrack(i)) fTracks[i].Convergency = cnv; }
    void     SetNFailedHits(Int_t i, Int_t nhits) { if (FindTrack(i)) fTracks[i].NFailedHits = nhits; }
    void     SetErrorFlag(Int_t i, Int_t eflag) { if (FindTrack(i)) fTracks[i].ErrorFlag = eflag; }
    void     SetErrorMessage(Int_t i, const Char_t* msg)  { if (FindTrack(i)) fTracks[i].ErrorMessage = msg; }
    
    void     AddErrorMessage(Int_t i, const Char_t* msg);
    
    void     SetSeedMomentum(Int_t i, TVector3& p)     { if (FindTrack(i)) fTracks[i].SeedMomentum = p; }
    void     SetSeedVertex(Int_t i, TVector3& v)       { if (FindTrack(i)) fTracks[i].SeedVertex = v; }
    void     SetSeedCov(Int_t i, const TMatrixDSym& c) { if (FindTrack(i)) { fTracks[i].SeedCov.ResizeTo(c); fTracks[i].SeedCov = c; } }  
   
    void     SetVertexChi2overNDF(Double_t chi2)     { fVertexChi2overNDF = chi2; }
    void     SetVertexRECcov(const TMatrixDSym& cov) { fVertexRECcov.ResizeTo(cov); fVertexRECcov = cov;}
  
    /* ===================== GETTERS ===================== */
    
    Int_t    GetEventId()         const { return fEventId; }
    UInt_t   GetRunId()           const { return fRunId;   }
    
    Bool_t   IsWellFittedEvent()  const { return GetNTracks(true,true,true) == GetNTracks(); }
     
    Int_t    GetNPrimTracks()     const { return fNVertexTracks;   } // total number of MC-tracks in the primary vertex
    Int_t    GetNSelectedTracks() const { return fNSelectedTracks; } // ideal tracks selected for reconstruction procedure
    Int_t    GetNTracks()         const { return fTracks.size();   } // processed (including not fitted) tracks 
    
    Int_t    GetNGoodTracks()     const;  // (fitted + converged + without errors) tracks
    
    Int_t    GetNTracks(Int_t& nfitted, Int_t& nconverged, Int_t& noerrors) const;
    Int_t    GetNTracks(Bool_t without_errors, Bool_t converged, Bool_t fitted = true) const;
    
    // +++++++++++++++++++++ TRACKS +++++++++++++++++++++++
      
    const SpdKFSimpleResTrack* GetTrack(Int_t i) const { return (FindTrack(i)) ? &fTracks[i] : 0; }
    
    // parameters
    Int_t    GetNTrack(Int_t i)      const { return (FindTrack(i)) ? fTracks[i].TrackNum : -1; } // Track MC index !
    Int_t    GetTrackPdg(Int_t i)    const { return (FindTrack(i)) ? fTracks[i].TrackPdg :  0; }
     
    Double_t GetGammaBetaMC(Int_t i)  const;
    Double_t GetGammaBetaREC(Int_t i) const;
     
    TVector3 GetMomMC(Int_t i)       const { return (FindTrack(i)) ? fTracks[i].MomentumMC  : TVector3(0,0,0) ; }
    Double_t GetMomentumMC(Int_t i)  const { return (FindTrack(i)) ? fTracks[i].MomentumMC.Mag()  : 0.; } 
    Double_t GetMCPt(Int_t i)        const { return GetMomMC(i).Perp(); }
    Double_t GetMCPz(Int_t i)        const { return GetMomMC(i).Z(); }
    Double_t GetMCPy(Int_t i)        const { return GetMomMC(i).Y(); }
    Double_t GetMCPx(Int_t i)        const { return GetMomMC(i).X(); }
    Double_t GetMCPtheta(Int_t i)    const { return GetMomMC(i).Theta()*TMath::RadToDeg(); }
    Double_t GetMCPphi(Int_t i)      const { return GetMomMC(i).Phi()*TMath::RadToDeg(); }
    
    TVector3 GetMomREC(Int_t i)      const { return (FindTrack(i)) ? fTracks[i].MomentumREC : TVector3(0,0,0) ; }
    Double_t GetMomentumRec(Int_t i) const { return (FindTrack(i)) ? fTracks[i].MomentumREC.Mag() : 0.; } 
    Double_t GetRECPt(Int_t i)       const { return GetMomREC(i).Perp(); }
    Double_t GetRECPz(Int_t i)       const { return GetMomREC(i).Z(); }
    Double_t GetRECPy(Int_t i)       const { return GetMomREC(i).Y(); }
    Double_t GetRECPx(Int_t i)       const { return GetMomREC(i).X(); }
    Double_t GetRECPtheta(Int_t i)   const { return GetMomREC(i).Theta()*TMath::RadToDeg(); }
    Double_t GetRECPphi(Int_t i)     const { return GetMomREC(i).Phi()*TMath::RadToDeg(); }
     
    // hits
    Int_t    GetNVhits(Int_t i)   const { return (FindTrack(i)) ? fTracks[i].NVhits : 0; }
    Int_t    GetNThits(Int_t i)   const { return (FindTrack(i)) ? (fTracks[i].NTBhits + fTracks[i].NTEChits) : 0; } 
    Int_t    GetNTBhits(Int_t i)  const { return (FindTrack(i)) ? fTracks[i].NTBhits : 0; } 
    Int_t    GetNTEChits(Int_t i) const { return (FindTrack(i)) ? fTracks[i].NTEChits : 0; } 
    Int_t    GetNTThits(Int_t i)  const { return (FindTrack(i)) ? (fTracks[i].NVhits + fTracks[i].NTBhits + fTracks[i].NTEChits) : 0; } 
    
    // convergency + errors
    Bool_t   GetIsGood(Int_t i)   const;
    Bool_t   GetIsWrong(Int_t i)           const { return !GetIsGood(i); }
    Bool_t   GetIsFitted(Int_t i)          const { return (FindTrack(i)) ? fTracks[i].IsFitted : false; }
    Bool_t   GetIsFullyConvergent(Int_t i) const { return (FindTrack(i)) ? (fTracks[i].Convergency == 1) : false; }
    Bool_t   HasNoErrors(Int_t i)          const { return (FindTrack(i)) ? (fTracks[i].ErrorFlag == 0) : false; }
    
    Bool_t   GetMaterialEffects(Int_t i) const { return (FindTrack(i)) ? fTracks[i].MaterialEffects: false; }
    Int_t    GetConvergency(Int_t i)     const { return (FindTrack(i)) ? fTracks[i].Convergency : 0; }
    Int_t    GetNFailedHits(Int_t i)     const { return (FindTrack(i)) ? fTracks[i].NFailedHits : 0; }
    Int_t    GetErrorFlag(Int_t i)       const { return (FindTrack(i)) ? fTracks[i].ErrorFlag : 0; }
    TString  GetErrorMessage(Int_t i)    const { return (FindTrack(i)) ? fTracks[i].ErrorMessage : ""; }
    
    Bool_t   FindTrack(Int_t i)    const { return !( (i<0) && (i>=Int_t(fTracks.size())) ); }
    
    // chisquare
    Double_t GetChi2(Int_t i)      const { return (FindTrack(i)) ? fTracks[i].Chi2 : 0; }
    Int_t    GetNDF(Int_t i)       const { return (FindTrack(i)) ? fTracks[i].NDF: 1; }
    Double_t GetChi2Delta(Int_t i) const { return (FindTrack(i)) ? fTracks[i].dChi2: 0; }
    
    Double_t GetChi2toNDF(Int_t i) const { 
         if (!FindTrack(i)) return -1;  
         return (fTracks[i].NDF > 0) ? fTracks[i].Chi2/fTracks[i].NDF : -1; 
    }
    
    Bool_t   GetCovMom(Int_t i, TMatrixDSym& c) const;
    Double_t GetMomSigma(Int_t i, Int_t axis /*1(px),2(py),3(pz)*/) const;
    Double_t GetPSigma(Int_t i) const;  // !ATTENTION! NOT IMPLEMENTED YET! (return -1)
    Double_t GetPtSigma(Int_t i) const; // !ATTENTION! NOT IMPLEMENTED YET! (return -1)

    Bool_t   GetIsCovMomEmpty(Int_t i)    const { return (FindTrack(i)) ? !Bool_t(fVertexRECcov.GetMatrixArray()) : true; }
    Bool_t   GetIsCovMomNotEmpty(Int_t i) const { return (FindTrack(i)) ?  Bool_t(fVertexRECcov.GetMatrixArray()) : false; }

    Double_t GetMomDeltaDivSigma(Int_t i, Int_t axis /*1(px),2(py),3(pz)*/) const;
    Double_t GetPDeltaDivSigma(Int_t i) const;  // !ATTENTION! NOT IMPLEMENTED YET! (return -1)
    Double_t GetPtDeltaDivSigma(Int_t i) const; // !ATTENTION! NOT IMPLEMENTED YET! (return -1)
    
    //  resolution (the method returns signed value if parameter sgnd = true !): 
    Double_t GetMomRes(Int_t i, Bool_t sgnd = false) const;    // +-|dp/p|  
    Double_t GetPtRes(Int_t i, Bool_t sgnd = false) const;     // +-|d(pt)/pt|
    Double_t GetPzRes(Int_t i, Bool_t sgnd = false) const;     // +-|d(pz)/pz|

    //  resolution: diffs
    Double_t GetDP(Int_t i, Bool_t sgnd = false) const;        // +-|dp|
    Double_t GetDPt(Int_t i, Bool_t sgnd = false) const;       // +-|d(pt)|
    Double_t GetDPz(Int_t i, Bool_t sgnd = false) const;       // +-|d(pz)|
    Double_t GetDPy(Int_t i, Bool_t sgnd = false) const;       // +-|d(py)|
    Double_t GetDPx(Int_t i, Bool_t sgnd = false) const;       // +-|d(px)|
    Double_t GetDTheta(Int_t i, Bool_t sgnd = false) const;    // +-|d(theta)|
    Double_t GetDPhi(Int_t i) const;                           //   |d(phi)|
    
    //  resolution: return  AVERAGED |dp/p| (fitted tracks only)
    Double_t GetMomRes(Bool_t without_errors = true, Bool_t converged = true) const; 
    Double_t GetMomResN(Int_t& nres, Bool_t without_errors = true, Bool_t converged = true) const; 
    
    //  resolution: return  AVERAGED |dpt/pt| (fitted tracks only)
    Double_t GetMomTRes(Bool_t without_errors = true, Bool_t converged = true) const; 
    Double_t GetMomTResN(Int_t& nres, Bool_t without_errors = true, Bool_t converged = true) const; 
    
    //  resolution: return number of tracks & AVERAGED values (fitted tracks only)
    Int_t    GetRes(Double_t& rp, Double_t& rpt, Double_t& rpz, Double_t& rtheta, Double_t& rphi,
                    Bool_t without_errors = true, Bool_t converged = true) const;  // !ATTENTION! NOT IMPLEMENTED YET! (return 0)
    Int_t    GetRes(Double_t& rp, Double_t& rpx, Double_t& rpy,  Double_t& rpz,
                    Bool_t without_errors = true, Bool_t converged = true) const;  // !ATTENTION! NOT IMPLEMENTED YET! (return 0)
                    
    // seed values                
    TVector3 GetSeedMomentum(Int_t i) const { return (FindTrack(i)) ? fTracks[i].SeedMomentum : TVector3(0,0,0) ; }
    TVector3 GetSeedVertex(Int_t i)   const { return (FindTrack(i)) ? fTracks[i].SeedVertex   : TVector3(0,0,0) ; }
    Bool_t   GetSeedCov(Int_t i, TMatrixDSym& c) const;
   
    // +++++++++++++++++++++ VERTEX +++++++++++++++++++++++
    
    TVector3 GetVertexMC()       const { return fVertexMC;  }
    TVector3 GetVertexRec()      const { return fVertexREC; }
  
    Double_t GetVertexMCr()      const { return fVertexMC.Mag(); }
    Double_t GetVertexMCrt()     const { return fVertexMC.Perp(); }
    Double_t GetVertexMCz()      const { return fVertexMC.Z(); }
    Double_t GetVertexMCtheta()  const { return fVertexMC.Theta()*TMath::RadToDeg(); }
    Double_t GetVertexMCphi()    const { return fVertexMC.Phi()*TMath::RadToDeg(); }
    
    Double_t GetVertexRECr()     const { return fVertexREC.Mag(); }
    Double_t GetVertexRECrt()    const { return fVertexREC.Perp(); }
    Double_t GetVertexRECz()     const { return fVertexREC.Z(); }
    Double_t GetVertexRECtheta() const { return fVertexREC.Phi()*TMath::RadToDeg(); }
    Double_t GetVertexRECphi()   const { return fVertexREC.Phi()*TMath::RadToDeg(); }
    
    Double_t GetVertexDMag()     const { return (fVertexMC-fVertexREC).Mag();  }
    Double_t GetVertexDRt()      const { return (fVertexMC-fVertexREC).Perp(); }
    Double_t GetVertexDX()       const { return fVertexMC.X()-fVertexREC.X();  }
    Double_t GetVertexDY()       const { return fVertexMC.Y()-fVertexREC.Y();  } 
    Double_t GetVertexDZ()       const { return fVertexMC.Z()-fVertexREC.Z();  }
    Double_t GetVertexDTheta()   const { return GetVertexMCtheta()-GetVertexRECtheta(); }
    Double_t GetVertexDPhi()     const;
    
    const TMatrixDSym& GetVertexRECcov() const { return fVertexRECcov; }
    Double_t GetVertexSigma(Int_t axis /*1(x),2(y),3(z)*/) const;
    
    Bool_t   GetIsVertexRECcovEmpty()    const { return (fVertexRECcov.GetMatrixArray()) ? false : true; }
    Bool_t   GetIsVertexRECcovNotEmpty() const { return (fVertexRECcov.GetMatrixArray()) ? true : false; }
    
    Double_t GetVertexChi2overNDF()      const { return fVertexChi2overNDF; }
    
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++
   
    virtual void PrintData(Int_t opt = 0) const;
    
private:
    
    UInt_t      fRunId;
    Int_t       fEventId;
    Int_t       fNVertexTracks;   // total number of primary tracks in the vertex
    Int_t       fNSelectedTracks; // selected vertex tracks
    
    TVector3    fVertexMC;
    TVector3    fVertexREC;
    
    TMatrixDSym fVertexRECcov;
    Double_t    fVertexChi2overNDF;
    
    std::vector<SpdKFSimpleResTrack> fTracks;
    
    ClassDef(SpdKFSimpleRes,1)
};

//_____________________________________________________________________________
inline Double_t SpdKFSimpleRes::GetMomRes(Int_t i, Bool_t sgnd) const  //  +-|dp/p|
{
    if (!FindTrack(i)) return 0;
    Double_t m = fTracks[i].MomentumMC.Mag();
    if (!(m > 0)) return -1;
    Double_t val = (m-fTracks[i].MomentumREC.Mag())/m;
    return (sgnd) ? val : TMath::Abs(val);
}

//_____________________________________________________________________________
inline Double_t SpdKFSimpleRes::GetPtRes(Int_t i, Bool_t sgnd) const //  +-|d(pt)/pt|
{
    if (!FindTrack(i)) return 0; 
    Double_t pt = fTracks[i].MomentumMC.Perp();
    if (!(pt > 0)) return -1;
    Double_t val = (pt-fTracks[i].MomentumREC.Perp())/pt;
    return (sgnd) ? val : TMath::Abs(val);
}

//_____________________________________________________________________________    
inline Double_t SpdKFSimpleRes::GetPzRes(Int_t i, Bool_t sgnd) const //  +-|d(pz)/pz|
{
    if (!FindTrack(i)) return 0; 
    Double_t pz = fTracks[i].MomentumMC.Z();
    if (!(TMath::Abs(pz) > 0)) return -1;
    Double_t val = ((pz-fTracks[i].MomentumREC.Z())/pz);
    return (sgnd) ? val : TMath::Abs(val);
}

//=================================================================================================   

//_____________________________________________________________________________    
inline Double_t SpdKFSimpleRes::GetDP(Int_t i, Bool_t sgnd) const //  +-|dp|
{
   if (!FindTrack(i)) return 0;
   Double_t val = fTracks[i].MomentumMC.Mag()-fTracks[i].MomentumREC.Mag();
   return (sgnd) ? val : TMath::Abs(val);
}

//_____________________________________________________________________________    
inline Double_t SpdKFSimpleRes::GetDPt(Int_t i, Bool_t sgnd) const //  +-|dpt|
{
   if (!FindTrack(i)) return 0;
   Double_t val = fTracks[i].MomentumMC.Perp()-fTracks[i].MomentumREC.Perp();
   return (sgnd) ? val : TMath::Abs(val);
}

//_____________________________________________________________________________    
inline Double_t SpdKFSimpleRes::GetDPz(Int_t i, Bool_t sgnd) const //  +-|d(pz)|
{
   if (!FindTrack(i)) return 0;
   Double_t val = fTracks[i].MomentumMC.Z()-fTracks[i].MomentumREC.Z();
   return (sgnd) ? val : TMath::Abs(val);
}

//_____________________________________________________________________________    
inline Double_t SpdKFSimpleRes::GetDPy(Int_t i, Bool_t sgnd) const //  +-|d(py)|
{
   if (!FindTrack(i)) return 0;
   Double_t val = fTracks[i].MomentumMC.Y()-fTracks[i].MomentumREC.Y();
   return (sgnd) ? val : TMath::Abs(val);
}

//_____________________________________________________________________________        
inline Double_t SpdKFSimpleRes::GetDPx(Int_t i, Bool_t sgnd) const //  +-|d(px)|
{
   if (!FindTrack(i)) return 0;
   Double_t val = fTracks[i].MomentumMC.X()-fTracks[i].MomentumREC.X();
   return (sgnd) ? val : TMath::Abs(val);
}

//_____________________________________________________________________________        
inline Double_t SpdKFSimpleRes::GetDTheta(Int_t i, Bool_t sgnd) const  //  +-|d(theta)|
{
   if (!FindTrack(i)) return 0;  
   Double_t val = fTracks[i].MomentumMC.Theta()-fTracks[i].MomentumREC.Theta();
   return (sgnd) ? val*TMath::RadToDeg() : TMath::Abs(val)*TMath::RadToDeg();
}

//_____________________________________________________________________________        
inline Double_t SpdKFSimpleRes::GetDPhi(Int_t i) const //  |d(phi)|
{
   if (!FindTrack(i)) return 0;     
   Double_t val = TMath::Abs(fTracks[i].MomentumMC.Phi()-fTracks[i].MomentumREC.Phi())*TMath::RadToDeg();
   return (val < 180.) ? val : (360.-val);
}

//=================================================================================================  

//_____________________________________________________________________________ 
inline Double_t SpdKFSimpleRes::GetVertexDPhi() const 
{
    Double_t val = TMath::Abs(GetVertexMCphi()-GetVertexRECphi()); 
    return (val < 180.) ? val : (360.-val);
}


   
#endif  /* __SPDKFSIMPLERES_H__ */

