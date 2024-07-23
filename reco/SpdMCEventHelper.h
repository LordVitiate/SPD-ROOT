// $Id$
// Author: artur   2020/10/06

#ifndef __SPDMCEVENTHELPER_H__
#define __SPDMCEVENTHELPER_H__

#include <TObject.h>
#include <TClonesArray.h>
#include <vector>
#include "SpdSetParSet.h"
#include "SpdMCTrackHit.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCEventHelper                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCEvent;
class SpdMCParticle;
class SpdVertexMC;

class SpdMCEventHelper: public TObject {

public:

    SpdMCEventHelper();
    virtual ~SpdMCEventHelper();

    static SpdMCEventHelper* Instance() { return (fInstance) ? fInstance : new SpdMCEventHelper(); }
    
    //------------------------------------------------------------------------------------------------------
    // ---------------------------------------- GETTERS ----------------------------------------------------
    //------------------------------------------------------------------------------------------------------
    
    Int_t  SearchForParticleId(Int_t mc_particle_id) const;  // safe! 
    Int_t  SearchForVertexId(Int_t mc_vertex_id)   const;    // safe!
    
    SpdMCParticle* SearchForParticle(Int_t mc_particle_id) const;  // safe! 
    SpdVertexMC*   SearchForVertex(Int_t mc_vertex_id) const;      // safe! 

    Int_t  SearchForMCVertex(Int_t rs_vertex_id) const;
    Int_t  SearchForMCVertexStrictly(Int_t rs_vertex_id) const;
    
    Int_t  GetNMCParticleDaughters(Int_t mc_particle_id) const; 
    
    //------------------------------------------------------------------------------------------------------
    // Fill SpdTrackMC hit array (hits) as a set of pointers to corresponding objects.
    // Returns "false" if:
    // - no mc-tracks object were specified or track_id is out of bounds (ouput array size = 0); 
    // - there is not full set of the hits in the output since some kind of hit-data-array were not specified;
    //-----------------------------------------------------------------------------------------------------
    Bool_t GetTrackMChits(Int_t track_id, std::vector<SpdMCTrackHit*>& hits) const;
    
    // ----------------------------------------------------------------------------------------------------
    // ---------------------------------------- SETTERS ---------------------------------------------------
    // ----------------------------------------------------------------------------------------------------
    
    void   SetParameters(SpdSetParSet* pars)       { fParameters = pars; }
     
    void   SetEvent(SpdMCEvent* event)             { fEvent = event;     }
    void   SetParticles(TClonesArray* parts)       { fParticles = parts; }
    void   SetVertices(TClonesArray* verts)        { fVertices  = verts; }
    
    void   SetItsHits(TClonesArray* hits)          { fItsHits = hits;    }
    void   SetTsHits(TClonesArray* hits)           { fTsHits  = hits;    }
    void   SetTofHits(TClonesArray* hits)          { fTofHits  = hits;   }
    void   SetEcalHits(TClonesArray* hits)         { fEcalHits = hits;   }
    void   SetRsHits(TClonesArray* hits)           { fRsHits  = hits;    }
    void   SetBbcHits(TClonesArray* hits)          { fBbcHits  = hits;   }
    void   SetAegHits(TClonesArray* hits)          { fAegHits  = hits;   }
    void   SetZdcHits(TClonesArray* hits)          { fZdcHits  = hits;   }
        
    void   SetTracks(TClonesArray* tracks)         { fTracks  = tracks;  }
    void   SetVerticesRC(TClonesArray* vv)         { fVerticesRC = vv;   }
    
    void   SetTracksRC(TClonesArray* tracks)       { fTracksRC  = tracks;      }
    void   SetTrackSeedsRC(TClonesArray* tracks)   { fTrackSeedsRC  = tracks;  }
        
    void   SetEcalClustersRC(TClonesArray* cl)     { fEcalClustersRC  = cl;    }
    void   SetEcalParticlesRC(TClonesArray* pp)    { fEcalParticlesRC = pp;    }
    void   SetEcalClustersMCInfo(TClonesArray* ci) { fEcalClustersMCInfo = ci; } 
    void   SetEcalParticlesMC(TClonesArray* pp)    { fEcalParticlesMC = pp;    }
     
    void   SetRsClustersMC(TClonesArray* cl)       { fRsClustersMC  = cl; }
    void   SetRsParticlesMC(TClonesArray* pp)      { fRsParticlesMC = pp; }
    
    void   SetTsParticles(TClonesArray* pp)        { fTsParticles = pp;  }
    void   SetTofParticles(TClonesArray* pp)       { fTofParticles = pp; }
    void   SetAegParticles(TClonesArray* pp)       { fAegParticles = pp; }

    // --------- GETTERS ---------
    
    const SpdSetParSet* GetParameters()          const { return fParameters; }
        
    const    SpdMCEvent* GetEvent()              const { return fEvent;      }
    const  TClonesArray* GetParticles()          const { return fParticles;  }
    const  TClonesArray* GetVertices()           const { return fVertices;   }
    
