// $Id$
// Author: artur   2015/03/12

#ifndef __SPDPYTHIA6GENERATOR_H__
#define __SPDPYTHIA6GENERATOR_H__

#include <TRandom3.h>
#include <map>
#include <set>

#include "SpdPythia6.h"
#include "SpdGenerator.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SpdPythia6Generator                                                       //
//                                                                            //
//                                                                            //
//  Method void Initialize(frame,beam,target,win).                            //
//                                                                            //
//  PURPOSE: to initialize the generation procedure.                          //
//  ARGUMENTS: See documentation for details.                                 //
//                                                                            //
//    frame   - specifies the frame of the experiment:                        //
//              "CMS","FIXT","USER","FOUR","FIVE","NONE"                      //
//    beam,                                                                   //
//    target  - beam and target particles                                     //
//              (with additionaly charges, tildes or "bar"):                  //
//              e, nu_e, mu, nu_mu, tau, nu_tau,                              //
//              gamma, pi, n, p,                                              //
//              Lambda, Sigma, Xi, Omega, pomeron, reggeon                    //
//                                                                            //
//    win:    - related to energy system (GeV):                               //
//              for frame == "CMS"  - total energy of system                  //
//              for frame == "FIXT" - momentum of beam particle               //
//              for frame == "USER" - dummy - see documentation.              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TMCParticle;

class SpdPythia6Generator : public SpdGenerator {

public: 

  SpdPythia6Generator();
  SpdPythia6Generator(const char* Name);
  
  virtual ~SpdPythia6Generator();

  SpdGenerator* CloneGenerator() const { return new SpdPythia6Generator(*this); }
  
  void ResetEvent();
 
  virtual Bool_t Init();
  
  virtual void Initialize(const Char_t *frame, const Char_t* beam, 
                          const Char_t *target, Float_t win /*GeV*/);
 
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen = 0);
  
  /* _____________ settings _____________ */
    
  void SetSeed(UInt_t seed = 0, UInt_t ncycle = 1);
   
  void SetVGenOpt(Int_t opt, Double_t vd = -1 /*cm*/); /* 0, 1, 2 */
  void SetKeepEvent(Bool_t v); 
  
  virtual void  SetSeeds(const std::vector<UInt_t>& seeds); 
  
  /* _____________ gettings _____________ */
  
  SpdPythia6* GetGenerator();
  
  inline const Char_t* GetFrame()  const { return fFrame;  }
  inline const Char_t* GetTarget() const { return fTarget; }
  inline const Char_t* GetBeam()   const { return fBeam;   }
  inline Float_t       GetEnergy() const { return fEnergy; }   
  
  virtual void  GetSeeds(std::vector<UInt_t>& seeds);
  
  inline  Int_t GetNPrimaries()  const { return fNP; }
  
  inline const TClonesArray* GetEvent() const { return fParticles; } 
  
  inline  Bool_t IsInit()      const { return fInit; }
  inline  Bool_t IsInitSeeds() const { return fInitSeeds; }
    
  virtual Bool_t IsDecayerEnable() const { return (fVgenopt == 0); }
 
  /* ____________________________________ */
  
  virtual Bool_t FillParsIn(SpdPrimGenParSet* params); 
  virtual Bool_t LoadParsFrom(SpdPrimGenParSet* params); 
  
  virtual void Print(Option_t* opt) const;
  
  /*_______________ test _______________ */
  
  void  SwitchTestMode(Int_t opt = 1) { fTestModeOpt = opt; }
  void  ClearTest();
  
  struct Py6TestData {
    
      Py6TestData();
      Py6TestData(Double_t p, Int_t i);
      
      Int_t    fIden;    // identification mark
      Double_t fN;       // total number of a such type particles
      Double_t fPsum;    // kinetic energy (summarized)
      Double_t fPmin;    // kinetic energy (min)
      Double_t fPmax;    // kinetic energy (max)
      
      void FillP(Double_t p) 
      { 
           fPsum += p;
           if (p < fPmin) fPmin = p;
           if (p > fPmax) fPmax = p;
      }
  };  
  
private:
   
  Bool_t GenerateDirectly(FairPrimaryGenerator* primGen = 0);
  Bool_t GenerateStandard(FairPrimaryGenerator* primGen = 0);
  
  void   InitSeeds();
  
  bool   IsAcceptableParticle(Int_t pdg, Bool_t edump) const;
  bool   IsAcceptableEvent(TClonesArray* plist, const std::set<Int_t>& psel, Bool_t edump) const; 
  
  TString        fFrame;      // frame of the experiment
  TString        fBeam;       // beam particle
  TString        fTarget;     // target particle
  Float_t        fEnergy;     // [GeV] energy of system (or beam)
   
  Int_t          fVgenopt;    // Generator option (default = 2)
  Bool_t         fKeepEvent;  // Indicates for event particles storing locally
  
  UInt_t         fSeed;       // Inner random seed
  UInt_t         fSeedCycle;  // [0,...]
  
  Bool_t         fInit;       // Init generator mark
  Bool_t         fInitSeeds;  // Init mark for generator point
  
  Int_t          fNP;         // Number of primary particles
  
  Double_t       fMaxDecayD;  // [cm] Max. decay vertex distance (default = 1e-12 m), only for Generator option = 2 
  
  SpdPythia6*    fPythia;     //! The generator
  
  /* ----- Local event particles storing (optionally) ----- */
  
  TClonesArray*  fParticles;  //! Current event particles local set (if fKeepEvent)
  
  /* ----- TESTING MODE  (optionally) ----- */
 
  Int_t    fTestModeOpt;      // Testing mode option
    
  UInt_t   fTestNAttempts;                       //!
  UInt_t   fTestNGenEvents;                      //!
  std::map<Int_t,Py6TestData*> fTestParticles;   //!
  
  void AddTestParticle(TMCParticle* part, Int_t iden);
  
  ClassDef(SpdPythia6Generator,1);

};

#endif
