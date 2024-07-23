// $Id$
// Author: artur   2021/02/08


//_____________________________________________________________________________
//
// SpdMCVerticesFitter
//_____________________________________________________________________________

#include "SpdMCVerticesFitter.h"
#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdVertexMC.h"
#include "SpdTrackMC.h"

#include "SpdDetectorList.h"
#include "SpdVertexFitPar.h"
#include "SpdTrackPropagatorGF.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"

#include "SpdField.h"

#include "KFParticle.h"
#include "KFPTrack.h"
#include "KFPVertex.h"
#include "KFParticleSIMD.h"

#include "Exception.h"

#include <TDatabasePDG.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMCVerticesFitter)

//_____________________________________________________________________________
SpdMCVerticesFitter::SpdMCVerticesFitter():
fPropagator(0),fEvent(0),fParticles(0),fVertices(0),fTracks(0),
fFitSecondaries(true),
fVertexFitAngleCut(3.0),
fNFittedVertices(0),fNTotalFittedVertices(0),
fNTotalPrimFittedVertices(0),fNTotalSecFittedVertices(0),
fVerboseLevel(1)
{

}

//_____________________________________________________________________________
SpdMCVerticesFitter::~SpdMCVerticesFitter() 
{
   if (fPropagator) delete fPropagator;
}

//_____________________________________________________________________________
InitStatus SpdMCVerticesFitter::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdMCVerticesFitter::Init> RootManager not instantiated " << endl;
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
   
   // --- get Tracks array ---
    
   fTracks = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTracks));
   if (!fTracks) return kFATAL; 
   
   //============== INIT TRACK PROPAGATOR ===================================/
   
   fPropagator = new SpdTrackPropagatorGF();
   if (!fPropagator->Init()) {
       cout << "-F- <SpdMCVerticesFitter::Init> "
            << "Track propagator not initialized properly" << endl;
       return kFATAL;
   }
      
   //============== PASS DATA TO EVENTHELPER ================================/   

   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
   if (!helper->GetEvent()) helper->SetEvent(fEvent);
   if (!helper->GetParticles()) helper->SetParticles(fParticles);
   if (!helper->GetVertices()) helper->SetVertices(fVertices);
   if (!helper->GetTracks()) helper->SetTracks(fTracks);
 
   if (fVerboseLevel > -1) cout << "-I- <SpdMCVerticesFitter::Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
SpdVertexMC* SpdMCVerticesFitter::GetPrimaryVertex() // private 
{
   SpdVertexMC* v = (SpdVertexMC*)fVertices->At(0);
   if (v->IsPrimary()) return v;
   v = 0;
   for (Int_t i(1); i<fVertices->GetEntriesFast(); i++) {
        v = (SpdVertexMC*)fVertices->At(i);
        if (v && v->IsPrimary()) return v;
   }
   return 0;
}

//________________________________________________________________________________________
void SpdMCVerticesFitter::Exec(Option_t* opt) 
{
   assert(fEvent);
   assert(fParticles);
   assert(fVertices);
   assert(fTracks);
   
   if (fVerboseLevel > 0) {
       cout << "\n-I- <SpdMCVerticesFitter::Exec> Process event ... " 
            << fEvent->GetEventId();// << "\n\n";
       if (fVerboseLevel > 1) cout << "\n";          
   }
  
   fNFittedVertices = 0;
   
   SpdVertexMC* vertex;
   
   vertex = GetPrimaryVertex();
   if (!vertex) return;
      
   // >>>>>>>>>>>>> fit primary vertex
   
   Bool_t fit_prim = false; 
   if (FitPrimaryVertex(vertex)) {
       fNFittedVertices++;
       fNTotalPrimFittedVertices++;
       fit_prim = true;
        //vertex->Print("fitpars"); // FIXME
       SearchForFinalState(vertex);
   }
   else {
       cout << "\n-W- <SpdMCVerticesFitter::Exec> Primary vertex not fitted " << endl;
       return;
   }
   
   Int_t nv = fVertices->GetEntriesFast();
   
   // >>>>>>>>>>>>> fit secondary vertices
   
   if (fFitSecondaries) {
       for (Int_t i(0); i<nv; i++) {
            vertex = (SpdVertexMC*)fVertices->At(i); 
            if (!vertex) continue;
            if (vertex->IsPrimary()) continue;
            if (FitSecondaryVertex(vertex)) {
                fNFittedVertices++;
                fNTotalSecFittedVertices++;
                //vertex->Print("fitpars"); // FIXME
            }
       }
   }
   
   fNTotalFittedVertices += fNFittedVertices;
   
   if (fVerboseLevel > 1) 
   { 
       cout << "\n-I- <SpdMCVerticesFitter::Exec> Print ... " << endl; 
   
       SpdMCEventHelper::Instance()->PrintVertices();

       cout << endl;
       cout << "Vertices (total):             " << nv << endl;
       cout << "Vertices (fitted:prim,sec):   " << fNFittedVertices 
            << ": " << 1 << ", " << fNFittedVertices-1 << endl;
   }
}

