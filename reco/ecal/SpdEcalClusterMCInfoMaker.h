// $Id$
// Author: artur   2021/05/31

#ifndef __SPDECALCLUSTERMCINFOMAKER_H__
#define __SPDECALCLUSTERMCINFOMAKER_H__

#include "SpdTask.h"
#include <TClonesArray.h>
#include <TVector3.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalClusterMCInfoMaker                                                  //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCEvent;
class SpdEcalMCHit;
class SpdEcalRCCluster;
class SpdEcalClusterMCInfo;
class SpdEcalMCParticle;

class SpdEcalClusterMCInfoMaker: public SpdTask {

public:

    SpdEcalClusterMCInfoMaker();
    virtual ~SpdEcalClusterMCInfoMaker();
 
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    // Save the info and mc-ecal-particles into the output file 
    void  SaveInfo(Bool_t save = true)              { fSaveInfo = save;          }
    void  SaveEcalParticles(Bool_t save = true)     { fSaveEcalParticles = save; }
    
    void  CreateEcalParticles(Bool_t create = true) { fCreateEcalParticles = create; }
    void  RemoveHitMCTruth(Bool_t remove = true)    { fRemoveHitMCTruth = remove;    }
    
    void  SetPrimVertexRegionR(Double_t r) { fPrimVertexRegionR = r; } // default = 1 cm
    
    void  SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }
    
protected:
    
    /* Data memebers */ 

    virtual void  MakeClusterInfo(SpdEcalRCCluster* cl);    
    virtual void  MakeEcalParticles();

    virtual Bool_t AcceptCluster(const SpdEcalRCCluster* cl) const;

    virtual SpdEcalClusterMCInfo* AddClusterInfo(Int_t clusteId);
    virtual SpdEcalMCParticle*    AddEcalParticle();

    virtual void RemoveMCTruth();
    
    // input parameters 
 
    // input data 
    SpdMCEvent*     fEvent;               //! Main mc-data object
    TClonesArray*   fParticles;           //! List of mc-particles 
    TClonesArray*   fHits;                //! List of ECAL mc-hits
    TClonesArray*   fClusters;            //! List of ECAL rc-clusters 
  
    TVector3        fPrimVertexPosition;  //! Primary vertex position
    
    // output data
    TClonesArray*   fInfo;                //! List of cluster infos
    TClonesArray*   fEcalParticles;       //! List of mc ecal-particles 
    
    // settings 
    Bool_t          fSaveInfo;            //! specifies whether to save hits into an output file
    Bool_t          fSaveEcalParticles;   //! specifies whether to save ecal-particles into an output file
    Bool_t          fCreateEcalParticles; //! specifies whether to create ecal-particles
    Bool_t          fRemoveHitMCTruth;    //! specifies whether to remove mc-thruth in ecal-hits
    Double_t        fPrimVertexRegionR;   //! radial size of vertex region (of type = 1), cm 
    
    // counters
    Int_t           fNTotalClusters;      //! total number of accepted clusters
    Int_t           fNTotalInfos;         //! total number of well-processed clusters
    Int_t           fNTotalParticles;     //! total number of ecal-particles
   
    //-------------------------------
    Int_t           fVerboseLevel;    //! verbose level 
      
    struct part_info 
    {
      part_info():last_state(-1),first_state(-1),id(-1),edep(0),mother(0) {} 

      Int_t        last_state;   // last state region id
      Int_t        first_state;  // first state region id
      Int_t        id;           // first state particle local number 
      Double_t     edep;         // total energy deposit 
      part_info*   mother;       // mother particle
    };
   
    ClassDef(SpdEcalClusterMCInfoMaker,1)
};

#endif  /* __SPDECALCLUSTERMCINFOMAKER_H__ */

