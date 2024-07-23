
// example of V0 finder using SpdVertexCombiFinder() and KFParticle package 
//-------------------------------------------------------------------------

SpdMCDataIterator* IT = 0;

// parameters
const SpdSetParSet* Pars_ = 0;

//----------------------------------
// pointers to current data objects

// maim MC
const SpdMCEvent*     Event_     = 0;          //! Main mc-data object
const TClonesArray*   Particles_ = 0;          //! List of mc-particles 
const TClonesArray*   Vertices_  = 0;          //! List of mc-vertices

// hits
const TClonesArray*   ItsHits_   = 0;          //! List of ITS mc-hits
//const TClonesArray*   TsHits_    = 0;          //! List of TS mc-hits

// objects
const TClonesArray*   Tracks_          = 0;    //! List of mc-tracks 
const TClonesArray*   VerticesRC_      = 0;    //! List of rc-vertices 
const TClonesArray*   fTracks          = 0;    //! List of mc-tracks 

// selection for V0 finder
//------------------------
std::vector<KFParticle> fKFparticles;          // vector with selected tracks 
std::vector<Int_t> fpdgs;                      // vector for SpdVertexCombiFinder

Int_t   fMinItsHits   = 3;                     // minimum Its hits for track
std::vector<Int_t> fDaughters = { 211, -211};  // decay particles (K0-decay)
//std::vector<Int_t> fDaughters = { 321, -211, 211};  // decay particles (D0-decay)
//std::vector<Int_t> fDaughters = { 2212, -211};  // decay particles (L0-decay)

//Bool_t  hardTrackCut  = true;                // hard track selection cut (tpars->GetIsGood()) 
Bool_t  hardTrackCut  = false;                 // hard track selection cut (tpars->GetIsGood()) 

Float_t fMaxChi2Part       = 3.0;              // maximum chi2 between 2 tracks (sel01)
Float_t fMinChi2PV         = 5.0;              // minimum chi2 track to PV      (sel02)
Float_t fMinLdLcut         = 5.0;              // additional L/dL cut           (sel03)
std::vector<Float_t> fMass = {0.45, 0.55};     // mass window for selection     (sel04)

Bool_t fConstrainToPV = true;                  // constrain to PV

#define HomogeneousField                       // set HomogeneousField field

//____________________________________________________________________
void DoSomething()
{
    //if (Event_)      Event_->PrintEvent();
    //if (Particles_)  SpdMCEventHelper::Instance()->PrintParticles();
    //if (Vertices_)   SpdMCEventHelper::Instance()->PrintVertices();
    //if (VerticesRC_) SpdMCEventHelper::Instance()->PrintVerticesRC();
    //if (Tracks_)     SpdMCEventHelper::Instance()->PrintTracks();
}

//____________________________________________________________________
void SelectTracks()
{
   // select tracks for constructing of V0 particle
   //----------------------------------------------
   fKFparticles.clear();    
   fpdgs.clear();    

   // select track and set KFParticle track
   //---------------------------------------
   SpdTrackMC* track;
   SpdTrackFitPar* tpars;  
   const SpdTrackState* state;

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
    
      if (std::find(fDaughters.begin(),fDaughters.end(),pdgTrk)==fDaughters.end()) continue;

      // 1st cut 
      if (track->GetNHitsIts() < fMinItsHits) continue;
        
      // 2nd cut 
      if (!track->GetIsFitted()) continue;

      // WARNING 3rd cut : hard cut! WARNING
      tpars = track->GetFitPars();
      if (!tpars) continue;

      if (hardTrackCut && !tpars->GetIsGood()) continue;  // 3-d cut

      state = tpars->GetFirstState();
        
      // set KF track parameters
      //------------------------ 
      TVector3 trkPos = state->GetPosition();
      TVector3 trkMom = state->GetMomentum();
      const TMatrixDSym& trkCov = state->GetCov();
        
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
      kfTrack.SetCharge(pdgTrk/abs(pdgTrk));
//        kfTrack.SetCharge(part->Charge()/3);

//      pdgTrk = (pdgTrk/abs(pdgTrk)>0) ? 211 : -211;
//      pdgTrk = (pdgTrk/abs(pdgTrk)>0) ? 2212 : -211;
//      pdgTrk = (pdgTrk/abs(pdgTrk)>0) ? 211 : -321;

      KFParticle p1(kfTrack, pdgTrk);        
      fKFparticles.push_back(p1);
      fpdgs.push_back(pdgTrk);

//      cout << "id/pdg: "<< track->GetId() << "/" << pdgTrk  << " ... ok " << endl;
   }        
}

