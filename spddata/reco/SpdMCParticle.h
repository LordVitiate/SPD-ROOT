// $Id$
// Author: artur   2020/09/22

#ifndef __SPDMCPARTICLE_H__
#define __SPDMCPARTICLE_H__

#include <TObject.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TMath.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCParticleInfo                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCParticleInfo: public TObject {

public:

    SpdMCParticleInfo():fFirstVertexId(-99),fLastVertexId(-99),fTrackId(-1),
                        fTsParticleId(-1),fTofParticleId(-1),fEcalParticleId(-1),fRsParticleId(-1),
                        fZdcParticleId(-1),fBbcParticleId(-1),fAegParticleId(-1){}
                        
    virtual ~SpdMCParticleInfo() {}
    
    /* Associated data info */
    
    /* particle vertex */
    Int_t      fFirstVertexId;       // origin vertex id = -99(undef.),-1 (primary, mother id = -1), 0, 1, ...
    Int_t      fLastVertexId;        // disappearing vertex id = -99 (undef.) -1 (primary, mother id = -1), 0, 1, ...
    
    /* associated objects: tracks, clusters, ... */
    Int_t      fTrackId;             // associated track id = -1 (no track), 0, 1, ...
    
    Int_t      fTsParticleId;        // associated TS particle id = -1 (no particle), 0, 1, ...
    Int_t      fTofParticleId;       // associated TOF particle id = -1 (no particle), 0, 1, ...
    Int_t      fEcalParticleId;      // associated ECAL particle id = -1 (no particle), 0, 1, ...
    Int_t      fRsParticleId;        // associated RS particle id = -1 (no particle), 0, 1, ...
    
    Int_t      fZdcParticleId;       // associated ZDC particle id = -1 (no particle), 0, 1, ...
    Int_t      fBbcParticleId;       // associated BBC particle id = -1 (no particle), 0, 1, ...
    Int_t      fAegParticleId;       // associated AEG particle id = -1 (no particle), 0, 1, ...
    
    ClassDef(SpdMCParticleInfo,1)
};

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCParticle                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCParticle: public TObject {

public:

    SpdMCParticle();
    virtual ~SpdMCParticle();
    
    virtual void DeleteInfo();
    virtual void Clear(Option_t* option = "");
    
    /* Set methods */
    
    void  SetMCTrackId(Int_t id )      { fMCTrackId     = id; }
    void  SetMCGeneratorId(Int_t id)   { fMCGeneratorId = id; }
    
    void  SetId(Int_t id)              { fId = id;         }
    void  SetMotherId(Int_t id)        { fMotherId = id;   }
    void  SetGeneration(Int_t ig)      { fGeneration = ig; }
    
    void  SetVertexG1MotherId(Int_t id)          { fVertexG1MotherId  = id; }
    void  SetVertexG1MotherPdg(Int_t pdg)        { fVertexG1MotherPdg = pdg; }
    void  SetVertexG1Mother(Int_t id, Int_t pdg) { fVertexG1MotherId  = id; fVertexG1MotherPdg = pdg; }
    
    void  SetSpecialMotherId(Int_t id, Int_t i = 0)          { fSpecialMotherId[0] = id;  }
    void  SetSpecialMotherSid(Int_t sid, Int_t i = 0)        { fSpecialMotherId[1] = sid; }
    void  SetSpecialMother(Int_t id, Int_t sid, Int_t i = 0) { fSpecialMotherId[0] = id; fSpecialMotherId[1] = sid; }
          
    void  SetPdgCode(Int_t pdg)        { fPdgCode = pdg; }
    void  SetVProcess(Int_t vp)        { fVProcess = vp; }
    void  SetVModule(Int_t vm)         { fVModule = vm;  }
    void  SetStartTime(Double_t t)     { fStartTime = t; }    
    void  SetStartPosition(TVector3 v) { fStartPos = v;  }
    void  SetStartMomentum(TVector3 p) { fStartMom = p;  }
    void  SetStartPosition(Double_t vx, Double_t vy, Double_t vz) { fStartPos.SetXYZ(vx,vy,vz); }
    void  SetStartMomentum(Double_t px, Double_t py, Double_t pz) { fStartMom.SetXYZ(px,py,pz); }
    
    void  SetChar(Char_t c) { fChar = c; }
    
    /* ++++++++ PARTICLE INFO ++++++++ */
    
    void  SetFirstVertexId(Int_t id)   { CreateInfo(); fInfo->fFirstVertexId  = id; }
    void  SetLastVertexId(Int_t id)    { CreateInfo(); fInfo->fLastVertexId   = id; }
    void  SetTrackId(Int_t id)         { CreateInfo(); fInfo->fTrackId        = id; }
    void  SetTsParticleId(Int_t id)    { CreateInfo(); fInfo->fTsParticleId   = id; }
    void  SetTofParticleId(Int_t id)   { CreateInfo(); fInfo->fTofParticleId  = id; }
    void  SetEcalParticleId(Int_t id)  { CreateInfo(); fInfo->fEcalParticleId = id; }
    void  SetRsParticleId(Int_t id)    { CreateInfo(); fInfo->fRsParticleId   = id; }
    void  SetZdcParticleId(Int_t id)   { CreateInfo(); fInfo->fZdcParticleId  = id; }
    void  SetBbcParticleId(Int_t id)   { CreateInfo(); fInfo->fBbcParticleId  = id; }
    void  SetAegParticleId(Int_t id)   { CreateInfo(); fInfo->fAegParticleId  = id; }
    
    /* Get methods */
       
    Int_t    GetMCTrackId()         const { return fMCTrackId;     }
    Int_t    GetMCGeneratorId()     const { return fMCGeneratorId; }
    
    Int_t    GetId()                const { return fId;         }
    Int_t    GetMotherId()          const { return fMotherId;   }
    Int_t    GetGeneration()        const { return fGeneration; }
    
    Int_t    GetVertexG1MotherId()  const { return fVertexG1MotherId;  }
    Int_t    GetVertexG1MotherPdg() const { return fVertexG1MotherPdg; }
    
    Int_t    GetSpecialMotherId(Int_t i = 0)  const { return fSpecialMotherId[i*2];   }
    Int_t    GetSpecialMotherSid(Int_t i = 0) const { return fSpecialMotherId[i*2+1]; }
          
    Int_t    GetPdgCode()  const { return fPdgCode;  }
    Int_t    GetVProcess() const { return fVProcess; }
    Int_t    GetVModule()  const { return fVModule;  }
    TString  GetVProcessName(Int_t proc_id) const;
    
    Bool_t   IsInDatabase() const;
    
    Double_t GetMass()            const;                        // GeV
    Double_t GetCharge()          const;                        // e
    Double_t GetStartEnergy()     const;                        // GeV
    TVector3 GetStartMom()        const { return fStartMom;  }  // GeV
    TVector3 GetStartPos()        const { return fStartPos;  }  // cm
    Double_t GetStartTime()       const { return fStartTime; }  // ns
    
    TLorentzVector GetStartMomL() const { return TLorentzVector(fStartMom,GetStartEnergy());  }
    TLorentzVector GetStartPosL() const { return TLorentzVector(fStartPos,fStartTime); }
 
    Double_t GetStartD()          const { return fStartPos.Mag();  }  // cm
    Double_t GetStartR()          const { return fStartPos.Perp(); }  // cm
    Double_t GetStartZ()          const { return fStartPos.Z();    }  // cm
    
    Double_t GetStartP()          const { return fStartMom.Mag();  }  // GeV
    Double_t GetStartPt()         const { return fStartMom.Perp(); }  // GeV
    Double_t GetStartPtheta()     const { return fStartMom.Theta()*TMath::RadToDeg(); }  // deg
    Double_t GetStartPphi()       const { return fStartMom.Phi()*TMath::RadToDeg();   }  // deg   
    
    Double_t GetStartBetaGamma()  const;                        // (p/m)
    
    TString  GetParticleName()    const; 
    TString  GetParticleType()    const; 
    
    Char_t   GetChar() const { return fChar; }
    
    /* ++++++++ PARTICLE INFO ++++++++ */
    
    const SpdMCParticleInfo* GetInfo() const { return fInfo; }
    
    Int_t  GetFirstVertexId()  const { return (fInfo) ? fInfo->fFirstVertexId : -1;  }
    Int_t  GetLastVertexId()   const { return (fInfo) ? fInfo->fLastVertexId : -1;   }
    Int_t  GetTrackId()        const { return (fInfo) ? fInfo->fTrackId : -1;        }
    Int_t  GetTsParticleId()   const { return (fInfo) ? fInfo->fTsParticleId : -1;   }
    Int_t  GetTofParticleId()  const { return (fInfo) ? fInfo->fTofParticleId : -1;  }
    Int_t  GetEcalParticleId() const { return (fInfo) ? fInfo->fEcalParticleId : -1; }
    Int_t  GetRsParticleId()   const { return (fInfo) ? fInfo->fRsParticleId : -1;   }
    Int_t  GetZdcParticleId()  const { return (fInfo) ? fInfo->fZdcParticleId : -1;  }
    Int_t  GetBbcParticleId()  const { return (fInfo) ? fInfo->fBbcParticleId : -1;  }
    Int_t  GetAegParticleId()  const { return (fInfo) ? fInfo->fAegParticleId : -1;  }
    
    virtual void Print(Option_t* option = "") const;

private:
    
    /* MC info */
    Int_t fMCTrackId;         // MC track ID: -1, 0, 1,...; 
    Int_t fMCGeneratorId;     // MC particle generator id (1,2...)
    
    /* Id */
    Int_t fId;                // unique particle id (0,1, ...)
    Int_t fMotherId;          // real mother particle id (-1: generator vertex particle; >= 0: other particles)
    
    Int_t fVertexG1MotherId;  // vertex mother particle (of generation = 1) id 
    Int_t fVertexG1MotherPdg; // vertex mother particle (of generation = 1) pdg number
    
    //-------------------------------------
    // special case - "physical" mother particle id
    // i = 0 :
    //     j = i*2   : v.m.p. id
    //     j = i*2+1 : v.m.p. special id
    
    Int_t fSpecialMotherId[2]; 
    
    //-------------------------------------
    //  particle generation definition:
    //
    //   0: in/inner primary vertex particle; 
    //   1: outer primary vertex particle;
    // > 1: other particles.
    
    Int_t fGeneration;       
    
    /* Particle info */
    
    Int_t      fPdgCode;        // PDG particle code
    Int_t      fVProcess;       // Vertex initial process
    Int_t      fVModule;        // Vertex module id (see SpdDetectorList/DetectorId/PassiveID)
    TVector3   fStartPos;       // Start position [cm]
    TVector3   fStartMom;       // Start momentum [GeV]
    Double_t   fStartTime;      // Start time [ns]
    
    //-----------------------------------------
    // 'x' :  particle of generation = 0 that have no real daughters of generation > 0
    // '-' :  "unvisible" particle (is not detectected directly by any detector, no direct hits)
    // '+' :  "visible" particle (is detected at least by one detector)
    
    Char_t     fChar;
    
    SpdMCParticleInfo* fInfo;   // assocciated objects info
    
    /* methods */
    
    inline void CreateInfo() { if (!fInfo) fInfo = new SpdMCParticleInfo(); }
    
    ClassDef(SpdMCParticle,1)
};

#endif  /* __SPDMCPARTICLE_H__ */

