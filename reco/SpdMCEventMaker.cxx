// $Id$
// Author: artur   2020/10/04


//_____________________________________________________________________________
//
// SpdMCEventMaker
//_____________________________________________________________________________

#include <TParticle.h>
#include <TMCProcess.h>

#include "FairRunIdGenerator.h" 

#include "SpdRunAna.h"

#include "SpdMCEventMaker.h"
#include "SpdMCTrack.h"
#include "SpdMCEventHeader.h"
#include "SpdPrimGenData.h"
#include "SpdBaseParSet.h"
#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdVertexMC.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"

#include <map>
#include <set>
#include <iostream>

using std::cout;
using std::endl;

using std::pair;

ClassImp(SpdMCEventMaker)

//_____________________________________________________________________________
SpdMCEventMaker::SpdMCEventMaker():SpdTask("MC event maker"),
fRunId(0),fEventId(-1),
fEventHeader(0),fMCTracks(0),fEvent(0),fParticles(0),fVertices(0),
fSaveEvent(true),fSaveParticles(true),fSaveVertices(true),
fParticleMaxGeneration(0),fVertexMaxGeneration(100),
fVerboseLevel(1)
{
  SpdParticlesAndProcesses::AddParticlesToDatabasePDG();
}

//_____________________________________________________________________________
SpdMCEventMaker::~SpdMCEventMaker() 
{
   if (fEvent) delete fEvent;
   if (fParticles) delete fParticles;
   if (fVertices) delete fVertices;
   
   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
   //helper->SetEvent(0);
   //helper->SetParticles(0);
   //helper->SetVertices(0);
   //helper->SetIdTable(0);
}

//_____________________________________________________________________________
void SpdMCEventMaker::Clear(Option_t* option)
{
    //if (fVerboseLevel > 0) cout << "\n-I- <SpdMCEventMaker::Clear> ";
    
//     Int_t max_gen = 2;
//     Int_t n = 0;
//     SpdMCParticle* p;  
//     Int_t np = fParticles->GetEntriesFast();

//     Bool_t remove = false;
//     
//     for (Int_t i(0); i < np; i++) 
//     {
//          if (!remove) {
//              p = (SpdMCParticle*)fParticles->At(i);
//              if (p->GetGeneration() > max_gen) {
//                  n = i; 
//                  remove = true;
//              }
//          }
//          
//          if (remove) {
//              cout << "remove " << i << " " << p << endl;
//              fParticles->RemoveAt(i);
//          }
//     }
// 
//     fParticles->Expand(n);   
    
//     n = 0;
//     for (Int_t i(0); i < np; i++)  {
//          p = (SpdMCParticle*)fParticles->At(i);
//          if (p->GetGeneration() > max_gen) {
//              n = i;
//              break;
//          }
//     }
//     
// 
//     fParticles->ExpandCreate(n);  
    
}

