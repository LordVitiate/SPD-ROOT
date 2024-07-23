// $Id$
// Author: artur   2021/07/25

#ifndef __SPDRSMCPARTICLE_H__
#define __SPDRSMCPARTICLE_H__

#include <TObject.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsMCParticle                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
// Particle state (region):                                                   //
//                                                                            //
//      0 = primary vertex,                                                   //
//      1 = small region around the vertex (i.e. "vertex region"),            //
//      2 = region between "vertex region" and Range System,                  //
//      3 = Range System region and beyond.                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

typedef std::pair<Int_t,Int_t> RsMCPartInfo; // < rs cluster id, rs cluster clparticle loc_id>

class SpdRsMCParticle: public TObject {

public:

    SpdRsMCParticle();
    virtual ~SpdRsMCParticle();
    
    void   Reset();
    void   RemoveInfo() { fInfo.clear(); }

    // ----- Setters -----
    void   SetId(Int_t id)               { fId = id;           }
    void   SetMotherId(Int_t id)         { fMotherId = id;     }
    void   SetParticleId(Int_t id)       { fParticleId = id;   } 
    void   SetParticlePdg(Int_t pdg)     { fParticlePdg = pdg; } 
    
    void   SetStates(Int_t s1, Int_t s2) { fFirstState = s1; fLastState = s2; }
    
    void   SetNClusters(Int_t n);
    void   SetClusterInfo(Int_t loc_id, Int_t cluster_id, Int_t cluster_part_id);
    
    // ----- Getters -----
    Int_t  GetId()          const { return fId;          }
    Int_t  GetMotherId()    const { return fMotherId;    }
    Int_t  GetParticleId()  const { return fParticleId;  }
    Int_t  GetParticlePdg() const { return fParticlePdg; }
    
    Int_t  GetFirstState()  const { return fFirstState;  }
    Int_t  GetLastState()   const { return fLastState;   }
    
    Int_t  GetNClusters()   const { return fInfo.size(); }
    
    Int_t  GetClusterId(Int_t loc_id = 0)           const { return fInfo[loc_id].first;  }
    Int_t  GetClusterParicleLocId(Int_t loc_id = 0) const { return fInfo[loc_id].second; }
    
    // Print
    virtual void Print(Option_t* option = "") const;    

private:
    
    Int_t     fId;                // unique id
    Int_t     fMotherId;          // mother rs-particle id 
    Int_t     fParticleId;        // mc-particle id
    Int_t     fParticlePdg;       // mc-particle pdg-number
     
    Int_t     fFirstState;        // first particle state region 
    Int_t     fLastState;         // last particle state region
    
    std::vector<RsMCPartInfo> fInfo;  // list of cluster particles 
    
    ClassDef(SpdRsMCParticle,1)
};

#endif  /* __SPDRSMCPARTICLE_H__ */

