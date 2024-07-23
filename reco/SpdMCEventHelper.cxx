// $Id$
// Author: artur   2020/10/06


//_____________________________________________________________________________
//
// SpdMCEventHelper
//_____________________________________________________________________________

#include <TDatabasePDG.h>
#include <TMCProcess.h>
#include <TMath.h>

#include "SpdMCEventHelper.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdVertexMC.h"

#include "SpdMCTrackHit.h"
#include "SpdTofMCHit.h"
#include "SpdEcalMCHit.h"
#include "SpdRsMCHit.h"

#include "SpdBbcMCHit.h"
#include "SpdAegMCHit.h"
#include "SpdZdcMCHit.h"

#include "SpdTrackMC.h"

#include "SpdTrackRCSeed.h"
#include "SpdTrackRC.h"
#include "SpdVertexRC.h"

#include "SpdEcalRCCluster.h"
#include "SpdEcalRCParticle.h"
#include "SpdEcalClusterMCInfo.h"
#include "SpdEcalMCParticle.h"

#include "SpdRsMCCluster.h"
#include "SpdRsMCParticle.h"

#include "SpdTsParticle.h"
#include "SpdTofParticle.h"
#include "SpdAegParticle.h"

#include "SpdTrackFitPar.h"
#include "SpdVertexFitPar.h"

#include <iostream>

using std::cout;
using std::endl;

SpdMCEventHelper* SpdMCEventHelper::fInstance = 0;

ClassImp(SpdMCEventHelper)

//_____________________________________________________________________________
SpdMCEventHelper::SpdMCEventHelper():
fParameters(0),
fEvent(0),fParticles(0),fVertices(0),
fIdTable(0),
fItsHits(0),fTsHits(0),fTofHits(0),fEcalHits(0),fRsHits(0),
fBbcHits(0),fAegHits(0),fZdcHits(0),
fTracks(0),fVerticesRC(0),
fTracksRC(0),fTrackSeedsRC(0),
fEcalClustersRC(0),fEcalParticlesRC(0),
fEcalClustersMCInfo(0),fEcalParticlesMC(0),
fRsClustersMC(0),fRsParticlesMC(0),
fTsParticles(0),fTofParticles(0), fAegParticles(0)
{
    if (fInstance) { 
        cout << "-F- <SpdMCEventHelper::SpdMCEventHelper> Fatal error. " 
             << "Singleton object has already been created." << endl;
        exit;
    }
    
    fInstance = this;
}

//_____________________________________________________________________________
SpdMCEventHelper::~SpdMCEventHelper() 
{
    fInstance = 0;
}

//_____________________________________________________________________________
Int_t SpdMCEventHelper::SearchForParticleId(Int_t mc_particle_id) const 
{ 
    if (mc_particle_id < 0 || !fEvent || !fParticles) return -1;
    SpdMCParticle* part = (SpdMCParticle*)fParticles->At(mc_particle_id);
    if (!part || !fEvent->CheckParticleGeneration(part->GetGeneration())) return -1;
    return mc_particle_id;
}

//_____________________________________________________________________________
Int_t SpdMCEventHelper::SearchForVertexId(Int_t mc_vertex_id) const 
{  
    if (mc_vertex_id < 0 || !fEvent || !fVertices) return -1;
    SpdVertexMC* vtx = (SpdVertexMC*)fVertices->At(mc_vertex_id);
    if (!vtx || !fEvent->CheckVertexGeneration(vtx->GetGeneration())) return -1;
    return mc_vertex_id;
}

//_____________________________________________________________________________
SpdMCParticle* SpdMCEventHelper::SearchForParticle(Int_t mc_particle_id) const 
{ 
    if (mc_particle_id < 0 || !fEvent || !fParticles) return 0;
    SpdMCParticle* part = (SpdMCParticle*)fParticles->At(mc_particle_id);
    if (!part || !fEvent->CheckParticleGeneration(part->GetGeneration())) return 0;
    return part;
}

//_____________________________________________________________________________
SpdVertexMC* SpdMCEventHelper::SearchForVertex(Int_t mc_vertex_id) const 
{  
    if (mc_vertex_id < 0 || !fEvent || !fVertices) return 0;
    SpdVertexMC* vtx = (SpdVertexMC*)fVertices->At(mc_vertex_id);
    if (!vtx || !fEvent->CheckVertexGeneration(vtx->GetGeneration())) return 0;
    return vtx;
}

//________________________________________________________________________________________
Int_t SpdMCEventHelper::SearchForMCVertex(Int_t rs_vertex_id) const
{
   if (rs_vertex_id < 0 || !fVerticesRC || !fTracks) return -2;
   
   SpdVertexRC* v = (SpdVertexRC*)fVerticesRC->At(rs_vertex_id);
   if (!v) return -2;
   
   Bool_t is_mc(true), start(true);
   Int_t vid;
   SpdTrackMC* t_mc;
   
   for (auto t_id : v->GetTracks()) 
   {
        t_mc = (SpdTrackMC*)fTracks->At(t_id);
        if (!t_mc) return -2;

        if (start) {
            vid = t_mc->GetVertexId();
            start = false;
        }
        else {
            if (vid != t_mc->GetVertexId()) {
                is_mc = false;
                vid = -1;
                break;
            }
        }
    }
    return vid;
}