//_____________________________________________________________________________
InitStatus SpdMCEventMaker::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdMCEventMaker::Init> RootManager not instantiated" << endl;
       return kFATAL;
   }
   
   // Genarate run ID
   
   FairRunIdGenerator genid;
   fRunId = genid.generateId();
    
   //========================== ADD PARAMETERS =============================/
   
   AddParameters();
      
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // get Event header
   
   fEventHeader = (SpdMCEventHeader*)ioman->GetObject("MCEventHeader.");
   
   if (!fEventHeader) {
       cout << "-F- <SpdMCEventMaker::Init>" << "No HEADER object " << endl;
       return kFATAL;
   }
   
   // get Tracks array
   
   fMCTracks = (TClonesArray*)ioman->GetObject("SpdMCTrack");
   
   if (!fMCTracks) {
       cout << "-F- <SpdMCEventMaker::Init> " << "No MCTrack array " << endl;
       return kFATAL;
   }
   
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fEvent = new SpdMCEvent();
   ioman->Register(SpdDataBranchName(kMCEvent), "SpdMCEvent", fEvent, fSaveEvent);
   
   fParticles = new TClonesArray("SpdMCParticle");
   ioman->Register(SpdDataBranchName(kMCParticles), "SpdMCParticle", fParticles, fSaveParticles);
   
   fVertices = new TClonesArray("SpdVertexMC");
   ioman->Register(SpdDataBranchName(kMCVertices), "SpdVertexMC", fVertices, fSaveVertices);
   
   if (fParticleMaxGeneration < 1) fParticleMaxGeneration = Int_t(1e9);
   
   //============== PASS DATA TO EVENTHELPER ================================/
  
   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
   helper->SetEvent(fEvent);
   helper->SetParticles(fParticles);
   helper->SetVertices(fVertices);
   
   helper->SetIdTable(&fIds);
   
   if (fVerboseLevel > -1) cout << "-I- <SpdMCEventMaker::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdMCEventMaker::Exec(Option_t* opt) 
{
   assert(fEventHeader);
   assert(fMCTracks);
   
   fEventId++; // starts from 0!
         
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdMCEventMaker::Exec> Process event ... " 
            << fEventId << " (run: " << fRunId << ")," << "  mother (event,run): "
            << fEventHeader->GetEventID() << ", "<< fEventHeader->GetRunID() ; //<< "\n\n";
       if (fVerboseLevel > 1) cout << "\n";      
   }
   
   if (!fEvent) {
       cout << "-F- <SpdMCEventMaker::Exec> No base event object " << endl;
       exit(1);
   }
  
   if (!fParticles) {
       cout << "-F- <SpdMCEventMaker::Exec> No otput array of particles " << endl;
       exit(1);
   }
   
   if (!fVertices) {
       cout << "-F- <SpdMCEventMaker::Exec> No otput array of vertices " << endl;
       exit(1);
   }
   
   fEvent->Clear();
   
   fParticles->Delete();
   fVertices->Delete();
   
   fIds.clear();
          
   // >>>>> Fill event <<<<< 
   
   fEvent->SetRunId(fRunId);
   fEvent->SetEventId(fEventId); 
    
   fEvent->SetMotherRunId(fEventHeader->GetRunID());
   fEvent->SetMotherEventId(fEventHeader->GetEventID()); 

   Int_t Nmc = fMCTracks->GetEntriesFast();
   fIds.reserve(Nmc);
   
   // add parimary particles and vertex
   AddPrimaries();

   if (fParticleMaxGeneration < 2) {
       
       CheckParticles();
   
       fEvent->SetNParticles(fParticles->GetEntriesFast());
       fEvent->SetNMCTracks(Nmc);
   
       fEvent->SetMaxParticleGeneration(1); 
       fEvent->SetMaxVertexGeneration(0);
   
       fEventHeader->Reset();
       
       if (fVerboseLevel > 2) {
           cout << "\n-I- <SpdMCEventMaker::Exec> Only vertex particles has been added."
                << "Particles: " << fParticles->GetEntriesFast() 
                << ", Vertices: " << fVertices->GetEntriesFast()
                << ";  NG(particles): " << fParticleMaxGeneration
                << ", NG(vertices): " << fVertexMaxGeneration
                << "\n\n"; 
       }
       
       return;
   }
   
   SpdMCTrack *mctrack;
   Int_t mid;
   Int_t n(0);
   
   // ++++++++++ Fill Particles +++++++++++
   
   std::multimap<Int_t,Int_t> gn_id; // <generation, particle_id>
   
   std::vector<Int_t> id_gn(Nmc,0);  // particle_id[generation] 
   id_gn.assign(fIds.size(),1);
   
   std::set<Int_t> xi;
   
   n = fEvent->GetNOutPParticles();
   
   for (Int_t i = n; i<Nmc; i++) 
   {
        mctrack = (SpdMCTrack*)fMCTracks->At(i);
        mid = mctrack->GetFirstMotherId(); 
        if (mid < n) {
            id_gn[i] = id_gn[mid]+1;
            gn_id.insert(pair<Int_t,Int_t>(id_gn[i],i));
            n++;
        }
        else xi.insert(i);
   }
   
   if (xi.size() > 0) {
       if (fVerboseLevel > -2) {
           cout << "-W- <SpdMCEventMaker::Exec> Reject " << xi.size() 
                << " particles with (mother id > particle id) " << endl;
       }
       assert(xi.size() == 0); // FIXME    
   }
   
   n = 0; 
   
   SpdMCParticle *mcpart, *mmcpart;
   
   fIds.resize(Nmc,-1);
   Int_t max_id = -1;
   
   for (auto i : gn_id) 
   {
        if ((fParticleMaxGeneration > 0) && (i.first > fParticleMaxGeneration)) break;
        
        //cout << ++n << " " << i.first << " -> " << i.second << " " << fIds.size() << endl;
        
        mctrack = (SpdMCTrack*)fMCTracks->At(i.second);
        mcpart = AddParticle();
        
        // ATTENTION add particle to id-table ATTENTION
        fIds[i.second] = mcpart->GetId();
        
        if (i.second > max_id) max_id = i.second;
        
        // set mc-track id 
        mcpart->SetMCTrackId(i.second);
        
        // set genertion 
        mcpart->SetGeneration(i.first);
        
        // set data from mc-track 
        FillParticle(mctrack,mcpart);
        
        // set mother id
        mid = mctrack->GetFirstMotherId();
        mcpart->SetMotherId(fIds[mid]);
        
        // set data from mother particle 
        mmcpart = (SpdMCParticle*)fParticles->At(mcpart->GetMotherId());
    
        mcpart->SetMCGeneratorId(mmcpart->GetMCGeneratorId());
        mcpart->SetVertexG1MotherId(mmcpart->GetVertexG1MotherId());
        mcpart->SetVertexG1MotherPdg(mmcpart->GetVertexG1MotherPdg());
        mcpart->SetSpecialMotherId(mmcpart->GetSpecialMotherId());
        mcpart->SetSpecialMotherSid(mmcpart->GetSpecialMotherSid());
   }

   fIds.resize(max_id+1);

   id_gn.clear();
   gn_id.clear();
      
   CheckParticles();
   
   // ++++++++++ Fill Verticies +++++++++++
   
   Int_t n1 = fEvent->GetNTotalPParticles(); // number of (G0+G1)-particles
   Int_t n2 = fParticles->GetEntriesFast();  // total number of particles
   
   // actual max. particle and vertex generation numbers
   Int_t GPmax = ((SpdMCParticle*)fParticles->At(n2-1))->GetGeneration(); 
   Int_t GVmax = (GPmax > fVertexMaxGeneration) ? fVertexMaxGeneration : GPmax-1;   

   if (GVmax > 0) 
   {
       SpdVertexMC* vertex;
       Int_t vid;

       for (Int_t i = n1; i<n2; i++) 
       {
            mcpart = (SpdMCParticle*)fParticles->At(i);
            
            if (mcpart->GetGeneration() > GVmax+1) break;
            
            //cout << "part: " << i << " id/mid: " << mcpart->GetId() << " " << mid << endl;
            
            mid = mcpart->GetMotherId();
            mmcpart = (SpdMCParticle*)fParticles->At(mid);
            
            vid = mmcpart->GetLastVertexId();
            
            if (vid < 0) {
                // add new vertex
                vertex = AddVertex();
                vertex->SetGeneration(mmcpart->GetGeneration());
                vertex->AddIn(mid);
                vid = vertex->GetId();
                mmcpart->SetLastVertexId(vid);
            }
            else {
                // get existed vertex
                vertex = (SpdVertexMC*)fVertices->At(vid);   
            }
            
            // add particle to the vertex
            vertex->AddOut(i);
            
            // vertex_spatial_size
            SetVertexRegionSize(vertex);
       
            // vertex_process_id & vertex_position 
            SetVertexProcessAndPosition(vertex);
       
            // set particle first_vertex_id
            mcpart->SetFirstVertexId(vid);
       }
   }
   
   // +++++++++++++++++++++++++++++++++++++
   
   fEvent->SetNParticles(fParticles->GetEntriesFast());
   fEvent->SetNMCTracks(Nmc);
   
   fEvent->SetMaxParticleGeneration(GPmax); 
   fEvent->SetMaxVertexGeneration(GVmax);
   
   fEventHeader->Reset();
   
   // print
  
   if (fVerboseLevel > 1) 
   {
       fEvent->PrintEvent();
       SpdMCEventHelper::Instance()->PrintParticles();
       SpdMCEventHelper::Instance()->PrintVertices();
   
       //((SpdVertexMC*)fVertices->At(0))->Print();
       //((SpdMCParticle*)fParticles->At(0))->Print();
  
       cout << "\n-I- <SpdMCEventMaker::Exec> Particles: " << fParticles->GetEntriesFast() 
            << ", Vertices: " << fVertices->GetEntriesFast() 
            << "; NG(particles): " << GPmax
            << ", NG(vertices): " << GVmax
            << ", ID's(size): " << fIds.size()
            << "\n"; 
   }
}

