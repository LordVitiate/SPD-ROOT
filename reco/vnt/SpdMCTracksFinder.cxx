// $Id$
// Author: artur   2021/02/03


//_____________________________________________________________________________
//
// SpdMCTracksFinder
//_____________________________________________________________________________

#include "SpdRunAna.h"

#include "SpdMCTracksFinder.h"

#include "SpdTrackFitterGF.h"
#include "SpdTrackMC.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"

#include "SpdHit.h"

#include "SpdSetParSet.h"
#include "SpdParSet.h"
#include "SpdTrackFitPar.h"

#include "SpdMCEventHelper.h"

#include <map>
#include <set>

using std::map;
using std::set;
using std::pair;

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMCTracksFinder)

//_____________________________________________________________________________
SpdMCTracksFinder::SpdMCTracksFinder():
// track fitter
fFitter(0),
// input
fEvent(0),fParticles(0),fVertices(0),fItsHits(0),fTsHits(0),
// output
fTracks(0),
// settings
fFitTracks(true),fSaveTracks(true),fRemoveTrackPoints(false),
// checking & counters 
fHitsError(false),
fNHitsAccepted(0),fNHitsNoTruth(0),fNHitsRejectedPart(0),
fNTracksTotal(0),fNTracksAccepted(0),
// verbosity
fVerboseLevel(1)
{
   SetDefaultCuts();
}

//_____________________________________________________________________________
SpdMCTracksFinder::~SpdMCTracksFinder() 
{
   if (fFitter) delete fFitter;
   if (fTracks) delete fTracks;
   
   //SpdMCEventHelper::Instance()->SetTracks(0);
}

//_____________________________________________________________________________
SpdTrackFitterGF* SpdMCTracksFinder::Fitter()
{
   if (fFitter) return fFitter;
   fFitter = new SpdTrackFitterGF();
   return fFitter;
}

//_____________________________________________________________________________
void SpdMCTracksFinder::SetDefaultCuts()
{
   // max. generation value 
   fCheckMaxPartGeneration = true; 
   fMaxPartGeneration = 2;    
   
   // min. transverse momentum value [GeV/c] 
   fCheckMinPartPt = true;    
   fMinPartPt = 0.1;  
   
   // min. momentum value [GeV/c]  (WARNING! abs. min. value = 4 MeV)
   fCheckMinPartMomentum = true;    
   fMinPartMomentum = 4.e-3;  
   
   // min. (p/m)-value  (WARNING! abs. min. value = 0.05)
   fCheckMinPartBetaGamma = true; 
   fMinPartBetaGamma = 0.05;
   
   // min. theta value [deg] 
   fCheckMinPartTheta = false; 
   fMinPartTheta = 0.; 
   
   //----------------------------------------------------------------------
   
   // min. number of hits in vertex detector (ITS)
   fCheckMinItsHits = true;
   fMinItsHits = 1;
   
    // min. number of hits in straw tracker detector (TS)
   fCheckMinTsHits = true;
   fMinTsHits = 0;
   
   // min. number of hits in ITS+TS
   fCheckMinHits = true;
   fMinHits = 3;
}

