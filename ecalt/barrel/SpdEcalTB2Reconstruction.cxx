// $Id$
// Author: andre   2021/03/04

//_____________________________________________________________________________
//
// SpdEcalTB2Reconstruction
//_____________________________________________________________________________

#include "SpdEcalTB2Reconstruction.h"

#include <TMath.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include "TGeoScaledShape.h"
#include "TGeoArb8.h"

#include "FairRuntimeDb.h"
#include "FairRun.h"

//#include "SpdEcalTBGeoMapper.h"
#include "SpdEcalTBParSet.h"

#include "SpdEcalTB2Hit.h"
#include "SpdEcalTB2GeoMapper.h"
#include "SpdEcalTB2Cluster.h"
#include "SpdEcalTB2RecoParticle.h"

#include "SpdMCTrack.h"
#include "SpdGeoFactory.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <map>

using namespace std;

ClassImp(SpdEcalTB2Reconstruction)

//_____________________________________________________________________________
SpdEcalTB2Reconstruction::SpdEcalTB2Reconstruction(TString params):
FairTask("EcalTB Clustering"),fEventHeader(0),
fParsName(params),fParameters(0),
fGeoMapper(0),fSaveDetIdOption(0),
fHitsArray(0),
fTracksArray(0),
fClustersArray(0),
fParticlesArray(0),
fClusterMinDist(10.0),
fCellEnergyThreshold(0.02),
fRecoType(kSingleSimpleLog)
{

}

//_____________________________________________________________________________
SpdEcalTB2Reconstruction::~SpdEcalTB2Reconstruction() 
{

}

//_____________________________________________________________________________
void SpdEcalTB2Reconstruction::SetParameters() 
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
InitStatus SpdEcalTB2Reconstruction::Init() 
{   
    
    // get RootManager
  
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- <SpdEcalTB2Reconstruction::Init> "
             << "RootManager not instantiated!" << endl;
        return kFATAL;
    }
 
 
    // get Geometry parameters
    SetParameters();
      
    // get Event header
    fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
    if (!fEventHeader) {
        cout << "-E- <SpdEcalTB2Reconstruction::Init>" << "No HEADER object inside " << endl;
        return kERROR;
    }

    // get Points array
    fHitsArray = (TClonesArray*)ioman->GetObject("SpdEcalTB2Hit");
    
    if (!fHitsArray) {
        cout << "-W- <SpdEcalTB2Reconstruction::Init> " << "No EcalHits array!" << endl;
        return kERROR;
    }
    
    // get Tracks array
    fTracksArray = (TClonesArray*)ioman->GetObject("SpdMCTrack");
    
    if (!fTracksArray) {
        cout << "-W- <SpdEcalTB2Reconstruction::Init> " << "No MCTrack array!" << endl;
        return kERROR;
    }

    // Create and register output array of Digits
    fClustersArray = new TClonesArray("SpdEcalTB2Cluster",100);
    fParticlesArray = new TClonesArray("SpdEcalTB2RecoParticle",100);
    
    ioman->Register("SpdEcalTB2Cluster", "ECAL", fClustersArray, kTRUE);
    ioman->Register("MCEventHeader.", "Event", fEventHeader, kTRUE);
//    ioman->Register("SpdMCTrack", "Track", fTracksArray, kTRUE);
    ioman->Register("SpdEcalTB2RecoParticle", "Particles", fParticlesArray, kTRUE);                       
    
    cout << "-I- <SpdEcalTB2Reconstruction:Init> Intialization successfull " << endl;
    
    return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalTB2Reconstruction::Finish() 
{
    cout << "\n-I- <SpdEcalTB2Reconstruction::Finish>" << endl;
}



//________________________________________________________________________________________
Double_t SpdEcalTB2Reconstruction::GetOptimalLogPar(Double_t e, Double_t angle) {
 
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
        cout << "-F- <SpdEcalTB2Reconstruction::GetOptimalLogPar> : wrong size of values!" << endl;
        exit(1);
    }
    for (auto row: values) {
        if (row.size() != energies.size()) {
            cout << "-F- <SpdEcalTB2Reconstruction::GetOptimalLogPar> : wrong size of values[i]!" << endl;
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
        cout << "-F- <SpdEcalTB2Reconstruction::GetOptimalLogPar> : energy/angle bin not found!" << endl;
        exit(1);
    }
    
    
    return values[ia][ie];
    
}


