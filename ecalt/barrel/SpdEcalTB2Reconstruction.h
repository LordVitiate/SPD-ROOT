// $Id$
// Author: andre   2021/03/04

#ifndef __SPDECALTB2RECONSTRUCTION_H__
#define __SPDECALTB2RECONSTRUCTION_H__

#include "FairTask.h"

#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"
#include "SpdEcalTB2Cluster.h"
#include "SpdEcalTB2RecoParticle.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTB2Reconstruction                                                   //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdEcalTB2Reconstruction: public FairTask {

public:

    SpdEcalTB2Reconstruction(TString params = "EcalTBParSet");
    virtual ~SpdEcalTB2Reconstruction();
    
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
    std::vector<SpdEcalTB2RecoParticle> ReconstructCluster(SpdEcalTB2Cluster* cl);
    void DoClustering();
    
    void SetParameters();
    
    SpdMCEventHeader* fEventHeader; // Input event header
  
    TVector3 CorrectPosForAngle(TVector3 pos, Double_t energy);
    Double_t CorrectEnergyForAngle(Double_t energy, Double_t angle);
  
    TString       fParsName;        // Input parameters object name
    SpdParSet*    fParameters;      // Input parameters
    Int_t         fSaveDetIdOption; // Input detector's id saving option
    
    Int_t fRecoType;
    
    TClonesArray* fHitsArray;    //! Input  array of points 
    TClonesArray* fTracksArray;    //! Input  array of tracks
    TClonesArray* fClustersArray;    //! Output array of clusters
    TClonesArray* fParticlesArray;    //! Output array of reconstructed particles
        
    Double_t fClusterMinDist;
    Double_t fCellEnergyThreshold;
        
    ClassDef(SpdEcalTB2Reconstruction,1)
};

#endif  /* __SPDECALTB2RECONSTRUCTION_H__ */

