// $Id$
// Author: artur   2021/11/19

#ifndef __SPDMCTOFPARTICLEPRODUCER_H__
#define __SPDMCTOFPARTICLEPRODUCER_H__

#include "SpdTask.h"
#include <vector>
#include <string>
#include "TF1.h"
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCTofParticleProducer                                                   //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;
class SpdMCEvent;
class SpdTofParticle;
class SpdGeoMapper;
class SpdParSet;
class SpdTrackPropagatorGF;

class SpdMCTofParticleProducer: public SpdTask {

public:

    SpdMCTofParticleProducer();
    virtual ~SpdMCTofParticleProducer();
  
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    // Save particles into the output file 
    void SaveParticles(Bool_t save = true)  { fSaveParticles = save; }

    void SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }
    
    void SetTimeResolutionTof(Double_t sigmaTimeTof = 60 /* ps*/) { fsigmaTimeTof = sigmaTimeTof; }
private:
  
    /* Methods */
    
    virtual Bool_t LoadGeometry();
    
    virtual SpdTofParticle* AddParticle();
    
    void CheckParticle(SpdTofParticle* part);


    
    const std::vector<std::string> name_particles = {"pion", "kaon", "proton", "deuteron"};


    Double_t              CalculateTofMass2(Double_t timeTof, Double_t trackLen, Double_t momentum);
    std::vector<Double_t> CalculateSigma(Double_t momentum, Int_t hitLocation);
    std::vector<Double_t> CalculateMean(Double_t momentum, Int_t hitLocation);
    std::vector<Double_t> CalculateDistances(Double_t TofMass2, Double_t momentum, Int_t hitLocation);

    Double_t GetFitMass   (Double_t momentum, std::string name_particle, Int_t hitLocation);
    Double_t GetFitSigma_p(Double_t momentum, std::string name_particle, Int_t hitLocation);

    TF1 *GetSigma_P(Double_t xmin, Double_t xmax, std::vector<Double_t> vfit_sigma);
    TF1 *GetMean(Double_t xmin, Double_t xmax, Double_t fit_mean);

    Double_t fpmin, fpmax;

    std::vector<Double_t>  CalculateLikelihoods  (Double_t tofMass2, Double_t momentum, Int_t hitLocation);
    std::vector<Double_t>  CalculatePosteriorProbs(Double_t tofMass2, Double_t momentum);

    // magnetic field propagator
    SpdTrackPropagatorGF *fPropagator;



    // geometry
    SpdGeoMapper *fGeoMapper[2]; //! geometry mappers

    Double_t fDistanceToEndcap; //! distance to endcap along z-axis [cm]
    Double_t fBarrelMinRadius;  //! minimal barrel radius [cm]

    // input parameters
    TString fParsName[2];      //! Input parameters object name
    SpdParSet *fParameters[2]; //! Input parameters

    // input data
    SpdMCEvent *fEvent;       //! Main mc-data object
    TClonesArray *fParticles; //! List of mc-particles
    TClonesArray *fHits;      //! List of TOF mc-hits
    TClonesArray *fTracks;    //! List of mc-tracks

    // output data
    TClonesArray *fTofParticles; //! List of reconstructed particles

    // settings
    Bool_t fSaveParticles; //! specifies whether to save particles into output file

    //-----------------------
    Int_t fVerboseLevel; //! verbose level

    Double_t fsigmaTimeTof; // time resolution of the TOF
    ClassDef(SpdMCTofParticleProducer, 1)
};

#endif  /* __SPDMCTOFPARTICLEPRODUCER_H__ */

