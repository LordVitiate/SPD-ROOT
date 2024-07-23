// $Id$
// Author: artur   2021/11/19

#ifndef __SPDTSPARTICLE_H__
#define __SPDTSPARTICLE_H__

#include <TObject.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsParticle                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsParticle: public TObject {

public:

    SpdTsParticle();
    virtual ~SpdTsParticle();

    // ----- Setters -----
    
    void   SetId(Int_t id)        { fId = id;   }
    void   SetTrackId(Int_t id)   { fTrackId = id; }
    
    void   SetMomentum(Double_t mom)  { fMomentum = mom; }
    void   SetTruncMeandEdx(Double_t val) { fTruncMeandEdx = val; }
    
    void   SetLikelihoods(const std::vector<Double_t>& lh) { fLikelihoods = lh; }
    void   SetDistances(const std::vector<Double_t>& dist) { fDistances = dist; }
 
    // ----- Getters -----
    
    Int_t  GetId()          const { return fId; }
    Int_t  GetTrackId()     const { return fTrackId; }
    
    Double_t  GetMomentum()      const { return fMomentum; }
    Double_t  GetTruncMeandEdx() const { return fTruncMeandEdx; }
    
    const std::vector<Double_t>& GetLikelihoods() const { return fLikelihoods; }
    const std::vector<Double_t>& GetDistances()   const { return fDistances; }
    
    Double_t GetLikelihood(Int_t i)  const { return fLikelihoods.at(i); }
    Double_t GetDistance(Int_t i)    const { return fDistances.at(i); }
       
    virtual void Print(Option_t* option = "") const; 

private:
    
    Int_t  fId;        // own unique ID (index in the corresponding array) 
    Int_t  fTrackId;   // corresponding mc-track id
    
    Double_t fMomentum;       // momentum used for calculations [GeV/c]
    Double_t fTruncMeandEdx;  // truncated mean dE/dx [GeV/cm]
    
    std::vector<Double_t> fLikelihoods;  // likelihoods (gaussian) (0 - pion, 1 - kaon, 2 - proton)
    std::vector<Double_t> fDistances;    // distances (number of sigmas) (0 - pion, 1 - kaon, 2 - proton)
 
    ClassDef(SpdTsParticle,1)
};

#endif  /* __SPDTSPARTICLE_H__ */

