// $Id$
// Author: artur   2017/11/15

#ifndef __SPDISOTROPICGENERATOR_H__
#define __SPDISOTROPICGENERATOR_H__

#include <TObject.h>
#include <TRandom3.h>
#include <TVector3.h>
#include <TClonesArray.h>

#include <map>
#include <vector>

#include "SpdGenerator.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdIsotropicGenerator                                                      //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TParticle;

class SpdIsotropicGenerator : public SpdGenerator {

public:

    SpdIsotropicGenerator();
    SpdIsotropicGenerator(const Char_t* Name);
    
    virtual ~SpdIsotropicGenerator();
    
    void Reset();
    void ResetEvent();
 
    virtual Bool_t Init();
    
    virtual void   Initialize(Int_t pdg, Double_t momentum, Int_t np = 1);
    
    virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen = 0);
     
    /* _____________ settings _____________ */
    
    void SetSeed(UInt_t seed = 0);
    
    void SetVGenOpt(Int_t opt);
    void SetKeepEvent(Bool_t v); 
     
    void SetSpherical(Double_t tmin = 0., Double_t tmax = 180., /* deg, [0, 180] */ 
                      Double_t pmin = 0., Double_t pmax = 360.  /* deg, [0, 360] */ );
    
    void SetSolid(Double_t tmin = 0., Double_t tmax = 180., /* deg, 0, 180 */ 
                  Double_t phi = 0.,   /* deg, [0, 360] */ 
                  Double_t theta = 0.  /* deg, [0, 180] */ );  
 
    void SetPgid(Double_t dp /* GeV/c */, Int_t nstep);
    
    virtual void  SetSeeds(const std::vector<UInt_t>& seeds); 
    
    /* _____________ gettings _____________ */
    
    virtual void  GetSeeds(std::vector<UInt_t>& seeds);
    
    Int_t GetNPrimaries() const { return fNP; }
    
    const TClonesArray* GetEvent() const { return fParticles; }
    
    inline  Bool_t IsInit() const { return fInit; }
    
    /* ____________________________________ */
  
    virtual Bool_t FillParsIn(SpdPrimGenParSet* params); 
    virtual Bool_t LoadParsFrom(SpdPrimGenParSet* params); 
    
    virtual void Print(Option_t* opt) const;
    
private:
  
    Bool_t GenerateIsotropic_0(FairPrimaryGenerator* primGen = 0); // option = -1, -2
    Bool_t GenerateIsotropic_1(FairPrimaryGenerator* primGen = 0); // option = 1
    Bool_t GenerateIsotropic_2(FairPrimaryGenerator* primGen = 0); // option = 2
    
    void AddParticle(FairPrimaryGenerator* primGen, Double_t px, Double_t py, Double_t pz);
    void PrintParticle(TParticle* p, Int_t i, Bool_t add); 
    
    Int_t      fVgenopt;       // Generator option
    Bool_t     fKeepEvent;     // Indicates for event particles storing locally
    
    Int_t      fPdg;           // pdg-number
    Double_t   fMomentum;      // particle momentum (GeV/c)
    Int_t      fNPL;           // PARTICLES per event or LEVEL (option: -1,-2)
    
    // |fVgenopt| = 1
    Double_t   fTmin;          // cos(theta_min)
    Double_t   fTmax;          // cos(theta_max)
    Double_t   fPhimin;        // phi_min, rad
    Double_t   fPhimax;        // phi_max, rad
    
    // |fVgenopt| = 2
    Double_t   fXTmin;         // cos(theta_min)
    Double_t   fXTmax;         // cos(theta_max)
    Double_t   fXPhi;          // phi(solid angle), rad
    Double_t   fXTheta;        // theta(solid angle), rad
    
    // Momentum generation
    Double_t   fDP;            // momentum step
    Int_t      fNP;            // number of steps by momentum
    
    UInt_t     fSeed;          // Inner random seed
    Bool_t     fInit;          // Init mark
    
    Int_t      fECounter;      // event counter (<= fNE)
    Int_t      fNParticles;    // Number of primary particles
   
    TClonesArray* fParticles;  //! Current event
 
    // Auxiliary 
    std::map<std::pair<Int_t,Int_t>, Int_t> fEdges;
    std::vector<TVector3> fVertices;
   
    void SplitFace(const Int_t face[], Int_t level);
    void MakeBisection(Int_t& x, const Int_t& i1, const Int_t& i2);
    
    ClassDef(SpdIsotropicGenerator,1)
};

#endif  /* __SPDISOTROPICGENERATOR_H__ */

