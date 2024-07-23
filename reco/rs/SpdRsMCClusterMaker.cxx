// $Id$
// Author: artur   2021/06/03


//_____________________________________________________________________________
//
// SpdRsMCClusterMaker
//_____________________________________________________________________________

#include "SpdRsMCClusterMaker.h"

#include "SpdRunAna.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"

#include "SpdRsMCHit.h"
#include "SpdRsMCCluster.h"
#include "SpdRsMCParticle.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"

#include <map>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRsMCClusterMaker)

//_____________________________________________________________________________
SpdRsMCClusterMaker::SpdRsMCClusterMaker():
fEvent(0),fParticles(0),fHits(0),
fClusters(0),fRsParticles(0),
fSaveClusters(true),fSaveRsParticles(true),
fCreateRsParticles(true),fRemoveHitMCTruth(false),
fPrimVertexRegionR(1.0),
fNTotalHits(0),fNTotalClusters(0),fNTotalParticles(0),
fVerboseLevel(1)
{
 
}

//_____________________________________________________________________________
SpdRsMCClusterMaker::~SpdRsMCClusterMaker() 
{
 
}

//_____________________________________________________________________________
InitStatus SpdRsMCClusterMaker::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdRsMCClusterMaker::Init> RootManager not instantiated " << endl;
       return kFATAL;
   }
   
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // get Event 
   
   fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   if (!fEvent) return kFATAL;
   
   // get Particles array
   
   fParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCParticles));
   if (!fParticles) return kFATAL;
   
   // get Hits array
   
   fHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCRsHits));
   if (!fHits) return kFATAL;
   
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fClusters = new TClonesArray("SpdRsMCCluster");
   ioman->Register(SpdDataBranchName(kMCRsClusters),"SpdRsMCCluster", fClusters, fSaveClusters); 
   
   if (fCreateRsParticles) {
       fRsParticles = new TClonesArray("SpdRsMCParticle");
       ioman->Register(SpdDataBranchName(kMCRsParticles),"SpdRsMCParticle",fRsParticles,fSaveRsParticles);
   }
   
   //============== PASS DATA TO EVENTHELPER ================================/   
   
   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
   if (!helper->GetEvent())     helper->SetEvent(fEvent);
   if (!helper->GetParticles()) helper->SetParticles(fParticles);
   if (!helper->GetRsHits())    helper->SetRsHits(fHits);
   
   helper->SetRsClustersMC(fClusters);
   if (fRsParticles) helper->SetRsParticlesMC(fRsParticles);
 
   if (fVerboseLevel > -1) cout << "-I- <SpdRsMCClusterMaker::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}
//________________________________________________________________________________________
void SpdRsMCClusterMaker::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fHits);
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdRsMCClusterMaker::Exec> Process event ... " 
            << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";         
   }
   
   fClusters->Delete();
   if (fRsParticles) fRsParticles->Delete();
   
   if (fHits->GetEntriesFast() < 1) {
       cout << "\n-W- <SpdRsMCClusterMaker::Exec> Empty input data array (hits) ";
       return;
   }

   if (fParticles->GetEntriesFast() < 1) {
       cout << "\n-W- <SpdRsMCClusterMaker::Exec> Empty input data array (particles) ";
       return;
   }
      
   SpdMCParticle* part = (SpdMCParticle*)fParticles->At(0);
   fPrimVertexPosition = part->GetStartPos();
   
   // >>>>>>>>> Clustering >>>>>>>>>
   
   MakeClusters(); 
   
   // >>>>>>>>> Create RS-particles >>>>>>>>>
   
   if (fRsParticles && fClusters->GetEntriesFast() > 0) MakeRsParticles();

   fNTotalHits      += fHits->GetEntriesFast();
   fNTotalClusters  += fClusters->GetEntriesFast();
   if (fRsParticles) fNTotalParticles += fRsParticles->GetEntriesFast();
   
   if (fVerboseLevel > 1) 
   {
       cout << "\n-I- <SpdEcalClusterMCInfoMaker::Exec> Print ... " << endl; 
       cout << "Hits:                   " << fHits->GetEntriesFast() << endl; 
       cout << "Clusters:               " << fClusters->GetEntriesFast() << endl;
       cout << "Number of particles:    " 
            << ((fRsParticles) ? fRsParticles->GetEntriesFast() : 0) << endl;
   }
   
   if (fRemoveHitMCTruth) RemoveMCTruth();
}