//___________________________________________________________________________________________
void findDecayV0() 
{
    IT = new SpdMCDataIterator();
    
    //----------------------------------------------------------
    // Add input data files
    //----------------------------------------------------------
    // (1) Add single file which is located in the /macro directory  
    
//    IT->AddSourceFile("reco_full_K0.root");
    IT->AddSourceFile("reco_full_V0.root");
    
    //----------------------------------------------------------
    // Activate input data branches for reading
    
    IT->ActivateBranch("all");
     
    //----------------------------------------------------------
    // Iterator initialization
    IT->Init();
    
    //----------------------------------------------------------
    // Get data pointers
    
    Pars_       = IT->GetParameters(); 
    
    Event_      = IT->GetEvent();
    Particles_  = IT->GetParticles();    

    Tracks_     = IT->GetTracks();         
    fTracks     = IT->GetTracks();         
    VerticesRC_ = IT->GetVerticesRC();     

    // histos set
    //------------
    Char_t name[100];
    snprintf(name,100,"Inv. mass of V0 (#chi^{2}_{trk} < %3.1f)",fMaxChi2Part); 
    TH1D* hmass1 = new TH1D("hmass1", name, 100, 0.0, 2.5);

    snprintf(name,100,"Inv. mass of V0 (#chi^{2}_{PV} > %3.1f)",fMinChi2PV); 
    TH1D* hmass2 = new TH1D("hmass2", name, 100, 0.0, 2.5);

    snprintf(name,100,"Inv. mass of V0 (L/dL > %3.1f)",fMinLdLcut); 
    TH1D* hmass3 = new TH1D("hmass3", name, 100, 0.0, 2.5);

    snprintf(name,100,"Inv. mass of V0 (#chi^{2}_{PV} > %3.1f + L/dL > %3.1f + #chi^{2}_{trk} < %3.1f)",
                                                         fMinChi2PV, fMinLdLcut, fMaxChi2Part); 
    TH1D* hmass4 = new TH1D("hmass3", name, 100, 0.0, 2.5);

//    TH1D* hlength = new TH1D("hlength", "Decay Length of V0", 100, 0, 25.);
//    TH1D* hctau   = new TH1D("hlength", "Life time of V0",    100, 0, 25.);

    //-----------------------------------------------------------------------

    std::vector<Float_t> a1;
    std::vector<Float_t> a2;
    a1.reserve(10000);
    a2.reserve(10000);
   
    Int_t ne_total(0);
    Int_t nevent(-1);

    SpdTrackPropagatorGF fPropagator;     
    fPropagator.Init();     

    SpdVertexCombiFinder* vfinder = new SpdVertexCombiFinder();

//    while (IT->NextEvent() && ne_total<5) 
    while (IT->NextEvent()) 
    {
        ++nevent;
        cout << " >>>>> Current events: " << nevent << endl; 

        // vprim->IsPrimary()     - only primary reco vertex
        // vpar                   - there is reco parametrs
        // fKFparticles.size()>=2 - number of selected tracks
        //---------------------------------------------------- 

        SelectTracks();                              // select tracks

        if (fKFparticles.size() < fDaughters.size()) continue;  // skip if N_trk selected <

        SpdVertexRC* vprim = (SpdVertexRC*)VerticesRC_->At(0);  // get primary reco vertex
        if (!vprim || !vprim->IsPrimary()) continue;

        SpdPrimVertexFitPar* vpar = dynamic_cast<SpdPrimVertexFitPar*>(vprim->GetFitPars());
        if (!vpar) continue;

        TVector3 recVtx = vpar->GetVertex();            // reconstructed PV
        const TMatrixDSym& recCov = vpar->GetCov();     // Cov of PV

        // set field in PV position 
        //-------------------------- 
        Float_t fieldBz = 0.;
        SpdField* field = fPropagator.GetField();
        if (field) fieldBz = field->GetBz(recVtx.X(),recVtx.Y(),recVtx.Z());
#ifdef HomogeneousField 
        KFParticle::SetField(fieldBz);
#endif
        // set KFPVertex
        //---------------
        KFPVertex vert;
        vert.SetXYZ( recVtx.X(), recVtx.Y(), recVtx.Z() );
        vert.SetCovarianceMatrix( (float)recCov(0,0),
                                  (float)recCov(1,0), (float)recCov(1,1),
                                  (float)recCov(2,0), (float)recCov(2,1), (float)recCov(2,2) );

        vert.SetNContributors( vpar->GetNTracks() );
        vert.SetChi2( (float)vpar->GetChi2() );
        KFParticle pVtx(vert);                      // set primary vertex as KFParticle
        const KFParticle pVertex = pVtx;            // init primary vertex

        // use SpdVertexCombiFinder()
        //---------------------------
        vfinder->InitParticles(fpdgs);              // init list of particles
        vfinder->InitVertex(fDaughters);            // init decay particles in SV 

        Int_t nDaughters = (int)fDaughters.size();
        const KFParticle* vDaughters[nDaughters]; 

        Float_t chi2TrkPV[nDaughters];   // array of chi2 track to PV
        Float_t chi2Trk2Trk;             // chi2 between 2 tracks

        std::vector<Int_t> vc;
        while (vfinder->Next(vc))
        {
            for (Int_t i(0); i<Int_t(vc.size()); i++) 
            { 
                vDaughters[i] = &fKFparticles[vc[i]];
                chi2TrkPV[i]  =  fKFparticles[vc[i]].GetDeviationFromVertex(pVtx);
            } 

            // find maximum distance between 2 tracks combination
            //----------------------------------------------------
            Float_t chi2Tmp;
            chi2Trk2Trk = 0.0;
            for (Int_t i(0); i<Int_t(vc.size())-1; i++)
            {
               for (Int_t j(i); j<Int_t(vc.size()); j++)
               {
                  chi2Tmp = fKFparticles[vc[i]].GetDeviationFromParticle(fKFparticles[vc[j]]);
                  if (chi2Tmp > chi2Trk2Trk) chi2Trk2Trk = chi2Tmp; 
               }  
            }

            // check maximum distance between tracks (sel01)
            //----------------------------------------------
            Bool_t fSel01 = true;
            if (chi2Trk2Trk > fMaxChi2Part) fSel01 = false;    // max distance

            // check distance to PV (sel02)
            //-----------------------------
            Bool_t fSel02 = true;
            for (Int_t i(0); i<Int_t(vc.size()); i++)
            {
              if (chi2TrkPV[i] < fMinChi2PV) fSel02 = false;
            }

            // Decay particle reconstruction
            KFParticle K0_1;
            K0_1.SetProductionVertex(pVtx);
        
            if (fConstrainToPV > 0) K0_1.Construct(vDaughters,nDaughters,&pVtx,-1);
            else K0_1.Construct(vDaughters,nDaughters,0,-1);

// fill histos
//------------
            Float_t m1, dm1;
            Float_t minMass = 0.0;
            for (Int_t i(0); i<Int_t(vc.size()); i++)
            {
               fKFparticles[vc[i]].GetMass(m1,dm1);
               if (m1 > minMass) minMass = m1;
            }

            Float_t fV0mass = K0_1.GetMass();
            if (fV0mass <= minMass) continue;      // check reco mass     

            float length, dLength;
            K0_1.GetDecayLength(length,dLength);

            // check L/dL (sel03)
            //--------------------
            Bool_t fSel03 = true;
            if (abs(length)/dLength < fMinLdLcut) fSel03 = false;

            // check mass window (sel04)
            //--------------------------
            Bool_t fSel04 = false;                
            if (fV0mass>fMass[0] && fV0mass<fMass[1]) fSel04 = true;

            // fil histos
            //------------

            if (fSel01) hmass1->Fill(fV0mass);   // chi2 to track

            if (fSel02) hmass2->Fill(fV0mass);   // chi2 to PV

            if (fSel03) hmass3->Fill(fV0mass);   // L/dl

            if (fSel01 && fSel02 && fSel03) hmass4->Fill(fV0mass); // sel01+sel02+sel03

//            hlength->Fill(abs(K0_1.GetDecayLength()));
//            hctau->Fill(abs(K0_1.GetLifeTime()));

            if (Int_t(vc.size()) != 2) continue;   // need only 2 particle for A-P plot 

            // ArmenterosPodolanski
            // GetArmenterosPodolanski(pos, neg, QtAlfa[2]);
            float QtAlfa[2]; 
            if ((int)fKFparticles[vc[0]].GetQ()>0) 
                K0_1.GetArmenterosPodolanski(fKFparticles[vc[0]],fKFparticles[vc[1]],QtAlfa);
             else 
                K0_1.GetArmenterosPodolanski(fKFparticles[vc[1]],fKFparticles[vc[0]],QtAlfa);

                if (fSel01 && fSel02 && fSel03 && fSel04)
                { 
                  a1.push_back(QtAlfa[1]);
                  a2.push_back(QtAlfa[0]);
                }

        }  // end loop over combinations

// possible parameters of reconstructed V0
//--------------------------------------------------------------
// K0_1.GetX()              - X coordinate of particle   
// K0_1.GetErrX()           - error of X coordinate   
// K0_1.GetY()              - Y coordinate of the particle
// K0_1.GetErrY()           - error of Y coordinate    
// K0_1.GetZ()              - Z coordinate of the particle   
// K0_1.GetErrZ()           - error of Z coordinate    
// K0_1.GetPx()             - X component of the momentum   
// K0_1.GetErrPx()          - error of X-compoment of momentum   
// K0_1.GetPy()             - Y component of the momentum   
// K0_1.GetErrPy()          - error of Y-compoment of momentum   
// K0_1.GetPz()             - Z component of the momentum   
// K0_1.GetErrPz()          - error of Y-compoment of momentum   
// K0_1.GetE()              - energy of the particle   
// K0_1.GetErrE()           - error of energy    
//
// K0_1.GetMass()           - mass   
// K0_1.GetErrMass()        - error of mass
// K0_1.GetP()              - momentum   
// K0_1.GetErrP()           - error of momentum
// K0_1.GetPt()             - transverse momentum   
// K0_1.GetErrPt()          - error of transverse momentum 
// K0_1.GetEta()            - transverse pseudorapidity   
// K0_1.GetErrEta()         - error of pseudorapidity 
// K0_1.GetPhi()            - azimuthal angle    
// K0_1.GetErrPhi()         - error of azimuthal angle  
// K0_1.GetPhi()            - azimuthal angle    
// K0_1.GetErrPhi()         - error of azimuthal angle  
// K0_1.GetDecayLength()    - decay length    
// K0_1.GetErrDecayLength() - error of decay length  
// K0_1.GetLifeTime()       - life time    
// K0_1.GetErrLifeTime()    - error of life time  
// K0_1.GetTheta()          - polar angle 
//----------------------------------------------------------------

        DoSomething();

        ne_total++;

    }   // end event loop

    cout << ">>>>>> Events (total): " << ne_total << endl;

//    if (Pars_) Pars_->PrintParameters(0); // option = 0,1 

//----------------------------------------------------
      gStyle->SetFrameLineWidth(3);

      TCanvas* c = new TCanvas("c", "", 1200, 800);     
      c->Divide(2,2);     

      c->cd(1);     
      hmass1->SetLineWidth(2.);
      hmass1->GetXaxis()->SetTitleSize(0.05);
      hmass1->GetXaxis()->SetTitle("GeV/c^{2}");
      hmass1->Draw();     

      c->cd(2);     
      hmass2->SetLineWidth(2.);
      hmass2->GetXaxis()->SetTitleSize(0.05);
      hmass2->GetXaxis()->SetTitle("GeV/c^{2}");
      hmass2->Draw();     

      c->cd(3);     
      hmass3->SetLineWidth(2.);
      hmass3->GetXaxis()->SetTitleSize(0.05);
      hmass3->GetXaxis()->SetTitle("GeV/c^{2}");
      hmass3->Draw();     

      c->cd(4);     
      hmass4->SetLineWidth(2.);
      hmass4->GetXaxis()->SetTitleSize(0.05);
      hmass4->GetXaxis()->SetTitle("GeV/c^{2}");
      hmass4->Draw();     

//      hlength->SetLineWidth(2.);
//      hlength->GetXaxis()->SetTitleSize(0.05);
//      hlength->GetXaxis()->SetTitle("cm");
//      hlength->Draw();     

//    Armenteros Podolanski plot
//-------------------------------
      TCanvas* c1 = new TCanvas("c1", "", 1200, 800);
      c1->Divide(1,1);

      TGraph *gr = new TGraph(a1.size(),a1.data(),a2.data());
      c1->cd(1);

      gr->SetTitle("Armenteros Podolanski");
      gr->GetXaxis()->SetLimits(-1.0, 1.0);
      gr->GetXaxis()->SetTitle( "(p^{+}_{L}-p^{-}_{L}) / (p^{+}_{L}+p^{-}_{L})");

      gr->GetYaxis()->SetLimits( 0.0, 0.3);
      gr->GetYaxis()->SetTitle( "P_{T}, GeV/c");

      gr->SetMarkerStyle(20);
//      gr->SetMarkerSize(0.5);
      gr->SetMarkerColor(1);
      gr->SetLineColor(0);

      gr->SetMaximum(0.30);
//      gr->SetMaximum(1.15);

      gr->Draw();
}