//_____________________________________________________________________________
InitStatus SpdMCTracksFinder::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdMCTrackFinder::Init> RootManager not instantiated " << endl;
       return kFATAL;
   }
   
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // get Event 
   
   fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   if (!fEvent) return kFATAL;
   
   // get MC-Particles array
   
   fParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCParticles));
   if (!fParticles) return kFATAL;
   
   // get MC-Vertices array
   
   fVertices = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCVertices));
   if (!fVertices) return kFATAL;
      
   Bool_t init_input = false; 
      
   // --- get ItsHits array ---
    
   fItsHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCItsHits));
   if (fItsHits) init_input = true;
   
   // --- get TsHits array ---
    
   fTsHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTsHits));
   if (fTsHits) init_input = true;
    
   if (!init_input) {
       cout << "-F- <SpdTsMCHitProducer::Init> No input data " << endl;
       return kFATAL; 
   }
   
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fTracks = new TClonesArray("SpdTrackMC");
   ioman->Register(SpdDataBranchName(kMCTracks),"SpdMCTrack", fTracks, fSaveTracks);
   
   //TClonesArray* array = dynamic_cast<TClonesArray*>(ioman->GetObject("MCParticles"));
   //if (array) ioman->Register("MCParticles","MCParticle",array,true); 
   
   //ioman->Register("MC","SpdMCTrack", fEvent, fSaveTracks);
   //ioman->Register(SpdDataBranchName(kMCParticles), "SpdMCParticle", fParticles, fSaveParticles);
   
   //============== PASS DATA TO EVENTHELPER ================================/   

   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
   helper->SetTracks(fTracks);
   
   if (!helper->GetEvent()) helper->SetEvent(fEvent);
   if (!helper->GetParticles()) helper->SetParticles(fParticles);
   if (!helper->GetVertices()) helper->SetVertices(fVertices);

   //============== CREATE AND INITIALIZE FITTER ============================/
   
   if (fFitTracks) 
   {
       if (!fFitter) fFitter = new SpdTrackFitterGF();
       if (!fFitter->Init()) {
           cout << "-F- <SpdMCTrackFinder::Init> Fitter not initalized properly" << endl;
           return kFATAL;
       }
       else {
           if (fVerboseLevel > -1) {
               cout << "-I- <SpdMCTrackFinder::Init> Fitter created and initalized " << endl;
           }
       }
   }
   
   if (fVerboseLevel > -1) cout << "-I- <SpdMCTrackFinder::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdMCTracksFinder::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   
   if (!fItsHits) assert(fTsHits);
   else assert(fItsHits); 
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdMCTrackFinder::Exec> Process event ... " 
            << fEvent->GetEventId();// << "\n\n";
       if (fVerboseLevel > 1) cout << "\n";          
   }
  
   fTracks->Delete();

   // >>>>> Track MC-finding <<<<<
   
   typedef std::pair<Double_t, spd_mctrack_point_> point_ ;
   
   struct track 
   {
       track(): nits(0),nts_b(0),nts_ec(0) {}
       std::multimap<Double_t, spd_mctrack_point_> points; // <hit time, track point>
       Int_t nits;
       Int_t nts_b;
       Int_t nts_ec;
       Int_t nts() { return nts_b+nts_ec; }
       Int_t n() { return nits+nts_b+nts_ec; }
   };
   
   set<Int_t> rejected_particles;
   map<Int_t, track*> accepted_particles; 

   Int_t n, pid;
   Int_t na(0), nr[4] = {0,0,0,0}, nsum;
   SpdMCTrackHit* hit;
   SpdMCParticle* part;
   track* x;
   
   Int_t nhacc(0), nhrej_notruth(0), nhrej_particle(0);
   Bool_t errors = false;

   //---------------------------------
   // ITS hits processing
   
   if (fItsHits) 
   {
       n = fItsHits->GetEntriesFast();
       
       for (Int_t i(0); i<n; i++) 
       {
            hit = dynamic_cast<SpdMCTrackHit*>(fItsHits->At(i));
            
            assert(hit);
           
             // reject hit if there is no mc-truth
            if (!hit->GetMCTruth()) {
                nr[0]++;
                continue;
            }
 
            //ATTENTION add hit produced by a single particle only 
            if (!hit->IsSingleParticleHit()) {
                nr[1]++;
                continue;
            }
            
            pid = hit->GetParticleId();
            
            // check if the hit particle is rejected
            if (rejected_particles.find(pid) != rejected_particles.end()) {
                nr[2]++;
                continue;
            }
               
            // accept particle (create track)
            auto ix = accepted_particles.find(pid);
            
            if (ix != accepted_particles.end()) 
            {   // add point to existed track
                x = ix->second;
                if (hit->GetModId() == kSpdIts) x->nits++;
                else { nr[3]++; continue; }
                x->points.insert(point_(hit->GetHitTime(),spd_mctrack_point_(hit->GetId(),kMCItsHits)));
            }
            else
            {  
                // accept/reject particle 
                part = dynamic_cast<SpdMCParticle*>(fParticles->At(pid));
                
                /* ATTENTION Сuts applying ATTENTION */
                if (!AcceptParticle(part)) {
                    rejected_particles.insert(pid);
                    nr[2]++;
                    continue;
                }
                    
                // create new track and add point to 
                x = new track();
                if (hit->GetModId() == kSpdIts) x->nits++;
                else { nr[3]++; continue; }
                x->points.insert(point_(hit->GetHitTime(),spd_mctrack_point_(hit->GetId(),kMCItsHits)));
                accepted_particles.insert(pair<Int_t,track*>(pid,x));
            }
            na++;
       }
       
       // Counters
       fNHitsNoTruth += nr[0];
       fNHitsRejectedPart += nr[2];
       fNHitsAccepted += na;
       
       nhacc += na;
       nhrej_notruth += nr[0];
       nhrej_particle += nr[2];
       
       if (fVerboseLevel > 2) {
           cout << "\n-I- <SpdMCTracksFinder::Exec> [ITS] accepted: " 
                << na << ", no-truth: " << nr[0] << ", rejected (by particle): " << nr[2]; 
       }
       
       // Checking ... 
       nsum = na;
       for (int i(0); i<4; i++) nsum += nr[i];
       
       if (n != nsum || nr[1] > 0 || nr[3] > 0) {
           cout << "\n-E- <SpdMCTracksFinder::Exec> [ITS] Hits total: " << n << "; processed: " << nsum << endl;
           cout << "-E- <SpdMCTracksFinder::Exec> [ITS] accepted: " << na 
                << ", rejected: " << nr[0] << ", " << nr[1] << ", " << nr[2] << ", " << nr[3]; 
           fHitsError = true;    
           errors = true;   
       }
   }
   
   //---------------------------------
   // TS hits processing
    
   if (fTsHits) 
   {
  
       n = fTsHits->GetEntriesFast();
       na = 0;
       for (int i(0); i<4; i++) nr[i] = 0;
   
       for (Int_t i(0); i<n; i++) 
       {
            hit = dynamic_cast<SpdMCTrackHit*>(fTsHits->At(i));
            
            assert(hit);
           
             // reject hit if there is no mc-truth
            if (!hit->GetMCTruth()) {
                nr[0]++;
                continue;
            }
            
            //ATTENTION add hit produced by a single particle only 
            if (!hit->IsSingleParticleHit()) {
                nr[1]++;
                continue;
            }
            
            pid = hit->GetParticleId();
            
            // check if the hit particle is rejected
            if (rejected_particles.find(pid) != rejected_particles.end()) {
                nr[2]++;
                continue;
            }
               
            // accept particle (create track)
            auto ix = accepted_particles.find(pid);
            
            if (ix != accepted_particles.end()) 
            {   // add point to existed track
                x = ix->second;
                     if (hit->GetModId() == kSpdTsTB)  x->nts_b++;
                else if (hit->GetModId() == kSpdTsTEC) x->nts_ec++;
                else { nr[3]++; continue; }
                x->points.insert(point_(hit->GetHitTime(),spd_mctrack_point_(hit->GetId(),kMCTsHits)));
            }
            else 
            { 
                // accept/reject particle 
                part = dynamic_cast<SpdMCParticle*>(fParticles->At(pid));
                
                /* ATTENTION Сuts applying ATTENTION */
                if (!AcceptParticle(part)) {
                    rejected_particles.insert(pid);
                    nr[2]++;
                    continue;
                }
                
                // create new track and add point to 
                x = new track();
                     if (hit->GetModId() == kSpdTsTB)  x->nts_b++;
                else if (hit->GetModId() == kSpdTsTEC) x->nts_ec++;
                else { nr[3]++; continue;}
                x->points.insert(point_(hit->GetHitTime(),spd_mctrack_point_(hit->GetId(),kMCTsHits)));
                accepted_particles.insert(pair<Int_t,track*>(pid,x));
            }
            na++;
       }
       
       // Counters
       fNHitsNoTruth += nr[0];
       fNHitsRejectedPart += nr[2];
       fNHitsAccepted += na;
       
       nhacc += na;
       nhrej_notruth += nr[0];
       nhrej_particle += nr[2];
       
       // Checking ... 
       nsum = na;
       for (int i(0); i<4; i++) nsum += nr[i];
       
       if (fVerboseLevel > 2) {
           cout << "\n-I- <SpdMCTracksFinder::Exec> [TS] accepted: " 
                << na << ", no-truth: " << nr[0] << ", rejected (by particle): " << nr[2]; 
       }
       
       if (n != nsum || nr[1] > 0 || nr[3] > 0) {
           cout << "\n-E- <SpdMCTracksFinder::Exec> [TS] Hits total: " << n << "; processed: " << nsum << endl;
           cout << "-E- <SpdMCTracksFinder::Exec> [TS] accepted: " << na 
                << ", rejected: " << nr[0] << ", " << nr[1] << ", " << nr[2] << " " << nr[3]; 
           fHitsError = true; 
           errors = true;  
       }
   }
    
   //-------------------------------------------------------------
   // Create tracks
   //
   
   fNTracksTotal += accepted_particles.size();
   
   SpdTrackMC* mctrack;        
   for (auto it : accepted_particles) 
   {
        x = it.second;
        
        /* ATTENTION Сuts applying ATTENTION */
        if (fCheckMinItsHits && (x->nits  < fMinItsHits)) continue;
        if (fCheckMinTsHits  && (x->nts() < fMinTsHits))  continue;
        if (fCheckMinHits    &&   (x->n() < fMinHits))    continue;
        
        mctrack = AddTrack();
        
        // ATTENTION track settings ATTENTION
        
        part = (SpdMCParticle*)fParticles->At(it.first);
       
        part->SetTrackId(mctrack->GetId()); 
       
        mctrack->SetParticleId(it.first);
        mctrack->SetParticlePdg(part->GetPdgCode());
        
        Int_t vid = part->GetFirstVertexId();
        mctrack->SetVertexId(SpdMCEventHelper::Instance()->SearchForVertexId(vid));
        
        mctrack->SetNHitsIts(x->nits);
        mctrack->SetNHitsTsB(x->nts_b);
        mctrack->SetNHitsTsEC(x->nts_ec);
        
        // add points
        for (auto ix : x->points) mctrack->AddPoint(ix.second);
   }
   
   // Counters
   Int_t ntot_tracks = accepted_particles.size();
   fNTracksAccepted += fTracks->GetEntriesFast();

   // delete temporary objects
   for (auto it : accepted_particles) delete it.second; 
   rejected_particles.clear();
   accepted_particles.clear();
   
   //---------------------------------------
   // >>>>> Track fitting (optionally) <<<<< 
    
   Int_t nfitted_tracks = 0;
  
   if (fFitTracks && fFitter) {
       
       fFitter->InitFitter();
  
       SpdTrackFitPar* fitpars;
       Int_t ntracks = fTracks->GetEntriesFast();
       
       for (Int_t i(0); i<ntracks; i++) 
       {
            mctrack = dynamic_cast<SpdTrackMC*>(fTracks->At(i));
            
            std::vector<Int_t>& ids = fFitter->GetTrackPointIds();
            
            // create array of track hits
            std::vector<SpdHit*> track;
            PrepareTrack(mctrack,track,ids);
       
            // create output fit parameters
            fitpars = mctrack->FitParameters();
            fitpars->SetTrackId(mctrack->GetId());
            
            // init fitter start parameters
            part = dynamic_cast<SpdMCParticle*>(fParticles->At(mctrack->GetParticleId()));
            
            //part->Print(""); //FIXME
            
            fFitter->SetPdg(part->GetPdgCode());
            fFitter->SetMomentum(part->GetStartMom());
            fFitter->SetVertex(part->GetStartPos());
            fFitter->SetTime(part->GetStartTime());
             
            // fit track
            if (!fFitter->FitTrack(track,fitpars)) {
                //fitpars->Print();   //FIXME
                mctrack->DeleteFitPars();
            }
            else {
                CheckTrack(track,fitpars);
                nfitted_tracks++;
                //fitpars->Print();   //FIXME
            }
       }
   }
   
   //---------------------------------------
   // Remove track points (optionally)
   
   if (fRemoveTrackPoints) {
       Int_t ntracks = fTracks->GetEntriesFast();
       for (Int_t i(0); i<ntracks; i++) {
            mctrack = dynamic_cast<SpdTrackMC*>(fTracks->At(i));
            if (mctrack) mctrack->RemovePoints();
       }
   }
   
   if (fVerboseLevel > 1) 
   { 
       cout << "\n-I- <SpdMCTracksFinder::Exec> Print ... " << endl; 
       
       SpdMCEventHelper::Instance()->PrintTracks();   
       
       if (fVerboseLevel > 2) { 
           SpdMCEventHelper::Instance()->PrintTracks2(1,"fitpars"); 
       }
       
       Int_t nits(0), nts(0), nnh(0);
       if (fItsHits) {
           nits = fItsHits->GetEntriesFast();
           nnh += nits;
       }
       if (fTsHits) {
           nts = fTsHits->GetEntriesFast();
           nnh += nts;
       } 
       
       Int_t nrej = nhrej_notruth+nhrej_particle;
       cout << endl;
       cout << "Hits (total|ITS,TS):               " << nnh << ": " << nits << ", " << nts << endl;
       cout << "Hits (accepted):                   " << nhacc << endl;
       cout << "Hits (rejected|notruth,particle):  " << nrej << ": " << nhrej_notruth << ", " << nhrej_particle << endl;  
       cout << "Errors (hits):                     " << (errors ? "yes" : "no") << endl;
       cout << "Tracks (total,accepted):           " << ntot_tracks << "/" << fTracks->GetEntriesFast() << endl;
       cout << "Fit:                               " << ((fFitTracks && fFitter) ? "yes" : "no") << endl;
       cout << "Fitted tracks:                     " << nfitted_tracks 
            << endl;
   }
}

