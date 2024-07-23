// $Id$
// Author: artur   2021/03/15


//_____________________________________________________________________________
//
// SpdEcalRCMaker
//_____________________________________________________________________________

#include "SpdEcalRCMaker.h"

#include "SpdRunAna.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"
#include "SpdCommonGeoMapper.h"
#include "SpdEcalBGeoMapper.h"
#include "SpdEcalECGeoMapper.h"
#include "SpdGeoFactory.h"

#include "SpdMCEvent.h"
#include "SpdEcalRCParticle.h"
#include "SpdEcalRCCluster.h"

#include "SpdEcalMCHit.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"
#include "SpdEcalPiGammaSeparator.h" 

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalRCMaker)
 
Double_t SpdEcalRCMaker::fAngles[ASIZE] = 
 { 0, 10, 20, 30, 40, 50, 60, 70 };

Double_t SpdEcalRCMaker::fEnergies[ESIZE] = 
 { 0.1, 0.2, 0.4, 0.5, 0.8, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0 };
 
Double_t SpdEcalRCMaker::fLogPars[ASIZE][ESIZE] =
{
 { 4.0, 2.2, 3.0, 3.2, 3.4, 3.4, 3.6, 3.7, 3.8, 3.8, 3.9, 3.8, 3.9, 3.9 }, // 0
 { 2.4, 2.2, 2.6, 2.6, 3.0, 3.0, 3.0, 3.2, 3.2, 3.0, 3.0, 3.0, 3.0, 3.0 }, // 1
 { 1.6, 1.8, 2.2, 2.4, 2.4, 2.4, 2.2, 2.2, 3.9, 4.3, 4.3, 4.7, 4.9, 5.3 }, // 2
 { 1.6, 1.6, 2.0, 2.2, 2.6, 3.0, 3.2, 3.7, 4.1, 4.3, 4.5, 4.9, 5.1, 5.5 }, // 3
 { 1.6, 1.6, 2.0, 2.2, 2.8, 2.6, 2.8, 3.2, 3.0, 3.4, 4.1, 4.7, 4.9, 5.3 }, // 4
 { 1.6, 1.6, 2.2, 2.2, 2.8, 3.0, 3.4, 3.6, 3.7, 3.2, 4.5, 4.9, 4.3, 5.1 }, // 5
 { 1.8, 1.6, 2.2, 2.2, 2.6, 2.8, 3.2, 4.0, 4.0, 3.8, 4.5, 5.5, 5.5, 5.5 }, // 6
 { 5.1, 1.6, 2.2, 2.4, 2.6, 3.0, 3.4, 3.6, 4.1, 4.7, 4.9, 5.7, 6.5, 6.5 }  // 7    
};

//_____________________________________________________________________________
SpdEcalRCMaker::SpdEcalRCMaker():
fEvent(0),fHits(0),
fClusters(0),fParticles(0),
fClusterMinDist(10.0),fCellEnergyThreshold(0.02),
fClusterRecoType(kSpdClRecoSingleSimpleLog),
fBarrelRadius(120.0), fECMinDist(200.0),
//fClusterRecoType(kSpdClRecoSingleSimpleLinear),
fSaveClusters(true),fSaveParticles(true),
fNTotalHits(0),fNTotalAcceptedHits(0),
fNTotalClusters(0),fNTotalParticles(0),
fVerboseLevel(1)
{
    
}

//_____________________________________________________________________________
SpdEcalRCMaker::~SpdEcalRCMaker() 
{
 
}

