// $Id$
// Author: artur   2021/10/06

#ifndef __SPDPYTHIA8DECAYER_H__
#define __SPDPYTHIA8DECAYER_H__

#include "TVirtualMCDecayer.h"
#include "SpdPythia8.h"

#include <map>
#include <set>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPythia8Decayer                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArrray;
class TLorentzVector;
class SpdPythia8;
class SpdPrimGenParSet;
class SpdDecayer;

class SpdPythia8Decayer: public TVirtualMCDecayer {

public:

    SpdPythia8Decayer();
    SpdPythia8Decayer(SpdDecayer* decayer);
    
    virtual ~SpdPythia8Decayer();
    
    virtual void      Init();
    
    virtual void      Decay(Int_t pdg, TLorentzVector* p);
    virtual Int_t     ImportParticles(TClonesArray *particles);
    
    void              SetSeed(UInt_t seed = 0, Bool_t UseExternalGenerator = false);
    void              SetDecayer(SpdDecayer* decayer) { fDecayer = decayer; }
    
    UInt_t            GetSeed()    const { return fSeed;      }
    Bool_t            GetExtGen()  const { return fUseExtGen; } 
    SpdDecayer*       GetDecayer() const { return fDecayer;   }
    
    SpdPythia8*       GetPythia()  { return fPythia; }

    virtual Float_t   GetLifetime(Int_t pdg); // particle lifetime in seconds
    virtual Float_t   GetPartialBranchingRatio(Int_t ipart) { return 0.; }
    
    virtual Bool_t    FillParsIn(SpdPrimGenParSet* params, Int_t index);
    virtual Bool_t    LoadParsFrom(SpdPrimGenParSet* params, Int_t index);  
    
    void              PrintParticleDecayChannels(Int_t pdg);
    void              SelectForcedDecay(Int_t pdg, Int_t channel);
    void              ForceSelectedDecays();
    void              ClearSelectedDecays() { fSelectedDecays.clear(); } 
    
    void              SetMayDecay(Int_t pdg, Bool_t mayDecay = true);
    
    // -----------------------------------------------------------------
    virtual void      SetForceDecay(Int_t /*type*/) {} // not implemented here
    virtual void      ForceDecay() {}              // not implemented here
    virtual void      ReadDecayTable() {}          // not implemented here
   
protected:

    Int_t             fParticlePdg;  //! current particle id
    TClonesArray*     fParticles;    //! list of output particles (!NOT USED!)
    
    UInt_t            fSeed;         //! inner random seed [0, PYTHIA8_RANMAX]
    Bool_t            fUseExtGen;    //! use external generator in Pythia8 (deafult: false)
        
    Bool_t            fInit;          
    SpdDecayer*       fDecayer;      
 
    SpdPythia8*       fPythia;       //! pointer to Pythia8
    
    std::map< Int_t, std::set<Int_t> > fSelectedDecays;

    // auxiliary 
    inline Pythia8::Pythia* Pythia() { return fPythia->fPythia; }

    ClassDef(SpdPythia8Decayer,1)
};

#endif  /* __SPDPYTHIA8DECAYER_H__ */

