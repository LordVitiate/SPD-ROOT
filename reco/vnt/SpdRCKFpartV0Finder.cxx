// $Id$
// Author: artur   2021/04/22


//_____________________________________________________________________________
//
// SpdRCKFpartV0Finder
//_____________________________________________________________________________

#include "SpdRCKFpartV0Finder.h"
#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdVertexMC.h"
#include "SpdTrackMC.h"

#include "FairRunAna.h"
#include "SpdRunAna.h"

#include "SpdVertexRC.h"

#include "SpdDetectorList.h"
#include "SpdVertexFitPar.h"
#include "SpdTrackPropagatorGF.h"

#include "SpdVertexRC.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"

#include "SpdField.h"

#include "KFParticle.h"
#include "KFPTrack.h"
#include "KFPVertex.h"
#include "KFParticleSIMD.h"

#include "Exception.h"

#include <TDatabasePDG.h>

#include <exception>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRCKFpartV0Finder)

//_____________________________________________________________________________
SpdRCKFpartV0Finder::SpdRCKFpartV0Finder(): 
//
fPropagator(0),fVertexFinder(0),
// input
fEvent(0),fParticles(0),fTracks(0),
// input/output
fVerticesRC(0),
// options & cuts
fMinItsHits(3),
fUsedTypeOfPV(1),fConstrainToPV(1),
fMinChi2PV(0.5), fMaxChi2Part(10.),
fMinMass(0.00),  fMaxMass(2.50),
// counter
fNTotalSecFittedVertices(0),
// verbosity
fVerboseLevel(1)
{
 
}

//_____________________________________________________________________________
SpdRCKFpartV0Finder::~SpdRCKFpartV0Finder() 
{
   if (fPropagator) delete fPropagator;
   if (fVertexFinder) delete fVertexFinder;
}

//_____________________________________________________________________________
InitStatus SpdRCKFpartV0Finder::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdRCKFpartV0Finder::Init> RootManager not instantiated " << endl;
       return kFATAL;
   }
   
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // get Event 
   
   fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   if (!fEvent) return kFATAL;
   
   // get MC-Particles array
   
   fParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCParticles));
   if (!fParticles) return kFATAL;
   
   // --- get Tracks array ---
    
   fTracks = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTracks));
   if (!fTracks) return kFATAL; 

   // ----- get RC-Vertices array -----
   
   fVerticesRC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCVertices));
   if (!fVerticesRC) return kFATAL;
   
   //============== INIT TRACK PROPAGATOR & VERTEX FINDER ==================/
   
   fPropagator = new SpdTrackPropagatorGF();
   if (!fPropagator->Init()) {
       cout << "-F- <SpdRCKFpartV0Finder::Init> "
            << "Track propagator not initialized properly" << endl;
       return kFATAL;
   }
   
   fVertexFinder = new SpdVertexCombiFinder();
   
   //============== PASS DATA TO EVENTHELPER ================================/   

   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
  
   if (!helper->GetEvent())      helper->SetEvent(fEvent);
   if (!helper->GetParticles())  helper->SetParticles(fParticles);
   if (!helper->GetTracks())     helper->SetTracks(fTracks);
   if (!helper->GetVerticesRC()) helper->SetVerticesRC(fVerticesRC);
   
   if (fVerboseLevel > -1) {
       cout << "-I- <SpdRCKFpartV0Finder::Init> Intialization successfull " << endl;
   }
  
   return kSUCCESS;
}

//_______________________________________________________________________________________
void SpdRCKFpartV0Finder::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fTracks);
   assert(fVerticesRC);

   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdRCKFpartV0Finder::Exec> Process event ... " 
            << fEvent->GetEventId();// << "\n\n";
       if (fVerboseLevel > 1) cout << "\n";          
   }
   
   if (fVerticesRC->GetEntriesFast() > 1) {
       cout << "\n-E- <SpdRCKFpartV0Finder::Exec> "
            << "Input data array (rc-vertices) should include primary vertex only" << endl; 
       return;   
   }
   
   if (!GetPrimaryVertex()) {
       cout << "\n-W- <SpdRCKFpartV0Finder::Exec> No primary vertex"; 
       return; 
   }
      
   Int_t nsec = FindSecondaryVertices();
   fNTotalSecFittedVertices += nsec;

   if (fVerboseLevel > 1) { 
       cout << "\n-I- <SpdRCKFpartV0Finder::Exec> Print ... " << endl; 
       cout << "-I- <SpdRCKFpartV0Finder::Exec> Secondary fitted vertices :" 
            << fNTotalSecFittedVertices << endl;    
   }
}