//_____________________________________________________________________________
InitStatus SpdEcalRCMaker::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdEcalRCMaker::Init> RootManager not instantiated " << endl;
       return kFATAL;
   }
   
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // get Event 
   
   fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   if (!fEvent) return kFATAL;
   
   // get Hits array
   
   fHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCEcalHits));
   if (!fHits) return kFATAL;
   
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fClusters = new TClonesArray("SpdEcalRCCluster");
   ioman->Register(SpdDataBranchName(kRCEcalClusters),"SpdEcalRCCluster", fClusters, fSaveClusters); 
   
   fParticles = new TClonesArray("SpdEcalRCParticle");
   ioman->Register(SpdDataBranchName(kRCEcalParticles),"SpdEcalRCParticle", fParticles, fSaveParticles); 
   
   //============== PASS DATA TO EVENTHELPER ================================/   
   
   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
   if (!helper->GetEvent())    helper->SetEvent(fEvent);
   if (!helper->GetEcalHits()) helper->SetEcalHits(fHits);
   
   helper->SetEcalClustersRC(fClusters);
   helper->SetEcalParticlesRC(fParticles);

   //============== SET GEOMETRY VARIABLES ================================/   
   
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();

   Int_t nBasketModulesZ = -1;
   Int_t nBasketModulesPhi = -1;
   Int_t nBasketsZ = -1;
   Int_t nCellsPhi = -1;
   Int_t nSectors = -1;

   SpdParSet* parsB = dynamic_cast<SpdParSet*>(run->GetParameters("EcalTBParSet"));
   if (!parsB) {
        cout << "SpdEcalRCMaker::Init: could not get the barrel parameter set!" << endl;
   }
   else {
       TString mapperNameB;
       parsB->GetParameter("Mapper",mapperNameB); 
       SpdEcalBGeoMapper* mapperB = dynamic_cast<SpdEcalBGeoMapper*>(SpdGeoFactory::Instance()->Mapper(mapperNameB));
       if (!mapperB) {
            cout << "SpdEcalRCMaker::Init: could not get the barrel GeoMapper!" << endl;
       }
       else {
            fBarrelRadius = mapperB->GetBarrelRadius();
            nBasketsZ = mapperB->GetNZBaskets();
            nSectors = mapperB->GetNSectors();
            mapperB->LoadParametersFrom(parsB);
            mapperB->InitGeometry();
            mapperB->LockGeometry();
       }
       Bool_t ok1 = parsB->GetParameter("EcalBarrelBasketNModulesZ", nBasketModulesZ);
       Bool_t ok2 = parsB->GetParameter("EcalBarrelBasketNModulesPhi", nBasketModulesPhi);
       if (ok2) nCellsPhi = 2*nBasketModulesPhi*nSectors;
   }

   SpdParSet* parsEC = dynamic_cast<SpdParSet*>(run->GetParameters("EcalTECParSet"));
   if (!parsEC) {
        cout << "SpdEcalRCMaker::Init: could not get the endcap parameter set!" << endl;
   }
   else {
       TString mapperNameEC;
       parsEC->GetParameter("Mapper",mapperNameEC); 
       SpdEcalECGeoMapper* mapperEC = dynamic_cast<SpdEcalECGeoMapper*>(SpdGeoFactory::Instance()->Mapper(mapperNameEC));
       if (!mapperEC) {
            cout << "SpdEcalRCMaker::Init: could not get the endcap GeoMapper!" << endl;
       }
       else {
            mapperEC->LoadParametersFrom(parsEC);
            mapperEC->InitGeometry();
            mapperEC->LockGeometry();
            fECMinDist = mapperEC->GetEndcapMinDist();
       }
   }

   fPiGammaSeparator = SpdEcalPiGammaSeparator(nBasketModulesZ, nBasketModulesPhi, nBasketsZ, nCellsPhi);
 
   if (fVerboseLevel > -1) cout << "-I- <SpdEcalMCHitProducer::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalRCMaker::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fHits);
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdEcalRCMaker::Exec> Process event ... " 
            << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";         
   }
   
   fClusters->Delete();
   fParticles->Delete();
   
   Int_t Nhits = fHits->GetEntriesFast();
   
   if (Nhits == 0) {
       cout << "\n-W- <SpdEcalRCMaker::Exec> Empty array of hits ";
       return;
   }
   

   Int_t nhits[2] = {0,0}, nahits[3] = {0,0};
   Int_t nclusters[2] = {0,0}, nparticles[2] = {0,0};
   
   // >>>>>>>>> Clustering >>>>>>>>>

   SpdEcalMCHit* hit;

   std::list<SpdEcalMCHit*> b_hits;      // barrel hits
   std::list<SpdEcalMCHit*> ec_hits[2];  // endcaps hits
   
   for (Int_t i(0); i<Nhits; i++) 
   {
        hit = dynamic_cast<SpdEcalMCHit*>(fHits->At(i));
        if (!hit) continue;
        
        if (hit->GetModId() == kSpdEcalTB) {
            // search for barrel hit
            if (AcceptHit(hit)) b_hits.push_back(hit);
            nhits[0]++;
        }
        else if (hit->GetModId() == kSpdEcalTEC) {
            // search for endcap hit
            if (AcceptHit(hit)) {
                     if (hit->GetEndcapID() == 0) ec_hits[0].push_back(hit);
                else if (hit->GetEndcapID() == 1) ec_hits[1].push_back(hit);
            }
            nhits[1]++;
        }
   }

   nahits[0]  = b_hits.size();
   nahits[1] += ec_hits[0].size();
   nahits[1] += ec_hits[1].size();
   
   nclusters[0]  = DoClustering(b_hits);
   nclusters[1] += DoClustering(ec_hits[0]);
   nclusters[1] += DoClustering(ec_hits[1]);

   Int_t Nclusters = nclusters[0]+nclusters[1];
   
   fNTotalHits += Nhits;
   fNTotalAcceptedHits += (nahits[0]+nahits[1]);
   
   fNTotalClusters += (nclusters[0]+nclusters[1]);
   
   // >>>>>>>>> Particle reconstruction >>>>>>>>>
    
   SpdEcalRCCluster* cluster;
    
   Int_t np;
   for (Int_t i(0); i<Nclusters; i++) {
        cluster = (SpdEcalRCCluster*)fClusters->At(i);
        if (cluster->GetSize() > 0) {
            np = RecoCluster(cluster);
            if (np > 0) {
                if (cluster->IsBarrelCluster()) nparticles[0] += np;
                if (cluster->IsEndcapCluster()) nparticles[1] += np;
            }
            else {
                if (fVerboseLevel > 3) {
                    cout << "\n-W- <SpdEcalRCMaker::Exec> No particles for cluster " 
                         << cluster->GetId();
                }
            }
        }
        else cout << "\n-W- <SpdEcalRCMaker::Exec> Empty cluster " << i;  
   }

   Int_t Nparticles = nparticles[0]+nparticles[1];
    
   fNTotalParticles += Nparticles;
    
   if (fVerboseLevel > 1) 
   {
        cout << "\n-I- <SpdEcalRCMaker::Exec> Print ... " << endl; 
       
        if (fVerboseLevel > 2 && fClusters->GetEntriesFast() > 0) 
        {
            cout << "Print clusters [first(barrel,endcap)]: " << endl; 
            if (nclusters[0] > 0) {
                cluster = dynamic_cast<SpdEcalRCCluster*>(fClusters->At(0));
                if (cluster) cluster->Print();
            }
            if (nclusters[1] > 0) {
                cluster = dynamic_cast<SpdEcalRCCluster*>(fClusters->At(nclusters[0]));
                if (cluster) cluster->Print();
            }
        }
        
        if (fVerboseLevel > 2 && fParticles->GetEntriesFast() > 0) 
        {
            cout << "Print particles [first(barrel,endcap)]: " << endl; 
            
            SpdEcalRCParticle* particle;
            
            if (nparticles[0] > 0) {
                particle = dynamic_cast<SpdEcalRCParticle*>(fParticles->At(0));
                if (particle) particle->Print();
            }
            if (nparticles[1] > 0) {
                particle = dynamic_cast<SpdEcalRCParticle*>(fParticles->At(nparticles[0]));
                if (particle) particle->Print();
            }
        }
     
        cout << "Hits(accepted,total)             " << nahits[0]+nahits[1] << "/" << Nhits << endl;
        cout << "Hits(barrel|accepted,total):     " << nahits[0] << "/" << nhits[0] << endl;
        cout << "Hits(endcaps|accepted,total):    " << nahits[1] << "/" << nhits[1] << endl; 
        cout << "Clusters|Particles (total):      " << Nclusters << "/" << Nparticles << endl;
        cout << "Clusters(barrel,endcaps):        " << nclusters[0] << "/" << nclusters[1] << endl;
        cout << "Particles(barrel,endcaps):       " << nparticles[0] << "/" << nparticles[1] << endl;
   }
}