//________________________________________________________________________________________ 
Bool_t SpdMCTracksFinder::AcceptParticle(SpdMCParticle* part)
{
   //cout << "<SpdMCTracksFinder::AcceptParticle> (1) \n"; 
   
   if (!part) return false;
   
   // is particle in DB?
   if (!part->IsInDatabase()) return false;
   
   // is particle charged?
   if (part->GetCharge() == 0) return false;
   
   // check particle generation
   if (fCheckMaxPartGeneration && (part->GetGeneration() > fMaxPartGeneration)) return false;
   
   // check particle transverse momentum value
   if (fCheckMinPartPt && (part->GetStartPt() < fMinPartPt)) return false;

   // check particle momentum value
   if (fCheckMinPartMomentum && (part->GetStartP() < fMinPartMomentum)) return false;
   
   // check particle p/m relation
   if (fCheckMinPartBetaGamma && (part->GetStartBetaGamma() < fMinPartBetaGamma)) return false;

   // check particle momentum theta
   if (fCheckMinPartTheta && (part->GetStartPtheta() < fMinPartTheta)) return false;

   //cout << "<SpdMCTracksFinder::AcceptParticle> (2)\n\n";
   
   return true;   
}

//________________________________________________________________________________________ 
SpdTrackMC* SpdMCTracksFinder::AddTrack()
{
   if (!fTracks) return 0;
   Int_t size = fTracks->GetEntriesFast();
   SpdTrackMC* track = new ((*fTracks)[size]) SpdTrackMC();
   track->SetId(size);
   return track; 
}

