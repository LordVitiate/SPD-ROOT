// $Id$
// Author: artur   2021/11/19

#ifndef __SpdAegParticle_H__
#define __SpdAegParticle_H__

#include <TObject.h>
#include <vector>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdAegParticle                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdAegParticle: public TObject {

public:

    SpdAegParticle();
    virtual ~SpdAegParticle();
    
    // ----- Setters -----
    
    void   SetId(Int_t id)       { fId = id;      }
    void   SetHitId(Int_t id)    { fHitId = id;   }
    void   SetTrackId(Int_t id)  { fTrackId = id; }


    void   SetLikelihoods(std::vector<Double_t> Likelihoods)        { fLikelihoods= Likelihoods; }
    void   SetMomentum(Double_t mom)  { fMomentum= mom; }

     void   SetPth(std::vector<Double_t> vpth)        { fvpth= vpth; }


    // ----- Getters -----
    
    Int_t  GetId()        const  { return fId;      }
    Int_t  GetHitId()     const  { return fHitId;   }
    Int_t  GetTrackId()   const  { return fTrackId; }



    
    const std::vector<Double_t>   GetLikelihoods()                const  { return fLikelihoods; }
    Double_t                      GetLikelihoods(Int_t i)         const  { return fLikelihoods[i]; }

    const std::vector<Double_t>   GetPth()                const  { return fvpth; }


    virtual void Print(Option_t* option = "") const; 
    
private:
    
    Int_t    fId;       // own unique ID (index in the corresponding array) 
    Int_t    fHitId;    // corresponding hit id
    Int_t    fTrackId;  // corresponding (mc- or rc-) track id
    Double_t fMomentum;  // momentum used for calculations [GeV/c]

    std::vector<Double_t> fLikelihoods; // likelihoods (gaussian): 0 - pion,  1 - kaon, 2 - proton
  
    std::vector<Double_t> fvpth; // pth : 0 - pion,  1 - kaon, 2 - proton


    ClassDef(SpdAegParticle,1)
};

#endif  /* __SpdAegParticle_H__ */