//________________________________________________________________________________________
SpdVertexRC* SpdRCKFpartV0Finder::GetPrimaryVertex() 
{
   SpdVertexRC* v = (SpdVertexRC*)fVerticesRC->At(0); 
   if (v && v->IsPrimary()) return v;
   return 0;
}

//________________________________________________________________________________________
Bool_t SpdRCKFpartV0Finder::GetPrimMCVertexPos(TVector3& v)
{
   if (!fParticles || !fTracks) return false;
   SpdVertexRC* vtx = GetPrimaryVertex();
   if (!vtx) return false;
   const std::set<int>& tracks = vtx->GetTracks();
   SpdTrackMC* track;
   for (auto track_id : tracks) {
        track = (SpdTrackMC*)fTracks->At(track_id);
        if (!track) continue;
        if (track->GetVertexId() == 0) {
            SpdMCParticle* part = (SpdMCParticle*)fParticles->At(track->GetParticleId());
            if (!part) return false;
            v = part->GetStartPos();
            return true;
        }
   }
   return false;
}

//________________________________________________________________________________________    
Int_t SpdRCKFpartV0Finder::FindSecondaryVertices()
{
   //cout << "\n-I- <SpdRCKFpartV0Finder::FindSecondaryVertices> Process event ... ";

   Int_t nTracks = fTracks->GetEntriesFast();    // number of tracks
   if (nTracks == 0) return 0;

   // set primary vertex (reconstructed or simulated)
   //-------------------------------------------------
   
   SpdVertexRC* vprim = GetPrimaryVertex();
   if (!vprim) return 0;
   SpdPrimVertexFitPar* vprimpar = dynamic_cast<SpdPrimVertexFitPar*>(vprim->GetFitPars());
   if (!vprimpar) return 0;
    
   TVector3 recVtx = vprimpar->GetVertex();      // reconstructed PV
   const TMatrixDSym& recCov = vprimpar->GetCov();

   if (fUsedTypeOfPV < 1) { 
       //WARNING try to get simulated PV WARNING
       if (!GetPrimMCVertexPos(recVtx)) return 0; 
   }

   KFPVertex vert;
   
   vert.SetXYZ(recVtx.X(), recVtx.Y(), recVtx.Z());
   vert.SetCovarianceMatrix( (float)recCov(0,0),
                             (float)recCov(1,0), (float)recCov(1,1),
                             (float)recCov(2,0), (float)recCov(2,1), (float)recCov(2,2) );

   vert.SetNContributors(vprimpar->GetNTracks());
   vert.SetChi2(vprimpar->GetChi2());
   
   KFParticle pVtx(vert);      // set primary vertex as KFParticle
   
   fPVertex = pVtx;            // init primary vertex
   
   // set field
   //-----------
   
   Float_t fieldBz = 0;
    
   SpdField* field = fPropagator->GetField();
   if (field) fieldBz = field->GetBz(recVtx.X(),recVtx.Y(),recVtx.Z());
    
   KFParticle::SetField(fieldBz);    
   
   // select track and set track parameters
   //---------------------------------------
   
   SpdTrackMC* track;
   SpdTrackFitPar* tpars;  
   const SpdTrackState* state;
   
   fTracksId.clear();
   fKFparticles.clear();
   
   std::vector<Int_t> ppdgs;
  
   Int_t pdgTrk;
   for (Int_t i(0); i<fTracks->GetEntriesFast(); i++) 
   {
        track = (SpdTrackMC*)fTracks->At(i);
        if (!track) continue;
    
        // I. search for the track particle in the list of daughters
        //---------------------------------------------------------
    
        pdgTrk = track->GetParticlePdg();  // get track particle type (pdg)
       
        TParticlePDG* part = TDatabasePDG::Instance()->GetParticle(pdgTrk);
        if (!part) continue;
    
        if (fDaughters.find(pdgTrk) == fDaughters.end()) continue;
        
        // II. check track (fit) parameters
        //----------------------------------------------------------
        
        // 1st cut 
        if (track->GetNHitsIts() < fMinItsHits) continue;
        
        // 2nd cut 
        if (!track->GetIsFitted()) continue;
    
        // WARNING 3rd cut : hard cut! WARNING
        tpars = track->GetFitPars();
        
        if (!CheckTrack(tpars)) {
           // cout << " >>>>>>>>>>>>>> 2nd cut " << endl; 
           // track->Print("fitpars"); 
           continue;
        }
       
        // III. check track deviation from the Primary Vertex 
        //---------------------------------------------------
        
        state = tpars->GetFirstState();  // get track parameters at first hit position
        
        TVector3 trkPos = state->GetPosition();
        TVector3 trkMom = state->GetMomentum();
        const TMatrixDSym& trkCov = state->GetCovariance();
        
        KFPTrack kfTrack;
        kfTrack.SetParameters( trkPos.X(), trkPos.Y(), trkPos.Z(), 
                               trkMom.X(), trkMom.Y(), trkMom.Z() );

        Double_t C[21] = { trkCov(0,0),
                           trkCov(1,0),trkCov(1,1),
                           trkCov(2,0),trkCov(2,1),trkCov(2,2),
                           trkCov(3,0),trkCov(3,1),trkCov(3,2),trkCov(3,3),
                           trkCov(4,0),trkCov(4,1),trkCov(4,2),trkCov(4,3),trkCov(4,4),
                           trkCov(5,0),trkCov(5,1),trkCov(5,2),trkCov(5,3),trkCov(5,4),trkCov(5,5) };

        kfTrack.SetCovarianceMatrix(C);

        kfTrack.SetNDF(tpars->GetNDF());
        kfTrack.SetChi2(tpars->GetChi2());
        kfTrack.SetCharge(part->Charge()/3);
 
        KFParticle p1(kfTrack, pdgTrk);    
        
        // ATTENTION check track deviation ATTENTION
        if (p1.GetDeviationFromVertex(fPVertex) < fMinChi2PV) continue;
    
        // ADD TRACK
        //----------------------------
        
        ppdgs.push_back(pdgTrk);
        fKFparticles.push_back(p1);
        fTracksId.push_back(track->GetId());
        
        //cout << "\nid/pdg: "<< track->GetId() << "/" << pdgTrk << " ... ok ";
   }
  
   //cout << " KFparticles.size() = " << KFparticles.size() << endl;
  
   if (fKFparticles.size() < 2) return 0; // number of particles should be >= 2
  
   // init vertex finder
   fVertexFinder->InitParticles(ppdgs);

   // Construct vertices 
   //---------------------------------------------------------------
   
   Int_t nV0 = 0; 
   for (auto v: fVertices) nV0 += ConstructVertex(v);
   
   return nV0;
}

