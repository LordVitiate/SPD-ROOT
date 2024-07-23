// $Id$
// Author: artur   2021/03/16

#ifndef __SPDMCDATAITERATOR_H__
#define __SPDMCDATAITERATOR_H__

#include <TObject.h>
#include <TString.h>

#include "SpdTask.h"
#include "SpdDetectorList.h"
#include "SpdSetParSet.h"
#include "SpdMCEvent.h"
 
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCDataIterator                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRunAna;
class FairFileSource;
class SpdRootFileSink;
class SpdMCDataIterator;

class SpdMCDataIterTask: public FairTask {

public:
    
    SpdMCDataIterTask();
    virtual ~SpdMCDataIterTask();
    
    virtual void SetIter(SpdMCDataIterator* iter) { fIter = iter; }
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish() {}
    
protected:
    
    SpdMCDataIterator* fIter;
    
    ClassDef(SpdMCDataIterTask,1)
};

class SpdMCDataIterator: public TObject {

public:

    SpdMCDataIterator();
    virtual ~SpdMCDataIterator();

    // initialization
    
    virtual Bool_t Init();
    virtual Bool_t Init(Int_t nentry); // init with enent of id = nentry (nentry < 0 <=> Init())

    //--------------------------
    // setters 
   
           // input/output 
    void   SetCurrentDataDir(TString dir);
    
           // input
    void   AddSourceFile(TString fname);
    void   ActivateBranch(const Char_t* data);
    void   InactivateBranch(const Char_t* data);
    
           // output
    void   SetOutputFile(TString fname);
    void   AddBranchToOutput(TString branch_name);
   
    //--------------------------
    // data (getters)
    
    Int_t GetRunId()           const  { return fRunId;          }
    Int_t GetCurrentEntry()    const  { return fCurrentEntry;   }
    Int_t GetLastSavedEntry()  const  { return fLastSavedEntry; }
    
    Int_t GetNEvents()         const  { return fNEvents; }
     
    const SpdSetParSet*   GetParameters() const;
    
    const SpdMCEvent*     GetEvent()      const { return fEvent;     }
    const TClonesArray*   GetParticles()  const { return fParticles; }
    const TClonesArray*   GetVertices()   const { return fVertices;  }
    
    const TClonesArray*   GetItsHits()    const { return fItsHits;  } 
    const TClonesArray*   GetTsHits()     const { return fTsHits;   } 
    const TClonesArray*   GetTofHits()    const { return fTofHits;  } 
    const TClonesArray*   GetEcalHits()   const { return fEcalHits; } 
    const TClonesArray*   GetRsHits()     const { return fRsHits;   } 
    const TClonesArray*   GetBbcHits()    const { return fBbcHits;  }
    const TClonesArray*   GetAegHits()    const { return fAegHits;  }
    const TClonesArray*   GetZdcHits()    const { return fZdcHits;  } 

    const TClonesArray*   GetTracks()     const { return fTracks;     }     
    const TClonesArray*   GetVerticesRC() const { return fVerticesRC; }
    
    const TClonesArray*   GetTracksRC()      const { return fTracksRC;     }     
    const TClonesArray*   GetTrackSeedsRC()  const { return fTrackSeedsRC; }     
        
    const TClonesArray*   GetEcalClustersRC()     const { return fEcalClustersRC;     }     
    const TClonesArray*   GetEcalParticlesRC()    const { return fEcalParticlesRC;    }     
    const TClonesArray*   GetEcalClustersMCInfo() const { return fEcalClustersMCInfo; }
    const TClonesArray*   GetEcalParticlesMC()    const { return fEcalParticlesMC;    }

    const TClonesArray*   GetRsClustersMC()  const { return fRsClustersMC;  }
    const TClonesArray*   GetRsParticlesMC() const { return fRsParticlesMC; }

    const TClonesArray*   GetTsParticles()   const { return fTsParticles;  }
    const TClonesArray*   GetTofParticles()  const { return fTofParticles; }
    const TClonesArray*   GetAegParticles()  const { return fAegParticles; }

    //--------------------------
    // iterator methods
    
    virtual void   Reset();
    virtual Bool_t NextEvent();
    virtual void   SkipEventToSave() { fSaveEvent = false; }

