// $Id$
// Author: artur   2021/02/16


//_____________________________________________________________________________
//
// SpdRCVerticesFinder
//_____________________________________________________________________________

#include "SpdRCVerticesFinder.h"
#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdVertexMC.h"
#include "SpdTrackMC.h"

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

ClassImp(SpdRCVerticesFinder)

//_____________________________________________________________________________
SpdRCVerticesFinder::SpdRCVerticesFinder():
// input
fEvent(0),fParticles(0),fTracks(0),
//output
fVertices(0),
//
fPropagator(0),
// options & cuts
fMinItsHits(2),
// primary
fBeamSigmaX(0.1),fBeamSigmaY(0.1),
fMinDzCluster(2.5),fMinChi2Cluster(32.0),
fVertexFitAngleCut(3.0),
// secondary
fFitSecondaries(true),fTrackSelOption(1),
fUsedTypeOfPV(0),fConstrainToPV(1),
fMinChi2PV(2.0),fMaxDist(0.075),
fMinMass(0.00),fMaxMass(2.50),
fFirstDaughter(211),fSecondDaughter(211),
// to save data
fSaveVertices(true),
// counters
fNFittedVertices(0),fNTotalFittedVertices(0),
fNTotalPrimFittedVertices(0),fNTotalSecFittedVertices(0),
// verbosity
fVerboseLevel(1)
{
fBeamLinePos.SetXYZ(0.,0.,0.); 
fBeamLineDir.SetXYZ(0.,0.,1.);
}

//_____________________________________________________________________________
SpdRCVerticesFinder::~SpdRCVerticesFinder() 
{
   if (fPropagator) delete fPropagator;
}

//_____________________________________________________________________________
InitStatus SpdRCVerticesFinder::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdRCVerticesFinder::Init> RootManager not instantiated " << endl;
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
   
   //============== CREATE AND REGISTER OUTPUT DATA OBJECTS ================/
   
   fVertices = new TClonesArray("SpdVertexRC");
   ioman->Register(SpdDataBranchName(kRCVertices),"SpdVertexRC", fVertices, fSaveVertices);
   
   //============== INIT TRACK PROPAGATOR ===================================/
   
   fPropagator = new SpdTrackPropagatorGF();
   if (!fPropagator->Init()) {
       cout << "-F- <SpdRCVerticesFinder::Init> "
            << "Track propagator not initialized properly" << endl;
       return kFATAL;
   }
   
   //============== PASS DATA TO EVENTHELPER ================================/   

   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
  
   helper->SetVerticesRC(fVertices);
   
   if (!helper->GetEvent()) helper->SetEvent(fEvent);
   if (!helper->GetParticles()) helper->SetParticles(fParticles);
   if (!helper->GetTracks()) helper->SetTracks(fTracks);
   
   if (fVerboseLevel > -1) cout << "-I- <SpdRCVerticesFinder::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
SpdVertexRC* SpdRCVerticesFinder::GetPrimaryVertex() //private
{
   SpdVertexRC* v = (SpdVertexRC*)fVertices->At(0); 
   if (v && v->IsPrimary()) return v;
   return 0;
}

//________________________________________________________________________________________
Bool_t SpdRCVerticesFinder::GetPrimMCVertexPos(TVector3& v) //private
{
   if (!fParticles) return false;
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
void SpdRCVerticesFinder::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fTracks);

   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdRCVerticesFitter::Exec> Process event ... " 
            << fEvent->GetEventId();// << "\n\n";
       if (fVerboseLevel > 1) cout << "\n";          
   }
   
   fNFittedVertices = 0;
   
   fVertices->Delete();
   
   if (FindPrimaryVertex()) {
       fNFittedVertices++;
       fNTotalPrimFittedVertices++;
   }
   else {
       cout << "\n-W- <SpdRCVerticesFinder::Exec> Primary vertex not fitted " << endl;
       return;
   }
   
   Int_t nsec = 0;
   
   if (fFitSecondaries) 
   {
       nsec = FindSecondaryVertices();
    
       fNFittedVertices += nsec;
       fNTotalSecFittedVertices += nsec;
   }
   
   fNTotalFittedVertices += fNFittedVertices;
   
   if (fVerboseLevel > 1) 
   { 
       cout << "\n-I- <SpdRCVerticesFinder::Exec> Print ... " << endl; 

       SpdMCEventHelper::Instance()->PrintVerticesRC();

       cout << endl;
       cout << "Vertices (fitted:prim,sec):   " << fNFittedVertices 
            << ": " << 1 << ", " << fNFittedVertices-1 << endl;    
   }
}