//________________________________________________________________________________________
Bool_t SpdMCVerticesFitter::FitPrimaryVertex(SpdVertexMC* vertex)
{
    vertex->DeleteFitPars();
    
    if (!vertex->HasOut()) return false; 
    
    Double_t theta_min = fVertexFitAngleCut*TMath::DegToRad();
        
    const std::set<Int_t>& parts = vertex->GetOutParticles();
    
    Int_t track_id;
    SpdTrackMC* track;
    SpdTrackFitPar* tpars;  
    const SpdTrackState* state;
     
    //-------------------------------------------
    // Select tracks
    //-------------------------------------------
    
    std::vector<SpdTrackMC*> tracks;
    Int_t ntracks = 0;
    
    if (fVerboseLevel > 3) cout << "primary: select tracks ... " << endl;
    
    for (const Int_t& i : parts) 
    {
         track_id = ((SpdMCParticle*)fParticles->At(i))->GetTrackId();
         if (track_id < 0) continue;
         
         if (fVerboseLevel > 3) cout << "primary: track_id = " << track_id << endl;
         
         ntracks++;
         track = (SpdTrackMC*)fTracks->At(track_id);
         
         // 1st cut 
         if (!track || !track->GetIsFitted()) {
             if (fVerboseLevel > 3) cout << "\t >>>>>>>>>>>>>> 1st cut " << endl;
             continue;
         }
         
         // WARNING 2nd cut : hard cut! WARNING
         tpars = track->GetFitPars();
         if (!tpars || !tpars->GetIsGood()) {
             if (fVerboseLevel > 3) cout << "\t >>>>>>>>>>>>>> 2nd cut " << endl; 
             //track->Print("fitpars"); 
             continue; 
         }
         
         // 3rd cut
         state = tpars->GetFirstState(); 
         if (TMath::Abs(state->GetMomentum().Theta()-TMath::PiOver2()) < theta_min) {
             if (fVerboseLevel > 3) {
                 TVector3 mom = state->GetMomentum();
                 Double_t th = mom.Theta()*TMath::RadToDeg(); 
                 cout << "\t >>>>>>>>>>>>>> 3rd cut; theta: " << th 
                      << " momentum: " << mom.X() << " " << mom.Y() << " " << mom.Z() << endl; 
             }
             //track->Print("fitpars"); 
             continue; 
         }
         
         if (fVerboseLevel > 3) cout << "\t >>>>>>>>>>>>>> accepted " << endl;
         
         tracks.push_back(track);
    }
    
    //ATTENTION add vertex info ATTENTION
    vertex->SetNTracks(ntracks); 
   
    // WARNING 4rd cut
    if (tracks.size() < 2) return false;
    
    //-------------------------------------------
    // >>>>>>>>>>> Fit Vertex <<<<<<<<<<<<
    //-------------------------------------------
    
    if (fVerboseLevel > 3) {
        cout << "primary: +++++ fit vertex ... " << endl; 
    }
    
    SpdPrimVertexFitPar* vpars = dynamic_cast<SpdPrimVertexFitPar*>(vertex->FitPars());
    
    assert(vpars);
    
    vpars->SetNTracks(tracks.size());
    
    //cout << "-I-<FitPrimaryVertex> " << "vertex: " << vertex->GetId() << " tracks: " << tracks.size() << endl;
    
    //------------------------------------------
    // vertex fit algorithm for CBM experiment
    //------------------------------------------
    
    SpdVertexMC* vprim = GetPrimaryVertex();
    
    // Iterative fit of the vertex (CDM)
    //----------------------------------
    const Double_t CutChi2 = 3.5*3.5;
    const Int_t MaxIter = 5; //3

    // Vertex state vector and covariance matrix
    //-------------------------------------------
    Double_t r[3] = {0,0,0}; 

    if (vprim) {
        r[0] = vprim->GetX();
        r[1] = vprim->GetY();
        r[2] = vprim->GetZ(); 
    }

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
             
       TVector3 vPos;
       
       //-----------------------
       // start loop over tracks
    
       for (SpdTrackMC* t : tracks) 
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
                
                SpdTrackState* final_state = tpars->FinalState();
                *final_state = stateOut;
                
                TVector3 d = vertex->GetPos()-final_state->GetPosition();
                final_state->SetPosDelta(d.Mag());
                final_state->SetTime(state->GetTime()+final_state->GetTime());
                
                //tpars->Print(); //FIXME
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
    
    if (fVerboseLevel > 3) {
        if (is_fitted) cout << "\t >>>>>>>>>>>>>> fitted " << endl; 
        else cout << "\t >>>>>>>>>>>>>> NOT fitted " << endl; 
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
      
    return true;
}

//________________________________________________________________________________________
Bool_t SpdMCVerticesFitter::FitSecondaryVertex(SpdVertexMC* vertex)
{   
    vertex->DeleteFitPars();
    
    if (!vertex->HasOut()) return false; 
    
    SpdVertexMC* vprim = GetPrimaryVertex();
    if (!vprim) return false;
    SpdPrimVertexFitPar* vprimpar = dynamic_cast<SpdPrimVertexFitPar*>(vprim->GetFitPars());
    if (!vprimpar) return false;
    
    const std::set<Int_t>& parts = vertex->GetOutParticles();
    
    Int_t track_id;
    SpdTrackMC* track;
    SpdTrackFitPar* tpars;  
    SpdTrackState* state;
     
    //-------------------------------------------
    // Select tracks
    //-------------------------------------------
    
    std::vector<SpdTrackMC*> tracks;
    Int_t ntracks = 0;
    
    if (fVerboseLevel > 3) {
        cout << "secondary (id = " << vertex->GetId() << "): select tracks ... " << endl;
    }
    
    for (const Int_t& i : parts) 
    {
         track_id = ((SpdMCParticle*)fParticles->At(i))->GetTrackId();
         if (track_id < 0) continue;
         
         if (fVerboseLevel > 3) cout << "secondary: track_id = " << track_id << endl;
         
         ntracks++;
         track = (SpdTrackMC*)fTracks->At(track_id);
         
         // 1st cut 
         if (!track || !track->GetIsFitted()) {
             if (fVerboseLevel > 3) cout << "\t >>>>>>>>>>>>>> 1st cut " << endl;
             continue;
         }
    
         // WARNING 2nd cut : hard cut! WARNING
         tpars = track->GetFitPars();
         if (!tpars || !tpars->GetIsGood()) {
             if (fVerboseLevel > 3) cout << "\t >>>>>>>>>>>>>> 2nd cut " << endl; 
             //track->Print("fitpars"); 
             continue; 
         }
    
         if (fVerboseLevel > 3) cout << "\t >>>>>>>>>>>>>> accepted " << endl;
         
         tracks.push_back(track);
    }
    
    //ATTENTION add vertex info ATTENTION
    vertex->SetNTracks(ntracks); 
   
    // WARNING 4rd cut
    if (tracks.size() < 2) return false;
        
    //>>>>>>>>>>> Fit Vertex <<<<<<<<<<<<
    if (fVerboseLevel > 3) {
        cout << "secondary: +++++ fit vertex ... id = " << vertex->GetId() << " tracks = " << tracks.size() << endl; 
    }
    
    SpdTrackFitPar* pars;

    // set primary vertex - PV (reconstructed or simulated)
    //-------------------------------------------------
    TVector3 pvVtx(vprim->GetPos());                  // simulated PV
    
    //TVector3 recVtx = vprimpar->GetVertex();               // reconstructed PV
    //const TMatrixDSym& recCov = vprimpar->GetCovariance(); // rec. PV covariance
    //pvVtx = recVtx;                                        // set PV vertex   
    
    // set field
    //-----------

    Float_t fieldBz = 0;
    
    SpdField* field = fPropagator->GetField();
    if (field) fieldBz = field->GetBz(pvVtx.X(),pvVtx.Y(),pvVtx.Z());
    
    KFParticle::SetField(fieldBz);              
    
    // track parameters
    //--------------------
   
    std::vector<KFParticle> KFparticles;
       
    Int_t trkPdg, trkCharge;
    TVector3 trkPos, trkMom;
    
    for (const SpdTrackMC* trk : tracks) 
    {
       // set KF track parameters
       //------------------------ 
    
       tpars = dynamic_cast<SpdTrackFitPar*>(trk->GetFitPars());
       if (!tpars) continue;
    
       state = tpars->GetFirstState();
       if (!state) continue;
       
       trkPdg = trk->GetParticlePdg();
       
       TParticlePDG* part = TDatabasePDG::Instance()->GetParticle(trkPdg);
       if (!part) continue;
       trkCharge = part->Charge()/3;
       
       trkPos = state->GetPosition();
       trkMom = state->GetMomentum();        
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
       
       kfTrack.SetCharge(trkCharge);

       KFParticle p1(kfTrack,trkPdg);        
       KFparticles.push_back(p1);
    }
    
    if (KFparticles.size() < 2) {
        if (fVerboseLevel > 3) cout << "\t >>>>>>>>>>>>>> n(KF-particles) < 2 " << endl; 
        return false;
    }
    //cout << "-I-<FitSecondaryVertex> (2) " << "vertex: " << vertex->GetId() << " particles: " << KFparticles.size() << endl;
    
    // construct production vertex
    //-----------------------------
    
    KFPVertex vert;
    
    vert.SetXYZ(pvVtx.X(),pvVtx.Y(),pvVtx.Z());
    //vert.SetCovarianceMatrix( (float)recCov(0,0),
    //                          (float)recCov(1,0), (float)recCov(1,1),
    //                          (float)recCov(2,0), (float)recCov(2,1), (float)recCov(2,2) ); 
    
    vert.SetNContributors(vprimpar->GetNTracks());
    vert.SetChi2(vprimpar->GetChi2());
    
    // set primary vertex as KFParticle
    const KFParticle pVertex(vert);
    
    // construct decay V0
    //--------------------
    
    const Int_t NDaughters = KFparticles.size();
    const KFParticle* vDaughters[NDaughters];
     
    Int_t i(0);
    for (const KFParticle& part : KFparticles) vDaughters[i++] = &part;
        
    //float dist2part = KFparticles[0].GetDistanceFromParticle(KFparticles[1]);    
    //cout << "distance: " << dist2part << endl;
    //
    //TParticlePDG* part1 = TDatabasePDG::Instance()->GetParticle(-3122);
    //TParticlePDG* part2 = TDatabasePDG::Instance()->GetParticle(-412);
    //cout << "MASS: " << part1->Mass() << " " << part2->Mass() << endl;
    
    // Create fit parameters
    //----------------------------------
    
    SpdVertexFitPar* vpar = dynamic_cast<SpdVertexFitPar*>(vertex->FitPars());
    if (!vpar) {
        if (fVerboseLevel > 3) cout << "\t >>>>>>>>>>>>>> no fit parameters" << endl;
        return false;
    }
    
    Bool_t ConstrainToPV = 0;
    
    KFParticle K0_1;
    K0_1.SetProductionVertex(pVertex);

    if (!ConstrainToPV)  K0_1.Construct(vDaughters,NDaughters,0,-1);
    else                 K0_1.Construct(vDaughters,NDaughters,&pVertex,-1);

    TVector3 v1(K0_1.GetX(),K0_1.GetY(),K0_1.GetZ());  // decay vertex

    float mass    = K0_1.GetMass();
    float massErr = K0_1.GetErrMass();
    float mom     = K0_1.GetP();
    float ene     = K0_1.GetE();
    float eta     = K0_1.GetRapidity();
    float theta   = K0_1.GetTheta();
    float phi     = K0_1.GetPhi();
    float length  = (!ConstrainToPV) ? (v1-pvVtx).Mag() :  K0_1.GetDecayLength();;
        
    vpar->SetMass(mass);        
    vpar->SetMassErr(massErr);  
    vpar->SetP(mom);            
    vpar->SetEnergy(ene);       
    vpar->SetEta(eta);     
    vpar->SetTheta(theta); 
    vpar->SetPhi(phi);     
    vpar->SetLength(length);    
    
    if (fVerboseLevel > 3) cout << "\t >>>>>>>>>>>>>> fitted " << endl; 
    
    return true;
}

//________________________________________________________________________________________  
void SpdMCVerticesFitter::SearchForFinalState(SpdVertexMC* vertex)
{
    if (!vertex->HasOut()) return; 
    
    SpdPrimVertexFitPar* vpars = dynamic_cast<SpdPrimVertexFitPar*>(vertex->FitPars());
    if (!vpars) return;
    
    const std::set<Int_t>& parts = vertex->GetOutParticles();
    
    Int_t track_id;
    SpdTrackMC* track;
    SpdTrackFitPar* tpars;  
    const SpdTrackState* state;
     
    //-------------------------------------------
    // Select tracks
    //-------------------------------------------
    
    std::vector<SpdTrackMC*> tracks;
    Int_t ntracks = 0;
    
    //cout << "\n primary tracks: "  << parts.size() << endl;
    
    for (const Int_t& i : parts) 
    {
         track_id = ((SpdMCParticle*)fParticles->At(i))->GetTrackId();
         
         //if (track_id < 0) cout << "\n no track for the particle ";
         if (track_id < 0) continue;
         
         ntracks++;
         track = (SpdTrackMC*)fTracks->At(track_id);
         
         //if (!track || !track->GetIsFitted()) cout << "\n no track (or track is not fitted): " << ntracks-1ж
         if (!track || !track->GetIsFitted()) continue;
         
         tpars = track->GetFitPars();
         
         //if (!tpars) cout << "\n no track fit pars: "  << ntracks-1;
         if (!tpars) continue; 
         
         //if (tpars->GetFinalState()) cout << "\n track already has final state: " << ntracks-1;
         if (tpars->GetFinalState()) continue;
         
         tracks.push_back(track);
         
         //cout << "\n search for final state: " << ntracks-1;
    }
    
    // get primary vertex
    TVector3 vtx = vpars->GetVertex();
    Int_t tpdg;
    
    for (SpdTrackMC* t : tracks) {
        
         tpdg  = t->GetParticlePdg(); 
         tpars = t->GetFitPars();
         state = tpars->GetFirstState();
        
         SpdTrackState stateOut = *state;
        
         fPropagator->InitTrack(tpdg);
        
         // extrapolate to primary vertex (sim or rec)
         //-------------------------------------------      
         try 
         {
            fPropagator->ExtrapolateToPoint(vtx,stateOut); 
         }
         catch (genfit::Exception& e) 
         { 
            cout << "-E- <SpdMCVerticesFinder::SearchForFinalState> bad track extrapolation " << endl; 
            continue; 
         }  
         
         SpdTrackState* final_state = tpars->FinalState();
         *final_state = stateOut;
                
         TVector3 d = vertex->GetPos()-final_state->GetPosition();
         final_state->SetPosDelta(d.Mag());
         final_state->SetTime(state->GetTime()+final_state->GetTime());
    }
}
 
//________________________________________________________________________________________  
void SpdMCVerticesFitter::Finish()
{
    if (fVerboseLevel > -1) {
        cout << "\n-I- <SpdMCVerticesFitter::Finish> vertices (fitted) [primary,secondary]: " << fNTotalFittedVertices  
             << " [" << fNTotalPrimFittedVertices << ", " << fNTotalSecFittedVertices << "] ";//<< endl;
    }
}




