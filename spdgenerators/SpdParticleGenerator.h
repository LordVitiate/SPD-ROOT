// $Id$
// Author: artur   2019/02/01

#ifndef __SPDPARTICLEGENERATOR_H__
#define __SPDPARTICLEGENERATOR_H__

#include "SpdGenerator.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdParticleGenerator                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdParticleGenerator: public SpdGenerator {

public:

    SpdParticleGenerator();
    
    SpdParticleGenerator(const char* Name);
    
    SpdParticleGenerator(Int_t pdg, 
                         Double_t px, Double_t py, Double_t pz, /*GeV*/       
                         const char* Name = "SpdParticleGenerator", 
                         Int_t np = 1, 
                         Double_t vx = 0., Double_t vy = 0., Double_t vz = 0., /*cm*/
                         Double_t t = 0. /*s*/
                        );
    
    SpdParticleGenerator(Int_t Z, Int_t A,  
                         Double_t px, Double_t py, Double_t pz, /*GeV*/       
                         const char* Name = "SpdParticleGenerator", 
                         Int_t np = 1, 
                         Double_t vx = 0., Double_t vy = 0., Double_t vz = 0., /*cm*/
                         Double_t t = 0. /*s*/
                        );
      
    virtual ~SpdParticleGenerator();
       
    SpdGenerator* CloneGenerator() const { return new SpdParticleGenerator(*this); }
 
    virtual void   Reset();
    
    virtual Bool_t Init();
    
    virtual void   Initialize(Int_t pdg,  
                              Double_t px, Double_t py, Double_t pz, /*GeV*/             
                              Int_t np = 1, 
                              Double_t vx = 0., Double_t vy = 0., Double_t vz = 0., /*cm*/
                              Double_t t = 0. /*s*/
                             );
    
    virtual void   Initialize(Int_t Z, Int_t A,  
                              Double_t px, Double_t py, Double_t pz, /*GeV*/             
                              Int_t np = 1, 
                              Double_t vx = 0., Double_t vy = 0., Double_t vz = 0., /*cm*/
                              Double_t t = 0. /*s*/
                             );
    
    virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);
    
    /* setters */
    virtual Bool_t EnableExternalDecayer();
    
    virtual void   SetPdg(Int_t pdg);
    virtual void   SetNP(Int_t np);  
    virtual void   SetMomentum(Double_t px, Double_t py, Double_t pz);
    virtual void   SetMomThetaPhi(Double_t p, Double_t theta, Double_t phi);
    virtual void   SetVertex(Double_t vx, Double_t vy, Double_t vz);   
    virtual void   SetTime(Double_t t);
    virtual void   SetIonQ(Int_t Q);
    virtual void   SetIonExcEnergy(Int_t ee);
    
    /* getters */
    inline Int_t    GetPdg() const { return fPdg; }
    inline Int_t    GetNP()  const { return fNP;  }
    inline void     GetMomentum(Double_t& px, Double_t& py, Double_t& pz) const { px = fPx; py = fPy; pz = fPz; }
    inline void     GetVertex(Double_t& vx, Double_t& vy, Double_t& vz)   const { vx = fVx; vy = fVy; vz = fVz; }
    inline Double_t GetTime() const { return fTime; }
    inline Int_t    GetIonQ() const { return fIonQ; }
    inline Double_t GetIonExcEnergy() const { return fIonExcEnergy; }
    
    inline Bool_t   IsInit() const { return fInit; }
    
    virtual Bool_t  IsDecayerEnable() const { return fEnableDecayer; }
       
    /*  fill/load parameters */
    virtual Bool_t FillParsIn(SpdPrimGenParSet* params); 
    virtual Bool_t LoadParsFrom(SpdPrimGenParSet* params);
       
    /*  print generator state */
    virtual void   Print(Option_t* opt) const;
   
    /* static methods */
    static void    Momentum(Double_t p/*GeV*/, Double_t theta /*deg*/, Double_t phi/*deg*/,
                            Double_t& px, Double_t& py, Double_t& pz);
    static void    Momentum(Double_t p/*GeV*/,
                            Double_t& px, Double_t& py, Double_t& pz);
    
    static Int_t   IonPdg(Int_t Z, Int_t A);
    static Int_t   IonZ(Int_t pdg);
    static Int_t   IonA(Int_t pdg);
    static void    IonZA(Int_t pdg, Int_t& Z, Int_t& A);
    static Bool_t  IsIon(Int_t pdg);
    static TString IonName(Int_t pdg);
   
protected:

    Int_t      fPdg;           // Particle type 
    Int_t      fNP;            // Multiplicity (number of particles to generate) 
    Double_t   fPx, fPy, fPz;  // Particle momentum [GeV/c]
    Double_t   fVx, fVy, fVz;  // Particle vertex coordinates [cm]
    Double_t   fTime;          // Vertex time [s] 
    Int_t      fIonQ;          // Ion electric charge [e-] 
    Double_t   fIonExcEnergy;  // Ion excitation energy [GeV]
    
    Bool_t     fEnableDecayer; // Indicates that external decayer is enable
    
    Bool_t     fInit;          // Init mark

    virtual Bool_t AddIon(Int_t pdg);
    virtual Bool_t CheckParticle(Int_t pdg);
    
    ClassDef(SpdParticleGenerator,1)
};

#endif  /* __SPDPARTICLEGENERATOR_H__ */