//________________________________________________________________________________________ 
Int_t SpdEcalRCMaker::DoClustering(std::list<SpdEcalMCHit*>& hits)
{
   if (hits.empty()) return 0;
   
   const Int_t ncl = fClusters->GetEntriesFast();
   const Int_t modId = hits.front()->GetModId();
   
   Int_t detId = -1;
   if (modId == kSpdEcalTB) detId = hits.front()->GetBasketID();
   else if (modId == kSpdEcalTEC) detId = hits.front()->GetEndcapID();
   
   //if (hits.front()->IsBarrelHit()) detId = hits.front()->GetBasketID();
   //else if (hits.front()->IsEndcapHit()) detId = hits.front()->GetEndcapID();
   
   SpdEcalRCCluster* cluster;
   SpdEcalMCHit* cell;
   TVector3 cellPos, hitPos;
   Bool_t toSearch;
   
   while (!hits.empty()) 
   {
        // add new cluster
        cluster = AddCluster();
        cluster->SetModId(modId);
        cluster->SetDetId(detId);
        
        // add new cell into cluster and remove cell from the list
        cluster->AddCell(hits.front()->GetId()); 
        cluster->AddEnergy(hits.front()->GetEnergy()); 

        hits.pop_front(); 

        // search for other cells in this cluster
        toSearch = true;
        
        while (toSearch) 
        {
            toSearch = false;
        
            // loop over all input cells 
            for (auto it = hits.begin(); it != hits.end(); it++) {
            
                 hitPos = (*it)->GetPosition();
        
                 // loop over all cells in the current cluster
                 for (auto ic : cluster->GetCells()) {
                      cell = (SpdEcalMCHit*)fHits->At(ic);
                      cellPos = cell->GetPosition();
                      if ((hitPos - cellPos).Mag() < fClusterMinDist) {
                          toSearch = true;
                          break;
                      }
                 }
                
                 // add new cell into cluster and remove cell from the list
                 if (toSearch) {   
                     cluster->AddCell((*it)->GetId());
                     cluster->AddEnergy((*it)->GetEnergy()); 
                     hits.erase(it);
                     break;
                 }
            }
        }
    }
    
    return (fClusters->GetEntriesFast()-ncl);
}

