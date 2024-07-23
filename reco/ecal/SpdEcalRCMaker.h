// $Id$
// Author: artur   2021/03/15

#ifndef __SPDECALRCMAKER_H__
#define __SPDECALRCMAKER_H__

#include "SpdTask.h"

#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>
#include "SpdEcalCommon.h" 
#include "SpdEcalPiGammaSeparator.h" 
#include <list>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalRCMaker                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCEvent;
class SpdEcalMCHit;
class SpdEcalRCParticle;
class SpdEcalRCCluster;

class SpdEcalRCMaker: public SpdTask {

public:

    SpdEcalRCMaker();
    virtual ~SpdEcalRCMaker();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    void SetClusterMinDist(Double_t minDist /*cm*/)   { fClusterMinDist = minDist;  }
    void SetCellEnergyThreshold(Double_t thr /*GeV*/) { fCellEnergyThreshold = thr; }
    void SetClusterRecoType(Int_t type);
    Int_t PredictParticleType(const SpdEcalRCCluster* cl, const TClonesArray* hits) const;
    
    // Save the hits into the output file 
    void  SaveClusters(Bool_t save = true)    { fSaveClusters  = save; }
    void  SaveParticles(Bool_t save = true)   { fSaveParticles = save; }

    void  SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }

protected:

    /* Methods */

    virtual Bool_t AcceptHit(const SpdEcalMCHit* hit) const;
     
    virtual SpdEcalRCCluster*  AddCluster();
    virtual SpdEcalRCParticle* AddParticle();

    virtual Int_t  DoClustering(std::list<SpdEcalMCHit*>& hits);
 
    virtual Int_t  RecoCluster(SpdEcalRCCluster* cl);
    
    virtual Int_t  RecoSingleSimpleLinear(SpdEcalRCCluster* cl);
    virtual Int_t  RecoSingleSimpleLog(SpdEcalRCCluster* cl);
    virtual Int_t  RecoSingleShower(SpdEcalRCCluster* cl);
    virtual Int_t  RecoMultiShower(SpdEcalRCCluster* cl);
    
    Double_t GetOptimalLogPar(Double_t e, Double_t angle /*rad*/) const;
    
    Double_t CorrectEnergyForAngleB(Double_t energy, Double_t angle /*deg*/) const; // barrel
    TVector3 CorrectPosForAngleB(TVector3 pos, Double_t energy) const; // barrel
    TVector3 CorrectPosForAngleEC(TVector3 pos, Double_t e) const; // endcaps
   
    /* Data memebers */ 
    
    // input data 
    SpdMCEvent*    fEvent;                //! Main mc-data object
    TClonesArray*  fHits;                 //! List of mc-hits 
    
    // output data 
    TClonesArray*  fClusters;             //! List of reconstructed clusters
    TClonesArray*  fParticles;            //! List of reconstructed particles 
    
    // settings
    Double_t       fClusterMinDist;       //! clusterization distance,  [cm]
    Double_t       fCellEnergyThreshold;  //! min. cell energy deposit, [GeV]
    Int_t          fClusterRecoType;      //! algoritm type for cluster reconstruction
    
    Bool_t         fSaveClusters;         //! specifies whether to save clusters into output file 
    Bool_t         fSaveParticles;        //! specifies whether to save particles into output file 
    
    // counters
    Int_t          fNTotalHits;           //! total number of processed hits
    Int_t          fNTotalAcceptedHits;   //! total number of accepted hits
    Int_t          fNTotalClusters;       //! total number of clusters that were found
    Int_t          fNTotalParticles;      //! total number of reconstructed particles (cluster pars.)
      
    //-----------------------
    Int_t          fVerboseLevel;         //! verbose level 
    
    //-----------------------
    Double_t       fBarrelRadius;         //! radius of barrel from geometry
    Double_t       fECMinDist;            //! minimum distance from Z = 0 to the endcaps

    //-----------------------
    SpdEcalPiGammaSeparator fPiGammaSeparator;
    
    // auxiliary
    static const int ESIZE = 14;             //!
    static const int ASIZE = 8;              //!  
    
    static Double_t fAngles[ASIZE];          //!
    static Double_t fEnergies[ESIZE];        //!
    static Double_t fLogPars[ASIZE][ESIZE];  //!
    
    ClassDef(SpdEcalRCMaker,1)
};

//----------------------------------------------------------------------------
inline void SpdEcalRCMaker::SetClusterRecoType(Int_t type) 
{ 
    if (fClusterRecoType >= kSpdClRecoSingleSimpleLinear &&
        fClusterRecoType < kSpdClRecoLast) fClusterRecoType = type;
    else fClusterRecoType = kSpdClRecoSingleSimpleLog;
}

#endif  /* __SPDECALRCMAKER_H__ */