//________________________________________________________________________________________ 
void SpdMCTracksFinder::PrepareTrack(SpdTrackMC* mctrack, 
                                     std::vector<SpdHit*>& track, 
                                     std::vector<Int_t>& ids) const
{
    track.clear();
    ids.clear();
    
    const std::vector<spd_mctrack_point_>& points = mctrack->GetTrackPoints();
    SpdHit* hit;
    Int_t n(0);
    ids.reserve(points.size());
    
    for (auto x : points) {
        if (x.second == kMCItsHits) {
            track.push_back((SpdHit*)fItsHits->At(x.first));
            ids.push_back(n);
        }
        else if (x.second == kMCTsHits) {
            track.push_back((SpdHit*)fTsHits->At(x.first));
            ids.push_back(n);
        }
        n++;
    }
}

//________________________________________________________________________________________
void SpdMCTracksFinder::CheckTrack(std::vector<SpdHit*>& track, SpdTrackFitPar* pars)
{
    SpdMCTrackHit* hit; 
    
    TVector3 pos;
    Double_t d;
    
    if (pars->GetFirstState()) 
    {
        d = -1;
        pos = pars->GetFirstState()->GetPosition();
        hit = dynamic_cast<SpdMCTrackHit*>(track[0]);  
        if (hit) d = (pos - hit->GetMCPoint()).Mag();         
        pars->GetFirstState()->SetPosDelta(d);
    }
    
    if (pars->GetLastState()) 
    {
        d = -1;
        pos = pars->GetLastState()->GetPosition();
        hit = dynamic_cast<SpdMCTrackHit*>(track[track.size()-1]);  
        if (hit) d = (pos - hit->GetMCPoint()).Mag();         
        pars->GetLastState()->SetPosDelta(d);
    }
}