//________________________________________________________________________________________  
Int_t SpdEcalRCMaker::RecoCluster(SpdEcalRCCluster* cl)
{
    switch (fClusterRecoType) 
    {
        case kSpdClRecoSingleSimpleLinear : return RecoSingleSimpleLinear(cl);
        case kSpdClRecoSingleSimpleLog    : return RecoSingleSimpleLog(cl);
        case kSpdClRecoSingleShower       : return RecoSingleShower(cl);
        case kSpdClRecoMultiShower        : return RecoMultiShower(cl);
        default : break;
    }
    
    cout << "\n-E- <SpdEcalRCMaker::RecoCluster> Unknown reconstruction type: " 
         << fClusterRecoType << endl;
             
    return 0;
}

//________________________________________________________________________________________ 
Int_t SpdEcalRCMaker::RecoSingleSimpleLinear(SpdEcalRCCluster* cl)
{
    Double_t e, Et = 0.;
    TVector3 pos_weighted;
    SpdEcalMCHit* hit(0);
    
    for (auto& ic : cl->GetCells()) {
         hit = (SpdEcalMCHit*)fHits->At(ic);
         e = hit->GetEnergy();
         Et += e;
         pos_weighted += e*hit->GetPosition();
    }
    
    if (!(Et > 0)) return 0;
    
    TVector3 pos = pos_weighted*(1.0/Et);
    
    if (cl->GetModId() == kSpdEcalTEC) pos = CorrectPosForAngleEC(pos, Et);
    
    SpdEcalRCParticle* part = AddParticle();
    
    part->SetClusterId(cl->GetId());
    part->SetEnergy(Et);
    part->SetPosition(pos);
    part->SetPID(PredictParticleType(cl, fHits));
    
    cl->AddParticle(part->GetId());
    
    return 1;
}

