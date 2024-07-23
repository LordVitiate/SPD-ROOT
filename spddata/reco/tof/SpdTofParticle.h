// $Id$
// Author: artur   2021/11/19

#ifndef __SPDTOFPARTICLE_H__
#define __SPDTOFPARTICLE_H__

#include <TObject.h>
#include <vector>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTofParticle                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTofParticle: public TObject {

public:

    SpdTofParticle();
    virtual ~SpdTofParticle();
    
    // ----- Setters -----
    
    void   SetId(Int_t id)       { fId = id;      }
    void   SetHitId(Int_t id)    { fHitId = id;   }
    void   SetTrackId(Int_t id)  { fTrackId = id; }

    void   SetTofMass2(Double_t TofMass2)  { fTofMass2= TofMass2; }
    void   SetMomentum(Double_t mom)  { fMomentum= mom; }

    void   SetDistances(std::vector<Double_t> Distances)             { fDistances= Distances; }
    void   SetSigma(std::vector<Double_t> Sigma)                     { fSigma= Sigma; }
    void   SetMean(std::vector<Double_t> Mean)                     { fMean= Mean; }

    void   SetLikelihoods(std::vector<Double_t> Likelihoods)         { fLikelihoods= Likelihoods; }
    void   SetPosteriorProbs(std::vector<Double_t> PosteriorProbs)   { fPosteriorProbs= PosteriorProbs; }
 
 

    // ----- Getters -----
    
    Int_t  GetId()        const  { return fId;      }
    Int_t  GetHitId()     const  { return fHitId;   }
    Int_t  GetTrackId()   const  { return fTrackId; }

    Double_t   GetTofMass2()         const  { return fTofMass2; }
    Double_t   GetMomentum()         const  { return fMomentum; }


    const std::vector<Double_t>   GetSigma()                      const  { return fSigma; }
    Double_t                      GetSigma(Int_t i)               const  { return fSigma[i]; }

    const std::vector<Double_t>   GetMean()                       const  { return fMean; }
    Double_t                      GetMean(Int_t i)                const  { return fMean[i]; }

    const std::vector<Double_t>   GetDistances()                  const  { return fDistances; }
    Double_t                      GetDistances(Int_t i)           const  { return fDistances[i]; }

    const std::vector<Double_t>   GetLikelihoods()                const  { return fLikelihoods; }
    Double_t                      GetLikelihoods(Int_t i)         const  { return fLikelihoods[i]; }

    const std::vector<Double_t>   GetPosteriorProbs()             const  { return fPosteriorProbs; }
    Double_t                      GetPosteriorProbs(Int_t i)      const  { return fPosteriorProbs[i]; }

    virtual void Print(Option_t* option = "") const; 
    
private:
    
    Int_t    fId;       // own unique ID (index in the corresponding array) 
    Int_t    fHitId;    // corresponding hit id
    Int_t    fTrackId;  // corresponding (mc- or rc-) track id
  
    Double_t fTofMass2; // Mass-squared information from the Time-of-Flight system
    Double_t fMomentum;  // momentum used for calculations [GeV/c]

    // 0 - pion, 1- kaon, 2 - proton, 3 - deutron 
    std::vector<Double_t> fSigma; // sigma 
    std::vector<Double_t> fMean;  // mean 
    std::vector<Double_t> fDistances; // distances (number of sigmas) 
    std::vector<Double_t> fLikelihoods; // likelihoods (gaussian)
    std::vector<Double_t> fPosteriorProbs; // posteriors probability 
  


    ClassDef(SpdTofParticle,1)
};

#endif  /* __SPDTOFPARTICLE_H__ */

