// $Id$
// Author: artur   2020/09/22

#ifndef __SPDMCEVENT_H__
#define __SPDMCEVENT_H__

#include <TNamed.h>
#include <TObjArray.h>
#include <vector>

#include "SpdBaseParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCEvent                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCEvent: public TNamed {

public:

    SpdMCEvent();
    virtual ~SpdMCEvent();

    virtual void Clear(Option_t* option = "");
    
    /* ====== SETTERS ======*/
    
    void    SetRunId(UInt_t id)  { fRunId = id;   }
    void    SetEventId(Int_t id) { fEventId = id; }
    
    void    SetMotherRunId(UInt_t id)     { fMotherRunId = id;   }
    void    SetMotherEventId(Int_t id)    { fMotherEventId = id; }
    
    // Primary particles
    void    SetNTotalPParticles(Int_t np) { fNTotalPParticles = np; }
    void    SetNInPParticles(Int_t np)    { fNInPParticles = np;    }
    void    SetNOutPParticles(Int_t np)   { fNOutPParticles = np;   }
    
    // INPUT mc-tracks & OUPUT particles 
    void    SetNMCTracks(Int_t nt)        { fNMCTracks  = nt; }
    void    SetNParticles(Int_t np)       { fNParticles = np; }
     
    // OUTPUT DATA:
    void    SetMaxParticleGeneration(Int_t g) { fMaxParticleGeneration = g; }  
    void    SetMaxVertexGeneration(Int_t g)   { fMaxVertexGeneration = g;   }   
    
    Bool_t  AddInfo(TString info_name); // "true" if has not been added previously
    
    /* ====== GETTERS ====== */
    
    UInt_t  GetRunId()   const { return fRunId;   }
    Int_t   GetEventId() const { return fEventId; }
    
    Int_t   GetNTotalPParticles() const { return fNTotalPParticles; }
    Int_t   GetNInPParticles()    const { return fNInPParticles;    }
    Int_t   GetNOutPParticles()   const { return fNOutPParticles;   }

    Int_t   GetNMCTracks()  const { return fNMCTracks;  }
    Int_t   GetNParticles() const { return fNParticles; }
    
    Int_t   GetMaxParticleGeneration()       const { return fMaxParticleGeneration; }  
    Int_t   GetMaxVertexGeneration()         const { return fMaxVertexGeneration;   } 
    Bool_t  CheckParticleGeneration(Int_t g) const { return !(g < 0) && !(g > fMaxParticleGeneration); }
    Bool_t  CheckVertexGeneration(Int_t g)   const { return !(g < 0) && !(g > fMaxVertexGeneration);   }
    
    Bool_t  HasFullListOfParticles() const;
    
    Int_t   GetNInfo() const { return (fInfo) ? fInfo->GetEntriesFast() : 0; }

    SpdBaseParSet* GetInfo(TString info_name);
        
    void    PrintEvent(Int_t opt = 0) const;
    
protected:
    
    UInt_t  fRunId;                  // run id
    Int_t   fEventId;                // event id (0,1,...)

    UInt_t  fMotherRunId;            // mother (or MC) run id
    Int_t   fMotherEventId;          // mother (or MC) event id
    
    Int_t   fNTotalPParticles;       // total number of primary particles (GENERATION = 0+1)
    Int_t   fNInPParticles;          // number of "incoming" primary particles (MID = -1)
    Int_t   fNOutPParticles;         // number of "outgoing" primary particles (GENERATION = 1)
    
    Int_t   fNMCTracks;              // mc-tracks (total input)
    Int_t   fNParticles;             // particles (total output)
    
    Int_t   fMaxParticleGeneration;  // max. particle generation number in the output
    Int_t   fMaxVertexGeneration;    // max. vertex generation number in the output
    
    TObjArray*  fInfo;               // list of infos 
    
    ClassDef(SpdMCEvent,1)
};

#endif  /* __SPDMCEVENT_H__ */

