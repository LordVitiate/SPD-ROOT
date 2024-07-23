
// $Id$
// Author: artur   2021/10/23

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPythia8Generator                                                        //
//                                                                            //
// A wrapper on Pythia8                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __SPDPYTHIA8GENERATOR_H__
#define __SPDPYTHIA8GENERATOR_H__

#include <TClonesArray.h>
#include "SpdGenerator.h"
#include "SpdPythia8.h"

class SpdPythia8Generator : public SpdGenerator {
    
public:
    struct P8EventFilter {
        // This function should be overloaded with
        // your own event filter. It returns "true"
        // for good events and "false" otherwise.
        virtual bool CheckEvent(SpdPythia8* pythia);

        virtual ~P8EventFilter() = default;

        bool FilterEvent(SpdPythia8* pythia);
    };

    P8EventFilter* event_filter = nullptr;

    SpdPythia8Generator();
    SpdPythia8Generator(const Char_t* Name);
    
    virtual ~SpdPythia8Generator();

    virtual Bool_t Init(); 
    
    SpdGenerator* CloneGenerator() const { return new SpdPythia8Generator(*this); }

    virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen = 0);
    
    /* _____________ settings _____________ */
    
    void SetParameters(const Char_t* par);
    void SetParameter(const Char_t* par);
    
    void SetMaxDecayVertexDistance(Double_t d) { fMaxDecayD = d; fInit = false; }
    
    void SetExternalGenerator(Bool_t UseExternalGenerator = true);
    void SetGenSeed(UInt_t seed = 0);
    void SetSeed(UInt_t seed = 0, Bool_t UseExternalGenerator = false);
    
    virtual void SetSeeds(const std::vector<UInt_t>& /*seeds*/);
    
    void SetBeam(Int_t beamA, Int_t beamB, Double_t Ecms);
  
    /* _____________ gettings _____________ */
    
    SpdPythia8*   GetGenerator();
    
    Bool_t        IsInit() const { return fInit; }
    
    Double_t      GetMaxDecayVertexDistance() const { return fMaxDecayD; }
    
    UInt_t        GetSeed()   const { return fSeed;      }
    Bool_t        GetExtGen() const { return fUseExtGen; } 
    
    virtual void  GetSeeds(std::vector<UInt_t>& seeds);
    
    Int_t         GetBeamA()  const { return fBeamA;  }
    Int_t         GetBeamB()  const { return fBeamB;  }
    Double_t      GetEnergy() const { return fEnergy; }
    
    TClonesArray* GetCurrentEvent() const { return (fPythia) ? (TClonesArray*)fPythia->GetListOfParticles() : 0; }
    
    /* ____________________________________ */
  
    virtual Bool_t FillParsIn(SpdPrimGenParSet* params); 
    virtual Bool_t LoadParsFrom(SpdPrimGenParSet* params); 
   
    virtual void   Print(Option_t* opt) const;

private:
        
    SpdPythia8*    fPythia;        //! pointer to Pythia8
    
    Bool_t         fInit;          //! Init generator mark
    Double_t       fMaxDecayD;     //! max. decay vertex distance, [cm] 
    UInt_t         fSeed;          //! inner random seed [0, 2147483647]
    Bool_t         fUseExtGen;     //! use external generator in Pythia8 (deafult: false)
    
    Int_t          fBeamA;         //! Beam A particle type
    Int_t          fBeamB;         //! Beam B particle type
    Double_t       fEnergy;        //! CM energy of collision
   
   ClassDef(SpdPythia8Generator,1);
};

#endif /* !SPDP8GENERATOR_H */