//________________________________________________________________________________________
Int_t SpdMCEventHelper::SearchForMCVertexStrictly(Int_t rs_vertex_id) const
{
    if (rs_vertex_id < 0 || !fEvent || !fParticles || !fVertices || !fVerticesRC || !fTracks) return -2;
   
    SpdVertexRC* v = (SpdVertexRC*)fVerticesRC->At(rs_vertex_id);
    if (!v) return -2;
   
    Bool_t is_mc(true), start(true);
    Int_t vid;
    SpdTrackMC* t_mc;
   
    for (auto t_id : v->GetTracks()) 
    {
         t_mc = (SpdTrackMC*)fTracks->At(t_id);
         if (!t_mc) return -2;

         if (start) {
             vid = t_mc->GetVertexId();
             start = false;
             Int_t nv = GetNMCParticleDaughters(t_mc->GetParticleId());
             if (nv != v->GetTracks().size()) return -1;
         }
         else {
             if (vid != t_mc->GetVertexId()) {
                 is_mc = false;
                 vid = -1;
                 break;
             }
         }
    }
    return vid; 
}

//________________________________________________________________________________________ 
Int_t SpdMCEventHelper::GetNMCParticleDaughters(Int_t mc_particle_id) const
{
    if (!fEvent || !fParticles || !fVertices) return -1;
    SpdMCParticle* part = SearchForParticle(mc_particle_id);
    if (!part) return 0;
    SpdVertexMC* vtx = SearchForVertex(part->GetLastVertexId());
    return (vtx) ? vtx->GetNOut() : 0;
}