//________________________________________________________________________________________
Bool_t SpdRCVerticesFinder::FindPrimaryVertex()
{
   Double_t theta_min = fVertexFitAngleCut*TMath::DegToRad();
       
   SpdTrackMC* track;
   SpdTrackFitPar* tpars;  
   const SpdTrackState* state;
    
   //-------------------------------------------
   // Select tracks
   //-------------------------------------------
   
   std::vector<SpdTrackMC*> tracks;
   
   for (Int_t i(0); i<fTracks->GetEntriesFast(); i++) 
   {
        track = (SpdTrackMC*)fTracks->At(i);
        if (!track) continue;
       
        // 1st cut 
        if (track->GetNHitsIts() < fMinItsHits) continue;
    
        // 2nd cut 
        if (!track->GetIsFitted()) continue;
   
        tpars = track->GetFitPars();
        
        // WARNING 3rd cut : hard cut! WARNING
        if (!CheckTrack(tpars,1)) {
            // cout << " >>>>>>>>>>>>>> 2nd cut " << endl; 
            // track->Print("fitpars"); 
            continue;
        }
        
        // 4th cut
        state = tpars->GetFirstState(); 
        if (TMath::Abs(state->GetMomentum().Theta()-TMath::PiOver2()) < theta_min) {
            //TVector3 mom = state->GetMomentum();
            //Double_t th = mom.Theta()*TMath::RadToDeg(); 
            //cout << " >>>>>>>>>>>>>> 3rd cut; theta: " << th 
            //     << " momentum: " << mom.X() << " " << mom.Y() << " " << mom.Z() << endl; 
            //track->Print("fitpars"); 
            continue; 
        }
   
        tracks.push_back(track);
        
        //cout << " \t... has been added "  << endl;
   }
      
   // WARNING 5th cut
   if (tracks.size() < 2) return false;
   
   SpdVertexRC* vertex = AddVertex(0); // add vertex as primary
   
   // add tracks to the vertex 
   for (auto tt : tracks) vertex->AddTrack(tt->GetId());
   
   //-------------------------------------------
   // >>>>>>>>>>> Fit Vertex <<<<<<<<<<<<
   //-------------------------------------------
   SpdPrimVertexFitPar* vpars = dynamic_cast<SpdPrimVertexFitPar*>(vertex->FitPars());
   assert(vpars);
   
   vpars->SetNTracks(tracks.size());
   
   //cout << "-I-<FitPrimaryVertex> " << "vertex: " << vertex->GetId() << " tracks: " << tracks.size() << endl;
   
   //------------------------------------------
   // vertex fit algorithm for CBM experiment
   //------------------------------------------
   
   // Iterative fit of the vertex (CDM)
   //----------------------------------
   const Double_t CutChi2 = 3.5*3.5;
   const Int_t MaxIter = 5; //3

   // Vertex state vector and covariance matrix
   //-------------------------------------------
   Double_t zSeed = FindPrimVertexCand();
   Double_t r[3] = {0,0,zSeed}; 
   Double_t C[6];
   
   for(Int_t i = 0; i<6; i++) C[i] = 0;
   C[0] = C[2] = 5.;
   C[5] = 0.25;
  
   // Chi^2 & NDF
   //-------------------------------------------
   Int_t    refNDF;
   Double_t refChi2;
   
   Bool_t is_fitted = false;
   
   // Iterative fit of the vertex
   //-----------------------------
   for (Int_t iteration = 0; iteration < MaxIter; ++iteration)
   {
      // Store the vertex from the previous iteration
      //---------------------------------------------
      Double_t r0[3], C0[6];
      for( Int_t i=0; i<3; i++ ) r0[i] = r[i];
      for( Int_t i=0; i<6; i++ ) C0[i] = C[i];

      // Initialize the vertex covariance, Chi^2 & NDF
      //-----------------------------------------------
      C[0] = 100.;
      C[1] =   0.; C[2] = 100.;
      C[3] =   0.; C[4] =   0.; C[5] = 100.;

      refNDF = -3;
      refChi2 = 0.0;
            
      //TMatrixDSym vCov;
      TVector3 vPos; //, vMom;
      
      //-----------------------
      // start loop over tracks
   
      for (const SpdTrackMC* t : tracks) 
      {
           tpars = t->GetFitPars();
           state = tpars->GetFirstState();
           
           // init propagator 
           fPropagator->InitTrack(t->GetParticlePdg(),1);
       
           TVector3 planePos(0.0, 0.0, r0[2]);
           TVector3 planeDir(0.0, 0.0, 1.0);
           
           /*----------------------------------------------*/
           // ATTENTION   Track extrapolation    ATTENTION
           /*----------------------------------------------*/
   
           SpdTrackState stateOut = *state;
           stateOut.SetTime(0);
           
           try { fPropagator->ExtrapolateToPlane(planePos,planeDir,stateOut); }
           catch (genfit::Exception& e) { continue; }
           
           /*----------------------------------------------*/
           /* ATTENTION Create final track state ATTENTION */
           /*----------------------------------------------*/
           
           if (iteration == MaxIter-1) {
               
               is_fitted = true;
               
               //cout << ">>>>>>>>>>>>>>>>> Track id: " 
               //     << t->GetId() << " iteration: " << iteration << " z = " << r0[2] 
               //     << " z0 = " << vertex->GetZ() << endl;
           }
           
           vPos = stateOut.GetPosition();
           
           const TVectorD& state5  = fPropagator->GetState5();  
           const TMatrixDSym& cov5 = fPropagator->GetStateCov5x5();
           
           TVector3 vMU(-state5[1], -state5[2], 1.0);

           Double_t vV[10] = { cov5[3][3], cov5[3][4], cov5[4][4],
                               cov5[1][3], cov5[1][4], cov5[1][1],
                               cov5[2][3], cov5[2][4], cov5[2][1],
                               cov5[2][2] };
                               
           Double_t a = 0, b = 0;
           {
              Double_t zeta[2] = { r0[0]-vPos.X(), r0[1]-vPos.Y() };

              // Check the track Chi^2 deviation from the r0 vertex estimate    
              //-------------------------------------------------------------
              Double_t S[3] = { (C0[2]+vV[2]), -(C0[1]+vV[1]), (C0[0]+vV[0]) };
              Double_t s = S[2]*S[0] - S[1]*S[1];
              Double_t chi2 = zeta[0]*zeta[0]*S[0] + 2*zeta[0]*zeta[1]*S[1] 
                                                 +   zeta[1]*zeta[1]*S[2];
              // std::cout << " chi2 = " << chi2 << std::endl;

              if ( chi2 > s*CutChi2 ) continue;

              // Fit of the vertex track slopes (a,b) to the r0 vertex estimate
              //-----------------------------------------------------------------
              s = vV[0]*vV[2] - vV[1]*vV[1];
              
              if ( s < 1.E-20 ) continue;
              
              s = 1./s;
           
              a = vMU.X() + s*( ( vV[3]*vV[2] - vV[4]*vV[1])*zeta[0]
                          +     (-vV[3]*vV[1] + vV[4]*vV[0])*zeta[1] );       

              b = vMU.Y() + s*( ( vV[6]*vV[2] - vV[7]*vV[1])*zeta[0]
                          +     (-vV[6]*vV[1] + vV[7]*vV[0])*zeta[1] );       
           }        

           // std::cout << " a = " << a << " b = " << b << std::endl; 

           // Update the vertex (r,C) with the track estimate (m,V) :
           //--------------------------------------------------------

           // Linearized measurement matrix H = { { 1, 0, -a}, { 0, 1, -b} };
           //-----------------------------------------------------------------

           // Residual zeta (measured - estimated)
           //--------------------------------------
           Double_t zeta[2] = { vPos.X() - ( r[0] - a*(r[2]-r0[2]) ),
                                vPos.Y() - ( r[1] - b*(r[2]-r0[2]) ) };

           // std::cout << " zeta[0] = " << zeta[0] << " zeta[1] = " << zeta[1]
           //           << " zeta[2] = " << zeta[2] << std::endl;

           // CHt = CH’
           //----------
           Double_t CHt[3][2] = { { C[0] - a*C[3], C[1] - b*C[3]},
                                  { C[1] - a*C[4], C[2] - b*C[4]},
                                  { C[3] - a*C[5], C[4] - b*C[5]} };

           // S = (H*C*H’ + V )^{-1}
           //-------------------------
           Double_t S[3] = { vV[0] + CHt[0][0] - a*CHt[2][0],
                             vV[1] + CHt[1][0] - b*CHt[2][0],
                             vV[2] + CHt[1][1] - b*CHt[2][1] };

           // Invert S
           //----------
           { 
              Double_t s = S[0]*S[2] - S[1]*S[1];
              if (s < 1.E-20) continue;
              s = 1./s;
              Double_t S0 = S[0];
              S[0] = s*S[2];
              S[1] = -s*S[1];
              S[2] = s*S0;
           }

           // Calculate Chi^2
           //-----------------
           refChi2 += zeta[0]*zeta[0]*S[0] + 2*zeta[0]*zeta[1]*S[1]
                                           +   zeta[1]*zeta[1]*S[2];
           refNDF += 2; 

           // std::cout << " refChi2 = " << refChi2 << " refNDF = " << refNDF << std::endl; 

           // Kalman gain K = CH’*S
           //------------------------
           Double_t K[3][2];
           for (Int_t i(0); i<3; ++i) { K[i][0] = CHt[i][0]*S[0] + CHt[i][1]*S[1] ; 
                                        K[i][1] = CHt[i][0]*S[1] + CHt[i][1]*S[2] ; }

           // New estimation of the vertex position r += K*zeta 
           //---------------------------------------------------
           for (Int_t i(0); i<3; ++i) r[i] += K[i][0]*zeta[0] + K[i][1]*zeta[1];

           //std::cout << " trk = " << t->GetId() << " r[0] = " << r[0] << " r[1] = " << r[1]
           //           << " r[2] = " << r[2] << std::endl;

           // New covariance matrix C -= K*(CH’)’
           //--------------------------------------
           C[0] -= K[0][0]*CHt[0][0] + K[0][1]*CHt[0][1];
           C[1] -= K[1][0]*CHt[0][0] + K[1][1]*CHt[0][1];
           C[2] -= K[1][0]*CHt[1][0] + K[1][1]*CHt[1][1];
           C[3] -= K[2][0]*CHt[0][0] + K[2][1]*CHt[0][1];
           C[4] -= K[2][0]*CHt[1][0] + K[2][1]*CHt[1][1];
           C[5] -= K[2][0]*CHt[2][0] + K[2][1]*CHt[2][1];                    
      }
   }
                
   if (!is_fitted) return false; 

   // Fill fit parameters

   vpars->SetVertex(r[0],r[1],r[2]);
   
   TMatrixDSym VC(3);
   VC(0,0) = C[0]; VC(0,1) = C[1]; VC(0,2) = C[3];
   VC(1,0) = C[1]; VC(1,1) = C[2]; VC(1,2) = C[4]; 
   VC(2,0) = C[3]; VC(2,1) = C[4]; VC(2,2) = C[5];
   
   vpars->SetCovariance(VC);
   
   vpars->SetChi2(refChi2);
   vpars->SetNDF(refNDF);
   
   //vertex->Print("fitpars"); 
     
   return true;  
}
    