//________________________________________________________________________________________
TVector3 SpdEcalTB2Reconstruction::CorrectPosForAngle(TVector3 pos, Double_t energy) {

    TVector3 newPos = pos;
    
    Double_t coeff1 = -0.055    + 0.0182*energy   - 0.001113*energy*energy;  
    Double_t coeff2 = -1.005e-6 - 3.752e-7*energy + 2.363e-8*energy*energy; 
    Double_t oldZ = pos.Z();
    Double_t zdiff = coeff1*oldZ + coeff2*oldZ*oldZ*oldZ;

    Double_t newZ = oldZ - zdiff;
//    Double_t newZ = pos.Z() + pos.Z()*(0.07) + pos.Z()*pos.Z()*6e-7 + pos.Z()*pos.Z()*pos.Z()*7e-7;
    newPos.SetZ(newZ);
        
    return newPos;
}

//________________________________________________________________________________________
Double_t SpdEcalTB2Reconstruction::CorrectEnergyForAngle(Double_t energy, Double_t angle) {

    Double_t b1 = -0.02756 + 9.911e-5*angle;
    Double_t c1 = 0.06048  - 0.0001362*angle;
    Double_t x0 = 0.6555   + 0.009557*angle;  

    Double_t ediff;

    if (energy < x0) {
        ediff = b1*energy + c1*energy*energy;
    }   
    else {
        ediff = -c1*x0*x0 + energy*(b1 + 2*c1*x0);
    }

    Double_t newE = max(fCellEnergyThreshold, energy - ediff);
    return newE;
}


//________________________________________________________________________________________
vector<SpdEcalTB2RecoParticle> SpdEcalTB2Reconstruction::ReconstructCluster(SpdEcalTB2Cluster* cl) {
    
    vector<Int_t> cells = cl->GetCells();
    if (cells.size() == 0) {
        cout << "-F- <SpdEcalTB2Reconstruction::ReconstructCluster> : empty cells vector" << endl;
    }
    
    vector<SpdEcalTB2RecoParticle> particles;
    
    
    if (fRecoType == kSingleSimpleLinear) {
        Double_t Et = 0.;
        TVector3 pos_weighted = TVector3(0.,0.,0.);
        
        for (Int_t ic = 0; ic < cells.size(); ++ic) {
            SpdEcalTB2Hit* hit = (SpdEcalTB2Hit*)fHitsArray->At(ic);
            Double_t e = hit->GetEnergy();
            Et += e;
            pos_weighted += e*hit->GetCellPosition();
        }
        
        SpdEcalTB2RecoParticle newpart(Et, pos_weighted*(1.0/Et));
        particles.push_back(newpart);
    }
    
    else if (fRecoType == kSingleSimpleLog) {
        
        //first, calculate approximate position
        Double_t Et = 0.;
        TVector3 pos_linWeighted = TVector3(0.,0.,0.);
        TVector3 pos_logWeighted = TVector3(0.,0.,0.);
        
        for (Int_t ic = 0; ic < cells.size(); ++ic) {
            SpdEcalTB2Hit* hit = (SpdEcalTB2Hit*)fHitsArray->At(ic);
            Double_t e = hit->GetEnergy();
            Et += e;
            pos_linWeighted += e*hit->GetCellPosition();
        }
        
        Double_t logPar = GetOptimalLogPar(Et, fabs(TMath::Pi() - pos_linWeighted.Theta()));
        
        Double_t sumWeight = 0.;
        
        for (Int_t ic = 0; ic < cells.size(); ++ic) {
            SpdEcalTB2Hit* hit = (SpdEcalTB2Hit*)fHitsArray->At(ic);
            Double_t e = hit->GetEnergy();
            Double_t weight = max(0., logPar + TMath::Log(e) - TMath::Log(Et));
            pos_logWeighted += weight*hit->GetCellPosition();
            sumWeight += weight;
        }
        
        TVector3 pos = pos_logWeighted*(1.0/sumWeight);
        pos = CorrectPosForAngle(pos, Et);
        Et = CorrectEnergyForAngle(Et, TMath::RadToDeg()*fabs(TMath::Pi()/2 - pos.Theta()));
        
        SpdEcalTB2RecoParticle newpart(Et, pos);
        particles.push_back(newpart);
        
    }
    else if (fRecoType == kSingleShower) {
        cout << "-F- <SpdEcalTB2Reconstruction::ReconstructCluster> : "
             <<" kSingleShower reconstruction algorithm is not implemented yet!" << endl;
        exit(1);
    }
    else if (fRecoType == kMultiShower) {
        cout << "-F- <SpdEcalTB2Reconstruction::ReconstructCluster> : "
             <<" kMultiShower reconstruction algorithm is not implemented yet!" << endl;
        exit(1);
    }
    else {
        cout << "-F- <SpdEcalTB2Reconstruction::ReconstructCluster> : reconstruction type " 
             << fRecoType << " not found!" << endl;
        exit(1);
    }
    
    return particles;    
}

