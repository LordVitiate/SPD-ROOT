// $Id$
// Author: artur   2020/10/04

#ifndef __SPDMCEVENTMAKER_H__
#define __SPDMCEVENTMAKER_H__

#include "SpdTask.h"

#include <TString.h>
#include <TClonesArray.h>

class SpdMCTrack;
class SpdMCEventHeader;
class SpdMCEvent;
class SpdMCParticle;
class SpdVertexMC;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCEventMaker                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCEventMaker: public SpdTask {

public:

    SpdMCEventMaker();
    virtual ~SpdMCEventMaker();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    virtual void Clear(Option_t* option = "");
 
    // Setters
    
    inline void SetParticleMaxGeneration(Int_t maxg); 
    inline void SetVertexMaxGeneration(Int_t maxv); 
    
    // Save data (particles & vertices) into the output file 
    void SaveEvent(Bool_t save = true)     { fSaveEvent = save;     }
    void SaveParticles(Bool_t save = true) { fSaveParticles = save; }
    void SaveVertices(Bool_t save = true)  { fSaveVertices  = save; }
    
    void SetVerboseLevel(Int_t level)      { fVerboseLevel = level; }
    
    // Getters
    
    SpdMCEvent*     GetEvent()     const { return fEvent;     }
    TClonesArray*   GetParticles() const { return fParticles; }
    TClonesArray*   GetVertices()  const { return fVertices;  }
    
protected:

    /* methods */
    
    void           AddParameters();
    Bool_t         AddPrimaries(); 
    
    SpdMCParticle* AddParticle();
    SpdVertexMC*   AddVertex();
    
    void           SetVertexRegionSize(SpdVertexMC* v);
    void           SetVertexProcess(SpdVertexMC* v);
    void           SetVertexProcessAndPosition(SpdVertexMC* v);
    void           CheckParticles();  
   
    void           FillParticle(SpdMCTrack* ip, SpdMCParticle* op);
    
    /* data memebers */ 
    UInt_t               fRunId;       //! run id
    Int_t                fEventId;     //! event id
     
    // Input data
    SpdMCEventHeader*    fEventHeader; //! Input event header
    TClonesArray*        fMCTracks;    //! Input array of mc-tracks
     
    // Output data
    SpdMCEvent*          fEvent;       //! Main output data object
    TClonesArray*        fParticles;   //! List of output mc-particles 
    TClonesArray*        fVertices;    //! List of output mc-vertices 
    
    //---------------
    Bool_t     fSaveEvent;             //! specifies whether to save base event object into an output file
    Bool_t     fSaveParticles;         //! specifies whether to save particles into an output file
    Bool_t     fSaveVertices;          //! specifies whether to save vertices into an output file

    Int_t      fParticleMaxGeneration; //! if < 1: make all generations (default = 0) 
    Int_t      fVertexMaxGeneration;   //! min = 0 (only primary vertex), max = fParticleMaxGeneration-1 (default = 1)
    
    //---------------
    Int_t      fVerboseLevel;          //! verbose level 
    
    //---------------
    std::vector<Int_t> fIds;           //! old mc-track id -> new particle id 
    
    ClassDef(SpdMCEventMaker,1)
};

//------------------------------------------------------------------------------
void SpdMCEventMaker::SetParticleMaxGeneration(Int_t maxg) 
{ 
    fParticleMaxGeneration = maxg; 
    SetVertexMaxGeneration(fVertexMaxGeneration);
}

//------------------------------------------------------------------------------
void SpdMCEventMaker::SetVertexMaxGeneration(Int_t maxv) 
{ 
  if (maxv < 0) maxv = 0;
    
  if (fParticleMaxGeneration < 1) {
      fVertexMaxGeneration = maxv;
      return;
  }
  
  if (maxv < fParticleMaxGeneration) fVertexMaxGeneration = maxv;
  else fVertexMaxGeneration = fParticleMaxGeneration-1;
}

#endif  /* __SPDMCEVENTMAKER_H__ */

