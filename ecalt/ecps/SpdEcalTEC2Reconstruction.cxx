// $Id$
// Author: andre   2021/03/04

//_____________________________________________________________________________
//
// SpdEcalTEC2Reconstruction
//_____________________________________________________________________________

#include "SpdEcalTEC2Reconstruction.h"
#include "SpdEcalTEC2RecoParticle.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include "TGeoScaledShape.h"
#include "TGeoArb8.h"

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdEcalTECGeoMapper.h"
#include "SpdEcalTECParSet.h"

#include "SpdEcalTEC2Hit.h"

#include "SpdMCTrack.h"
#include "SpdEcalTEC2Cluster.h"
#include "SpdEcalTEC2Hit.h"
#include "SpdGeoFactory.h"

#include <iostream>
#include <map>

using namespace std;

ClassImp(SpdEcalTEC2Reconstruction)

//_____________________________________________________________________________
SpdEcalTEC2Reconstruction::SpdEcalTEC2Reconstruction(TString params):
FairTask("EcalTEC Clustering"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fHitsArray(0),
//fTracksArray(0),
fClustersArray(0),
fParticlesArray(0),
fClusterMinDist(10.0),
fCellEnergyThreshold(0.02),
fRecoType(kSingleSimpleLog)
{

}

//_____________________________________________________________________________
SpdEcalTEC2Reconstruction::~SpdEcalTEC2Reconstruction() 
{

}

//_____________________________________________________________________________
void SpdEcalTEC2Reconstruction::SetParameters() 
{   
    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    fParameters = (SpdParSet*)rtdb->getContainer(fParsName);
    ((FairParSet*)fParameters)->init();
  
    //fParameters->printParams();
    //fParameters->printByLabel(-1);
    //fParameters->printByLabel(-2);

    TString mapper;
    fParameters->GetParameter("Mapper",mapper); 
    fGeoMapper = SpdGeoFactory::Instance()->Mapper(mapper);
    
    if (fGeoMapper) {
      
        fGeoMapper->LoadParametersFrom(fParameters);
   
        fGeoMapper->InitGeometry();
        fGeoMapper->LockGeometry();
  
        fGeoMapper->Print("");
    }
    
    fParameters->GetParameter("Detector/SaveDetIdOption",fSaveDetIdOption);
}

//_____________________________________________________________________________
InitStatus SpdEcalTEC2Reconstruction::Init() 
{   
    // get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdEcalTEC2Reconstruction::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }

    // get Geometry parameters
    SetParameters();
    
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdEcalTEC2Reconstruction::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fHitsArray = (TClonesArray*)ioman->GetObject("SpdEcalTEC2Hit");
    
    if (!fHitsArray) {
        cout << "-W- <SpdEcalTEC2Reconstruction::Init> " << "No EcalHits array!" << endl;
        return kERROR;
    }
    
    // Create and register output array of Digits
    fClustersArray = new TClonesArray("SpdEcalTEC2Cluster",100);
    fParticlesArray = new TClonesArray("SpdEcalTEC2RecoParticle",100);
    
    ioman->Register("SpdEcalTEC2Cluster", "ECAL", fClustersArray, kTRUE);
    ioman->Register("SpdEcalTEC2RecoParticle", "ECALParticle", fParticlesArray, kTRUE);
                           
    cout << "-I- <SpdEcalTEC2Reconstruction:Init> Intialization successfull " << endl;
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalTEC2Reconstruction::Finish() 
{
    cout << "\n-I- <SpdEcalTEC2Reconstruction::Finish>" << endl;
}



//________________________________________________________________________________________
Double_t SpdEcalTEC2Reconstruction::GetOptimalLogPar(Double_t e, Double_t angle) {
 
    angle *= TMath::RadToDeg();
    
    vector<Double_t> energies = {0.1, 0.2, 0.4, 0.5, 0.8, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0};
    vector<Double_t> angles = {0, 10, 20, 30, 40, 50, 60, 70};
    
    vector<vector<Double_t> > values = {
        {4, 2.2, 3, 3.2, 3.4, 3.4, 3.6, 3.7, 3.8, 3.8, 3.9, 3.8, 3.9, 3.9},
        {2.4, 2.2, 2.6, 2.6, 3, 3, 3, 3.2, 3.2, 3, 3, 3, 3, 3},
        {1.6, 1.8, 2.2, 2.4, 2.4, 2.4, 2.2, 2.2, 3.9, 4.3, 4.3, 4.7, 4.9, 5.3},
        {1.6, 1.6, 2, 2.2, 2.6, 3, 3.2, 3.7, 4.1, 4.3, 4.5, 4.9, 5.1, 5.5},
        {1.6, 1.6, 2, 2.2, 2.8, 2.6, 2.8, 3.2, 3, 3.4, 4.1, 4.7, 4.9, 5.3},
        {1.6, 1.6, 2.2, 2.2, 2.8, 3, 3.4, 3.6, 3.7, 3.2, 4.5, 4.9, 4.3, 5.1},
        {1.8, 1.6, 2.2, 2.2, 2.6, 2.8, 3.2, 4, 4, 3.8, 4.5, 5.5, 5.5, 5.5},
        {5.1, 1.6, 2.2, 2.4, 2.6, 3, 3.4, 3.6, 4.1, 4.7, 4.9, 5.7, 6.5, 6.5} 
    };
    
    if (values.size() != angles.size()) {
        cout << "-F- <SpdEcalTEC2Reconstruction::GetOptimalLogPar> : wrong size of values!" << endl;
        exit(1);
    }
    for (auto row: values) {
        if (row.size() != energies.size()) {
            cout << "-F- <SpdEcalTEC2Reconstruction::GetOptimalLogPar> : wrong size of values[i]!" << endl;
            exit(1);
        }
    }
    
    Int_t ie(-1);
    if (e < energies[0]) ie = 0;
    else if (e > energies[energies.size()-1]) ie = energies.size()-1;
    else {
        auto lower = std::lower_bound(energies.begin(), energies.end(), e);
        ie = std::distance(energies.begin(), lower);
    }
    
    Int_t ia(-1);
    if (angle < angles[0]) ia = 0;
    else if (angle > angles[angles.size()-1]) ia = angles.size()-1;
    else {
        auto lower = std::lower_bound(angles.begin(), angles.end(), angle);
        ia = std::distance(angles.begin(), lower);
    }
    
    if (ia == -1 || ie == -1) {
        cout << "-F- <SpdEcalTEC2Reconstruction::GetOptimalLogPar> : energy/angle bin not found!" << endl;
        exit(1);
    }
    
    
    return values[ia][ie];
    
}


//________________________________________________________________________________________
TVector3 SpdEcalTEC2Reconstruction::CorrectPosForAngle(TVector3 pos) {

    TVector3 newPos = pos;
    
    newPos.SetX(pos.X()*1.01668);
    newPos.SetY(pos.Y()*1.01668);

    return newPos;
}


//________________________________________________________________________________________
vector<SpdEcalTEC2RecoParticle> SpdEcalTEC2Reconstruction::ReconstructCluster(SpdEcalTEC2Cluster* cl) {
    
    vector<Int_t> cells = cl->GetCells();
    if (cells.size() == 0) {
        cout << "-F- <SpdEcalTEC2Reconstruction::ReconstructCluster> : empty cells vector" << endl;
    }
    
    vector<SpdEcalTEC2RecoParticle> particles;
    
    
    if (fRecoType == kSingleSimpleLinear) {
        Double_t Et = 0.;
        TVector3 pos_weighted = TVector3(0.,0.,0.);
        
        for (Int_t ic = 0; ic < cells.size(); ++ic) {
            SpdEcalTEC2Hit* hit = (SpdEcalTEC2Hit*)fHitsArray->At(ic);
            Double_t e = hit->GetEnergy();
            Et += e;
            pos_weighted += e*hit->GetCellPosition();
        }
        
        TVector3 pos = pos_weighted*(1.0/Et);
        pos = CorrectPosForAngle(pos);
        
        SpdEcalTEC2RecoParticle newpart(Et, pos);
        particles.push_back(newpart);
    }
    
    else if (fRecoType == kSingleSimpleLog) {
        
        //first, calculate approximate position
        Double_t Et = 0.;
        TVector3 pos_linWeighted = TVector3(0.,0.,0.);
        TVector3 pos_logWeighted = TVector3(0.,0.,0.);
        
        for (Int_t ic = 0; ic < cells.size(); ++ic) {
            SpdEcalTEC2Hit* hit = (SpdEcalTEC2Hit*)fHitsArray->At(ic);
            Double_t e = hit->GetEnergy();
            Et += e;
            pos_linWeighted += e*hit->GetCellPosition();
        }
        
        Double_t logPar = GetOptimalLogPar(Et, pos_linWeighted.Theta());
        
        Double_t sumWeight = 0.;
        
        for (Int_t ic = 0; ic < cells.size(); ++ic) {
            SpdEcalTEC2Hit* hit = (SpdEcalTEC2Hit*)fHitsArray->At(ic);
            Double_t e = hit->GetEnergy();
            Double_t weight = max(0., logPar + TMath::Log(e) - TMath::Log(Et));
            pos_logWeighted += weight*hit->GetCellPosition();
            sumWeight += weight;
        }
        
        TVector3 pos = pos_logWeighted*(1.0/sumWeight);
        pos = CorrectPosForAngle(pos);
        
        SpdEcalTEC2RecoParticle newpart(Et, pos);
        particles.push_back(newpart);
        
    }
    else if (fRecoType == kSingleShower) {
        cout << "-F- <SpdEcalTEC2Reconstruction::ReconstructCluster> : "
             <<" kSingleShower reconstruction algorithm is not implemented yet!" << endl;
        exit(1);
    }
    else if (fRecoType == kMultiShower) {
        cout << "-F- <SpdEcalTEC2Reconstruction::ReconstructCluster> : "
             <<" kMultiShower reconstruction algorithm is not implemented yet!" << endl;
        exit(1);
    }
    else {
        cout << "-F- <SpdEcalTEC2Reconstruction::ReconstructCluster> : reconstruction type " 
             << fRecoType << " not found!" << endl;
        exit(1);
    }
    
    
    return particles;
    
}


//________________________________________________________________________________________
void SpdEcalTEC2Reconstruction::DoClustering() {
    
    Int_t nCells = fHitsArray->GetEntriesFast();
    vector<Bool_t> used(nCells, false);

    while (true) {
        
        Int_t firstCell = -1;
        for (Int_t ic = 0; ic < nCells; ++ic) {
            if (used[ic]) continue;
            SpdEcalTEC2Hit* hit = (SpdEcalTEC2Hit*)fHitsArray->At(ic);
            if (hit->GetEnergy() < fCellEnergyThreshold) continue;
            firstCell = ic;
            break;
        }
        if (firstCell == -1) break; //no cells over threshold: stop searching for clusters
        
        Int_t ne = fClustersArray->GetEntriesFast();
        SpdEcalTEC2Cluster* cluster = new ((*fClustersArray)[ne]) SpdEcalTEC2Cluster();
        
        cluster->AddCell(firstCell); 
        used[firstCell] = true;
        
        Bool_t toSearch = true;
        while (toSearch) {
            toSearch = false;
            for (Int_t ih = 0; (ih < nCells) && toSearch == false; ++ih) {
                if (used[ih]) continue;
                SpdEcalTEC2Hit* hit = (SpdEcalTEC2Hit*)fHitsArray->At(ih);
                if (hit->GetEnergy() < fCellEnergyThreshold) {
                    continue;
                }
                for (Int_t ic = 0; ic < cluster->GetSize(); ++ic) {
                    
                    TVector3 posHit = hit->GetCellPosition();
                    SpdEcalTEC2Hit* hitCl = (SpdEcalTEC2Hit*)fHitsArray->At(cluster->GetCells()[ic]);
                    TVector3 posCl = hitCl->GetCellPosition();
                    
                    if ((posHit - posCl).Mag() < fClusterMinDist) {
                        toSearch = true;
                        cluster->AddCell(ih);
                        used[ih] = true;
                        break;
                    }
                }
            }
        }        
        
    }
}
    
//________________________________________________________________________________________
void SpdEcalTEC2Reconstruction::Exec(Option_t* opt) 
{
    fClustersArray->Delete();
    fParticlesArray->Delete();
    
    //cout << "\n-I- <SpdEcalTEC2Reconstruction::Exec> Started with " 
    //     << "tracks/hits = ";
 
    if (!fClustersArray) {
        Fatal("SpdEcalTEC2Reconstruction::Exec", "No array of clusters");
    }

    DoClustering();
    
    for (Int_t ic = 0; ic < fClustersArray->GetEntriesFast(); ++ic) {
        SpdEcalTEC2Cluster* cl = (SpdEcalTEC2Cluster*)fClustersArray->At(ic);
        vector<SpdEcalTEC2RecoParticle> particles = ReconstructCluster(cl);
        for (auto part: particles) {
            Int_t ne = fParticlesArray->GetEntriesFast();
            SpdEcalTEC2RecoParticle* calopart = new ((*fParticlesArray)[ne]) SpdEcalTEC2RecoParticle(part);
        }
    }
    
    Int_t nhits = fHitsArray->GetEntriesFast();
    cout << "-I- <SpdEcalTEC2Reconstruction::Exec> clusters/hits = " 
        << fParticlesArray->GetEntriesFast()
        << "/" << fClustersArray->GetEntriesFast() 
        << "/" << nhits << endl;
}