//________________________________________________________________________________________  
Bool_t SpdMCEventMaker::AddPrimaries() 
{    
   if (!fEvent) return false;
   if (!fEventHeader) return false;
   if (!fMCTracks) return false;
    
   TObjArray* prim = fEventHeader->GetPrimGenDataList();
   
   if (!prim) return false;
   
   Int_t ngens = prim->GetEntriesFast();
   if (ngens < 1) return false;

   SpdPrimGenData* gendata;
   
   gendata = (SpdPrimGenData*)prim->At(0);
  
   if (ngens < 2) return false;
   
   Int_t np0(0), np(0), mid;
   
   TClonesArray*   pp;
   TParticle*      part;
   SpdMCTrack*     mctrack;
   SpdMCParticle*  mcpart;
   
   //Bool_t pythia8;         // *Pythia8 correction !!! 
   Int_t nn[3] = {0,0,0};  // n particles: total, incoming, outgoing
   
   fIds.resize(fEventHeader->GetNPrim());
   
   SpdVertexMC* vprim = AddVertex();
   vprim->SetGeneration(0);
   
   for (Int_t i(1); i < ngens; i++) 
   {
        gendata = (SpdPrimGenData*)prim->At(i);
        
        //pythia8 = (gendata->GetGenType().Contains("Pythia8")) ? true : false; // * Pythia8 correction !!! 
        
        pp = gendata->GetParticles();
        if (!pp) continue;
        np = pp->GetEntriesFast();
        if (np < 1) continue;
    
        for (Int_t j(0); j < np; j++) 
        {
             part = (TParticle*)pp->At(j);
             mcpart = AddParticle();
             
             // --- set mc-generator id ---
             mcpart->SetMCGeneratorId(part->GetFirstDaughter());  // = gendata->GetGenType()
             
             // --- set mc-track id ---
             mcpart->SetMCTrackId(part->GetSecondMother());
            
             // --- set particle mother id --- 
             mid = part->GetFirstMother();
             
             //if (pythia8 && mid == 0) mid = -1;  // * Pythia8 correction !!!
                 
             if (mid < 0) nn[1]++;
             else mid += np0;
            
             mcpart->SetMotherId(mid);
            
             // --- set particle generation ---  
             if (part->GetSecondMother() < 0) {
                 mcpart->SetGeneration(0);
                 if (mid < 0) vprim->AddIn(mcpart->GetId());
                 else vprim->AddInner(mcpart->GetId());
             }
             else {
                 mcpart->SetGeneration(1);
                 vprim->AddOut(mcpart->GetId());
                 nn[2]++;
                 // ATTENTION add particle to id-table ATTENTION
                 fIds[part->GetSecondMother()] = nn[0];
             }
             
             // --- set particle pdg code ---  
             mcpart->SetPdgCode(part->GetPdgCode());
             if (mcpart->GetMCTrackId() > -1) {
                 mctrack = (SpdMCTrack*)fMCTracks->At(mcpart->GetMCTrackId());
                 mcpart->SetPdgCode(mctrack->GetPdgCode());
             }
             
             // --- set particle vertex process ---  
             if (mcpart->GetGeneration() < 1) mcpart->SetVProcess(kPNoProcess);
             else mcpart->SetVProcess(kPPrimary);
             
             // --- set start time ---
             mcpart->SetStartTime(part->T());
             
             // --- set start momentum ---
             mcpart->SetStartMomentum(part->Px(),part->Py(),part->Pz());
 
             // --- set first_vertex_id ---
             if (mcpart->GetMotherId() > -1) mcpart->SetFirstVertexId(0);
             else mcpart->SetFirstVertexId(-1);
             
             // --- set (start_position &last_vertex_id) for particles of generation = 0 ! --- 
             if (mcpart->GetGeneration() < 1) {
                 mcpart->SetStartPosition(part->Vx() + fEventHeader->GetX(),
                                          part->Vy() + fEventHeader->GetY(),
                                          part->Vz() + fEventHeader->GetZ());
                 mcpart->SetLastVertexId(0);
             }
             
             // --- set G1-mother (id & pdg) for particles of generation = 1 ! --- 
             if (mcpart->GetGeneration() > 0) {
                 mcpart->SetVertexG1Mother(mcpart->GetId(),mcpart->GetPdgCode());
             }
        
             nn[0]++;
        }
        
        np0 += np;
   }
   
   // set (start_position & module (detector)) for particles of generation = 1
   for (Int_t i(0); i<nn[2]; i++) {
        mctrack = (SpdMCTrack*)fMCTracks->At(i);
        mcpart = (SpdMCParticle*)fParticles->At(fIds[i]);
        mcpart->SetStartPosition(mctrack->GetStartX(),mctrack->GetStartY(),mctrack->GetStartZ());
        mcpart->SetVModule(mctrack->GetStartModule());
   }
   
   // vertex_position
   vprim->SetPosition(fEventHeader->GetX(),fEventHeader->GetY(),fEventHeader->GetZ());
   
   // vertex_spatial_size
   SetVertexRegionSize(vprim);
   
   // vertex_process_id
   SetVertexProcess(vprim);
   
   //cout << "Particles (total/incoming/outgoint): " << nn[0] << "  " << nn[1] << "  " << nn[2] << endl;
   //cout << fIds.capacity() << " " << fIds.size() << endl;
   
   // set (SpecialMotherId & SpecialMotherSid)
   SpdMCParticle* prt;
   Int_t pdg, probe, mpdg22;

   for (Int_t i(0); i < fParticles->GetEntriesFast(); i++) {
       
        mcpart = (SpdMCParticle*)fParticles->At(i);
        prt = mcpart;      
       
        while (true) 
        {
           mid = prt->GetMotherId(); 
           pdg = prt->GetPdgCode();
    
           if (pdg != 22) {
               // check for charmonium, open charm or charmed baryon
               probe = SpdParticlesAndProcesses::VerifyPhysProbe(pdg);
           }
           else {
               // check for promt photon  
               if (mid == -1) break;
               mpdg22 = ((SpdMCParticle*)fParticles->At(mid))->GetPdgCode();
               if (SpdParticlesAndProcesses::IsQuarkOrGluon(mpdg22)) probe = kSpdPromtPhoton;
               else probe = kSpdUnknown;
           }
    
           if (probe != 0) {
               mcpart->SetSpecialMotherId(prt->GetId());
               mcpart->SetSpecialMotherSid(probe);
               //cout << "-I- <SpdMCEventMaker::AddPrimaries> id: " << i << " pdg: " << pdg 
               //     << " probe: " << probe << " probe id: " << prt->GetId() << endl;
           }
           
           if (mid == -1) break;
           prt = (SpdMCParticle*)fParticles->At(mid);
        }
   }
   
   // fill event
   fEvent->SetNTotalPParticles(nn[0]);
   fEvent->SetNInPParticles(nn[1]);
   fEvent->SetNOutPParticles(nn[2]);
}

