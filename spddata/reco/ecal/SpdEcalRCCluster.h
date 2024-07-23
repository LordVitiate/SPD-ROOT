// $Id$
// Author: artur   2021/03/15

#ifndef __SPDECALRCCLUSTER_H__
#define __SPDECALRCCLUSTER_H__

#include <TObject.h>
#include <vector>
#include "SpdDetectorList.h"

/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
// SpdEcalRCCluster                                                            //
//                                                                             //
// ECAL Cluster is a group of neighboring cells with energies above threshold. //
// The class contains information on:                                          //
// - indices of cells forming the cluster                                      //
// - indices of particles, which are reconstructed from this cluster           //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

class SpdEcalRCCluster: public TObject {

public:

    SpdEcalRCCluster();
    virtual ~SpdEcalRCCluster();
   
    // ----- Setters -----
    
    void  SetId(Int_t id)       { fId = id;     }
    void  SetModId(Int_t id)    { fModId = id;  }
    void  SetDetId(Int_t id)    { fDetId = id;  }
    void  SetEnergy(Double_t e) { fEnergy = e;  }
    void  SetInfoId(Int_t id)   { fInfoId = id; }
    
    void  AddEnergy(Double_t e) { fEnergy += e; }
    
    void  AddCell(Int_t id)     { fCells.push_back(id);     }
    void  AddParticle(Int_t id) { fParticles.push_back(id); }
    
    // ----- Getters -----
    
    Bool_t   IsBarrelCluster() const { return (fModId == kSpdEcalTB);  } 
    Bool_t   IsEndcapCluster() const { return (fModId == kSpdEcalTEC); }           
    
    Int_t    GetId()         const { return fId;     }
    Int_t    GetModId()      const { return fModId;  }
    Int_t    GetDetId()      const { return fDetId;  }
    Double_t GetEnergy()     const { return fEnergy; } 
    Int_t    GetInfoId()     const { return fInfoId; }
    
    Int_t    GetSize()       const { return fCells.size();     }
    Int_t    GetNCells()     const { return fCells.size();     }
    Int_t    GetNParticles() const { return fParticles.size(); }
    
    const std::vector<Int_t>& GetCells()     const { return fCells;     }
    const std::vector<Int_t>& GetParticles() const { return fParticles; }
    
    // Print
    virtual void Print(Option_t* option = "") const;
    
private:

    Int_t               fId;         // unique cluster id
    Int_t               fModId;      // supermodule (detector) ID 
    Int_t               fDetId;      // detector ID (basket,endcapZ+,endcapZ-)
    
    Double_t            fEnergy;     // total energy deposit (reconstructed)
    
    std::vector<Int_t>  fCells;      // list of cells (hits)
    std::vector<Int_t>  fParticles;  // list of particles (reconstructed)
    
    Int_t               fInfoId;     // cluster info id
    
    ClassDef(SpdEcalRCCluster,1)
};

#endif  /* __SPDECALRCCLUSTER_H__ */

