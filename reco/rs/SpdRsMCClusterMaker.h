// $Id$
// Author: artur   2021/06/03

#ifndef __SPDRSMCCLUSTERMAKER_H__
#define __SPDRSMCCLUSTERMAKER_H__

#include <TObject.h>
#include <TClonesArray.h>
#include <TVector3.h>
#include "SpdTask.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsMCClusterMaker                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCEvent;
class SpdRsMCHit;
class SpdRsMCCluster;
class SpdRsMCParticle;

class SpdRsMCClusterMaker: public SpdTask {

public:

    SpdRsMCClusterMaker();
    virtual ~SpdRsMCClusterMaker();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    // Save the hits into the output file 
    void  SaveClusters(Bool_t save = true)        { fSaveClusters    = save; }
    void  SaveRsParticles(Bool_t save = true)     { fSaveRsParticles = save; }
    
    void  CreateRsParticles(Bool_t create = true) { fCreateRsParticles = create; }
    void  RemoveHitMCTruth(Bool_t remove = true)  { fRemoveHitMCTruth = remove;  }
    
    void  SetPrimVertexRegionR(Double_t r) { fPrimVertexRegionR = r; } // default = 1 cm
  
    void  SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }

private:
    
    virtual void  MakeClusters();    
    virtual void  MakeRsParticles();
    
    virtual void  MakeClusterInfo(SpdRsMCCluster* cluster, Int_t mother_pid);    
    
    virtual SpdRsMCCluster*   AddCluster();
    virtual SpdRsMCParticle*  AddRsParticle();
    
    virtual void RemoveMCTruth();
    
    /* Data memebers */ 
    
    // input data 
    SpdMCEvent*    fEvent;                //! Main mc-data object
    TClonesArray*  fParticles;            //! List of mc-particles 
    TClonesArray*  fHits;                 //! List of mc-hits 
    
    TVector3       fPrimVertexPosition;   //! Primary vertex position
    
    // output data 
    TClonesArray*  fClusters;             //! List of reconstructed clusters
    TClonesArray*  fRsParticles;          //! List of mc rs-particles 
    
    Bool_t         fSaveClusters;         //! specifies whether to save clusters into output file 
    Bool_t         fSaveRsParticles;      //! specifies whether to save rs-particles into an output file
    Bool_t         fCreateRsParticles;    //! specifies whether to create rs-particles
    Bool_t         fRemoveHitMCTruth;     //! specifies whether to remove mc-thruth in rs-hits
    Double_t       fPrimVertexRegionR;    //! radial size of vertex region (of type = 1), cm 
    
    // counters
    Int_t          fNTotalHits;           //! total number of processed hits
    Int_t          fNTotalClusters;       //! total number of clusters that were found
    Int_t          fNTotalParticles;      //! total number of rs-particles
    
    //-----------------------
    Int_t          fVerboseLevel;         //! verbose level 

    ClassDef(SpdRsMCClusterMaker,1)
};

#endif  /* __SPDRSMCCLUSTERMAKER_H__ */