//________________________________________________________________________________________  
void SpdMCEventMaker::FillParticle(SpdMCTrack* ip, SpdMCParticle* op) 
{    
    if (!ip || !op) return;
    
    op->SetPdgCode(ip->GetPdgCode());     
    op->SetVProcess(ip->GetStartProcess());
    op->SetVModule(ip->GetStartModule());
    op->SetStartTime(ip->GetStartT());    
    op->SetStartPosition(ip->GetStartVertex()); 
    op->SetStartMomentum(ip->GetMomentum());
}

//________________________________________________________________________________________  
SpdMCParticle* SpdMCEventMaker::AddParticle() 
{
   if (!fParticles) return 0;
   Int_t size = fParticles->GetEntriesFast();
   SpdMCParticle* p = new ((*fParticles)[size]) SpdMCParticle();
   p->SetId(size);
   return p;
}

//________________________________________________________________________________________  
SpdVertexMC* SpdMCEventMaker::AddVertex() 
{
   if (!fVertices) return 0 ;
   Int_t size = fVertices->GetEntriesFast();
   SpdVertexMC* v = new ((*fVertices)[size]) SpdVertexMC();
   v->SetId(size);
   return v;
}

//________________________________________________________________________________________
void SpdMCEventMaker::SetVertexRegionSize(SpdVertexMC* v)
{
   if (!v) return;
   if (!fParticles) return;
   
   const Int_t n = v->GetNInOut()+v->GetNInner();
   const SpdMCParticle* idx[n];
   
   Int_t k(0);
   for (Int_t i(1); i<3; i++) {
        for (const int& j : v->GetParticles(i)) {
             idx[k++] = (SpdMCParticle*)fParticles->At(j);
        }
   }
 
   if (k == 1) {
       v->SetRegionSize(0.);
       return;
   }
   
   TVector3 x1, x2;
   Double_t mag, d(0.);
   for (Int_t i(0); i<k-1; i++) {
        x1 = idx[i]->GetStartPos();
        for (Int_t j = i+1; j<k; j++) {
             x2 = idx[j]->GetStartPos();
             mag = (x2-x1).Mag(); 
             if (mag > d) d = mag;
        } 
   }
   
   v->SetRegionSize(d);
    
   //cout << "region size = " << d << endl;
}

