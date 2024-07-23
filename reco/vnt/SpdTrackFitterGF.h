// $Id$
// Author: artur   2021/02/08

#ifndef __SPDTRACKFITTERGF_H__
#define __SPDTRACKFITTERGF_H__

#include <TObject.h>
#include <vector>

#include "AbsKalmanFitter.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTrackFitterGF                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTrackFitPar;
class SpdHit;
class SpdGFMagneticField;
class SpdGFHitCreator;
class SpdField;

class SpdTrackFitterGF: public TObject {

public:

    SpdTrackFitterGF();
    virtual ~SpdTrackFitterGF();
    
    virtual Bool_t Init();
   
    // Fitter
    
    virtual void   InitFitter();
    virtual void   DeleteFitter(); 
    
    virtual void   ResetCounter();
    virtual void   DeleteTrack();
   
    /* >>>>>>>>> TRACK FITTING <<<<<<<<<< */
    virtual Bool_t FitTrack(const std::vector<SpdHit*>& track, SpdTrackFitPar* pars);
    
    /* ------ SETTERS ------ */
  
    // Fitter options
    
    void  SetFitterOption(Int_t opt)          { fFitterOption = opt; } // default = 1
    void  SetMaterialEffectsOption(Int_t opt) { fMaterialsOpt = opt; } // 0(off), 1 (on,default)
    void  SetFitterMaxIterations(Int_t niter) { fFitterMaxIterations = (niter > 2) ? niter : 4; } 
    void  SetFitterMaxTrials(Int_t ntrials  ) { fFitterMaxTrials = (ntrials > 1) ? ntrials : 1; } 
    
    void  SetStartSeedMethod(Int_t method);
    
    // Storing intermediate track point's info
    
    void  StoreImPoints(Bool_t store = true)     { fStoreImPoints     = store; } // default = false
    
    void  StoreImMomentum(Bool_t store = true)   { fStoreImMomentum   = store; } // default = true     
    void  StoreImPosition(Bool_t store = true)   { fStoreImPosition   = store; } // default = false     
    void  StoreImCovariance(Bool_t store = true) { fStoreImCovariance = store; } // default = false   
    
    // External magnetic field
    
    void  SetExtField(SpdField* field) { fExtField = field; }
    
    // Start values (pdg code, momentum, vertex, covariance martrix)
    
    void  SetPdg(Int_t pdg) { fPdg = pdg; }
    void  SetMomentum(TVector3 p) { fStartMomentum = p; }
    void  SetMomentum(Double_t px, Double_t py, Double_t pz) { fStartMomentum.SetXYZ(px,py,pz); }
    void  SetVertex(TVector3 v) { fStartVertex = v; }
    void  SetVertex(Double_t vx, Double_t vy, Double_t vz) { fStartVertex.SetXYZ(vx,vy,vz);   }
    void  SetTime(Double_t t) { fStartTime = t; }
    void  SetCovarianceMatrix(const TMatrixDSym& c /*6x6*/) { fStartCovariance.ResizeTo(c); fStartCovariance = c;} 
    void  SetCovarianceMatrix(/*set default covariance martrix 6x6*/);
    
    // Smearing
    
    void  SmearStartVertex(Bool_t smear) { fSmearStartVertex = smear; }
    void  SmearStartMom(Bool_t smear)    { fSmearStartMom    = smear; }
    
    void  SetSmearingDMom(Double_t dp /*percents, 0.0 - 0.5*/);   // uniform
    void  SetSmearingDMomTheta(Double_t dtheta /*deg*/) { fSmearingDMomTheta = TMath::Abs(dtheta); }  // gaussian
    void  SetSmearingDMomPhi(Double_t dphi /*deg*/)     { fSmearingDMomPhi   = TMath::Abs(dphi);   }  // gaussian   
    void  SetSmearingDVxyz(Double_t dv /*cm*/)          { fSmearingDVxyz     = TMath::Abs(dv);     }  // uniform
 
    void  SetVerboseLevel(Int_t level)   { fVerboseLevel = level; }
 
    /* ------ GETTERS ------ */
    
    Bool_t IsFitterInit() const { return Bool_t(fFitter); }
    
    std::vector<Int_t>& GetTrackPointIds() { return fTrackPointId; } // filled in by an external trackmaker!
    
    SpdField*      GetField();
    genfit::Track* GetTrackGF() const { return fTrack; }
    
    /* ------- Print ------- */
    
    virtual void  PrintCounter();
    
protected:
    
    /* ----- Methods ----- */
    
    virtual Bool_t  LoadGeometry();
    virtual Bool_t  LoadField();
   