//________________________________________________________________________________________  
void SpdRsMCClusterMaker::MakeClusters()
{
   //cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " << endl; 
   //cout << " hits: " << fHits->GetEntriesFast() << "\n\n";
   
   Int_t Nhits = fHits->GetEntriesFast();
   SpdRsMCHit* hit;
   SpdRsHitMCTruth* hit_truth; 
   
   Int_t htype; // hit type: 0 = barrel, 1 = endcapZ+, 2 = endcapZ- 
   Int_t n, pid;
   
   std::map<Int_t, SpdRsMCCluster*> clusters[3]; // < mc-particle id, cluster* > [htype]
   std::map<Int_t, SpdRsMCCluster*>::iterator cit;
 
   SpdRsMCCluster* cluster;
   SpdMCParticle *part;
   Int_t vmodule;
   
   for (Int_t i(0); i<Nhits; i++) 
   {
       hit = (SpdRsMCHit*)fHits->At(i);
       if (!hit) continue;
      
       hit_truth = hit->GetMCTruth();
       if (!hit_truth) continue;
       
       n = hit_truth->GetN();
       if (n < 1) continue;
       
       if (hit->IsBarrelHit()) htype = 0; 
       else {
           htype = (hit->GetHitPos().Z() > 0) ? 1 : 2;
           //if (hit->GetHitPos().Z() > 0) htype = 1; 
           //else htype = 2;
       }
    
       //hit->Print();
       
       for (Int_t j(0); j<n; j++) 
       {
            pid = hit_truth->GetMother(j);
            part = (SpdMCParticle*)fParticles->At(pid);
                    
            while (true) 
            {
                vmodule = part->GetVModule();
                
                if (vmodule == kSpdRsTB || vmodule == kSpdRsTEC) {
                    part = (SpdMCParticle*)fParticles->At(part->GetMotherId());
                    continue;
                }
                
                pid = part->GetId();
                
                cit = clusters[htype].find(pid);
                
                //cout << "nhit: " << i << " htype: " << htype << " mother id: " << hit_truth->GetMother(j) 
                //     << " pid: " << pid << " j = " << j << endl;
                
                if (cit != clusters[htype].end()) 
                {
                    cluster = cit->second;
                    cluster->AddHit(hit->GetId());
                }
                else 
                {
                    cluster = AddCluster();
                    
                         if (htype == 0) { cluster->SetModId(kSpdRsTB);  cluster->SetDetId(0);  }
                    else if (htype == 1) { cluster->SetModId(kSpdRsTEC); cluster->SetDetId(1);  }
                    else                 { cluster->SetModId(kSpdRsTEC); cluster->SetDetId(-1); }
                  
                    cluster->AddHit(hit->GetId());
                    clusters[htype].insert(std::pair<Int_t,SpdRsMCCluster*>(pid,cluster));
                }
    
                break;
             }
       }
   }
   
   for (Int_t i(0); i<3; i++) {
        for (cit = clusters[i].begin(); cit != clusters[i].end(); cit++) {
             cluster = cit->second;
             MakeClusterInfo(cluster,cit->first);
        }
   }
}