//________________________________________________________________________________________
void SpdEcalTB2Reconstruction::DoClustering() {
        
    Int_t nCells = fHitsArray->GetEntriesFast();
    vector<Bool_t> used(nCells, false);

    while (true) {
        
        Int_t firstCell = -1;
        for (Int_t ic = 0; ic < nCells; ++ic) {
            if (used[ic]) continue;
            SpdEcalTB2Hit* hit = (SpdEcalTB2Hit*)fHitsArray->At(ic);
            if (hit->GetEnergy() < fCellEnergyThreshold) continue;
            firstCell = ic;
            break;
        }
        if (firstCell == -1) break; //no cells over threshold: stop searching for clusters
        
        Int_t ne = fClustersArray->GetEntriesFast();
        SpdEcalTB2Cluster* cluster = new ((*fClustersArray)[ne]) SpdEcalTB2Cluster();
    
        cluster->AddCell(firstCell); 
        used[firstCell] = true;
        
        Bool_t toSearch = true;
        while (toSearch) {
            toSearch = false;
            for (Int_t ih = 0; (ih < nCells) && toSearch == false; ++ih) {
                if (used[ih]) continue;
                SpdEcalTB2Hit* hit = (SpdEcalTB2Hit*)fHitsArray->At(ih);
                if (hit->GetEnergy() < fCellEnergyThreshold) {
                    continue;
                }
                for (Int_t ic = 0; ic < cluster->GetSize(); ++ic) {
                    
                    TVector3 posHit = hit->GetCellPosition();
                    SpdEcalTB2Hit* hitCl = (SpdEcalTB2Hit*)fHitsArray->At(cluster->GetCells()[ic]);
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
void SpdEcalTB2Reconstruction::Exec(Option_t* opt) 
{
    fClustersArray->Delete();
    fParticlesArray->Delete();
    
    cout << "\n-I- <SpdEcalTB2Reconstruction::Exec> Started with " 
         << "tracks/hits = ";
    
    if (fHitsArray && fHitsArray->GetEntriesFast() > 1) {
        cout << fTracksArray->GetEntriesFast() << "/"
             << fHitsArray->GetEntriesFast() << endl;
    }
    else {
        cout << "-W- <SpdEcalTB2Reconstruction::Exec> Empty array of hits " 
             << endl;
    }
    
    if (!fClustersArray) {
        Fatal("SpdEcalTB2Reconstruction::Exec", "No array of clusters");
    }

    DoClustering();
    
    for (Int_t ic = 0; ic < fClustersArray->GetEntriesFast(); ++ic) {
        SpdEcalTB2Cluster* cl = (SpdEcalTB2Cluster*)fClustersArray->At(ic);
        vector<SpdEcalTB2RecoParticle> particles = ReconstructCluster(cl);
        for (auto part: particles) {
            Int_t ne = fParticlesArray->GetEntriesFast();
            SpdEcalTB2RecoParticle* calopart = new ((*fParticlesArray)[ne]) SpdEcalTB2RecoParticle(part);
        }
    }
    
    Int_t nhits = fHitsArray->GetEntriesFast();
    
    cout << "-I- <SpdEcalTB2Reconstruction::Exec> particles/clusters/hits = " 
        << fParticlesArray->GetEntriesFast()
        << "/" << fClustersArray->GetEntriesFast() 
        << "/" << nhits << endl;

}







