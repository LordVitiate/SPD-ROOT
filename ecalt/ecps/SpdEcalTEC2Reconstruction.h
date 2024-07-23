// $Id$
// Author: andre   2021/03/04

#ifndef __SPDECALTEC2RECONSTRUCTION_H__
#define __SPDECALTEC2RECONSTRUCTION_H__

#include "FairTask.h"

#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"
#include "SpdEcalTEC2RecoParticle.h"
#include "SpdEcalTEC2Cluster.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTEC2Reconstruction                                                  //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdEcalTEC2Reconstruction: public FairTask {

public:

    SpdEcalTEC2Reconstruction(TString params = "EcalTECParSet");
    virtual ~SpdEcalTEC2Reconstruction();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    inline TString GetParsName() const { return fParsName; }
    
    void SetClusterMinDist(Double_t minDist) { fClusterMinDist = minDist; };
    void SetCellEnergyThreshold(Double_t thr) { fCellEnergyThreshold = thr; };
    
    void Reco_SingleSimpleLinear()   { fRecoType = kSingleSimpleLinear; };
    void Reco_SingleSimpleLog()      { fRecoType = kSingleSimpleLog; };
    void Reco_SingleShower()         { fRecoType = kSingleShower; };
    void Reco_MultiShower()          { fRecoType = kMultiShower; };
    
private:
  
    enum clusterReco 
    {
        kSingleSimpleLinear, //1 cluster = 1 particle, energy = sum of energies, position = mean with linear weights
        kSingleSimpleLog, //1 cluster = 1 particle, energy = sum of energies, position = mean with log weights
        kSingleShower, //1 cluster = 1 particle, energy from shower fit
        kMultiShower //fit with several shower shapes
    };
    
    
    SpdGeoMapper*  fGeoMapper;  // geometry mapper
    
    Double_t GetOptimalLogPar(Double_t e, Double_t angle);
    std::vector<SpdEcalTEC2RecoParticle> ReconstructCluster(SpdEcalTEC2Cluster* cl);
    void DoClustering();
    
    TVector3 CorrectPosForAngle(TVector3 pos);
    
    void SetParameters();
    
    SpdMCEventHeader* fEventHeader; // Input event header
  
    TString       fParsName;        // Input parameters object name
    SpdParSet*    fParameters;      // Input parameters
    Int_t         fSaveDetIdOption; // Input detector's id saving option
    Int_t         fRecoType;
    
    TClonesArray* fHitsArray;    //! Input  array of points 
    TClonesArray* fTracksArray;    //! Input  array of tracks
    TClonesArray* fClustersArray;    //! Output array of digits
    TClonesArray* fParticlesArray;    //! Output array of calo particles
        
    Double_t fClusterMinDist;
    Double_t fCellEnergyThreshold;
    
    ClassDef(SpdEcalTEC2Reconstruction,1)
};

#endif  /* __SPDECALTEC2RECONSTRUCTION_H__ */

