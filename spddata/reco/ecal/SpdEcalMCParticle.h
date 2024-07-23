// $Id$
// Author: artur   2021/07/12

#ifndef __SPDECALMCPARTICLE_H__
#define __SPDECALMCPARTICLE_H__

#include <TObject.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalMCParticle                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
// Particle state (region):                                                   //
//                                                                            //
//      0 = primary vertex,                                                   //
//      1 = small region around the vertex (i.e. "vertex region"),            //
//      2 = region between "vertex region" and calorimeter,                   //
//      3 = calorimeter region and beyond.                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

typedef std::pair<Int_t,Int_t> EcalMCPartInfo; // ecal cluster < mc-info id, mc-info clparticle loc_id>

class SpdEcalMCParticle: public TObject {

public:

    SpdEcalMCParticle();
    virtual ~SpdEcalMCParticle();
  
    void   Reset();
    void   RemoveInfo() { fInfo.clear(); }
  
    // ----- Setters -----
    void   SetId(Int_t id)               { fId = id;           }
    void   SetMotherId(Int_t id)         { fMotherId = id;     }
    void   SetParticleId(Int_t id)       { fParticleId = id;   } 
    void   SetParticlePdg(Int_t pdg)     { fParticlePdg = pdg; } 
    void   SetEdep(Double_t edep)        { fEdep = edep;       } 
    
    void   AddEdep(Double_t edep)        { fEdep += edep;      }
    
    void   SetStates(Int_t s1, Int_t s2) { fFirstState = s1; fLastState = s2; }
    
    void   SetNClusters(Int_t n);
    void   SetClusterInfo(Int_t loc_id, Int_t info_id, Int_t info_part_id);
    
    // ----- Getters -----
    Int_t  GetId()          const { return fId;          }
    Int_t  GetMotherId()    const { return fMotherId;    }
    Int_t  GetParticleId()  const { return fParticleId;  }
    Int_t  GetParticlePdg() const { return fParticlePdg; }
    
    Double_t GetEdep()      const { return fEdep;        }
    
    Int_t  GetFirstState()  const { return fFirstState;  }
    Int_t  GetLastState()   const { return fLastState;   }
    
    Int_t  GetNClusters()   const { return fInfo.size(); }
    
    Int_t  GetClusterInfoId(Int_t loc_id = 0)           const { return fInfo[loc_id].first;  }
    Int_t  GetClusterInfoParicleLocId(Int_t loc_id = 0) const { return fInfo[loc_id].second; }
    
    // Print
    virtual void Print(Option_t* option = "") const;

private:

    Int_t     fId;                // unique id
    Int_t     fMotherId;          // mother ecal-particle id 
    Int_t     fParticleId;        // mc-particle id
    Int_t     fParticlePdg;       // mc-particle pdg-number
    Double_t  fEdep;              // total particle response in the calorimeter (mc), GeV 
     
    Int_t     fFirstState;        // first particle state region 
    Int_t     fLastState;         // last particle state region 
    
    std::vector<EcalMCPartInfo> fInfo;  // list of cluster particles 

    ClassDef(SpdEcalMCParticle,1)
};

#endif  /* __SPDECALMCPARTICLE_H__ */