//________________________________________________________________________________________  
Int_t SpdRCKFpartV0Finder::ConstructVertex(const std::vector<Int_t>& vertex)
{
   if (!fVertexFinder->InitVertex(vertex)) return 0;
   
   //fVertexFinder->PrintSettings();
   //fVertexFinder->PrintVertexCandidates();
   
   const Int_t nDaughters = fVertexFinder->GetNVertexParticles();
   const KFParticle* vDaughters[nDaughters]; 
   
   std::vector<Int_t> vc;
   
   Float_t m, dm;
   
   Int_t nv = 0;
   
   SpdVertexRC* rc_vertex;
   SpdVertexFitParDaughterTrack rc_track;
   
   // loop over all combinations
   Int_t i;
   while (fVertexFinder->Next(vc))
   {
        i = 0;
        for (auto idx : vc) vDaughters[i++] = &fKFparticles[idx];
    
        // 1st vertex checkout
        if (!CheckVertex(vDaughters,nDaughters)) continue;
    
        // vertex reconstruction
        KFParticle K0_1;
        K0_1.SetProductionVertex(fPVertex);
        
        if (fConstrainToPV > 0) K0_1.Construct(vDaughters,nDaughters,&fPVertex,-1);
        else K0_1.Construct(vDaughters,nDaughters,0,-1);
 
        // 2nd vertex checkout
        if (!CheckDecay(K0_1,vDaughters,nDaughters)) continue;
        
        //i = 0;
        //for (auto idx : vc) {
        //     cout << idx << " " << i << " " << nDaughters << " " << vDaughters[i]->GetPDG() << endl;
        //     i++;
        //}

        nv++;
         
        rc_vertex = AddVertex();
        
        for (auto idx : vc) rc_vertex->AddTrack(fTracksId[idx]);
            
        // Create and fill fit parameters
        //----------------------------------
    
        SpdVertexFitPar2* vpar = dynamic_cast<SpdVertexFitPar2*>(rc_vertex->FitPars());
        assert(vpar);
        

        K0_1.TransportToProductionVertex(); // Transport the particle to its production vertex
        vpar->SetProductionVertex(K0_1.GetX(),K0_1.GetY(),K0_1.GetZ());  

        K0_1.TransportToDecayVertex(); // Transport the particle to its decay vertex
        vpar->SetDecayVertex(K0_1.GetX(),K0_1.GetY(),K0_1.GetZ());  

        vpar->SetMass(K0_1.GetMass(),K0_1.GetErrMass());  
        vpar->SetMom(K0_1.GetP(),K0_1.GetErrP());    
        vpar->SetPt(K0_1.GetPt(),K0_1.GetErrPt());      
        vpar->SetTheta(K0_1.GetTheta(),0);
        vpar->SetPhi(K0_1.GetPhi(),K0_1.GetErrPhi());    
        vpar->SetEta(K0_1.GetEta(),K0_1.GetErrEta());    
        vpar->SetLength(K0_1.GetDecayLength(),K0_1.GetErrDecayLength()); 
        vpar->SetLifetime(K0_1.GetLifeTime(),K0_1.GetErrLifeTime());
        
        vpar->SetChi2PV(K0_1.GetDeviationFromVertex(fPVertex));
        vpar->SetChi2SV(K0_1.GetChi2());
                   
        // fill arguments for ArmenterosPodolanski plot
        //---------------------------------------------
        
        if (nDaughters == 2) {
            
            Int_t q1 = vDaughters[0]->GetQ();
            Int_t q2 = vDaughters[1]->GetQ();
            
            if (q1 * q2 < 0) 
            {
                KFParticle& p1 = const_cast<KFParticle&>(*vDaughters[0]);
                KFParticle& p2 = const_cast<KFParticle&>(*vDaughters[1]);
             
                Float_t QtAlfa[2];
                
                if (q1 > 0.0) K0_1.GetArmenterosPodolanski(p1,p2,QtAlfa);
                else K0_1.GetArmenterosPodolanski(p2,p1,QtAlfa);
            
                vpar->SetQtAlfa1(QtAlfa[0]);
                vpar->SetQtAlfa2(QtAlfa[1]);
            }
        }
        
        // fill daughters
        i = 0;
        for (; i < nDaughters; i++) {
            
             vDaughters[i]->GetMass(m,dm);

             rc_track.Trk_mass_ = m;

             rc_track.Trk_mom_    = vDaughters[i]->GetP();
             rc_track.Trk_momErr_ = vDaughters[i]->GetErrP();

             rc_track.Trk_pt_     = vDaughters[i]->GetPt();
             rc_track.Trk_ptErr_  = vDaughters[i]->GetErrPt();

             rc_track.Trk_eta_    = vDaughters[i]->GetEta();
             rc_track.Trk_etaErr_ = vDaughters[i]->GetErrEta();
           
             rc_track.Trk_phi_    = vDaughters[i]->GetPhi();
             rc_track.Trk_phiErr_ = vDaughters[i]->GetErrPhi();
           
             rc_track.Trk_theta_  = vDaughters[i]->GetTheta();
        
             rc_track.Trk_chi2PV_ = vDaughters[i]->GetDeviationFromVertex(fPVertex); 
            
             vpar->AddDaughter(rc_track);
        }
        
        //rc_vertex->Print("fitpars");
        //cout << endl;
        //vpar->Print();
   }

   return nv;   
}

