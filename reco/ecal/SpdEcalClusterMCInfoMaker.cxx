// $Id$
// Author: artur   2021/05/31

//_____________________________________________________________________________
//
// SpdEcalClusterMCInfoMaker
//_____________________________________________________________________________

#include "SpdEcalClusterMCInfoMaker.h"

#include "SpdSetParSet.h"

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"
#include "SpdParticlesAndProcesses.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"

#include "SpdEcalMCHit.h"
#include "SpdEcalRCCluster.h"
#include "SpdEcalClusterMCInfo.h"
#include "SpdEcalMCParticle.h"

#include "SpdMCEventHelper.h"

#include <map>
#include <iostream>

using std::cout;
using std::endl;


ClassImp(SpdEcalClusterMCInfoMaker)

//_____________________________________________________________________________
SpdEcalClusterMCInfoMaker::SpdEcalClusterMCInfoMaker():
fEvent(0),fParticles(0),fHits(0),fClusters(0),
fInfo(0),fEcalParticles(0),
fSaveInfo(true),fSaveEcalParticles(true),
fCreateEcalParticles(true),fRemoveHitMCTruth(false),
fPrimVertexRegionR(1.0),
fNTotalClusters(0),fNTotalInfos(0),fNTotalParticles(0),
fVerboseLevel(1)
{
    
}

//_____________________________________________________________________________
SpdEcalClusterMCInfoMaker::~SpdEcalClusterMCInfoMaker() 
{
   if (fInfo) { fInfo->Delete(); delete fInfo; }
}

//_____________________________________________________________________________
InitStatus SpdEcalClusterMCInfoMaker::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdEcalClusterMCInfoMaker::Init> RootManager not instantiated " << endl;
       return kFATAL;
   }
   
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // get Event 
   
   fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   if (!fEvent) return kFATAL;
   
   // get Particles array
   
   fParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCParticles));
   if (!fParticles) return kFATAL;
   
   // get Ecal hits
   
   fHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCEcalHits));
   if (!fHits) return kFATAL;
   
   // get Ecal clusters
   
   fClusters = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCEcalClusters));
   if (!fClusters) return kFATAL;
   
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fInfo = new TClonesArray("SpdEcalClusterMCInfo");
   ioman->Register(SpdDataBranchName(kMCEcalClustersInfo),"SpdEcalClusterMCInfo",fInfo,fSaveInfo);  
   
   if (fCreateEcalParticles) {
       fEcalParticles = new TClonesArray("SpdEcalMCParticle");
       ioman->Register(SpdDataBranchName(kMCEcalParticles),"SpdEcalMCParticle",fEcalParticles,fSaveEcalParticles);
   }
   
   //============== PASS DATA TO EVENTHELPER ================================/   

   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
  
   if (!helper->GetEvent())          helper->SetEvent(fEvent);
   if (!helper->GetParticles())      helper->SetParticles(fParticles);
   if (!helper->GetEcalHits())       helper->SetEcalHits(fHits);
   if (!helper->GetEcalClustersRC()) helper->SetEcalClustersRC(fClusters);
   
   helper->SetEcalClustersMCInfo(fInfo);
   if (fEcalParticles) helper->SetEcalParticlesMC(fEcalParticles);
 
   if (fVerboseLevel > -1) {
       cout << "-I- <SpdEcalClusterMCInfoMaker::Init> Intialization successfull " << endl;
   }
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdEcalClusterMCInfoMaker::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fHits);
   assert(fClusters);
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdEcalClusterMCInfoMaker::Exec> Process event ... " 
            << fEvent->GetEventId(); 
       if (fVerboseLevel > 1) cout << "\n";         
   }

   fInfo->Delete();
   if (fEcalParticles) fEcalParticles->Delete();
   
   if (fParticles->GetEntriesFast() < 1) {
       cout << "\n-E- <SpdEcalClusterMCInfoMaker::Exec> Empty input data array (particles) " << endl; 
       return;
   }
    
   SpdMCParticle* part = (SpdMCParticle*)fParticles->At(0);
   fPrimVertexPosition = part->GetStartPos();
   
   Int_t NClusters(0);
   SpdEcalRCCluster* cluster;
   for (Int_t i(0); i<fClusters->GetEntriesFast(); i++) {
        cluster = (SpdEcalRCCluster*)fClusters->At(i);
        if (AcceptCluster(cluster)) {
            NClusters++;
            MakeClusterInfo(cluster);
        }
   }
  
   if (fEcalParticles && NClusters > 0) MakeEcalParticles();
   
   fNTotalClusters += NClusters;
   fNTotalInfos += fInfo->GetEntriesFast();
   
   if (fEcalParticles) fNTotalParticles += fEcalParticles->GetEntriesFast();
   
   if (fVerboseLevel > 1) 
   {
        cout << "\n-I- <SpdEcalClusterMCInfoMaker::Exec> Print ... " << endl; 
        cout << "Clusters (accepted):     " << NClusters << endl;
        cout << "Number of infos:         " << fInfo->GetEntriesFast() << endl; 
        cout << "Number of particles:     " 
             << ((fEcalParticles) ? fEcalParticles->GetEntriesFast() : 0) << endl;
   }
   
   if (fRemoveHitMCTruth) RemoveMCTruth();
}
    
