

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdUniformPtCostGenerator                                                        //
//                                                                            //
// A wrapper on Pythia8                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __SPDUNIFORMPTCOSTGENERATOR_H__
#define __SPDUNIFORMPTCOSTGENERATOR_H__

#include "SpdGenerator.h"

#include <TObject.h>
#include <TRandom3.h>
#include <TVector3.h>

#include <TClonesArray.h>



class SpdUniformPtCostGenerator : public SpdGenerator {
    
public:

    bool debug_output;

    SpdUniformPtCostGenerator();
    SpdUniformPtCostGenerator(const Char_t* Name);
    
    virtual ~SpdUniformPtCostGenerator() = default;

    void Reset();
    void ResetEvent();

    virtual Bool_t Init(); 
   
    virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen = 0);
    
    /* _____________ settings _____________ */
    
    void SetSeed(UInt_t seed = 0);
    virtual void SetSeeds(const std::vector<UInt_t>& /*seeds*/);

    
    void SetGenPars(Int_t pdg,
            Double_t costmin = -1., Double_t costmax = 1., /* cosTheta, [-1, 1] */ 
            Double_t ptmin = 0.1, Double_t ptmax = 5., /* pT, GeV */
            Int_t np = 1);

    virtual void  GetSeeds(std::vector<UInt_t>& seeds);
    
    Int_t GetNPrimaries() const { return fNParticles; }

    inline  Bool_t IsInit() const { return fInit; }
    
    virtual void Print(Option_t* opt) const;

private:
    // void AddParticle(FairPrimaryGenerator* primGen, Double_t px, Double_t py, Double_t pz);
    //    void PrintParticle(TParticle* p, Int_t i, Bool_t add); 

    Bool_t     fInit;          //! Init generator mark
    UInt_t     fSeed;          //! inner random seed [0, 2147483647]

    Int_t      fPdg;           // pdg-number
    Int_t      fNPL;           // PARTICLES per event or LEVEL

    Double_t   fpTmin;          // cos(pT_min)
    Double_t   fpTmax;          // cos(pT_max)
    Double_t   fCosTmin;        // cos(theta_min)
    Double_t   fCosTmax;        // cos(theta_max)

    Int_t      fNParticles;    // Number of primary particles

    ClassDef(SpdUniformPtCostGenerator, 1);
};

#endif /* !SPDUNIFORMPTCOSTGENERATOR_H */