    virtual genfit::Track* BuildTrack(SpdTrackFitPar* pars, const std::vector<SpdHit*>& points, 
                                      genfit::Track* track = 0);
    
    virtual void    SmearStartMom(TVector3& mom);
    virtual void    SmearStartVertex(TVector3& v);
    
    virtual Bool_t  DefineStartParameters(const std::vector<SpdHit*>& hits, TVector3& v, TVector3& mom);
    
    virtual void    AddIntermediateHits(SpdTrackFitPar* pars, const std::vector<SpdHit*>& hits);
     
    /* ----- Data members ----- */
    
    Bool_t  fIsInit;  //!

    SpdGFHitCreator*         fHitCreator;  //! GF-hits (measurements) creator
    genfit::AbsKalmanFitter* fFitter;      //! GenFit fitter
    SpdField*                fExtField;    //! external defined magnetic field (optional)
    
    genfit::Track*           fTrack;       //! current GF-track
     
    // seed values 
    Int_t         fPdg;                    //! particle PDG code     
    TVector3      fStartMomentum;          //! start track vertex momentum [GeV/c]
    TVector3      fStartVertex;            //! start track vertex position [cm]
    Double_t      fStartTime;              //! track time in an intitial position [ns] 
    TMatrixDSym   fStartCovariance;        //! covariation matrix of size (6x6) 
    
    /* FITTER & FIT OPTIONS */
    Int_t         fFitterOption;           //! method to update covariance matrix (default = 1)
    Int_t         fMaterialsOpt;           //! > 0 (+material effects, default), < = 0 (no mat. effects)  
    Int_t         fFitterMaxIterations;    //! max. number of iteration to reach convergency
    Int_t         fFitterMaxTrials;        //! max. number of admissible track rebuilding trials to reach convergency
    
    Int_t         fStartSeedMethod;        //! start seed (momentum, positio) method: 0 (default),1
    
    Bool_t        fStoreImPoints;          //! indicates wheather to store track intermediate points 
    Bool_t        fStoreImMomentum;        //! indicates wheather to store track intermediate point's momentum 
    Bool_t        fStoreImPosition;        //! indicates wheather to store track intermediate point's position 
    Bool_t        fStoreImCovariance;      //! indicates wheather to store track intermediate point's covariance 
       
    // smearing data members           
    Bool_t        fSmearStartVertex;       //! indicates wheather to smear start vertex position or not
    Bool_t        fSmearStartMom;          //! indicates wheather to smear start momentum or not
    Double_t      fSmearingDMom;           //! track start |p| smearing (+-)value, GeV/c
    Double_t      fSmearingDMomTheta;      //! track start p theta smearing (+-)value, deg 
    Double_t      fSmearingDMomPhi;        //! track start p theta smearing (+-)value, deg 
    Double_t      fSmearingDVxyz;          //! track start vertex x-,y,-z- position smearing (+-)value, cm
    
    // counters
    Int_t fN_proc;         //! total number of processed tracks
        
    Int_t fN_empty;        //! [fatal_error] number of empty tracks
    Int_t fN_notfitted;    //! [fatal_error] number of "non-fitted" tracks
    Int_t fN_ternedout;    //! [fatal_error] number of tracks for which scalar_prod(p_mc,p_rec) < 1   
    
    Int_t fN_ewmsgs;       //! [error] number of tracks fitted with (error & warning) messages 
  
    Int_t fN_fitted;       //! [ok] number of fitted tracks
    Int_t fN_conv;         //! [ok] number of fitted (GF-converged) tracks
    Int_t fN_good;         //! [ok] number of fitted (no-errors, GF-coverged) tracks
    Int_t fN_acceptable;   //! [ok] number of fitted (no-errors, chi2/ndf < 2) tracks
    
    //------------------------------------
    // additional: measurement point id vs. track point number
    
    std::vector<Int_t>     fTrackPointId;   //! point id [track point number]
    std::map<Int_t,Int_t>  fHtoP;           //! [GF measurement hit, track point]
    
    //------------------------------------
    Int_t    fVerboseLevel;                 // ! verbose level 
    
    ClassDef(SpdTrackFitterGF,1)
};

//----------------------------------------------------------------------------------------------
inline void SpdTrackFitterGF::SetStartSeedMethod(Int_t method)
{
    if (method < 0) fStartSeedMethod = 0;
    else fStartSeedMethod = 1;
}

//----------------------------------------------------------------------------------------------
inline void SpdTrackFitterGF::SetSmearingDMom(Double_t dp /*0.0 - 0.5*/)  
{ 
    fSmearingDMom = TMath::Abs(dp); 
    if (fSmearingDMom > 0.5) fSmearingDMom = 0.5;
}

 
#endif  /* __SPDTRACKFITTERGF_H__ */