//________________________________________________________________________________________  
void SpdMCTracksFinder::Finish()
{
   if (fVerboseLevel > -1) 
   { 
       cout << "\n-I- <SpdMCTracksFinder::Finish> ... ";
       
       if (fFitter) fFitter->PrintCounter();
     
       PrintCuts();
       
       cout << "\n-I- <SpdMCTracksFinder::Finish> tracks [acc/total]: " 
            << fNTracksAccepted << "/" << fNTracksTotal 
            << "; hits: [acc] = " << fNHitsAccepted 
            << ", [rej:notruth,particle] = (" << fNHitsNoTruth << "," << fNHitsRejectedPart 
            << "); errors (hits): " << fHitsError;// << endl;
   }
}

//________________________________________________________________________________________ 
void SpdMCTracksFinder::PrintCuts() const
{
   cout << "\n\t-I- <SpdMCTracksFinder::PrintCuts> \n\n"; 
   
   cout << "\tmax. generation (value):               " << ((fCheckMaxPartGeneration) ? "yes" : "no")
        << " (" << fMaxPartGeneration << ") " << endl;
   cout << "\tmin. transverse momentum (value):      " << ((fCheckMinPartPt) ? "yes" : "no") 
        << " (" << fMinPartPt << ", GeV) " << endl;        
   cout << "\tmin. momentum (value):                 " << ((fCheckMinPartMomentum) ? "yes" : "no") 
        << " (" << fMinPartMomentum << ", GeV) " << endl;  
   cout << "\tmin. (p/m)-value (value):              " << ((fCheckMinPartBetaGamma) ? "yes" : "no")  
        << " (" << fMinPartBetaGamma << ") " << endl;
   cout << "\tmin. momentum theta (value):           " << ((fCheckMinPartTheta) ? "yes" : "no") 
        << " (" << fMinPartTheta << ", deg) " << endl; 
   cout << endl;     
   cout << "\tmin. number of ITS hits (value):       " << ((fCheckMinItsHits) ? "yes" : "no") 
        << " (" << fMinItsHits << ") " << endl;
   cout << "\tmin. number of TS hits (value):        " << ((fCheckMinTsHits) ? "yes" : "no") 
        << " (" << fMinTsHits << ") " << endl;
   cout << "\tmin. number of ITS+TS hits (value):    " << ((fCheckMinHits) ? "yes" : "no") 
        << " (" << fMinHits << ") " << endl;

   //cout << "\n";
}