//________________________________________________________________________________________ 
Bool_t SpdMCEventHelper::GetTrackMChits(Int_t track_id, std::vector<SpdMCTrackHit*>& hits) const
{
    if (!hits.empty()) hits.clear();
    
    if (!fTracks) return false;
    if (track_id < 0 || track_id >= fTracks->GetEntriesFast()) return false;
    
    SpdTrackMC* track = dynamic_cast<SpdTrackMC*>(fTracks->At(track_id));
    if (!track) return false;
    
    Int_t np = track->GetNPoints();
    if (np == 0) return true;
    
    const std::vector<spd_mctrack_point_>& points = track->GetTrackPoints();
    
    hits.assign(np,0);
    
    SpdMCTrackHit* hit;
    
    Bool_t ok = true;
    Int_t n = -1;
    for (auto p: points) {
         n++;
         if (p.second == kMCItsHits) {
             if (!fItsHits) { ok = false; continue; }
             else {
                 hit = (SpdMCTrackHit*)fItsHits->At(p.first);
                 if (!hit) { ok = false; continue; }
                 hits[n] = hit;
             }
         }
         else if (p.second == kMCTsHits) {
             if (!fTsHits) { ok = false; continue; }
             else {
                 hit = (SpdMCTrackHit*)fTsHits->At(p.first); 
                 if (!hit) { ok = false; continue; }
                 hits[n] = hit;
             }
         }
         else {
             ok = false; 
             continue;
         }
    }
    
    return ok;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                                    Print methods
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//________________________________________________________________________________________  
void SpdMCEventHelper::PrintParticles(Int_t maxg) const
{
    cout << "-I- <SpdMCEventHelper::PrintParticles> " << "\n";
    
    if (maxg == 0) return;
    
    if (!fParticles) return;
    
    Int_t np = fParticles->GetEntriesFast();
    if (np < 1) return;
    
    printf("\n//-----------------------------------------------------------------------------------------------");
    printf("\n// MC:[GEN]  :   MC-Generator id                                                                 ");
    printf("\n// MC:[TID]  :   MC-Track id                                                                     ");
    printf("\n// C         :   direct particle response in (Its+Ts) system (yes: +, no: -)                     ");
    printf("\n// G         :   particle generation (0,1,...)                                                   ");
    printf("\n// ID        :   particle id                                                                     ");
    printf("\n// NAME      :   particle name                                                                   ");
    printf("\n// PDG       :   particle pdg number                                                             ");
    printf("\n// MOTHER    :   real mother particle id                                                         ");
    printf("\n// PROC      :   process id                                                                      ");
    printf("\n// DET       :   detector (supermodule) id                                                       ");
    printf("\n// M[G1]     :   mother particle of generation = 1 id                                            ");
    printf("\n// PDG[G1]   :   mother particle of generation = 1 pdg                                           ");
    printf("\n// M[S]      :   special mother particle id                                                      ");
    printf("\n// ID[S]     :   special process id                                                              ");
    printf("\n// P         :   particle momentum, GeV                                                          ");
    printf("\n// Th        :   particle momentum polar (theta) angle, deg                                      ");
    printf("\n// D0        :   distance from particle production vertex to (0,0,0) point, cm                   ");
    printf("\n// t0        :   particle production time, s                                                     ");
    printf("\n// FV        :   origin vertex id = -99 (unknown), -1 (primary incoming particle), 0,1, ...      ");
    printf("\n// LV        :   last vertex id = -99 (unknown), 0,1, ...                                        ");
    printf("\n// TRID      :   associated track id = -1 (no track), 0,1, ...                                   ");
    printf("\n// TID       :   associated TS particle id = -1 (no data), 0,1, ...                              ");
    printf("\n// FID       :   associated TOF particle id = -1 (no data), 0,1, ...                             ");
    printf("\n// EID       :   associated ECAL particle id = -1 (no particle), 0,1, ...                        ");
    printf("\n// RID       :   associated RS particle id = -1 (no particle), 0,1, ...                          ");
    printf("\n//-----------------------------------------------------------------------------------------------");
    
    char header[255];
   
    sprintf(header,"\n%13s %2s %3s %6s %14s %10s %6s %4s %3s %6s %10s %6s %6s %10s %9s %10s %10s  %4s %4s %4s %4s %4s %4s %4s\n\n",
            "MC: [GEN/TID]", "C", "G", "ID", "NAME", "PDG", "MOTHER", "PROC","DET",
            "M[G1]","PDG[G1]","M[S]","ID[S]",
            "P [GeV]","Th [deg]","D0 [cm]","t0 [ns]",
            "FV","LV","TRID","TID","FID","EID","RID"
          );
    
    printf("%s",header);
    
    SpdMCParticle* p;         
    const SpdMCParticleInfo* pinfo;
    
    Int_t gen, cgen(1), g, cg(1);
    Bool_t prim(true), start_sec(false);
    
    for (Int_t i(0); i < np; i++) {
        
         p = (SpdMCParticle*)fParticles->At(i);
     
         gen = p->GetMCGeneratorId();
         g = p->GetGeneration();
         
         if (maxg > 0 && g > maxg) break;
        
         if (prim) { 
             if (g > 1) { 
                 prim = false; 
                 printf("\n __________________ Secondary particles __________________  \n\n");
             }
         }
         
         if (g > 1) {
             if (cg != g) {
                 printf("%s",header);
                 cg = g;
             }
         }
         
         if (prim && gen != cgen) { cgen = gen; printf("\n"); } 
    
         printf("  [%3d/%5d]  %c %3d %6d %14s %10d %6d %4d %3d %6d %10d %6d %6d %10.3e %9.2f %10.3e %10.3e ",
                 gen,p->GetMCTrackId(),p->GetChar(), 
                 g, p->GetId(), p->GetParticleName().Data(), 
                 p->GetPdgCode(), p->GetMotherId(), p->GetVProcess(),p->GetVModule(),
                 p->GetVertexG1MotherId(),p->GetVertexG1MotherPdg(),
                 p->GetSpecialMotherId(),p->GetSpecialMotherSid(),
                 p->GetStartP(),p->GetStartPtheta(),p->GetStartD(),p->GetStartTime()
               );
         
         if (!p->GetInfo()) { printf("\n"); continue; }
         
         printf(" %4d %4d %4d %4d %4d %4d %4d\n", 
                p->GetFirstVertexId(),p->GetLastVertexId(),p->GetTrackId(),
                p->GetTsParticleId(),p->GetTofParticleId(),p->GetEcalParticleId(),p->GetRsParticleId()
               ); 
    }
    printf("\n");
    
    //const std::vector<Int_t>& Ids = *fIdTable;
    //for (Int_t i(0); i < Ids.size(); i++) cout << i << " " << Ids[i] << endl;
    //printf("\n");
}

//________________________________________________________________________________________  
void SpdMCEventHelper::PrintVertices(Int_t maxg, TString option) const
{
    cout << "-I- <SpdMCEventHelper::PrintVertices> " << "\n";
    
    if (maxg == 0) return;
    
    if (!fVertices) return;
    
    Int_t nv = fVertices->GetEntriesFast();
    if (nv < 1) return;
    
    printf("\n%5s %3s %35s %5s %5s %12s %12s %12s %12s %5s %5s %7s %7s %7s %16s \n",
           "ID","G","Process","S","DET","Size [cm]","  X [cm]","  Y [cm]","  Z [cm]",
           "FIT","NT",
           "N(in)","N(out)","N(inn)",
           "(id/pdg)in[0]");
    
    SpdVertexMC* v;
    SpdMCParticle* part;
    Int_t part_id;
    Int_t g, cg(-1);
    
    std::vector<SpdVertexMC*> vtxs;
    
    for (Int_t i(0); i<nv; i++) {
        
         v = (SpdVertexMC*)fVertices->At(i);
      
         g = v->GetGeneration();
         
         if (maxg > 0 && g > maxg) break;
         
         if (cg != g) { printf("\n"); cg = g; }
         
         printf("%5d %3d %35s %5s %5d %12.3e %12.3e %12.3e %12.3e %5s %5d %7d %7d %7d ",
                v->GetId(),g,v->GetProcessName(),(v->IsSingleProcessType() ? "yes" : "no"), 
                v->GetDetectorId(),v->GetRegionSize(),v->GetX(),v->GetY(),v->GetZ(),
                (v->GetFitPars() ? "yes" : "no"),v->GetNTracks(),
                v->GetNIn(),v->GetNOut(),v->GetNInner()
               );
         
         if (v->GetFitPars()) vtxs.push_back(v);
         
         if (!fParticles) continue;
         
         if (i != 0 && v->GetNIn() > 0) {
             part_id = v->GetInParticle(0);
             part = (SpdMCParticle*)fParticles->At(part_id);
             printf("%16s",Form("%d/%-d",part_id,part->GetPdgCode()));
         }
         else printf("%16s","-");

         printf("\n");
         
    }
    printf("\n");
    
    if (option == "nofitpars") return; 
        
    if (vtxs.size() == 0) return;
    if (!(fVertices && fParticles && fTracks)) return;
    
    Int_t nt;
    
    SpdMCParticle* part_mc;
  
    //--------------------------------------------
    v = (SpdVertexMC*)fVertices->At(0);
        
    if (v && v->IsPrimary()) {
            
        printf("\n >>>>>>>> Primary vertex (fit parameters): \n");
        printf("\n%5s %5s %10s %10s %10s %7s    %10s\n",
                "ID","FIT","X [cm]","Y [cm]","Z [cm]","NT(f)","TRACKS(ID)");
   
        nt = 15;
   
        TVector3 vpos;
        Int_t ntracks = -1;
            
        SpdPrimVertexFitPar* pars = dynamic_cast<SpdPrimVertexFitPar*>(v->GetFitPars());
        if (pars) {
            vpos = pars->GetVertex();
            ntracks = pars->GetNTracks();
        }
            
        if (nt > v->GetNTracks()) nt = v->GetNTracks();
        
        printf("\n%5d %5s %10f %10f %10f %7d    ",
               v->GetId(),(pars ? "yes" : "no"),
               vpos.X(),vpos.Y(),vpos.Z(),ntracks);
        Int_t j(0), tid;
        for (auto pid : v->GetOutParticles()) {
             part_mc = 0;
             part_mc = (SpdMCParticle*)fParticles->At(pid);
             if (!part_mc) continue;
             tid = part_mc->GetTrackId();
             if (tid > -1) { printf("(%d) ",tid); j++; }
             if (j == nt) break;
        }
        if (v->GetNTracks() > nt) printf(" ...");  
    
        vpos = v->GetPos();
        printf("\n mc-vertex: %10f %10f %10f\n",vpos.X(),vpos.Y(),vpos.Z()); 
    }
    
    //--------------------------------------------
  
    SpdVertexFitPar* pars;
    Double_t mass_mc, mass_rc; 
    TString name_mc("unknown");
    
    printf("\n >>>>>>>> Secondary vertices (fit parameters): \n");
    printf("\n%5s %5s %10s %10s %10s %7s    %10s\n","VID","PID","PART",
           "MASS(mc)","MASS(rc)","NT(f)","TRACKS(ID)");

    nt = 10;   
    
    for (auto vtx : vtxs) 
    {
         if (vtx->IsPrimary()) continue;
             
         mass_mc = -1;
         mass_rc = -1;
         name_mc =  "unknown";
         part_id = -1;
         
         if (nt > vtx->GetNTracks()) nt = vtx->GetNTracks();
         
         if (vtx->IsSingleInParticle()) {
             part_mc = 0;
             part_mc = (SpdMCParticle*)fParticles->At(vtx->GetInParticle());
             if (part_mc) {
                 name_mc = part_mc->GetParticleName();
                 mass_mc = part_mc->GetMass();
                 part_id = part_mc->GetId();
             }
         }
         
         pars = dynamic_cast<SpdVertexFitPar*>(vtx->GetFitPars());
         if (pars) mass_rc = pars->GetMass();
         
         printf("\n%5d %5d %10s %10f %10f %7d    ",
                vtx->GetId(),part_id,
                name_mc.Data(),mass_mc,mass_rc,vtx->GetNTracks());
         
         Int_t j(0), tid;
         for (auto pid : vtx->GetOutParticles()) {
              part_mc = 0;
              part_mc = (SpdMCParticle*)fParticles->At(pid);
              if (!part_mc) continue;
              tid = part_mc->GetTrackId();
              if (tid > -1) { printf("(%d) ",tid); j++; }
              if (j == nt) break;
         }
         if (vtx->GetNTracks() > nt) printf(" ...");  
    }
    
    if (!vtxs.empty()) printf("\n");  
        
    printf("\n");  
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintTracks(TString option) const
{
    cout << "-I- <SpdMCEventHelper::PrintTracks> " << "\n";

    if (!fTracks) return;
    
    Int_t nt = fTracks->GetEntriesFast();
    if (nt < 1) return;
    
    printf("\n%5s %8s %10s %14s %8s  %5s  %8s %8s %8s %8s  %8s \n\n",
           "ID","M(ID)","M(PDG)","NAME","V(ID)","FIT",
           "NH","N(Its)","N(TsB)","N(TsEC)","NP");
    
    Int_t pdg;
    SpdTrackMC* track;
    Int_t nn[5] = {0,0,0,0,0};
    for (Int_t i(0); i<nt; i++) {
         track = dynamic_cast<SpdTrackMC*>(fTracks->At(i));
         pdg = track->GetParticlePdg();
         printf("%5d %8d %10d %14s %8d  %5s  %8d %8d %8d %8d  %8d \n",
                track->GetId(),track->GetParticleId(),pdg,ParticleName(pdg).Data(),track->GetVertexId(),
                (track->GetFitPars() ? "yes" : "no"),
                track->GetNHits(),track->GetNHitsIts(),track->GetNHitsTsB(),track->GetNHitsTsEC(),
                track->GetNPoints());
         nn[0] += track->GetNHits();
         nn[1] += track->GetNHitsIts();
         nn[2] += track->GetNHitsTsB();
         nn[3] += track->GetNHitsTsEC();
         nn[4] += track->GetNPoints();
    }
    printf("\n%56s  %8d %8d %8d %8d  %8d \n","",nn[0],nn[1],nn[2],nn[3],nn[4]);
    printf("\n");
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintTracksRC() const
{
    cout << "-I- <SpdMCEventHelper::PrintTracksRC> " << "\n";

    if (!fTracksRC) return;
    
    Int_t nt = fTracksRC->GetEntriesFast();
    if (nt < 1) return;
    
    printf("\n%5s %8s %10s %14s %8s  %5s  %8s %8s %8s %8s  %8s \n\n",
           "ID","M(ID)","M(PDG)","NAME","V(ID)","FIT",
           "NH","N(Its)","N(TsB)","N(TsEC)","NP");

    Int_t pdg;
    SpdTrackRC* track;
    Int_t nn[5] = {0,0,0,0,0};
    for (Int_t i(0); i<nt; i++) {
         track = dynamic_cast<SpdTrackRC*>(fTracksRC->At(i));
         pdg = track->GetParticlePdg();
         printf("%5d %8d %10d %14s %8d  %5s  %8d %8d %8d %8d  %8d \n",
            track->GetId(),track->GetParticleId(),pdg,ParticleName(pdg).Data(),track->GetVertexId(),
           (track->GetFitPars() ? "yes" : "no"),
            track->GetNHits(),track->GetNHitsIts(),track->GetNHitsTsB(),track->GetNHitsTsEC(),
            track->GetNPoints());
         nn[0] += track->GetNHits();
         nn[1] += track->GetNHitsIts();
         nn[2] += track->GetNHitsTsB();
         nn[3] += track->GetNHitsTsEC();
         nn[4] += track->GetNPoints();
    }
    printf("\n%56s  %8d %8d %8d %8d  %8d \n","",nn[0],nn[1],nn[2],nn[3],nn[4]);
    printf("\n");
}

//________________________________________________________________________________________  
void SpdMCEventHelper::PrintVerticesRC() const
{
    cout << "-I- <SpdMCEventHelper::PrintVerticesRC> " << endl;
    
    if (!fVerticesRC) return;
    
    Int_t nv = fVerticesRC->GetEntriesFast();
    if (nv < 1) return;
    
    Int_t nt;
    SpdVertexRC* v;
    std::set<int>::const_iterator it;
    
    Bool_t vnp = false;
    if (fVertices && fParticles && fTracks) vnp = true;
    
    if (vnp) 
    {
        //--------------------------------------------
        nt = 10;
        
        SpdVertexMC* v_mc;
        SpdMCParticle* part_mc;
        
        v = (SpdVertexRC*)fVerticesRC->At(0);
        
        if (v->IsPrimary()) {
            
            printf("\n >>>>>>>> Primary vertex (fit parameters): \n");
            printf("\n%5s %5s %10s %10s %10s %7s    %10s\n",
                   "ID","FIT","X [cm]","Y [cm]","Z [cm]","NT(f)","TRACKS(ID)");
   
            TVector3 vpos;
            Int_t ntracks = -1;
            
            SpdPrimVertexFitPar* pars = dynamic_cast<SpdPrimVertexFitPar*>(v->GetFitPars());
            if (pars) {
                vpos = pars->GetVertex();
                ntracks = pars->GetNTracks();
            }
            
            if (nt > v->GetNTracks()) nt = v->GetNTracks();
            it = v->GetTracks().begin();
            
            printf("\n%5d %5s %10f %10f %10f %7d:   ",
                   v->GetId(),(pars ? "yes" : "no"),
                   vpos.X(),vpos.Y(),vpos.Z(),ntracks);
            for (Int_t j(0); j<nt; j++) printf("(%d) ",*it++);
            if (v->GetNTracks() > nt) printf(" ...");  
            printf("\n"); 
    
            v_mc = (SpdVertexMC*)fVertices->At(0);
            if (v_mc && v_mc->IsPrimary()) {
                vpos = v_mc->GetPos();
                printf(" mc-vertex: %10f %10f %10f\n",vpos.X(),vpos.Y(),vpos.Z()); 
            }
        }
        
        //--------------------------------------------
        nt = 10;
        
        SpdVertexFitPar*  pars;
        SpdVertexFitPar2* pars2;
        
        printf("\n >>>>>>>> Secondary vertices (fit parameters): \n");
        printf("\n%5s %5s %4s %7s %10s %10s %10s %7s    %10s\n","ID","FIT","TYPE","VID(mc)","PART(mc)",
               "MASS(mc)","MASS(rc)","NT(f)","TRACKS(ID)");
        
        Double_t mass_mc, mass_rc; 
        TString name_mc("unknown");
        Int_t vid;
        
        for (Int_t i(0); i<nv; i++) {
            
             v = (SpdVertexRC*)fVerticesRC->At(i);
             
             if (v->IsPrimary()) continue;
                  
             if (nt > v->GetNTracks()) nt = v->GetNTracks();
             it = v->GetTracks().begin();
             
             mass_mc = -1;
             mass_rc = -1;
             name_mc =  "unknown";
             v_mc = 0;
             part_mc = 0;
             pars = 0;
             vid = -1;
             
             //vid = SearchForMCVertex(i); // ATTENTION
             vid = SearchForMCVertexStrictly(i); // ATTENTION
             
             if (vid > -1) v_mc = (SpdVertexMC*)fVertices->At(vid);
             
             if (v_mc && v_mc->IsSingleInParticle()) {
                 part_mc = (SpdMCParticle*)fParticles->At(v_mc->GetInParticle());
                 if (part_mc) {
                     name_mc = part_mc->GetParticleName();
                     mass_mc = part_mc->GetMass();
                 }
             }
             
             pars = dynamic_cast<SpdVertexFitPar*>(v->GetFitPars());
             if (pars) mass_rc = pars->GetMass();
             else {
                 pars2 = dynamic_cast<SpdVertexFitPar2*>(v->GetFitPars());
                 if (pars2) mass_rc = pars2->GetMass();
             }
             
             printf("\n%5d %5s %4d %7d %10s %10f %10f %7d:   ",
                    v->GetId(),((pars || pars2) ? "yes" : "no"),v->GetVertexType(),
                    vid,name_mc.Data(),mass_mc,mass_rc,v->GetNTracks());
             for (Int_t j(0); j<nt; j++) printf("(%d) ",*it++);
             if (v->GetNTracks() > nt) printf(" ...");  
        }
        printf("\n");
    }
    else {
         printf("\n Secondary vertices (fit parameters): \n");
         printf("\n%5s %5s %7s    %10s\n","ID","FIT","NT(f)","TRACKS(ID)");
        
         for (Int_t i(0); i<nv; i++) {
        
             v = (SpdVertexRC*)fVerticesRC->At(i);
             
             if (v->IsPrimary()) continue;
             
             if (nt > v->GetNTracks()) nt = v->GetNTracks();
             it = v->GetTracks().begin();
        
             printf("\n%5d %5s %7d:   ",
                    v->GetId(),(v->GetFitPars() ? "yes" : "no"),v->GetNTracks());
             for (Int_t j(0); j<nt; j++) printf("(%d) ",*it++);
             if (v->GetNTracks() > nt) printf(" ...");
         }
         printf("\n");
    }   
        
    printf("\n");  
}

//________________________________________________________________________________________  
void SpdMCEventHelper::PrintEcalClustersMCInfo() const
{
    cout << "-I- <SpdMCEventHelper::PrintEcalClustersMCInfo> " << endl;
    
    if (!fEcalClustersMCInfo) return;
    
    Int_t ni = fEcalClustersMCInfo->GetEntriesFast();
    if (ni < 1) return;
    
    SpdEcalClusterMCInfo* info;

    printf("\n%8s %8s %14s %14s %5s %5s %8s %14s %14s %5s %5s %6s\n\n",
            "id","cl_id","Signal, MeV","Noise, MeV",
            "pid","mpid","mc_id","pdg","edep, MeV","s(1)","s(2)","epid");
    
    Int_t    cl_id, np;
    Double_t edep, noise;
    
    for (Int_t i(0); i<ni; i++)
    {
         info = dynamic_cast<SpdEcalClusterMCInfo*>(fEcalClustersMCInfo->At(i));
         
         cl_id = info->GetClusterId();
         edep  = info->GetClusterEdep()*1e3;
         noise = info->GetNoiseEdep()*1e3;
         np    = info->GetNParticles();
         
         printf("%8d %8d %14.6f %14.6f ",info->GetId(),cl_id,edep,noise);
         
         for (Int_t j(0); j<np; j++) {
              printf("%5d %5d %8d %14d %14.6f %5d %5d %6d",
                     j,info->GetParticleLocalMotherId(j),info->GetParticleId(j),
                     info->GetParticlePdg(j),info->GetParticleEdep(j)*1e3,
                     info->GetParticleFirstState(j),info->GetParticleLastState(j),
                     info->GetEcalParticleId(j)
              );
              if (j < np-1) printf("\n%48s","");
         }
         printf("\n");
    }
    printf("\n");
}
    
//________________________________________________________________________________________      
void SpdMCEventHelper::PrintEcalParticlesMC() const
{
    cout << "-I- <SpdMCEventHelper::PrintEcalParticlesMC> " << endl;
    
    if (!fEcalParticlesMC) return;
    
    Int_t ni = fEcalParticlesMC->GetEntriesFast();
    if (ni < 1) return;
    
    printf("\n%8s %8s %8s %14s %14s %5s %5s %8s %8s\n\n",
            "id","mid","mc_id","pdg","edep, MeV",
            "s(1)","s(2)","info_id","info_pid");
        
    SpdEcalMCParticle* p;
    
    Int_t  ncl;
    
    for (Int_t i(0); i<ni; i++)
    {
         p = dynamic_cast<SpdEcalMCParticle*>(fEcalParticlesMC->At(i));
        
         ncl = p->GetNClusters();

         printf("%8d %8d %8d %14d %14.6f %5d %5d ",
                p->GetId(),p->GetMotherId(),p->GetParticleId(),p->GetParticlePdg(),
                p->GetEdep()*1e3,p->GetFirstState(),p->GetLastState());
                
         for (Int_t j(0); j<ncl; j++) {
              printf("%8d %8d",p->GetClusterInfoId(j),p->GetClusterInfoParicleLocId(j));
              if (j < ncl-1) printf("\n%69s","");
         }
         printf("\n");
    }
    printf("\n");
}

//________________________________________________________________________________________  
void SpdMCEventHelper::PrintRsClustersMC() const
{
    cout << "-I- <SpdMCEventHelper::PrintRsClustersMC> " << endl;
    
    if (!fRsClustersMC) return;
    
    Int_t ni = fRsClustersMC->GetEntriesFast();
    if (ni < 1) return;
    
    SpdRsMCCluster* cl;

    printf("\n%6s %6s %6s %6s %6s %8s %14s %5s %5s %6s\n\n",
           "id","det_id","hits","pid","mpid","mc_id","pdg","s(1)","s(2)","rspid");
    
    Int_t np;
    
    for (Int_t i(0); i<ni; i++)
    {
         cl = dynamic_cast<SpdRsMCCluster*>(fRsClustersMC->At(i));
          
         np  = cl->GetNParticles();
                
         printf("%6d %6d %6d ",cl->GetId(),cl->GetDetId(),cl->GetNHits());
         
         for (Int_t j(0); j<np; j++) {
              printf("%6d %6d %8d %14d %5d %5d %6d",
                     j,cl->GetParticleLocalMotherId(j),cl->GetParticleId(j),
                     cl->GetParticlePdg(j),cl->GetParticleFirstState(j),cl->GetParticleLastState(j),
                     cl->GetRsParticleId(j)
              );
              if (j < np-1) printf("\n%21s","");
         }
         printf("\n");
    }
    printf("\n");
}
    
//________________________________________________________________________________________      
void SpdMCEventHelper::PrintRsParticlesMC() const
{
    cout << "-I- <SpdMCEventHelper::PrintRsParticlesMC> " << endl;
  
    if (!fRsParticlesMC) return;
    
    Int_t ni = fRsParticlesMC->GetEntriesFast();
    if (ni < 1) return;
    
    printf("\n%6s %6s %8s %14s %5s %5s %8s %8s\n\n",
            "id","mid","mc_id","pdg",
            "s(1)","s(2)","cl_id","cl_pid");
        
    SpdRsMCParticle* p;
    
    Int_t  ncl;
    
    for (Int_t i(0); i<ni; i++)
    {
         p = dynamic_cast<SpdRsMCParticle*>(fRsParticlesMC->At(i));
        
         ncl = p->GetNClusters();

         printf("%6d %6d %8d %14d %5d %5d ",
                p->GetId(),p->GetMotherId(),p->GetParticleId(),p->GetParticlePdg(),
                p->GetFirstState(),p->GetLastState());
                
         for (Int_t j(0); j<ncl; j++) {
              printf("%8d %8d",p->GetClusterId(j),p->GetClusterParicleLocId(j));
              if (j < ncl-1) printf("\n%50s","");
         }
         printf("\n");
    }
    printf("\n");
}

//________________________________________________________________________________________  
void SpdMCEventHelper::PrintParticles2(Int_t np, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintParticles2> " << endl;
    SpdMCParticle* part;
    PrintDataArray(fParticles,part,np,opt);
}

//________________________________________________________________________________________  
void SpdMCEventHelper::PrintVertices2(Int_t nvtx, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintVertices2> " << "\n\n";
    SpdVertexMC* vertex;
    PrintDataArray(fVertices,vertex,nvtx,opt);    
}

//________________________________________________________________________________________  
void SpdMCEventHelper::PrintItsHits2(Int_t nhits, Option_t* opt)  const
{
    cout << "-I- <SpdMCEventHelper::PrintItsHits2> " << "\n\n";
    SpdMCTrackHit* hit;
    PrintDataArray(fItsHits,hit,nhits,opt);
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintTsHits2(Int_t nhits, Option_t* opt)   const
{
    cout << "-I- <SpdMCEventHelper::PrintTsHits2> " << "\n\n";
    SpdMCTrackHit* hit;
    PrintDataArray(fTsHits,hit,nhits,opt);
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintTofHits2(Int_t nhits, Option_t* opt)   const
{
    cout << "-I- <SpdMCEventHelper::PrintTofHits2> " << "\n\n";
    SpdTofMCHit* hit;
    PrintDataArray(fTofHits,hit,nhits,opt);
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintEcalHits2(Int_t nhits, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintEcalHits2> " << "\n\n";
    SpdEcalMCHit* hit;
    PrintDataArray(fEcalHits,hit,nhits,opt);
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintRsHits2(Int_t nhits, Option_t* opt)   const
{
    cout << "-I- <SpdMCEventHelper::PrintRsHits2> " << "\n\n";
    SpdRsMCHit* hit;
    PrintDataArray(fRsHits,hit,nhits,opt);
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintBbcHits2(Int_t nhits, Option_t* opt)   const
{
    cout << "-I- <SpdMCEventHelper::PrintBbcHits2> " << "\n\n";
    SpdBbcMCHit* hit;
    PrintDataArray(fBbcHits,hit,nhits,opt);
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintAegHits2(Int_t nhits, Option_t* opt)   const
{
    cout << "-I- <SpdMCEventHelper::PrintAegHits2> " << "\n\n";
    SpdAegMCHit* hit;
    PrintDataArray(fAegHits,hit,nhits,opt);
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintZdcHits2(Int_t nhits, Option_t* opt)   const
{
    cout << "-I- <SpdMCEventHelper::PrintZdcHits2> " << "\n\n";
    SpdZdcMCHit* hit;
    PrintDataArray(fZdcHits,hit,nhits,opt);
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintTracks2(Int_t ntracks, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintTracks2> " << "\n\n";
    SpdTrackMC* track;
    PrintDataArray(fTracks,track,ntracks,opt);
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintVerticesRC2(Int_t nvtx, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintVerticesRC2> " << "\n\n";
    SpdVertexRC* vertex;
    PrintDataArray(fVerticesRC,vertex,nvtx,opt); 
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintTracksRC2(Int_t ntrk, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintTracksRC2> " << "\n\n";
    SpdTrackRC* track;
    PrintDataArray(fTracksRC,track,ntrk,opt); 
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintEcalClustersRC2(Int_t ncl, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintEcalClustersRC2> " << "\n\n";
    SpdEcalRCCluster* cluster;
    PrintDataArray(fEcalClustersRC,cluster,ncl,opt); 
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintEcalParticlesRC2(Int_t np, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintEcalParticlesRC2> " << "\n\n";
    SpdEcalRCParticle* particle;
    PrintDataArray(fEcalParticlesRC,particle,np,opt); 
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintEcalClustersMCInfo2(Int_t ni, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintEcalClustersMCInfo2> " << "\n\n";
    SpdEcalClusterMCInfo* info;
    PrintDataArray(fEcalClustersMCInfo,info,ni,opt); 
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintEcalParticlesMC2(Int_t ni, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintEcalParticlesMC2> " << "\n\n";
    SpdEcalMCParticle* particle;
    PrintDataArray(fEcalParticlesMC,particle,ni,opt); 
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintRsClustersMC2(Int_t ncl, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintRsClustersMC2> " << "\n\n";
    SpdRsMCCluster* cluster;
    PrintDataArray(fRsClustersMC,cluster,ncl,opt); 
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintRsParticlesMC2(Int_t ni, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintRsParticlesMC2> " << "\n\n";
    SpdRsMCParticle* particle;
    PrintDataArray(fRsParticlesMC,particle,ni,opt); 
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintTsParticles2(Int_t ni, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintTsParticles2> " << "\n\n";
    SpdTsParticle* particle;
    PrintDataArray(fTsParticles,particle,ni,opt); 
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintTofParticles2(Int_t ni, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintTofParticles2> " << "\n\n";
    SpdTofParticle* particle;
    PrintDataArray(fTofParticles,particle,ni,opt); 
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintAegParticles2(Int_t ni, Option_t* opt) const
{
    cout << "-I- <SpdMCEventHelper::PrintAegParticles2> " << "\n\n";
    SpdAegParticle* particle;
    PrintDataArray(fAegParticles,particle,ni,opt); 
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintTrackFitPars(Int_t ntracks, Option_t* opt) const
{    
    SpdTrackMC* track;
    SpdTrackFitPar* par;
    PrintDataParArray(fTracks,track,par,ntracks,opt);
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintVertexFitPars(Int_t nvtx, Option_t* opt) const
{
    SpdVertexMC*  vertex;
    SpdVtxFitPar* par; 
    PrintDataParArray(fVertices,vertex,par,nvtx,opt);
}

//________________________________________________________________________________________
void SpdMCEventHelper::PrintVertexRCFitPars(Int_t nvtx, Option_t* opt) const
{
    SpdVertexRC*  vertex;
    SpdVtxFitPar* par; 
    PrintDataParArray(fVerticesRC,vertex,par,nvtx,opt);
}

//________________________________________________________________________________________  
void SpdMCEventHelper::PrintMCProcesses()
{
    cout << "\n-------------------------------------------------------------------------\n";
    cout << "TABLE OF MC PROCESSES (Unique ID vs. Process type): \n\n";  
    for (int i(0); i<kMaxMCProcess; i++) printf("%-5d  %s\n",i,TMCProcessName[i]); 
    cout << "-------------------------------------------------------------------------\n";
}

//________________________________________________________________________________________
template <class A, class T> 
void SpdMCEventHelper::PrintDataArray(A* Array, T* obj, Int_t n, Option_t* opt) const
{
    if (!Array || !n) return;
    
    Int_t N = Array->GetEntriesFast();
    if (N < 1) return;
    if (n > N || n < 0) n = N;
    
    obj = 0; 
    for (Int_t i(0); i<n; i++) {
         obj = dynamic_cast<T*>(Array->At(i)); 
         if (obj) obj->Print(opt);
    }  
}

//________________________________________________________________________________________
template <class A, class T, class U> 
void SpdMCEventHelper::PrintDataParArray(A* Array, T* obj, U* par, Int_t n, Option_t* opt) const
{
    if (!Array || !n) return;
    
    Int_t N = Array->GetEntriesFast();
    if (N < 1) return;
    if (n > N || n < 0) n = N;
    
    obj = 0;
    par = 0;
    for (Int_t i(0); i<n; i++) {
         obj = dynamic_cast<T*>(Array->At(i)); 
         if (!obj) continue; 
         par = dynamic_cast<U*>(obj->GetFitPars());
         if (!par) continue;
         par->Print(opt);
    }   
}

//________________________________________________________________________________________
TString SpdMCEventHelper::ParticleName(Int_t pdg) const
{
    if (pdg > Int_t(1e9)) return "nucleus";  
    TParticlePDG* pdgpart = TDatabasePDG::Instance()->GetParticle(pdg);
    return (pdgpart) ? pdgpart->GetName() : "unknown";
}

//_____________________________________________________________________________
Int_t SpdMCEventHelper::GetParticleId(Int_t oldId) const 
{ 
    if (!fIdTable || !fEvent || !fParticles) return -1;
    if (oldId < 0 || oldId >= fIdTable->size()) return -1;
    
    Int_t ipart = (*fIdTable)[oldId];
    if (ipart < 0) return ipart;
    
    SpdMCParticle* part = (SpdMCParticle*)fParticles->At(ipart);
    if (!part || !fEvent->CheckParticleGeneration(part->GetGeneration())) return -1;
    
    return ipart; 
}
