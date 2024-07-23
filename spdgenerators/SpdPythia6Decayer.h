// $Id$
// Author: artur   2018/08/13

#ifndef __SPDPYTHIA6DECAYER_H__
#define __SPDPYTHIA6DECAYER_H__

#include <TPythia6Decayer.h>
#include <map>
#include <set>

//using std::map;
//using std::set;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPythia6Decayer                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TLorentzVector;
class TClonesArray;
class TPythia6;
class SpdPrimGenParSet;
class SpdDecayer;

class SpdPythia6Decayer: public TPythia6Decayer {

public:

    SpdPythia6Decayer();
    SpdPythia6Decayer(SpdDecayer* decayer);
    
    virtual ~SpdPythia6Decayer();
    
    virtual void  Init();
  
    virtual void  Decay(Int_t idpart, TLorentzVector* p);
    virtual Int_t ImportParticles(TClonesArray *particles);
   
    void   SetSeed(UInt_t seed = 0);
    
    UInt_t GetSeed() const { return fSeed; }
    
    TPythia6*   GetPythia();
    
    void        SetDecayer(SpdDecayer* decayer) { fDecayer = decayer; }
    SpdDecayer* GetDecayer() const { return fDecayer; }
    
    virtual Bool_t FillParsIn(SpdPrimGenParSet* params, Int_t index);
    virtual Bool_t LoadParsFrom(SpdPrimGenParSet* params, Int_t index);  
    
    virtual Float_t GetLifetime(Int_t pdg);
   
    void PrintParticleDecayChannels(Int_t particle /*pdg number*/);
    void SelectForcedDecay(Int_t particle, Int_t channel);
    void ForceSelectedDecays();
    void ClearSelectedDecays() { fSelectedDecays.clear(); } 
    
protected:
  
    Int_t         fParticlePdg;  //! current particle id
    TClonesArray* fParticles;    //! list of output particles
    
    UInt_t        fSeed;         //! Inner random seed (!NOT USED!)
    
    Bool_t        fInit;
    SpdDecayer*   fDecayer;
    
    std::map< Int_t, std::set<Int_t> > fSelectedDecays;
    
    ClassDef(SpdPythia6Decayer,1)
};

#endif  /* __SPDPYTHIA6DECAYER_H__ */