//________________________________________________________________________________________ 
Int_t SpdEcalRCMaker::RecoSingleSimpleLog(SpdEcalRCCluster* cl)
{
    // first, calculate approximate position

    //cout << "\n-I- <SpdEcalRCMaker::RecoSingleSimpleLog> cluster (1): " << cl->GetId() << endl;
    
    Double_t e, Et = 0.;
    TVector3 pos_linWeighted, pos_logWeighted;
    SpdEcalMCHit* hit(0);    
    
    for (auto& ic : cl->GetCells()) {
         hit = (SpdEcalMCHit*)fHits->At(ic);
         e = hit->GetEnergy();
         Et += e;
         pos_linWeighted += e*hit->GetPosition();
    }
   
    if (!(Et > 0)) return 0;
  
    Double_t logPar = GetOptimalLogPar(Et, fabs(TMath::Pi() - pos_linWeighted.Theta()));
    
    Double_t weight, sumWeight = 0.;
    
    for (auto& ic : cl->GetCells()) {
         hit = (SpdEcalMCHit*)fHits->At(ic);
         e = hit->GetEnergy();
         weight = TMath::Max(0., logPar + TMath::Log(e) - TMath::Log(Et));
         pos_logWeighted += weight*hit->GetPosition();
         sumWeight += weight;
    }
    if (!(sumWeight > 0)) return 0;
    TVector3 pos = pos_logWeighted*(1.0/sumWeight);
    
    if (cl->GetModId() == kSpdEcalTB) {
        pos = CorrectPosForAngleB(pos, Et);
        Et  = CorrectEnergyForAngleB(Et, TMath::RadToDeg()*fabs(TMath::Pi()/2 - pos.Theta()));
    }
    else if (cl->GetModId() == kSpdEcalTEC) {
        pos = CorrectPosForAngleEC(pos, Et);
    }
    
    if (!(Et > 0)) return 0; 
    SpdEcalRCParticle* part = AddParticle();
    
    part->SetClusterId(cl->GetId());
    part->SetEnergy(Et);
    part->SetPosition(pos);
    part->SetPID(PredictParticleType(cl, fHits));
    
    cl->AddParticle(part->GetId());
    
    //cout << "I- <SpdEcalRCMaker::RecoSingleSimpleLog> cluster (2): " << cl->GetId() << endl;
    
    return 1;  
}

//________________________________________________________________________________________ 
Int_t SpdEcalRCMaker::RecoSingleShower(SpdEcalRCCluster* cl)
{
    cout << "-W- <SpdEcalTB2Reconstruction::RecoSingleShower> "
         << "This reconstruction algorithm is not implemented yet" << endl;
    return 0;     
}

//________________________________________________________________________________________ 
Int_t SpdEcalRCMaker::RecoMultiShower(SpdEcalRCCluster* cl)
{ 
    cout << "-F- <SpdEcalTB2Reconstruction::RecoMultiShower> "
         << "This reconstruction algorithm is not implemented yet" << endl;
    return 0; 
}

//________________________________________________________________________________________
Double_t SpdEcalRCMaker::GetOptimalLogPar(Double_t e, Double_t angle) const
{ 
    angle *= TMath::RadToDeg();
        
    Int_t ie(-1);
         if (e <= fEnergies[0]) ie = 0;
    else if (e >= fEnergies[ESIZE-1]) ie = ESIZE-1;
    else ie = TMath::BinarySearch(ESIZE,fEnergies,e);
    
    Int_t ia(-1);
         if (angle <= fAngles[0]) ia = 0;
    else if (angle >= fAngles[ASIZE-1]) ia = ASIZE-1;
    else ia = TMath::BinarySearch(ASIZE,fAngles,angle);
    
    //cout << e << " "  << ie << ", " << angle << " " << ia << " " << fLogPars[ia][ie] << endl;
    
    if (ia == -1 || ie == -1) {
        cout << "-F- <SpdEcalRCMaker::GetOptimalLogPar> energy/angle bin not found!" << endl;
        cout << e << " " << angle << " " << ie << " " << ia << endl;
        exit(1);
    }
    
    return fLogPars[ia][ie];
}

