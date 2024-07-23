// $Id$
// Author: artur   2019/09/18

#ifndef __SPDIDEALKALMANFITTER_H__
#define __SPDIDEALKALMANFITTER_H__

#include <TString.h>
#include "FairTask.h"
#include "FairBaseParSet.h"
#include "SpdFieldPar.h"
#include "SpdGeoLoader.h"
#include "SpdIdealTrack.h"
#include "SpdBaseParSet.h"
#include "SpdGFMeasurementCreator.h"
#include "AbsKalmanFitter.h"
//#include <map>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdIdealKalmanFitter                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdGeoLoader;
class SpdIdealTrackColl;
class SpdKFSimpleRes;
class SpdGFMeasurementCreator;
class SpdGFMagneticField;

class SpdIdealKalmanFitter: public FairTask  {

public:

    SpdIdealKalmanFitter();
    virtual ~SpdIdealKalmanFitter();
    
    void SetTrackingGeoModules(Bool_t add_passives = kTRUE);
    
    void AddModule(TString module) { fGeoModules.push_back(module);  }
    void AddModule(Int_t module)   { fGeoModules.push_back(Form("%d",module)); }
    void ClearListOfGeoModules()   { fGeoModules.clear(); }
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    void DeleteGeoLoader();
    
    /* SETTERS */
    
    void  SetVerboseLevel(Int_t level) { fVerboseLevel = level; } 
    
    void  SetMaterialEffectsOption(Int_t opt /*-1,0(default),1 */) { fMaterialsOpt = opt; }
    
    void  SetSeedMomentum(Double_t mom /* GeV/c */) { fSeedMomentum = fabs(mom); } 
    void  SetHitRepresentation(Int_t DetId, GFMeasurementType GFhitType);
    
    void  SetVertexFindingMethod(Int_t method)            { fVertexFitMethod = method; } 
    void  SetVertexFindingAngleCut(Double_t acut /*deg*/) { fVertexFitAngleCut = acut; }
    
    void  SetStartSeedMethod(Int_t method) { fStartSeedMethod = method; }
    void  SetSmearSeedValues(Bool_t smear) { fSmearSeedValues = smear;  }
    
    void  SetSmearingAngleDelta(Double_t angle /*deg*/) { fSmearingAngleDelta = TMath::Abs(angle); }
    void  SetSmearingVertexDelta(Double_t dpos /*cm*/)  { fSmearingVertexDelta = TMath::Abs(dpos); }
    void  SetSmearingMomDelta(Double_t dp /*Gev/c*/)    { fSmearingMomDelta = TMath::Abs(dp); }
    void  SetNPointsVertexUse(Int_t np /* < 1 = max */) { fNPointsVertexUse = np; }
  
    void  SetFitterOption(Int_t opt) { fFitterOption = opt; }
  
    void  SetFitterMaxIterations(Int_t niter) { fFitterMaxIterations = (niter > 2) ? niter : 4;  } 
    void  SetFitterMaxTrials(Int_t ntrials) { fFitterMaxTrials = (ntrials > 1) ? ntrials : 1; } 
  
    void  SetStoreTracks(Bool_t store = true) { fStoreTracks = store; }
    
    /* GETTERS */
    
    SpdGFMeasurementCreator* GetMeasurementCreator() { return fMeasCreator; }
    
    // Parameters
    void  FillParametersIn(SpdBaseParSet* params);
    void  LoadParametersFrom(SpdBaseParSet* params);

private:
    
    SpdGFMagneticField*      fField;        //! Magnetic field
    SpdGFMeasurementCreator* fMeasCreator;  //! GenFit measurements creator
    genfit::AbsKalmanFitter* fFitter;       //! GenFit fitter
    
    /* methods */
    
    void LoadGeometry();
    void LoadField();
    void InitFitter();
    
    genfit::Track* BuildTrack(Int_t n, genfit::Track* track = 0);
    
    GFMeasurementType GetHitRepresentation(Int_t DetId) const;
    
    void           DefineTrackStartVertexAndMom(const SpdIdealTrack* track);
    void           DefineTrackStartVertexAndMom(const Int_t ntrack, const SpdIdealTrack* track);
    void           DefineStartVertexCovariance();
    
    void FindVertex(TVector3& vertex); 
    
    /* Geometry */
    Bool_t                   fOwnGeoLoader;        //! indicates geometry loader owning
    SpdGeoLoader*            fGeoLoader;           //! SPD geometry
    std::vector<TString>     fGeoModules;          //! geometry modules are to be built
    
    /* data memebers */ 
     
    SpdIdealTrackColl*       fEvent;               //! Input data 
    SpdKFSimpleRes*          fKFres;               //! Output data 
    TObjArray*               fTracks;              //! Output tracks
    
    SpdBaseParSet*           fParameters;
    
    Int_t                    fVerboseLevel;
        
    /* FIT OPTIONS */

    Int_t                    fMaterialsOpt;        // > 0 (+material effects, default), <=0 (no mat. effects)  
        
    // standard
    Double_t                 fSeedMomentum;        // start track vertex momentum [GeV/c]
    std::map<Int_t,Int_t>    fHitRepresentation;   // <Detector Id, GFMeasurementType>
    
    // advanced
    Int_t                    fVertexFitMethod;     // <1 (no fit); 1 (default)
    Double_t                 fVertexFitAngleCut;   // reject tracks with theta = 90+-cut, default cut = 3 [deg]
    
    Int_t                    fStartSeedMethod;     // 1, 2 = default
    Bool_t                   fSmearSeedValues;     // start values smearing flag 
  
    Double_t                 fSmearingMomDelta;    // track start momentum smearing (+-)value, GeV/c
    
    Double_t                 fSmearingAngleDelta;  // [StartSeedMethod = 1] track start angle smearing  (+-)value, deg () 
    Double_t                 fSmearingVertexDelta; // [StartSeedMethod = 1] track start vertex position smearing (+-)value, cm
    
    Int_t                    fNPointsVertexUse;    // [StartSeedMethod = ] number of vertex points are used to evaluate start parameters 
    
    Int_t                    fFitterMaxIterations; // max. number of iteration to reach convergency
    Int_t                    fFitterMaxTrials;     // max. number of admissible track rebuilding trials to reach convergency
    
    Int_t                    fFitterOption;        // method to update covariance matrix (default = 1)
    
    Bool_t                   fStoreTracks;         // if true, store tracks in the output file  
    
    TVector3                 fStartMomentum;       // start track vertex momentum [GeV/c]
    TVector3                 fStartVertex;         // start track vertex position [cm]
    TMatrixDSym              fStartSeedCov;        // covariation matrix of size (6x6) 
    
    ClassDef(SpdIdealKalmanFitter,1)
};

#endif  /* __SPDIDEALKALMANFITTER_H__ */