    virtual void   Finish();
    
protected:
 
    virtual void   GenerateRunId();
    virtual void   ActivateDataBranches();
    virtual void   SaveLastEvent();
  
    SpdRunAna*         fRun;
    SpdMCDataIterTask* fTask;
    FairFileSource*    fSource;
    SpdRootFileSink*   fSink;
    
    Bool_t             fInit;
    Int_t              fRunId;
    Int_t              fNFiles;
    Int_t              fNEvents;
    TString            fCurrentDataDir;
    Int_t              fCurrentEntry;
    Int_t              fLastSavedEntry;
    Bool_t             fSaveEvent;
    
    // input data
    Bool_t             fActivateEvent;
    Bool_t             fActivateParticles;
    Bool_t             fActivateVertices;
    
    Bool_t             fActivateItsHits;
    Bool_t             fActivateTsHits;
    Bool_t             fActivateTofHits;
    Bool_t             fActivateEcalHits;
    Bool_t             fActivateRsHits;
    Bool_t             fActivateBbcHits;
    Bool_t             fActivateAegHits;
    Bool_t             fActivateZdcHits;
    
    Bool_t             fActivateTracks;
    Bool_t             fActivateVerticesRC;
    
    Bool_t             fActivateTracksRC;
    Bool_t             fActivateTrackSeedsRC;
    
    Bool_t             fActivateEcalClustersRC;
    Bool_t             fActivateEcalParticlesRC;
    Bool_t             fActivateEcalClustersMCInfo; 
    Bool_t             fActivateEcalParticlesMC;
    
    Bool_t             fActivateRsClustersMC;
    Bool_t             fActivateRsParticlesMC;
    
    Bool_t             fActivateTsParticles;
    Bool_t             fActivateTofParticles;
    Bool_t             fActivateAegParticles;

    SpdMCEvent*        fEvent;                 //! Main mc-data object
    TClonesArray*      fParticles;             //! List of mc-particles 
    TClonesArray*      fVertices;              //! List of mc-vertices
    
    TClonesArray*      fItsHits;               //! List of ITS mc-hits
    TClonesArray*      fTsHits;                //! List of TS mc-hits
    TClonesArray*      fTofHits;               //! List of TOF mc-hits
    TClonesArray*      fEcalHits;              //! List of ECAL mc-hits
    TClonesArray*      fRsHits;                //! List of RS mc-hits
    TClonesArray*      fBbcHits;               //! List of BBC mc-hits
    TClonesArray*      fAegHits;               //! List of AEG mc-hits
    TClonesArray*      fZdcHits;               //! List of ZDC mc-hits

    TClonesArray*      fTracks;                //! List of mc-tracks 
    TClonesArray*      fVerticesRC;            //! List of rc-vertices 
    
    TClonesArray*      fTracksRC;              //! List of reco-tracks 
    TClonesArray*      fTrackSeedsRC;          //! List of seeds for reco-tracks 
        
    TClonesArray*      fEcalClustersRC;        //! List of ECAL rc-clusters 
    TClonesArray*      fEcalParticlesRC;       //! List of ECAL rc-particles 
    TClonesArray*      fEcalClustersMCInfo;    //! List of ECAL rc-clusters mc-info
    TClonesArray*      fEcalParticlesMC;       //! List of ECAL mc-particles 
    
    TClonesArray*      fRsClustersMC;          //! List of RS mc-clusters
    TClonesArray*      fRsParticlesMC;         //! List of RS mc-particles 
    
    TClonesArray*      fTsParticles;           //! List of TS particles
    TClonesArray*      fTofParticles;          //! List of TOF particles
    TClonesArray*      fAegParticles;          //! List of Aeg particles

    friend class SpdMCDataIterTask;
    
    ClassDef(SpdMCDataIterator,1)
};

//-----------------------------------------------------------------
inline void SpdMCDataIterator::SetCurrentDataDir(TString dir) 
{ 
    if (dir.IsWhitespace()) { fCurrentDataDir = ""; return; }
    if (!dir.EndsWith("/")) dir += "/";
    fCurrentDataDir = dir; 
}

#endif  /* __SPDMCDATAITERATOR_H__ */