//________________________________________________________________________________________     
void SpdEcalClusterMCInfoMaker::MakeClusterInfo(SpdEcalRCCluster* cl) 
{
   const std::vector<Int_t>& cells = cl->GetCells();
   
   if (fVerboseLevel > 3) {
       cout << "\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> cluster: " << cl->GetId() << "\n\n";
   }
      
   std::map<Int_t,Double_t> particles;   // particle_id -> edep
   std::map<Int_t,Double_t>::iterator it; 
   
   Double_t noise_edep(0);
   Int_t nnoise = 0;
 
   SpdEcalMCHit* cell;
   SpdEcalHitMCTruth* cell_truth;
   Int_t n, pid;
   Double_t edep;
   Double_t rcell, zcell;
  
   SpdMCParticle* part;
   Int_t vmodule;
   
   for (auto id : cells) 
   {
        cell = (SpdEcalMCHit*)fHits->At(id);
        if (!cell) continue;
        
        cell_truth = cell->GetMCTruth();
        if (!cell_truth) continue;
        
        n = cell_truth->GetN();
        if (n < 1) continue;
        
        rcell = cell->GetPositionR();
        zcell = abs(cell->GetPositionZ());
     
        for (Int_t i(0); i<n; i++) 
        {
             cell_truth->GetDataFast(i,pid,edep);
             //cout << " i = " << i << " pid = " << pid << " edep = " << edep << endl; 
             
             part = (SpdMCParticle*)fParticles->At(pid);
             
             while (true) 
             {
                /* mother = (SpdMCParticle*)fParticles->At(part->GetMotherId());
                
                cout << " id: " << part->GetId() << " pdg: " << part->GetPdgCode()
                     << " g: "  << part->GetGeneration() << " mod: " << part->GetVModule()
                     << " | " 
                     << " id: " << mother->GetId() << " pdg: " << mother->GetPdgCode() 
                     << " g: "  << mother->GetGeneration() << " mod: " << mother->GetVModule()
                     << " >>>  cell(r): " << rcell << " pos(r): " << part->GetStartR() 
                     << endl; */
                    
                vmodule = part->GetVModule();
                
                if (vmodule == kSpdEcalTB || vmodule == kSpdEcalTEC) {
                    part = (SpdMCParticle*)fParticles->At(part->GetMotherId());
                    continue;
                }
                
                //cout << "part: " << part->GetId() << " Rcell = " << rcell << " Rstart = " 
                //     << part->GetStartR() << " " << part->GetVModule() 
                //     << " Zcell = " << zcell << " Zstart = " << abs(part->GetStartZ())
                //     << " g = " << part->GetGeneration() << endl;
                     
                if (vmodule == kSpdRsTB || vmodule == kSpdRsTEC) {     
                    noise_edep += edep;
                    nnoise++;
                    break;
                }
                
                if (rcell < part->GetStartR()) {
                    noise_edep += edep;
                    nnoise++;
                    break;
                }
                
                // ???
                if (zcell < abs(part->GetStartZ())) {
                    noise_edep += edep;
                    nnoise++;
                    break;
                } 
                
                it = particles.find(part->GetId());
                if (it != particles.end()) it->second += edep;
                else particles.insert(std::pair<Int_t,Double_t>(part->GetId(),edep));
                
                //it = particles.find(part->GetId());
                //cout << "+++  id: " << part->GetId() << " g: "  << part->GetGeneration() 
                //            << " mod: " << part->GetVModule() << " edep = " << it->second << endl;
                 
                break;
             }
             //cout << "----- particles: " << particles.size() << " noise = " << noise_edep << endl;
        }
        //cout << endl; cell->Print();
   }
   
   //cout << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 2- " 
   //     << " cells = " << cells.size() << " particles = " << particles.size() 
   //     << " noise_edep = " << noise_edep << " n(noise) = " << nnoise << endl;
   
   SpdEcalClusterMCInfo* info = AddClusterInfo(cl->GetId());
   
   info->SetNoiseEdep(noise_edep);
       
   // ATTENTION set info_ID for corresponding cluster ATTENTION 
   cl->SetInfoId(info->GetId());
   
   if (particles.empty()) 
   {
       if (fVerboseLevel > 2) {
           cout << "\n-I- <SpdEcalClusterMCInfoMaker::MakeClusterInfo> "
                << "No particles in the cluster (noise only), add empty cluster. " << endl; 
       }
       if (fVerboseLevel > 3) {
           cl->Print();
           info->Print();
       }
       return;
   }
        
   std::map<Int_t,part_info*> cl_parts;   // particle_id -> part_info
   std::map<Int_t,part_info*>::iterator cit; 
   
   Int_t i[3] = {0,0,0};
   
   TVector3 StartPos;
   Double_t dV;
   Int_t mid, mid2;
   part_info *pm, *pm1, *pm2;
   
   for (auto xid : particles) 
   {
        part = (SpdMCParticle*)fParticles->At(xid.first);   
        
        part_info* pi = new part_info();         
        cl_parts[xid.first] = pi;
        
        pi->last_state = 3;
        pi->edep = xid.second; 
        
        if (fVerboseLevel > 3) {
            cout << "\n >>>>>>>>>>>>>>> xid = " << xid.first << " module: " << part->GetVModule() << endl;
        }
        
        //------------------------------------------------
        
        if (part->GetGeneration() == 1) 
        {
            if (fVerboseLevel > 3) cout << " [3 -> 0] " << endl;
            
            /*-----------------------*/
            /*       [3 -> 0]        */
            /*-----------------------*/
            
            pi->first_state = 0;
            pi->id = i[0]++; //-> first state
            
            pi->mother = 0; 
            
            continue; 
        }
        
        //------------------------------------------------
        
        StartPos = part->GetStartPos();
        dV = (StartPos-fPrimVertexPosition).Mag();
        
        //------------------------------------------------
        
        if (dV < fPrimVertexRegionR) 
        {
            if (fVerboseLevel > 3) cout << " [3 -> 1] ... [1 -> 0] " << endl;
                
            /*-----------------------*/
            /* [3 -> 1] ... [1 -> 0] */
            /*-----------------------*/
            
            // 3 (last state) -> 1 (first state)
            
            pi->first_state = 1;
            pi->id = i[1]++; //-> first state
            
            // 1 (last state) -> 0 (first state) 
            
            mid = part->GetVertexG1MotherId();
            cit = cl_parts.find(mid);
            
            if (cit == cl_parts.end()) 
            {
                pm = new part_info();
                cl_parts[mid] = pm;
                
                pm->last_state  = 1;
                pm->first_state = 0;
                pm->id = i[0]++; //-> first state
            }
            else pm = cit->second;
            
            pi->mother = pm;
            
            continue;
        }
       
        //------------------------------------------------
        
        part = (SpdMCParticle*)fParticles->At(part->GetMotherId());
        
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
    
        mid = part->GetId();
        cit = cl_parts.find(mid);
        
        //------------------------------------------------
        
        if (part->GetGeneration() == 1) 
        {
            if (fVerboseLevel > 3) cout << " [3 -> 2] ... [2 -> 0];  mid(2->0) = " << mid << endl;       
 
            /*-----------------------*/
            /* [3 -> 2] ... [2 -> 0] */
            /*-----------------------*/
            
            // 3 (last state) -> 2 (first state)
            
            pi->first_state = 2;
            pi->id = i[2]++;
        
            // 2 (last state) -> 0 (first state)
            
            if (cit == cl_parts.end())
            {
                pm = new part_info(); 
                cl_parts[mid] = pm;
                
                pm->last_state  = 2;
                pm->first_state = 0;
                pm->id = i[0]++; //-> first state
            }
            else pm = cit->second;
            
            pi->mother = pm;
            
            continue;
        }
        
        if (fVerboseLevel > 3) cout << "  [3 -> 2] ... [2 -> 1] ... [1 -> 0];  mid(2->1) = " << mid << endl;   
 
        /*------------------------------------*/
        /* [3 -> 2] ... [2 -> 1] ... [1 -> 0] */
        /*------------------------------------*/
        
        // 3 (last state) -> 2 (first state)
            
        pi->first_state = 2;
        pi->id = i[2]++; //-> first state
        
        if (cit == cl_parts.end())
        {            
            // 2 (last state) -> 1 (first state) 
            
            pm1 = new part_info(); 
            cl_parts[mid] = pm1;
            
            pm1->last_state  = 2;
            pm1->first_state = 1;
            pm1->id = i[1]++; //-> first state
            
            pi->mother = pm1;
            
            //  1 (last state) -> 0 (first state) 
            
            mid2 = part->GetVertexG1MotherId();
            cit = cl_parts.find(mid2);
     
            if (cit == cl_parts.end()) 
            {
                pm2 = new part_info();
                cl_parts[mid2] = pm2;
                
                pm2->last_state  = 1;
                pm2->first_state = 0;
                pm2->id = i[0]++; //-> first state
            }
            else pm2 = cit->second;

            pm1->mother = pm2;
        }
        else 
        {
            // 2 (last state) -> 1 (first state) 
            
            pm1 = cit->second;
            pi->mother = pm1;
          
            //  1 (last state) -> 0 (first state) 
            
            mid2 = part->GetVertexG1MotherId();
            cit = cl_parts.find(mid2);
            
            if (cit == cl_parts.end()) 
            {
                pm2 = new part_info();                
                cl_parts[mid2] = pm2;
                
                pm2->last_state  = 1;
                pm2->first_state = 0;
                pm2->id = i[0]++; //-> first state
            }
            else pm2 = cit->second;
            
            pm1->mother = pm2;
        }
   }
   
   if (fVerboseLevel > 3) cout << "\n >>>>>>>>>>>>>>>>>>>> particles: " << cl_parts.size() << endl;
      
   // >>>>>> sum energy deposit >>>>>>
   
   Double_t ed;
   part_info* pp;
   for (auto xid : cl_parts) 
   {   
       pp = xid.second;
       if (pp->last_state == 3 && pp->first_state != 0) 
       {
           ed = pp->edep;
           pp = pp->mother;
           while (pp) {
              pp->edep += ed;
              pp = pp->mother;
           }
       }
   }
   
   // >>>>>> add particles >>>>>>
   
   info->SetNParticles(i[0],i[1],i[2]);
   
   auto idx = [i](part_info* p) -> Int_t
   {
      if (!p) return -1; 
           if (p->first_state == 0) return p->id;
      else if (p->first_state == 1) return i[0]+p->id;
      else if (p->first_state == 2) return i[0]+i[1]+p->id;   
      return -1;    
   };
   
   Int_t loc_id, loc_mid;
   Int_t nn(0);
   for (auto xid : cl_parts) 
   {
        loc_id = idx(xid.second);
        loc_mid = idx(xid.second->mother);
        
        part = (SpdMCParticle*)fParticles->At(xid.first);
        info->SetParticle(loc_id, loc_mid, 
                          xid.first, part->GetPdgCode() , 
                          xid.second->first_state, xid.second->last_state,
                          xid.second->edep);
        nn++;
//         printf("%3d %8d    %3d %3d   %3d %3d %3d   %12.3f\n",
//                nn, xid.first,
//                xid.second->first_state, xid.second->last_state,
//                xid.second->id, loc_id, loc_mid, 
//                xid.second->edep);
   }
   
   for (auto p : cl_parts) delete p.second;

   if (fVerboseLevel > 3) {
       cl->Print();
       info->Print();
   }
   
//    if (/*(fEvent->GetEventId() == 3) &&*/ (cl->GetId() == 37) ) {
//        cl->Print();
//        info->Print();
//    }
}