//________________________________________________________________________________________    
Int_t SpdRCVerticesFinder::FindSecondaryVertices()
{
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

   TVector3 pvVtx = recVtx;   // reconstructed PV
   
   if (fUsedTypeOfPV < 1) {  //WARNING
       // try to get simulated PV
       if (!GetPrimMCVertexPos(pvVtx)) return 0; 
   }
   //cout << pvVtx.X() << " " << pvVtx.Y() << " " << pvVtx.Z() << " " << endl;
   
   // set field
   //-----------
   Float_t fieldBz = 0;
    
   SpdField* field = fPropagator->GetField();
   if (field) fieldBz = field->GetBz(pvVtx.X(),pvVtx.Y(),pvVtx.Z());
    
   KFParticle::SetField(fieldBz);    
   
   // set Primary Vertex (PV) as KFParticle 
   //------------------------------
   KFPVertex vert;
   vert.SetXYZ( pvVtx.X(), pvVtx.Y(), pvVtx.Z() );
   vert.SetCovarianceMatrix( (float)recCov(0,0),
                             (float)recCov(1,0), (float)recCov(1,1),
                             (float)recCov(2,0), (float)recCov(2,1), (float)recCov(2,2) );

   vert.SetNContributors(vprimpar->GetNTracks());
   vert.SetChi2(vprimpar->GetChi2());

   KFParticle p0(vert); 
   
   // select track and set track parameters
   //---------------------------------------
   SpdTrackMC* track;
   SpdTrackFitPar* tpars;  
   const SpdTrackState* state;
   
   std::vector<KFParticle> KFparticles;
   std::vector<Int_t> SelectedTracks;
  
   Int_t pdgTrk;
   for (Int_t i(0); i<fTracks->GetEntriesFast(); i++) 
   {
        track = (SpdTrackMC*)fTracks->At(i);
        if (!track) continue;
        
        //cout << " track_id = " << track->GetId() << endl;
    
        // check track
        //---------------------------------------------------
        // 1st cut 
        if (track->GetNHitsIts() < fMinItsHits) continue;
        
        // 2nd cut 
        if (!track->GetIsFitted()) continue;
    
        // WARNING 3rd cut : hard cut! WARNING
        tpars = track->GetFitPars();
   
        if (!CheckTrack(tpars,fTrackSelOption)) {
           // cout << " >>>>>>>>>>>>>> 2nd cut " << endl; 
           // track->Print("fitpars"); 
           continue;
        }
        
        pdgTrk = track->GetParticlePdg();
       
        TParticlePDG* part = TDatabasePDG::Instance()->GetParticle(pdgTrk);
        if (!part) continue;
        
        // FIXME check only first or second daugter particle types 
        //---------------------------------------------------
        if (fabs(pdgTrk) != fFirstDaughter && fabs(pdgTrk) != fSecondDaughter) continue;
  
        state = tpars->GetFirstState();

        // set KF track parameters
        //------------------------ 
        
        // get track parameters at first hit position
        //-------------------------------------------
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
        
        if (p1.GetDeviationFromVertex(p0) < fMinChi2PV) continue; 
         
        KFparticles.push_back(p1);
        
        SelectedTracks.push_back(track->GetId());
        
        //cout << "id/pdg: "<< track->GetId() << "/" << pdgTrk  << " ... ok " << endl;
   }
   
   if (KFparticles.size() < 2) return 0; //  number of KF particles >= 2
   
   // check vector with daughter candidates
   //--------------------------------------
   Int_t nv = 0;

   for (int i=0; i<KFparticles.size()-1; ++i) 
   {
      for (int j=i+1; j<KFparticles.size(); ++j) 
      { 
         // FIXME
         if ((int)KFparticles[i].GetQ()*(int)KFparticles[j].GetQ() > 0) continue;  

//          if ( !( (KFparticles[i].GetPDG()==fFirstDaughter   && 
//                   KFparticles[j].GetPDG()==fSecondDaughter) ||
//                  (KFparticles[i].GetPDG()==fSecondDaughter  && 
//                   KFparticles[j].GetPDG()==fFirstDaughter) ) ) continue;
         // FIXME
         if ( ( fabs(KFparticles[i].GetPDG())==fFirstDaughter    && 
                fabs(KFparticles[j].GetPDG())!=fSecondDaughter ) ||
              ( fabs(KFparticles[i].GetPDG())==fSecondDaughter   && 
                fabs(KFparticles[j].GetPDG())!=fFirstDaughter )   ) continue;

         // distance between two daughter particles
         //----------------------------------------
         float dist2part = KFparticles[i].GetDistanceFromParticle(KFparticles[j]);

         if (dist2part > fMaxDist) continue;      // check distance

         // parameter 1 - array of the daughter particles
         // parameter 2 - number of the daughter particles
         // parameter 3 - vertex (it should be the object of the KFParticle class)

         const KFParticle pVertex = p0;
         int   NDaughters = 2;
         const KFParticle *vDaughters[2] = { &KFparticles[i], &KFparticles[j] };

         // construct decay V0 particle without PV and mass constrain
         //------------------------------------------------------------
         KFParticle K0_1;
         K0_1.SetProductionVertex(pVertex);
         
         if (fConstrainToPV < 1) K0_1.Construct(vDaughters,NDaughters,0,-1);
         else                    K0_1.Construct(vDaughters,NDaughters,&pVertex,-1);

         TVector3 v1;
         v1.SetXYZ(K0_1.GetX(), K0_1.GetY(), K0_1.GetZ());  // decay vertex

         float mass    = K0_1.GetMass();
         float massErr = K0_1.GetErrMass();
         float mom     = K0_1.GetP();
         float ene     = K0_1.GetE();
         float eta     = K0_1.GetRapidity();
         float theta   = K0_1.GetTheta();
         float phi     = K0_1.GetPhi();
         float length  = (fConstrainToPV < 1) ? (v1-pvVtx).Mag() : K0_1.GetDecayLength();

         // check V0 mass window
         //----------------------
         if (mass < fMinMass || mass > fMaxMass) continue;  // mass window
         
         // Create new vertex
         //----------------------------------
         
         // add vertex as secondary with fit parameters of type = 1 ("SpdVertexFitPar")
         SpdVertexRC* vertex = AddVertex(1); 
         
         vertex->AddTrack(SelectedTracks[i]);
         vertex->AddTrack(SelectedTracks[j]);
   
         // Create fit parameters
         //----------------------------------
    
         SpdVertexFitPar* vpar = dynamic_cast<SpdVertexFitPar*>(vertex->FitPars());
         assert(vpar);
    
         nv++;
         
         vpar->SetMass(mass);        
         vpar->SetMassErr(massErr);  
         vpar->SetP(mom);            
         vpar->SetEnergy(ene);       
         vpar->SetEta(eta);     
         vpar->SetTheta(theta); 
         vpar->SetPhi(phi);     
         vpar->SetLength(length);   
         
         //vertex->Print("fitpars"); //FIXME
      }
   } 
   
   return nv; 
}

