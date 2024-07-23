// $Id$ // Author: vladimir 2022/04/10

#ifndef SpdRCTrackFitterGF_h
#define SpdRCTrackFitterGF_h

#include <TObject.h>
#include <vector>

#include "AbsKalmanFitter.h"
#include "KalmanFitter.h"
#include "KalmanFitterRefTrack.h"
#include "FullMeasurement.h"
#include "SpdTrackState.h"
#include "SpdRsMCHit.h"

#include "SpdMCStrawHit1D.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRCTrackFitterGF                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


class SpdTrackFitPar;
class SpdHit;              
class SpdGFMagneticField;
class SpdGFHitCreator;
class SpdField;

class SpdRCTrackFitterGF: public TObject {

public:

    SpdRCTrackFitterGF();
    virtual ~SpdRCTrackFitterGF();
    
    virtual Bool_t Init();
   
    virtual void   InitFitter();
    virtual void   DeleteFitter(); 
   
    virtual bool FitPoints(const std::vector<SpdMCStrawHit1D*>& points, SpdTrackState* start_state, SpdTrackFitPar* pars);
    
    void  SetMaterialEffectsOption(Int_t opt) { fMaterialsOpt = opt; } // 0(off), 1 (on,default)
    void  SetFitterMaxIterations(Int_t niter) { fFitterMaxIterations = niter; } 
    
    void  SetExtField(SpdField* field) { fExtField = field; }
    
    // Start values (pdg code, momentum, vertex, covariance martrix)
    
    void  SetPdg(Int_t pdg) { fPdg = pdg; }
    void  SetVerboseLevel(Int_t level)   { fVerboseLevel = level; }
 
    /* ------ GETTERS ------ */
    
    Bool_t IsFitterInit() { return Bool_t(fFitter); }
    
    
    SpdField*      GetField();
    genfit::Track* GetTrackGF() { return fTrack; }
    double GetChi2() { return fChi2; }
    int    GetNdf()  { return fNdf; }
    SpdTrackState GetTrackState() { return fTrackState; }
    
protected:
    
    virtual Bool_t  LoadGeometry();               // needed
    virtual Bool_t  LoadField();                  // needed

    Bool_t CheckInits();
   
    virtual bool BuildTrack(const std::vector<SpdMCStrawHit1D*>& points, SpdTrackState* start_state); 
     
    
    Bool_t  fIsInit;                             //!

    SpdGFHitCreator*               fHitCreator;  //! GF-hits (measurements) creator

    genfit::KalmanFitterRefTrack*  fFitter;      //! GenFit fitter
    SpdField*                      fExtField;    //! external defined magnetic field (optional)
    genfit::Track*                 fTrack;       //! cuurent GF-track
    SpdTrackState                  fTrackState;
     
    // seed values 
    Int_t         fPdg;                    //! particle PDG code     
    
    /* FITTER & FIT OPTIONS */
    Int_t         fFitterOption;           //! method to update covariance matrix (default = 1)
    Int_t         fMaterialsOpt;           //! > 0 (+material effects, default), < = 0 (no mat. effects)  
    Int_t         fFitterMaxIterations;    //! max. number of iteration to reach convergency

    double        fChi2;                   //!
    double        fNdf;                    //!
    
    Int_t    fVerboseLevel;
    
    ClassDef(SpdRCTrackFitterGF,1)
};

 
#endif  /* __SPDRCTRACKFITTERGF_H__ */