//________________________________________________________________________________________
TVector3 SpdEcalRCMaker::CorrectPosForAngleB(TVector3 pos, Double_t energy) const
{

    TVector3 newPos = pos;
    
    Double_t oldZ = pos.Z();
    Double_t oldR = pos.Perp();

    Double_t angle = TMath::RadToDeg()*fabs(TMath::Pi()/2 - pos.Theta());
    Double_t depthParLin = 14.21 + 0.055*angle;
    Double_t depthParLog = 2.11 - 0.03*angle;

    Double_t depth = depthParLin + depthParLog*TMath::Log(energy);
    Double_t zdiff = depth * TMath::Sin(angle*TMath::DegToRad()); 

    Double_t newZ = (oldZ > 0) ? oldZ - zdiff : oldZ + zdiff;
    newPos.SetZ(newZ);
    
    newPos.SetX(pos.X()*fBarrelRadius/oldR);
    newPos.SetY(pos.Y()*fBarrelRadius/oldR);
        
    return newPos;

}

//________________________________________________________________________________________
Double_t SpdEcalRCMaker::CorrectEnergyForAngleB(Double_t energy, Double_t angle) const 
{

    Double_t linPar = 0.055 - 0.001065*angle;
    Double_t logPar = 0.009045 - 0.0002776*angle;

    Double_t lostRatio = linPar + logPar*TMath::Log(energy);
    if (lostRatio > 1) return 0.;
    if (lostRatio < 0) return energy;

    Double_t newE = energy*(1+lostRatio);
    return newE;

}

//________________________________________________________________________________________
TVector3 SpdEcalRCMaker::CorrectPosForAngleEC(TVector3 pos, Double_t e) const
{
    Double_t thetax = TMath::ATan(fabs(pos.X())/fabs(pos.Z()));

    Double_t angle = TMath::RadToDeg()*thetax;
    Double_t depthParLin = 14.21 + 0.055*angle;
    Double_t depthParLog = 2.11 - 0.03*angle;

    Double_t depth = depthParLin + depthParLog*TMath::Log(e);
    Double_t xdiff = depth * TMath::Sin(angle*TMath::DegToRad()); 


    Double_t oldCoord = pos.X();
    Double_t oldZ = pos.Z();

    Double_t newX = (oldCoord > 0) ? oldCoord - xdiff : oldCoord + xdiff;

    TVector3 newPos = pos;
    newPos.SetX(newX);



    Double_t thetay = TMath::ATan(fabs(pos.Y())/fabs(pos.Z()));

    angle = TMath::RadToDeg()*thetay;

    depth = depthParLin + depthParLog*TMath::Log(e);
    Double_t ydiff = depth * TMath::Sin(angle*TMath::DegToRad()); 


    oldCoord = pos.Y();
    Double_t newY = (oldCoord > 0) ? oldCoord - ydiff : oldCoord + ydiff;

    newPos.SetY(newY);

    Double_t newZ = (oldZ < 0) ? -fECMinDist : fECMinDist;
    newPos.SetZ(newZ);

    return newPos;

}

//________________________________________________________________________________________ 
Int_t SpdEcalRCMaker::PredictParticleType(const SpdEcalRCCluster* cl, const TClonesArray* hits) const
{
    return fPiGammaSeparator.GetParticleType(cl, hits);
}

//________________________________________________________________________________________ 
Bool_t SpdEcalRCMaker::AcceptHit(const SpdEcalMCHit* hit) const
{
   return (hit->GetEnergy() > fCellEnergyThreshold) ? true : false;
}

//________________________________________________________________________________________  
SpdEcalRCCluster* SpdEcalRCMaker::AddCluster()
{
   if (!fClusters) return 0;
   Int_t size = fClusters->GetEntriesFast();
   SpdEcalRCCluster* cluster = new ((*fClusters)[size]) SpdEcalRCCluster();
   cluster->SetId(size);
   return cluster;
}

//________________________________________________________________________________________  
SpdEcalRCParticle* SpdEcalRCMaker::AddParticle()
{
   if (!fParticles) return 0;
   Int_t size = fParticles->GetEntriesFast();
   SpdEcalRCParticle* particle = new ((*fParticles)[size]) SpdEcalRCParticle();
   particle->SetId(size);
   return particle;
}

//________________________________________________________________________________________  
void SpdEcalRCMaker::Finish()
{
    if (fVerboseLevel > -1) {
        cout << "\n-I- <SpdEcalRCMaker::Finish> hits [acc/total]: " 
             << fNTotalAcceptedHits << "/" << fNTotalHits 
             << ", clusters/particles: " << fNTotalClusters << "/" << fNTotalParticles;
    }
}