//________________________________________________________________________________________ 
void SpdMCEventMaker::SetVertexProcess(SpdVertexMC* v)
{
   if (!v) return;
   if (!fParticles) return;
   
   const std::set<int>& parts = v->GetOutParticles();
   if (parts.size() < 1)  return;

   SpdMCParticle* part = (SpdMCParticle*)fParticles->At(*parts.begin());
   
   Int_t proc_id = part->GetVProcess(), cproc_id;
   
   std::multimap<Double_t,SpdMCParticle*> tpid; // <time,process_id> 
   Bool_t is_singleproctype = true; 
   
   for (const Int_t& i : parts) {
        part = (SpdMCParticle*)fParticles->At(i);
        tpid.insert(pair<Double_t,SpdMCParticle*>(part->GetStartTime(),part));
        cproc_id = part->GetVProcess();
        if (cproc_id != proc_id) is_singleproctype = false;   
        //cout << "-I- <SpdMCEventMaker::VertexProcess> " << i  
        //     << " proccess: " << proc_id << " " << cproc_id << endl;
   }
   
   part = (--tpid.end())->second;
   
   v->SetProcessId(part->GetVProcess());
   v->SetSingleProcessType(is_singleproctype);
   v->SetDetectorId(part->GetVModule());
}

//________________________________________________________________________________________ 
void SpdMCEventMaker::SetVertexProcessAndPosition(SpdVertexMC* v)
{
   if (!v) return;
   if (!fParticles) return;
   
   const std::set<int>& parts = v->GetOutParticles();
   if (parts.size() < 1)  return;

   SpdMCParticle* part = (SpdMCParticle*)fParticles->At(*parts.begin());
   
   Int_t proc_id = part->GetVProcess(), cproc_id;
   
   std::multimap<Double_t,SpdMCParticle*> tpid; // <time,particle> 
   Bool_t is_singleproctype = true; 
   
   for (const Int_t& i : parts) {
        part = (SpdMCParticle*)fParticles->At(i);
        tpid.insert(pair<Double_t,SpdMCParticle*>(part->GetStartTime(),part));
        cproc_id = part->GetVProcess();
        if (cproc_id != proc_id) is_singleproctype = false;   
        //cout << "-I- <SpdMCEventMaker::VertexProcess> " << i  
        //     << " proccess: " << proc_id << " " << cproc_id << endl;
   }
   
   part = (--tpid.end())->second;
  
   v->SetPosition(part->GetStartPos());
   v->SetProcessId(part->GetVProcess());
   v->SetSingleProcessType(is_singleproctype);
   v->SetDetectorId(part->GetVModule());
}