//________________________________________________________________________________________ 
void SpdRsMCClusterMaker::MakeClusterInfo(SpdRsMCCluster* cluster, Int_t mother_pid)
{
   //cout << " mother_pid = " << mother_pid << endl;
   //cluster->Print();
   
   SpdMCParticle* part = (SpdMCParticle*)fParticles->At(mother_pid);  
   
   //---------------------------------------------------------
   // 1 particle:  
   // [ 3 -> 0 ] 
   
   if (part->GetGeneration() == 1) 
   {
       cluster->SetNParticles(1);
       
       // 3 (last state) -> 0 (first state); add  particle of local id = 0.
       cluster->SetParticle(0, -1, mother_pid, part->GetPdgCode(), 0, 3);
       return; 
   }
   
   TVector3 StartPos = part->GetStartPos();
   Double_t dV = (StartPos-fPrimVertexPosition).Mag();
        
   //--------------------------------------------------------     
   // 2 particles: 
   // [3 -> 1 ] -> ... -> [ 1 -> 0 ]
        
   if (dV < fPrimVertexRegionR) 
   {
       cluster->SetNParticles(2);

       // 3 (last state) -> 1 (first state); add particle of local id = 1.
       cluster->SetParticle(1, 0, mother_pid, part->GetPdgCode(), 1, 3);
       
       mother_pid = part->GetVertexG1MotherId();
       part = (SpdMCParticle*)fParticles->At(mother_pid);  
  
       // 1 (last state) -> 0 (first state); add particle of local id = 0.
       cluster->SetParticle(0, -1, mother_pid, part->GetPdgCode(), 0, 1);   
       
       return; 
   }
   
   //--------------------------------------------------------
   
   Int_t id0  = mother_pid;       
   Int_t pdg0 = part->GetPdgCode(); 
   
   while (true) 
   {
       StartPos = part->GetStartPos();
       dV = (StartPos-fPrimVertexPosition).Mag();
       if (dV > fPrimVertexRegionR) {
           part = (SpdMCParticle*)fParticles->At(part->GetMotherId());
           continue;
       }
       break;
   }
   
   //--------------------------------------------------------
   // 2 particles: 
   // [ 3 -> 2 ] -> ... -> [ 2  -> 0 ]
        
   if (part->GetGeneration() == 1) 
   {
       cluster->SetNParticles(2);
    
       // 3 (last state) -> 2 (first state); add particle of local id = 1.
       cluster->SetParticle(1, 0, id0, pdg0, 2, 3);
       
       // 2 (last state) -> 0 (first state); add particle of local id = 0
       cluster->SetParticle(0, -1, part->GetId(), part->GetPdgCode(), 0, 2); 
       
       return;
   }
   
   //--------------------------------------------------------
   // 3 particles:
   // [ 3 -> 2 ] -> ... -> [ 2  -> 1 ] -> ... -> [ 1 -> 0 ] 
   
   cluster->SetNParticles(3);
    
   // -> 3 (last state) -> 2 (first state); add particle of local id = 2
   cluster->SetParticle(2, 1, id0, pdg0, 2, 3);
   
   // -> 2 (last state) -> 1 (first state); add  particle of local id = 1
   cluster->SetParticle(1, 0, part->GetId(), part->GetPdgCode(), 1, 2); 
   
   mother_pid = part->GetVertexG1MotherId();
   part = (SpdMCParticle*)fParticles->At(mother_pid);  
       
   // -> 1 (last state) -> 0 (first state); add particle of local id = 0
   cluster->SetParticle(0, -1, mother_pid, part->GetPdgCode(), 0, 1); 
}

