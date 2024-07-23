// $Id$
// Author: artur   2021/06/03

#ifndef __SPDRSMCCLUSTER_H__
#define __SPDRSMCCLUSTER_H__

#include <TObject.h>
#include <vector>
#include "SpdDetectorList.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsMCCluster                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsMCCluster: public TObject {

public:

    SpdRsMCCluster();
    virtual ~SpdRsMCCluster();
    
    void   Reset();
    
    // ----- Setters -----
    
    void    SetId(Int_t id)    { fId = id;    }
    void    SetModId(Int_t id) { fModId = id; }
    void    SetDetId(Int_t id) { fDetId = id; }
    
    void    AddHit(Int_t id);
    
            /* cluster mc-info */

    void    SetNParticles(Int_t n);
    
    void    SetParticle(Int_t loc_id, Int_t loc_mid, 
                        Int_t id, Int_t pdg, 
                        Int_t first_state, Int_t last_state);
    
    void    SetRsParticleId(Int_t loc_id, Int_t id);
     
    // ----- Getters -----
    
    Bool_t  IsBarrelCluster() const { return (fModId == kSpdRsTB);  } 
    Bool_t  IsEndcapCluster() const { return (fModId == kSpdRsTEC); }           
    
    Int_t   GetId()           const { return fId;    }
    Int_t   GetModId()        const { return fModId; }
    Int_t   GetDetId()        const { return fDetId;  }
  
    Int_t   GetSize()         const { return fHits.size(); }
    Int_t   GetNHits()        const { return fHits.size(); }
 
    const std::vector<Int_t>& GetHits() const { return fHits; }
    
             /* cluster mc-info */
    
    Int_t   GetNParticles()  const { return fData.size(); }
    
    Int_t   GetParticleId(Int_t loc_id = 0)   const { return fData[loc_id].id;   }
    Int_t   GetParticlePdg(Int_t loc_id = 0)  const { return fData[loc_id].pdg;  }
    
    Int_t   GetParticleLocalMotherId(Int_t loc_id = 0) const { return fData[loc_id].loc_mid;     }
    Int_t   GetParticleFirstState(Int_t loc_id = 0)    const { return fData[loc_id].first_state; }
    Int_t   GetParticleLastState(Int_t loc_id = 0)     const { return fData[loc_id].last_state;  }
    
    Int_t   GetRsParticleId(Int_t loc_id = 0) const { return fData[loc_id].rspart_id; }

    // Print
    virtual void Print(Option_t* option = "") const;
    
    struct clparticle 
    {
        clparticle():loc_mid(-1),id(-1),pdg(0),first_state(-1),last_state(-1),rspart_id(-1){}
      
        Int_t    loc_mid;        // local (fData) mother particle id
        Int_t    id;             // mc-particle unique ID
        Int_t    pdg;            // mc-particle pdg code
        
        /* particle states: 0 = primary vertex, 
         *                  1 = small region around the vertex (i.e. "vertex region"),
         *                  2 = region between "vertex region" and Range System
         *                  3 = Range System region and beyond
         */
        Int_t    first_state;    // first particle state region
        Int_t    last_state;     // last particle state region
        
        Int_t    rspart_id;    // ecal-particle unique id 
    };
    
private:

    Int_t   fId;       // unique cluster id
    Int_t   fModId;    // supermodule (detector) ID 
    Int_t   fDetId;    // detector ID (barrel = 0, endcapZ+ = 1, endcapZ- = -1)
    
    std::vector<Int_t>      fHits;     // list of hits
    std::vector<clparticle> fData;     // list of mc-data

    ClassDef(SpdRsMCCluster,1)
};

#endif  /* __SPDRSMCCLUSTER_H__ */