    const  TClonesArray* GetItsHits()            const { return fItsHits;    }
    const  TClonesArray* GetTsHits()             const { return fTsHits;     }
    const  TClonesArray* GetTofHits()            const { return fTofHits;    }
    const  TClonesArray* GetEcalHits()           const { return fEcalHits;   }
    const  TClonesArray* GetRsHits()             const { return fRsHits;     }
    const  TClonesArray* GetBbcHits()            const { return fBbcHits;    }
    const  TClonesArray* GetAegHits()            const { return fAegHits;    }
    const  TClonesArray* GetZdcHits()            const { return fZdcHits;    }
    
    const  TClonesArray* GetTracks()             const { return fTracks;     }
    const  TClonesArray* GetVerticesRC()         const { return fVerticesRC; }
    
    const  TClonesArray* GetTracksRC()           const { return fTracksRC;     }
    const  TClonesArray* GetTracksSeedsRC()      const { return fTrackSeedsRC; }
    
    const  TClonesArray* GetEcalClustersRC()     const { return fEcalClustersRC;     }
    const  TClonesArray* GetEcalParticlesRC()    const { return fEcalParticlesRC;    }
    const  TClonesArray* GetEcalClustersMCInfo() const { return fEcalClustersMCInfo; }
    const  TClonesArray* GetEcalParticlesMC()    const { return fEcalParticlesMC;    }
    
    const  TClonesArray* GetRsClustersMC()       const { return fRsClustersMC;  }
    const  TClonesArray* GetRsParticlesMC()      const { return fRsParticlesMC; }
    
    const  TClonesArray* GetTsParticles()        const { return fTsParticles;  }
    const  TClonesArray* GetTofParticles()       const { return fTofParticles; }
    const  TClonesArray* GetAegParticles()       const { return fAegParticles; }

    //===============================================================================================
    // Print methods
    
    //-----------------------------------------------------------------------------------------------
    // PrintParticles()
    //-----------------------------------------------------------------------------------------------
    // MC:[GEN]  :   MC-Generator id
    // MC:[TID]  :   MC-Track id
    // C         :   direct particle response in (Its+Ts) system (yes: +, no: -) 
    // G         :   particle generation (0,1,...)
    // ID        :   particle id 
    // NAME      :   particle name
    // PDG       :   particle pdg number
    // MOTHER    :   real mother particle id 
    // PROC      :   process id
    // DET       :   detector (supermodule) id                                                     
    // M[G1]     :   mother particle of generation = 1 id 
    // PDG[G1]   :   mother particle of generation = 1 pdg 
    // M[S]      :   special ("physical") mother particle id 
    // ID[S]     :   special ("physical") process id 
    // P         :   particle momentum, GeV 
    // Th        :   particle momentum polar angle (theta), deg
    // D0        :   distance from particle production vertex to (0,0,0) point, cm
    // t0        :   particle production time, s
    // FV        :   origin vertex id = -99 (unknown), -1 (primary incoming particle), 0,1, ...
    // LV        :   last vertex id = -99 (unknown), 0,1, ...
    // TRID      :   associated track id = -1 (no track), 0,1, ...
    // TID       :   associated TS particle id = -1 (no data), 0,1, ...
    // FID       :   associated TOF particle id = -1 (no data), 0,1, ...
    // EID       :   associated ECAL particle id = -1 (no particle), 0,1, ...
    // RID       :   associated RS particle id = -1 (no particle), 0,1, ...
    //-----------------------------------------------------------------------------------------------
    
    void PrintParticles(Int_t maxg = -1) const;  // print MC-vertices
    
    void PrintVertices(Int_t maxg = -1, TString option = "") const; // print MC-vertices, option = "nofitpars"
    void PrintTracks(TString option = "") const;                    // print MC-tracks, option = "nofitpars"
    
    void PrintEcalClustersMCInfo() const;
    void PrintEcalParticlesMC() const;
    
    void PrintRsClustersMC() const;
    void PrintRsParticlesMC() const;
   
    void PrintTracksRC() const;
    void PrintVerticesRC() const;
    
    //---------------------------------------------------------------------
    // nparts, nvtx, ntrack, nhits, ... : 
    //   1) < 0 = print all; 
    //   2) = 0: no print;
    //   3) > 0: print the number of objects
 
    void PrintParticles2(Int_t np = 2, Option_t* opt = "")  const;
    void PrintVertices2(Int_t nvtx = 2, Option_t* opt = "") const;
    
    void PrintItsHits2(Int_t nhits = 2, Option_t* opt = "")  const;
    void PrintTsHits2(Int_t nhits = 2, Option_t* opt = "")   const;
    void PrintTofHits2(Int_t nhits = 2, Option_t* opt = "")  const;
    void PrintEcalHits2(Int_t nhits = 2, Option_t* opt = "") const;
    void PrintRsHits2(Int_t nhits = 2, Option_t* opt = "")   const;
    void PrintBbcHits2(Int_t nhits = 2, Option_t* opt = "")   const;
    void PrintAegHits2(Int_t nhits = 2, Option_t* opt = "")   const;
    void PrintZdcHits2(Int_t nhits = 2, Option_t* opt = "")   const;
    