//________________________________________________________________________________________
void SpdEcalClusterMCInfoMaker::MakeEcalParticles()
{
    // mc_particle_id -> array of [info_id, loc_part_id]
    std::map< Int_t, std::vector<EcalMCPartInfo> > parts;
    std::map< Int_t, std::vector<EcalMCPartInfo> >::iterator it;
    
    // first_state -> < mc_particle_id -> mc_particle_mother_id>
    std::multimap< Int_t, std::pair<Int_t, Int_t> > parts2;      
    std::multimap< Int_t, std::pair<Int_t, Int_t> >::const_iterator it2; 
     
    Int_t ni = fInfo->GetEntriesFast();
    Int_t np, mc_id, mc_mid, state1;
    SpdEcalClusterMCInfo* info;
    
    for (Int_t i(0); i<ni; i++) // i = info_id 
    {
         info = (SpdEcalClusterMCInfo*)fInfo->At(i); 
         
         //cout << i << " " << "cluster id: " << info->GetClusterId() << endl;
         
         np = info->GetNParticles();
         
         for (Int_t j(0); j<np; j++) // j = loc_part_id
         {
              mc_id = info->GetParticleId(j);
              it = parts.find(mc_id);
              
              if (it != parts.end()) it->second.push_back(EcalMCPartInfo(i,j));
              else 
              {
                  std::vector<EcalMCPartInfo> v;
                  v.push_back(EcalMCPartInfo(i,j));
                  parts[mc_id] = v;
                  
                  state1 = info->GetParticleFirstState(j);
                  
                  mc_mid = info->GetParticleLocalMotherId(j);
                  if (mc_mid > -1) mc_mid = info->GetParticleId(mc_mid);
                  
                  std::pair<Int_t,Int_t> pp(mc_id,mc_mid);
                  parts2.insert(std::pair< Int_t, std::pair<Int_t,Int_t> >(state1,pp));
              }
         }
    }
    
    // mc_particle_id -> [ecal_particle_id, mother_ecal_particle_id]
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

    SpdMCParticle *mcpart;
    SpdEcalMCParticle* epart;
    Int_t ix, epid;
    
    for (it2 = parts2.begin(); it2 != parts2.end(); it2++) 
    {
         epart = AddEcalParticle();
         mc_id = it2->second.first;
         
         itx = idx.find(mc_id);

         epart->SetMotherId(itx->second.second);
         
         it = parts.find(mc_id);         
         info = (SpdEcalClusterMCInfo*)fInfo->At((it->second)[0].first); 
         ix = (it->second)[0].second;
                  
         epart->SetParticleId(mc_id);
         epart->SetParticlePdg(info->GetParticlePdg(ix));
         epart->SetStates(info->GetParticleFirstState(ix),info->GetParticleLastState(ix));
         
         epart->SetNClusters(it->second.size());
         
         epid = epart->GetId();
    
         nn = 0;
         for (auto xx: it->second) 
         {
              info = (SpdEcalClusterMCInfo*)fInfo->At(xx.first); 
              
              ix = xx.second;
              
              epart->AddEdep(info->GetParticleEdep(ix));
              epart->SetClusterInfo(nn,xx.first,ix);
              
              /* ! ATTENTION set ecal-particle-id to ecal-partice info ATTENTION */
              info->SetEcalParticleId(ix,epid);
                        
              nn++;
                            
              //cout << ix << " edep = " << info->GetParticleEdep(ix)*1e3 << " id = " 
              //    << info->GetParticleId(ix)  << " pdg =  " << info->GetParticlePdg(ix) << endl;
              //info->Print("");
         }
         
         /* ! ATTENTION set ecal-particle-id to mc-partice info ATTENTION */
         mcpart = (SpdMCParticle*)fParticles->At(mc_id);
         if (mcpart) mcpart->SetEcalParticleId(epid);
             
         //epart->Print("");
    }
}