//________________________________________________________________________________________  
SpdVertexRC* SpdRCVerticesFinder::AddVertex(Int_t vtype) 
{
   if (!fVertices) return 0 ;
   Int_t size = fVertices->GetEntriesFast();
   SpdVertexRC* v = new ((*fVertices)[size]) SpdVertexRC();
   v->SetId(size);
   v->SetVertexType(vtype);
   return v;
}

//________________________________________________________________________________________  
Double_t SpdRCVerticesFinder::FindPrimVertexCand()
{
  /** The function provides simple estimation z-coordinate of primary vertex  
   ** with beam-line constrain as seed for PV fitting program: \n
   ** 1) it uses as an input an array with tracks;\n
   ** 2) select good fitted tracks; \n
   ** 3) 1-st step - input particles are transported to the beam-line; \n
   ** 4) the weight for each particle is calculated according to its errors,
   ** if errors are not defined after extrapolation or if particle more then 10*sigma
   ** away from the beam-line the track is rejected from this analysis; \n
   ** 6) 2-th step - construct z-cluster of tracks around the track with best weight; \n
   ** 7) tracks are checked with chi2 deviation from z-coordinate of best-track 
   **    within fMinChi2Cluster; \n
   ** 8) finally, mean cluster z-position is estimated; \n
   **/

   if (fVerboseLevel > 2) cout << "\n-I- <SpdRCVerticesFitter::FindPrimVertexCand> ";

   SpdTrackMC* track;
   SpdTrackFitPar* tpars;  
   const SpdTrackState* state;
    
   //-------------------------------------------
   // Select tracks
   //-------------------------------------------
   Double_t tmpZ = 0.0;
   Double_t sumW = 0.0;

   std::vector<Double_t>    fZpos;
   std::vector<Double_t>    fZcov;
   std::vector<Double_t>    fWeight;

   //std::vector<SpdTrackMC*> tracks;
   
   for (Int_t i(0); i<fTracks->GetEntriesFast(); i++) 
   {
       track = (SpdTrackMC*)fTracks->At(i);
       if (!track) continue;
       
       // 1st cut 
       if (track->GetNHitsIts() < fMinItsHits) continue;
    
       // 2nd cut 
       if (!track->GetIsFitted()) continue;
   
       // WARNING 3rd cut WARNING
       tpars = track->GetFitPars();
       
       if (!CheckTrack(tpars,1)) {
           //cout << " >>>>>>>>>>>>>> 3rd cut " << endl; 
           //track->Print("fitpars"); 
           continue;
       }
       
       // extrapolate track to beam-line
       //--------------------------------
       fPropagator->InitTrack(track->GetParticlePdg(),1);
           
       state = tpars->GetFirstState();
       SpdTrackState stateOut = *state;

       try 
       {
          fPropagator->ExtrapolateToLine(fBeamLinePos,fBeamLineDir,stateOut); 
       }
       catch (genfit::Exception& e) 
       { 
          cout << "\n-E- <SpdRCVerticesFinder::FindPrimVertexCand> bad track extrapolation "; 
          continue; 
       }

       // get track parameters at the point of closest approach to beam-line (POCA)
       //--------------------------------------------------------------------------
       TVector3 trkPos = stateOut.GetPosition();
       const TMatrixDSym& trkCov = stateOut.GetCovariance();

       Double_t r = sqrt(trkPos.X()*trkPos.X() + trkPos.Y()*trkPos.Y());
       Double_t sigmaXY = sqrt(fBeamSigmaX*fBeamSigmaX + fBeamSigmaY*fBeamSigmaY);

       if (r > 10.*sigmaXY) continue;    // far away from beam-line (10*sigma)

       Double_t tmpW = trkCov(0,0) + trkCov(1,1) + trkCov(2,2);  // track's weight at POCA
       if (!(tmpW > 0)) continue; // unexpected error :)       

       // fill vectors       
       //tracks.push_back(track);    // add selected track

       tmpW  = 1.0/sqrt(tmpW);
       tmpZ += tmpW*trkPos.Z();
       sumW += tmpW;       
       
       fWeight.push_back(tmpW);
       fZpos.push_back(trkPos.Z());
       fZcov.push_back(trkCov(2,2));
   }

//   cout << " tracks.size() = " << tracks.size() << endl;
      
   // WARNING 4rd cut
//   if (tracks.size() < 2) return 0.0;
//   if (fWeight.empty())   return 0.0;
   
   if (fWeight.size() < 2) return 0.0;
       
   tmpZ = (sumW > 0.0) ? tmpZ/sumW : 0.0;  // 1-st z-vertex estimation

   // find track ("best" track) with maximum weight and worst track
   //--------------------------------------------------------------
   Int_t    bestTrk, worstTrk;
   Double_t bestW, maxDz;
   Int_t maxIter = 3;
   for (Int_t iter=0; iter<maxIter; ++iter)              // loop over iteration 
   {
      bestTrk = worstTrk = -1;
      bestW = maxDz = 0.0; 
      for (Int_t itrk=0; itrk<fWeight.size(); ++itrk)
      {
         if (abs(fZpos[itrk]-tmpZ) > maxDz)   // worst track
         {
            maxDz = abs(fZpos[itrk]-tmpZ);
            worstTrk = itrk;
         }

         if (abs(fZpos[itrk]-tmpZ) < fMinDzCluster && fWeight[itrk] > bestW)
         {
            bestTrk = itrk;
            bestW   = fWeight[itrk];
         } 
      }
      if (bestTrk >= 0) break;

      Double_t newZ = 0.0;
      Double_t newW = 0.0;
      for (Int_t itrk=0; itrk<fWeight.size(); ++itrk)  // mean cluster without worst track
      {
         if (itrk == worstTrk) continue;

         newZ += fWeight[itrk]*fZpos[itrk];
         newW += fWeight[itrk];
      }

      tmpZ = (newW > 0.0) ? newZ/newW : 0.0;  // 2-nd z-vertex estimation 
   }
   
   if (bestTrk < 0) return 0.0;

   // estimation of z-cluster position around "best" track
   //------------------------------------------------------ 
   tmpZ = 0.0;
   sumW = 0.0;
   for (Int_t itrk=0; itrk<fWeight.size(); ++itrk)
   {
      Double_t tmpChi2 = (fZpos[bestTrk]-fZpos[itrk]) *
                         (fZpos[bestTrk]-fZpos[itrk]) / (fZcov[bestTrk] + fZcov[itrk]);

      if (abs(fZpos[bestTrk]-fZpos[itrk])<0.5*fMinDzCluster && tmpChi2<fMinChi2Cluster)   
      {
         tmpZ  += fWeight[itrk] * fZpos[itrk];
         sumW  += fWeight[itrk];
      }   
   }

   return tmpZ = (sumW > 0.0) ? tmpZ/sumW : 0.0;
}

//________________________________________________________________________________________
Bool_t SpdRCVerticesFinder::CheckTrack(SpdTrackFitPar* pars, Int_t opt)
{
   if (!pars)    return false;
   if (opt < 1)  return true;
   if (opt == 1) return (pars->GetChi2() <= 0.0 || pars->GetChi2overNDF() > 12.0) ? false : true;
   return pars->GetIsGood();
}

//________________________________________________________________________________________  
void SpdRCVerticesFinder::Finish()
{
   if (fVerboseLevel > -1) {
       cout << "\n-I- <SpdRCVerticesFitter::Finish> vertices (fitted) [primary,secondary]: " << fNTotalFittedVertices  
            << " [" << fNTotalPrimFittedVertices << ", " << fNTotalSecFittedVertices << "] ";//<< endl;
   }
}




