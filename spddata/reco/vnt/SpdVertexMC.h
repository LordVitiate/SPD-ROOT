// $Id$
// Author: artur   2020/10/01

#ifndef __SPDVERTEXMC_H__
#define __SPDVERTEXMC_H__

#include <TObject.h>
#include <TVector3.h>
#include <set>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdVertexMC                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdVtxFitPar;

class SpdVertexMC: public TObject {

public:

    SpdVertexMC();
    virtual ~SpdVertexMC();
    
    virtual void DeleteFitPars();
    virtual void Clear(Option_t* option = "");
    virtual void RemoveParticles(Option_t* option = "all");  // "all", "in", "out", "inner"
    
    //------------ Setters ------------
    
    void SetId(Int_t id)         { fId = id; }
    void SetGeneration(Int_t ig) { fGeneration = ig; }
    void SetProcessId(Int_t ip)  { fProcessId = ip; }
    void SetDetectorId(Int_t id) { fDetectorId = id; }

    void SetPosition(Double_t x, Double_t y, Double_t z) { fX = x; fY = y; fZ  = z; }
    void SetPosition(const TVector3 pos) { fX = pos.X(); fY = pos.Y(); fZ = pos.Z(); }
    
    void SetRegionSize(Double_t r) { fRegionSize = r; }
    
    void SetSingleProcessType(Bool_t spt) { fIsSingleProcessType = spt; }
    
            /* add particle (id) */
    void AddIn(Int_t id)     { fInParticles.insert(id);    }
    void AddOut(Int_t id)    { fOutParticles.insert(id);   }  
    void AddInner(Int_t id)  { fInnerParticles.insert(id); }
   
    void SetNTracks(Int_t n) { fNTracks = n; }  
   
    // get (or create) vertex fit paramaters
    SpdVtxFitPar* FitPars();
    
    //------------ Getters ------------
    Bool_t IsPrimary()      const { return (fGeneration == 0); }

    Int_t  GetId()          const { return fId; }
    Int_t  GetGeneration()  const { return fGeneration; }
    Int_t  GetProcessId()   const { return fProcessId;  }
    Int_t  GetDetectorId()  const { return fDetectorId; }
    
    const Char_t* GetProcessName() const;
    
            /* return vertex coordinates */
    Bool_t   IsVertexPosDefined()  const { return (fZ > -1.e9); }
    Bool_t   IsSingleProcessType() const { return fIsSingleProcessType; }
    Bool_t   IsSingleInParticle()  const { return (fInParticles.size() == 1); }
    
    Double_t GetX()   const { return fX; }
    Double_t GetY()   const { return fY; }
    Double_t GetZ()   const { return fZ; }
    TVector3 GetPos() const { return TVector3(fX,fY,fZ); }
    
    Double_t GetRegionSize() const { return fRegionSize; }
    
            /* return number of particle */
    Int_t GetNIn()    const { return fInParticles.size();    }
    Int_t GetNOut()   const { return fOutParticles.size();   }
    Int_t GetNInner() const { return fInnerParticles.size(); }
    Int_t GetNInOut() const { return fInParticles.size() + fOutParticles.size(); }
    
    Bool_t HasIn()    const { return !fInParticles.empty();    }
    Bool_t HasOut()   const { return !fOutParticles.empty();   }
    Bool_t HasInner() const { return !fInnerParticles.empty(); }
    
            /* find particle of such id */
    Bool_t FindIn(Int_t id)    const { return (fInParticles.find(id) != fInParticles.end());       }
    Bool_t FindOut(Int_t id)   const { return (fOutParticles.find(id) != fOutParticles.end());     }
    Bool_t FindInner(Int_t id) const { return (fInnerParticles.find(id) != fInnerParticles.end()); }
    
    // WARNING Slow methods (complicity ~O(i))
    Int_t GetInParticle(Int_t i = 0) const; 
    Int_t GetOutParticle(Int_t i = 0) const;
    Int_t GetInnerParticle(Int_t i = 0) const;
    
    const std::set<Int_t>& GetInParticles()    const { return fInParticles;    }
    const std::set<Int_t>& GetOutParticles()   const { return fOutParticles;   }
    const std::set<Int_t>& GetInnerParticles() const { return fInnerParticles; }
    
    const std::set<Int_t>& GetParticles(Int_t i) const 
    { 
        switch (i) {
           case 0: { return fInParticles;    }
           case 1: { return fInnerParticles; }
           case 2: { return fOutParticles;   }
           break;
        }
        return fInParticles;
    }
    
    Int_t GetNTracks() const { return fNTracks; }  
    
    SpdVtxFitPar* GetFitPars() const { return fFitPars; }
   
    virtual void Print(Option_t* option = "") const;

private:

    Int_t    fId;          // vertex unique id (index in the corresponding array); id < 0 = undefined
    Int_t    fGeneration;  // vertex generation ( = 0 for primary vertex)
    Int_t    fProcessId;   // vertex (last) process id (default: kPNoProcess (=44), see ROOT:TMCProcess.h)
    Int_t    fDetectorId;  // vertex position detector id (module, see SpdDetectorList.h)
    
    Bool_t   fIsSingleProcessType; // default = true

    Double_t fX, fY, fZ;   // vertex (last) coordinates [cm]
    Double_t fRegionSize;  // vertex spatial size [cm] (max. distance between start positions of any two constituent particles)

    std::set<Int_t> fInParticles;    // list of MC in-particles (id)
    std::set<Int_t> fOutParticles;   // list of MC out-particles (id)
    std::set<Int_t> fInnerParticles; // list of MC inner vertex particles (id)
    
    Int_t    fNTracks;     // number of tracks in the vertex accepted for fit! 

    SpdVtxFitPar* fFitPars;  // fit parameters (optionally)

    ClassDef(SpdVertexMC,1)
};

#endif  /* __SPDVERTEXMC_H__ */