//________________________________________________________________________________________  
void SpdRsMCClusterMaker::MakeRsParticles()
{
    // mc_particle_id -> array of [cluster_id, loc_part_id]
    std::map< Int_t, std::vector<RsMCPartInfo> > parts;
    std::map< Int_t, std::vector<RsMCPartInfo> >::iterator it;
    
    // first_state -> < mc_particle_id -> mc_particle_mother_id>
    std::multimap< Int_t, std::pair<Int_t, Int_t> > parts2;      
    std::multimap< Int_t, std::pair<Int_t, Int_t> >::const_iterator it2; 
     
    Int_t ni = fClusters->GetEntriesFast();
    Int_t np, mc_id, mc_mid, state1;
    SpdRsMCCluster* cluster;
    
    for (Int_t i(0); i<ni; i++) // i = cluster_id 
    {
         cluster = (SpdRsMCCluster*)fClusters->At(i); 
         np = cluster->GetNParticles();
         
         for (Int_t j(0); j<np; j++) // j = loc_part_id
         {
              mc_id = cluster->GetParticleId(j);
              it = parts.find(mc_id);
              
              if (it != parts.end()) it->second.push_back(RsMCPartInfo(i,j));
              else 
              {
                  std::vector<RsMCPartInfo> v;
                  v.push_back(RsMCPartInfo(i,j));
                  parts[mc_id] = v;
                  
                  state1 = cluster->GetParticleFirstState(j);
                  
                  mc_mid = cluster->GetParticleLocalMotherId(j);
                  if (mc_mid > -1) mc_mid = cluster->GetParticleId(mc_mid);
                  
                  std::pair<Int_t,Int_t> pp(mc_id,mc_mid);
                  parts2.insert(std::pair< Int_t, std::pair<Int_t,Int_t> >(state1,pp));
              }
         }
    }
    
    // mc_particle_id -> [rs_particle_id, mother_rs_particle_id]
    std::map< Int_t, std::pair<Int_t, Int_t> > idx; 
    std::map< Int_t, std::pair<Int_t, Int_t> >::const_iterator itx;
    
    Int_t nn(0);
    for (it2 = parts2.begin(); it2 != parts2.end(); it2++) 
    {
         if (it2->first == 0) idx[it2->second.first] = std::pair<Int_t,Int_t>(nn++,-1);
         else {
             itx = idx.find(it2->second.second);
             if (itx != idx.end()) idx[it2->second.first] = std::pair<Int_t,Int_t>(nn++,itx->second.first);
             else { /* unxepected error ??? */ idx[it2->second.first] = std::pair<Int_t,Int_t>(nn++,-2); }
         }
    }
    
    SpdMCParticle*   mcpart;
    SpdRsMCParticle* rspart;
    Int_t ix, rs_pid;
    
    for (it2 = parts2.begin(); it2 != parts2.end(); it2++) 
    {
         rspart = AddRsParticle();
         mc_id = it2->second.first;
         
         itx = idx.find(mc_id);

         rspart->SetMotherId(itx->second.second);

         it = parts.find(mc_id);         
         cluster = (SpdRsMCCluster*)fClusters->At((it->second)[0].first); 
         ix = (it->second)[0].second;
                 
         rspart->SetParticleId(mc_id);
         rspart->SetParticlePdg(cluster->GetParticlePdg(ix));
         rspart->SetStates(cluster->GetParticleFirstState(ix),cluster->GetParticleLastState(ix));
         
         rspart->SetNClusters(it->second.size());
         
         rs_pid = rspart->GetId();
         
         nn = 0;
         for (auto xx: it->second) 
         {
              cluster = (SpdRsMCCluster*)fClusters->At(xx.first); 
              ix = xx.second;
              
              rspart->SetClusterInfo(nn,xx.first,ix);
              
              /* ! ATTENTION set rs-particle-id to cluster info ATTENTION */
              cluster->SetRsParticleId(ix,rs_pid);
                        
              nn++;
                            
              //cout << ix << " id = " << info->GetParticleId(ix)  
              //     << " pdg =  " << info->GetParticlePdg(ix) << endl;
              //info->Print("");
         }
         
         /* ! ATTENTION set rs-particle-id to mc-partice info ATTENTION */
         mcpart = (SpdMCParticle*)fParticles->At(mc_id);
         if (mcpart) mcpart->SetRsParticleId(rs_pid);
             
         //rspart->Print("");
    } 
}

//________________________________________________________________________________________     
void SpdRsMCClusterMaker::RemoveMCTruth()
{
   SpdRsMCHit* cell;
   for (Int_t i(0); i<fHits->GetEntriesFast(); i++) {
        cell = (SpdRsMCHit*)fHits->At(i);
        if (cell) cell->RemoveMCTruth();  
   }
}

//________________________________________________________________________________________  
SpdRsMCCluster* SpdRsMCClusterMaker::AddCluster()
{
   if (!fClusters) return 0;
   Int_t size = fClusters->GetEntriesFast();
   SpdRsMCCluster* cluster = new ((*fClusters)[size]) SpdRsMCCluster();
   cluster->SetId(size);
   return cluster;
}

//________________________________________________________________________________________
SpdRsMCParticle* SpdRsMCClusterMaker::AddRsParticle()
{
   if (!fRsParticles) return 0;
   Int_t size = fRsParticles->GetEntriesFast();
   SpdRsMCParticle* particle = new ((*fRsParticles)[size]) SpdRsMCParticle();
   particle->SetId(size);
   return particle;  
}

//________________________________________________________________________________________  
void SpdRsMCClusterMaker::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdRsMCClusterMaker::Finish> hits: " << fNTotalHits 
            << ", clusters: " << fNTotalClusters << ", particles: " << fNTotalParticles;
   }
}


