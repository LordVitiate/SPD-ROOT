// $Id$
// Author: artur   2021/11/19

#ifndef __SpdMCAegParticleProducer_H__
#define __SpdMCAegParticleProducer_H__

#include "SpdTask.h"
#include <vector>
#include <string>
#include "TF1.h"
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCAegParticleProducer                                                   //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;
class SpdMCEvent;
class SpdAegParticle;
class SpdGeoMapper;
class SpdParSet;
class SpdTrackPropagatorGF;

class SpdMCAegParticleProducer: public SpdTask {

public:

    SpdMCAegParticleProducer();
    virtual ~SpdMCAegParticleProducer();
  
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    // Save particles into the output file 
    void SaveParticles(Bool_t save = true)  { fSaveParticles = save; }

    void SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }
    
private:
  
    /* Methods */
    
    virtual Bool_t LoadGeometry();
    
    virtual SpdAegParticle* AddParticle();
    
    void CheckParticle(SpdAegParticle* part);

    const std::vector<std::string> name_particles = {"pion", "kaon", "proton"};
    const std::vector<int> pdg_particles = {211, 321, 2212};


    std::vector<Double_t>  CalculateLikelihoods  (Double_t momentum);
    std::vector<Double_t>  CalculatePth ();

    double n;

    // magnetic field propagator
    SpdTrackPropagatorGF *fPropagator;



    // geometry
    SpdGeoMapper *fGeoMapper[1]; //! geometry mappers

    Double_t fDistanceToEndcap; //! distance to endcap along z-axis [cm]

    // input parameters
    TString fParsName[2];      //! Input parameters object name
    SpdParSet *fParameters[2]; //! Input parameters

    // input data
    SpdMCEvent *fEvent;       //! Main mc-data object
    TClonesArray *fParticles; //! List of mc-particles
    TClonesArray *fHits;      //! List of Aerogel mc-hits
    TClonesArray *fTracks;    //! List of mc-tracks

    // output data
    TClonesArray *fAegParticles; //! List of reconstructed particles

    // settings
    Bool_t fSaveParticles; //! specifies whether to save particles into output file

    //-----------------------
    Int_t fVerboseLevel; //! verbose level

    ClassDef(SpdMCAegParticleProducer, 1)
};

#endif  /* __SpdMCAegParticleProducer_H__ */