//________________________________________________________________________________________
Bool_t SpdRCKFpartV0Finder::CheckTrack(SpdTrackFitPar* pars)
{
   if (!pars) return false;
   if (fTrackSelOption < 1) return true;
   //cout << "  <SpdRCKFpartV0Finder::CheckTrack> " << pars->GetIsGood() << endl;
   return pars->GetIsGood();
}

//________________________________________________________________________________________  
Bool_t SpdRCKFpartV0Finder::CheckVertex(const KFParticle** pp, Int_t np)
{
   for (Int_t i(0); i < np-1; ++i) {    
        for (Int_t j = i+1; j < np; ++j) { 
             if (pp[i]->GetDeviationFromParticle(*pp[j]) > fMaxChi2Part) return false;
        }  
   }
   return true; 
}
 
//________________________________________________________________________________________  
Bool_t SpdRCKFpartV0Finder::CheckDecay(const KFParticle& p0, const KFParticle** pp, Int_t np)
{
   if (np > 2) return true;

   Float_t m1, m2, dm1, dm2;
   
   pp[0]->GetMass(m1,dm1);
   pp[1]->GetMass(m2,dm2);
   
   if (p0.GetMass() <= std::max(m1,m2)) return false; 
            
   return true;   
}
 
//________________________________________________________________________________________  
void SpdRCKFpartV0Finder::AddVertexCandidate(Int_t pdg1, Int_t pdg2)
{
   fDaughters.insert(pdg1);
   fDaughters.insert(pdg2);
    
   std::vector<Int_t> v = {pdg1, pdg2};
   fVertices.push_back(v);
}