    void PrintTracks2(Int_t ntracks = 2, Option_t* opt = "")  const;
    void PrintVerticesRC2(Int_t nvtx = 2, Option_t* opt = "") const;
    void PrintTracksRC2(Int_t ntrk = 2, Option_t* opt = "")   const;
    
    void PrintEcalClustersRC2(Int_t ncl = 2, Option_t* opt = "")    const;
    void PrintEcalParticlesRC2(Int_t np = 2, Option_t* opt = "")    const;
    void PrintEcalClustersMCInfo2(Int_t ni = 2, Option_t* opt = "") const;
    void PrintEcalParticlesMC2(Int_t ni = 2, Option_t* opt = "")    const;
    
    void PrintRsClustersMC2(Int_t ncl = 2, Option_t* opt = "") const;
    void PrintRsParticlesMC2(Int_t ni = 2, Option_t* opt = "") const;
    
    void PrintTsParticles2(Int_t ni = 2, Option_t* opt = "")  const;
    void PrintTofParticles2(Int_t ni = 2, Option_t* opt = "") const;
    void PrintAegParticles2(Int_t ni = 2, Option_t* opt = "") const;

    void PrintTrackFitPars(Int_t ntracks = 2, Option_t* opt = "") const;
    void PrintVertexFitPars(Int_t nvtx = 2, Option_t* opt = "")   const;
    void PrintVertexRCFitPars(Int_t nvtx = 2, Option_t* opt = "") const;
    //--------------------------------------------------------------
    
    static void PrintMCProcesses();
    
    //--------------------------------------------------------------
    // Additional: MC id table
    
    void SetIdTable(const std::vector<Int_t>* ids) { fIdTable = ids; } 
    const std::vector<Int_t>* GetIdTable() const { return fIdTable; }
    
    Int_t GetParticleId(Int_t oldId) const; //safe
    
private:
    
    static SpdMCEventHelper* fInstance;
    
    const SpdSetParSet*  fParameters;         //! Assembly of MC parameters
    
    // base MC
    const SpdMCEvent*    fEvent;              //! Base MC event object
    
    const TClonesArray*  fParticles;          //! List of mc-particles 
    const TClonesArray*  fVertices;           //! List of mc-vertices 
    
    // hits
    const TClonesArray*  fItsHits;            //! List of ITS mc-hits (silicon)
    const TClonesArray*  fTsHits;             //! List of TS mc-hits (straw)  
    const TClonesArray*  fTofHits;            //! List of TOF mc-hits  
    const TClonesArray*  fEcalHits;           //! List of ECAL mc-hits  
    const TClonesArray*  fRsHits;             //! List of RS mc-hits  
    const TClonesArray*  fBbcHits;            //! List of BBC mc-hits
    const TClonesArray*  fAegHits;            //! List of AEG mc-hits
    const TClonesArray*  fZdcHits;            //! List of ZDC mc-hits  
    
    // objects
    const TClonesArray*  fTracks;             //! List of mc-tracks
    const TClonesArray*  fVerticesRC;         //! List of rc-vertices

    const TClonesArray*  fTracksRC;           //! List of rc-tracks
    const TClonesArray*  fTrackSeedsRC;       //! List of seeds for rc-tracks

    const TClonesArray*  fEcalClustersRC;     //! List ot ECAL rc-clusters
    const TClonesArray*  fEcalParticlesRC;    //! List ot ECAL rc-particles 
    const TClonesArray*  fEcalClustersMCInfo; //! List of ECAL rc-cluster infos
    const TClonesArray*  fEcalParticlesMC;    //! List of ECAL mc-particles
    
    const TClonesArray*  fRsClustersMC;       //! List of RS mc-clusters
    const TClonesArray*  fRsParticlesMC;      //! List of RS mc-particles
    
    const TClonesArray*  fTsParticles;        //! List of TS particles
    const TClonesArray*  fTofParticles;       //! List of TOF particles
    const TClonesArray*  fAegParticles;       //! List of Aeg particles

    //---------------------------------------------------------------------------------
    // MC id table (taken from MC event maker)
    // old mc-track id -> new particle id
    // if fIdTable[i] = -1 -> no corresponding particle in the new array of particles! 
    //---------------------------------------------------------------------------------
    const std::vector<Int_t>* fIdTable;  //! 
    
    /* Methods */
    
    template <class A, class T> 
             void PrintDataArray(A*, T*, Int_t n, Option_t* opt) const;   
    template <class A, class T, class U> 
             void PrintDataParArray(A* Array, T* obj, U* par, Int_t n, Option_t* opt) const;

    TString ParticleName(Int_t pdg) const;

    ClassDef(SpdMCEventHelper,1)
};



#endif  /* __SPDMCEVENTHELPER_H__ */

