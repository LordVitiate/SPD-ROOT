// $Id$
// Author: artur   2021/05/31

#ifndef __SPDECALCLUSTERMCINFO_H__
#define __SPDECALCLUSTERMCINFO_H__

#include <TObject.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalClusterMCInfo                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalClusterMCInfo: public TObject {

public:

    SpdEcalClusterMCInfo();
    virtual ~SpdEcalClusterMCInfo();
    
    void   Reset();
    
    // Setters
    void   SetId(Int_t id)        { fId = id; }
    void   SetClusterId(Int_t id) { fClusterId = id; }
    
    void   SetNParticles(Int_t n0, Int_t n1,  Int_t n2);
    
    void   SetParticle(Int_t loc_id, Int_t loc_mid, 
                       Int_t id, Int_t pdg, 
                       Int_t first_state, Int_t last_state,
                       Double_t edep);
    
    void   SetEcalParticleId(Int_t loc_id, Int_t id);
    
    void   SetNoiseEdep(Double_t ed) { fNoiseEdep = ed; }
    
    // Getters
    Int_t    GetId()        const { return fId; }  
    Int_t    GetClusterId() const { return fClusterId; }
    
    Int_t    GetNParticles()            const { return fData.size(); }
    Int_t    GetNParticles(Int_t state) const { return (state < 0 || state > 2) ? 0 : fNParticles[state]; }
    
    Int_t    GetParticleId(Int_t loc_id = 0)   const { return fData[loc_id].id;   }
    Int_t    GetParticlePdg(Int_t loc_id = 0)  const { return fData[loc_id].pdg;  }
    Double_t GetParticleEdep(Int_t loc_id = 0) const { return fData[loc_id].edep; }
    
    Int_t    GetParticleLocalMotherId(Int_t loc_id = 0) const { return fData[loc_id].loc_mid;     }
    Int_t    GetParticleFirstState(Int_t loc_id = 0)    const { return fData[loc_id].first_state; }
    Int_t    GetParticleLastState(Int_t loc_id = 0)     const { return fData[loc_id].last_state;  }
    
    Int_t    GetEcalParticleId(Int_t loc_id = 0) const { return fData[loc_id].ecalpart_id; }
    
    Double_t GetNoiseEdep() const { return fNoiseEdep; }
    Double_t GetClusterEdep() const; // GeV
    Double_t GetClusterTotalEdep() const; // GeV (cluster edep + noise)
    
    // Print
    void   Print(const Option_t* opt = 0) const;
    
    struct clparticle 
    {
        clparticle():loc_mid(-1),id(-1),pdg(0),edep(0),first_state(-1),last_state(-1),ecalpart_id(-1){}
      
        Int_t    loc_mid;        // local (fData) mother particle id
        Int_t    id;             // mc-particle unique ID
        Int_t    pdg;            // mc-particle pdg code
        Double_t edep;           // total particle detector response in the cluster
        
        /* particle states: 0 = primary vertex, 
         *                  1 = small region around the vertex (i.e. "vertex region"),
         *                  2 = region between "vertex region" and calorimeter
         *                  3 = calorimeter region and beyond
         */
        Int_t    first_state;    // first particle state region
        Int_t    last_state;     // last particle state region
        
        Int_t    ecalpart_id;    // ecal-particle unique id 
    };
    
private:
    
    Int_t    fId;         // hit own unique ID (index in the corresponding array) 
    Int_t    fClusterId;  // cluster unique ID (index in the corresponding array)
    
    Double_t fNoiseEdep; 

    Int_t    fNParticles[3];
    std::vector<clparticle> fData;
    
    ClassDef(SpdEcalClusterMCInfo,1)
};

#endif  /* __SPDECALCLUSTERMCINFO_H__ */