//________________________________________________________________________________________  
void SpdRCKFpartV0Finder::AddVertexCandidate(Int_t pdg1, Int_t pdg2, Int_t pdg3)
{
   fDaughters.insert(pdg1);
   fDaughters.insert(pdg2);
   fDaughters.insert(pdg3);
    
   std::vector<Int_t> v = {pdg1, pdg2, pdg3};
   fVertices.push_back(v);
}

//________________________________________________________________________________________  
void SpdRCKFpartV0Finder::AddVertexCandidate(Int_t pdg1, Int_t pdg2, Int_t pdg3, Int_t pdg4)
{
   fDaughters.insert(pdg1);
   fDaughters.insert(pdg2);
   fDaughters.insert(pdg3);
   fDaughters.insert(pdg4);
    
   std::vector<Int_t> v = {pdg1, pdg2, pdg3, pdg4};
   fVertices.push_back(v);
}

//________________________________________________________________________________________  
void SpdRCKFpartV0Finder::AddVertexCandidate(const std::vector<Int_t>& pdg)
{
   std::vector<Int_t> v;
   v.resize(pdg.size());
   Int_t j(0);
   for (auto i : pdg) {
        fDaughters.insert(i);
        v[j++] = i;
   }
   fVertices.push_back(v);
}

//________________________________________________________________________________________  
void SpdRCKFpartV0Finder::AddVertexCandidate(Int_t n, const Int_t* pdg)
{
   std::vector<Int_t> v;
   v.resize(n);
   for (Int_t i(0); i < n; i++) {
        fDaughters.insert(pdg[i]);
        v[i] = pdg[i];
   }
   fVertices.push_back(v);
}

//________________________________________________________________________________________  
SpdVertexRC* SpdRCKFpartV0Finder::AddVertex() 
{
   if (!fVerticesRC) return 0 ;
   Int_t size = fVerticesRC->GetEntriesFast();
   SpdVertexRC* v = new ((*fVerticesRC)[size]) SpdVertexRC();
   v->SetId(size);
   v->SetVertexType(2);
   return v;
}

//________________________________________________________________________________________  
void SpdRCKFpartV0Finder::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdKFpartV0Fitter::Finish> Fitted secondary vertices: "   
            << fNTotalSecFittedVertices << endl;
   }
}



