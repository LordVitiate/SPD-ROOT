// $Id$
// Author: artur   2021/11/19

#ifndef __SPDMCTSPARTICLEPRODUCER_H__
#define __SPDMCTSPARTICLEPRODUCER_H__

#include "SpdTask.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCTsParticleProducer                                                    //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;
class SpdMCEvent;
class SpdTsParticle;
class SpdGeoMapper;
class SpdParSet;
class SpdTrackPropagatorGF;

class SpdMCTsParticleProducer: public SpdTask {

public:

    SpdMCTsParticleProducer();
    virtual ~SpdMCTsParticleProducer();
  
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    
    // Getters
    Int_t GetMomentumOption()  const { return fMomentumOption; }
    
    // Setters
    void SetMomentumOption(Int_t opt=0)  { fMomentumOption = opt; }
    
    
    // Save particles into the output file 
    void SaveParticles(Bool_t save = true)  { fSaveParticles = save; }

    void SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }
    
    // Parameterizations used
    static Double_t MeanParameterization(Int_t itype, Double_t mom);
    static Double_t RelativeSigmaParameterization(Int_t itype, Double_t mom, Int_t nhits);
    static Double_t SigmaParameterization(Int_t itype, Double_t mom, Int_t nhits);
    
private:
    
    /* Methods */
    
    virtual Bool_t LoadGeometry();
    
    virtual SpdTsParticle* AddParticle();
    
    void CalculateLikelihoodsAndDistances(SpdTsParticle* part, Double_t momentum, Double_t value, Int_t nhits);
    
    // magnetic field propagator
    SpdTrackPropagatorGF* fPropagator;
    
    // geometry
    SpdGeoMapper*  fGeoMapper[2];         //! geometry mappers
 
    // input parameters 
    TString        fParsName[2];          //! Input parameters object name
    SpdParSet*     fParameters[2];        //! Input parameters
    
    // input data 
    SpdMCEvent*    fEvent;                //! Main mc-data object
    TClonesArray*  fParticles;            //! List of mc-particles 
    TClonesArray*  fHits;                 //! List of TS mc-hits 
    TClonesArray*  fTracks;               //! List of mc-tracks 
     
    // output data 
    TClonesArray*  fTsParticles;          //! List of reconstructed particles 
    
    // constants
    static const int kNTYPES = 3;                           //! Number of recognized particle types 
                                                            // (0 - pions, 1 - kaons, 2 - protons)
    static const Int_t kPDG[];    //! PDG codes of respective particle types
                                         // = {211, 321, 2212};
    // settings
    Int_t          fMomentumOption;       // how to calculate momentum (possible values: 0(default), 1)

    Bool_t         fSaveParticles;        //! specifies whether to save particles into output file 
    
    //-----------------------
    Int_t          fVerboseLevel;         //! verbose level 
    
    ClassDef(SpdMCTsParticleProducer,1)
};

#endif  /* __SPDMCTSPARTICLEPRODUCER_H__ */