//________________________________________________________________________________________  
Bool_t SpdEcalClusterMCInfoMaker::AcceptCluster(const SpdEcalRCCluster* cl) const
{
   if (!cl || cl->GetNCells() < 1) return false; 
   return true;
}

//________________________________________________________________________________________     
SpdEcalClusterMCInfo* SpdEcalClusterMCInfoMaker::AddClusterInfo(Int_t clusteId)
{
   if (!fInfo) return 0;
   Int_t size = fInfo->GetEntriesFast();
   SpdEcalClusterMCInfo* info = new ((*fInfo)[size]) SpdEcalClusterMCInfo();
   info->SetId(size);
   info->SetClusterId(clusteId);
   return info; 
}

//________________________________________________________________________________________
SpdEcalMCParticle* SpdEcalClusterMCInfoMaker::AddEcalParticle()
{
   if (!fEcalParticles) return 0;
   Int_t size = fEcalParticles->GetEntriesFast();
   SpdEcalMCParticle* particle = new ((*fEcalParticles)[size]) SpdEcalMCParticle();
   particle->SetId(size);
   return particle;  
}

//________________________________________________________________________________________     
void SpdEcalClusterMCInfoMaker::RemoveMCTruth()
{
   SpdEcalMCHit* cell;
   for (Int_t i(0); i<fHits->GetEntriesFast(); i++) {
        cell = (SpdEcalMCHit*)fHits->At(i);
        if (cell) cell->RemoveMCTruth();  
   }
}

//________________________________________________________________________________________  
void SpdEcalClusterMCInfoMaker::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdEcalClusterMCInfoMaker::Finish> "
            << "clusters (accepted): " << fNTotalClusters << ", infos: " << fNTotalInfos
            << ", particles: " << fNTotalParticles;// << endl;
   }
}