//________________________________________________________________________________________  
void SpdMCEventMaker::CheckParticles()
{
    Int_t nprim = fEvent->GetNTotalPParticles();
    
    SpdMCParticle *part, *pp;
    Int_t ni;
    std::set<Int_t> np;
    
    for (Int_t i(0); i<nprim; i++) {
         part = (SpdMCParticle*)fParticles->At(i);
         if (part->GetGeneration() > 0) {
             pp = part;
             while (true) {
                ni = pp->GetMotherId();
                if (ni < 0) break;
                np.insert(ni);
                pp = (SpdMCParticle*)fParticles->At(ni);
             }
         }
    }
    
    for (Int_t i(0); i<nprim; i++) {
         part = (SpdMCParticle*)fParticles->At(i);
         if (part->GetGeneration() < 1) {
             if (np.find(i) == np.end()) 
                 ((SpdMCParticle*)fParticles->At(i))->SetChar('0');
         }
    }
}

//________________________________________________________________________________________
void SpdMCEventMaker::AddParameters()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   
   SpdSetParSet* params = run->GetParSetAssembly();
   
   FairParGenericSet* pars;
   
   pars = run->GetParameters("SpdFieldPar");
   if (pars) params->SetFieldPars(pars);
   
   pars = run->GetParameters("PrimGenParSet");
   if (pars) params->SetPrimGenPars(pars);
   
   pars = run->GetParameters("PassiveGeoParSet");
   if (pars) params->SetPassiveGeoPars(pars);
}

//________________________________________________________________________________________  
void SpdMCEventMaker::Finish()
{
   if (fVerboseLevel > -1) cout << "\n\n-I- <SpdMCEventMaker::Finish>"; //<< endl;
}




