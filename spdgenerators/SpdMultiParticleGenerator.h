// $Id$
// Author: artur   2019/02/01

#ifndef __SPDMULTIPARTICLEGENERATOR_H__
#define __SPDMULTIPARTICLEGENERATOR_H__

#include "SpdGenerator.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMultiParticleGenerator                                                  //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMultiParticleGenerator: public SpdGenerator {

#define QH_    1000000000
#define EECUT_ 1e-14   
  
public:

    SpdMultiParticleGenerator();
    SpdMultiParticleGenerator(const char* Name);
    
    virtual ~SpdMultiParticleGenerator();
   
    virtual void    Reset();
    
    virtual Bool_t  Init();
    
    virtual Bool_t  ReadEvent(FairPrimaryGenerator* primGen);
    
    /* setters */
    virtual Bool_t  EnableExternalDecayer();
    virtual void    SaveParticles(Bool_t sp = kTRUE) { fSaveParticles = sp; }
    
    virtual void    SetNP(Int_t np);   
    virtual void    SetParticle(Int_t i, Double_t px /*GeV*/, Double_t py /*GeV*/, Double_t pz /*GeV*/,
                                Int_t pdg);
    virtual void    SetParticle(Int_t i, Double_t px /*GeV*/, Double_t py /*GeV*/, Double_t pz /*GeV*/,
                                Int_t Z, Int_t A, Int_t Q = QH_ /*e-*/, Double_t EE = 0. /*GeV*/);
    virtual void    SetVertex(Int_t i, Double_t vx /*cm*/, Double_t vy /*cm*/, Double_t vz /*cm*/);
    virtual void    SetTime(Int_t i, Double_t t /*s*/);
    
    /* getters */
    inline Int_t    GetN() const { return fNPAccepted; }
    
    inline Int_t    GetPdg(Int_t i) const;
    inline Bool_t   GetMomentum(Int_t i, Double_t& px, Double_t& py, Double_t& pz) const;
    inline Bool_t   GetVertex(Int_t i, Double_t& vx, Double_t& vy, Double_t& vz) const;
    inline Double_t GetTime(Int_t i) const;
    inline Int_t    GetIonQ(Int_t i) const;
    inline Double_t GetIonExcEnergy(Int_t i) const;
    
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
  
    Int_t  fNP;                             // Total number of particles (np)
     
    std::vector<Int_t>      fPdg;           // Particle type 
    std::vector<Double_t>   fMomentum;      // Particle momentum: px,py,pz (array of size = 3*np), [GeV/c] 
    std::vector<Double_t>   fVertex;        // Vertex coordinates: x,y,z (array of size = 3*np), [cm]
    std::vector<Double_t>   fTime;          // Vertex time [s]
    std::vector<Int_t>      fIonCharge;     // Ion electric charge [e-] 
    std::vector<Double_t>   fIonExcEnergy;  // Ion excitation energy [GeV]
    
    Int_t  fNPAccepted;                     // Number of accepted particles       
    std::vector<Bool_t>     fPAccepted;     // set of accepted particles 
    
    Bool_t     fEnableDecayer;  // Indicates that external decayer is enable
    Bool_t     fSaveParticles;  // Specifies whether to save the list of particles as parameters 
    
    Bool_t     fInit;           // Init mark
    
    virtual Bool_t AddIon(Int_t Z, Int_t A, Int_t Q, Double_t EE); 
    virtual Bool_t CheckParticle(Int_t pdg);
   
    Bool_t     CheckParticleNum(const Char_t* func, Int_t pnum) const;
    Bool_t     CheckInit(const Char_t* func) const;
   
    ClassDef(SpdMultiParticleGenerator,1)
};

//_____________________________________________________________________________  
inline Int_t SpdMultiParticleGenerator::GetPdg(Int_t i) const 
{ 
    return (i < 0 || i >= fNP) ? 0 : fPdg[i];
}

//_____________________________________________________________________________  
inline Bool_t SpdMultiParticleGenerator::GetMomentum(Int_t i, Double_t& px, Double_t& py, Double_t& pz) const 
{ 
    if (i < 0 || i >= fNP) return kFALSE;
  
    px = fMomentum[3*i]; 
    py = fMomentum[3*i+1]; 
    pz = fMomentum[3*i+2]; 
    
    return kTRUE;
}

//_____________________________________________________________________________  
inline Bool_t SpdMultiParticleGenerator::GetVertex(Int_t i, Double_t& vx, Double_t& vy, Double_t& vz) const 
{ 
    if (i < 0 || i >= fNP) return kFALSE;
   
    if (fVertex.empty()) { 
        vx = 0; vy = 0; vz = 0; 
        return kTRUE;
    }
 
    vx = fVertex[3*i]; 
    vy = fVertex[3*i+1]; 
    vz = fVertex[3*i+2];     
   
    return kTRUE;
}

//_____________________________________________________________________________  
inline Double_t SpdMultiParticleGenerator::GetTime(Int_t i) const 
{
    if (i < 0 || i >= fNP) return 0.;
    return (fTime.empty()) ? 0 : fTime[i]; 
}

//_____________________________________________________________________________  
inline Int_t SpdMultiParticleGenerator::GetIonQ(Int_t i) const 
{ 
    if (i < 0 || i >= fNP) return 0;
    return (fIonCharge.empty()) ? 0 : fIonCharge[i];
}

//_____________________________________________________________________________  
inline Double_t SpdMultiParticleGenerator::GetIonExcEnergy(Int_t i) const 
{ 
    if (i < 0 || i >= fNP) return 0.;
    return (fIonExcEnergy.empty()) ? 0 : fIonExcEnergy[i];
}

#endif  /* __SPDMULTIPARTICLEGENERATOR_H__ */

