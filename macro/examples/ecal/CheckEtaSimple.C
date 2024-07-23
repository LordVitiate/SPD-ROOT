
SpdMCDataIterator* IT = 0;

// pointers to current data objects that we'll need
const TClonesArray*   Tracks_          = 0;    //! List of mc-tracks 
const TClonesArray*   VerticesRC_      = 0;    //! List of rc-vertices 
const TClonesArray*   EcalParticlesRC_ = 0;    //! List of ECAL rs-particles 

// output histogramms
TH1D* h_mpim_pip_pi0   = new TH1D("h_mpim_pip_pi0", "3#pi mass (+-0); M{3 #pi} [GeV]; Entries", 1000, 0, 1.0);
TH1D* h_mpim_pip_gamma = new TH1D("h_mpim_pip_gamma", "2#pi#gamma mass (+-gamma); M{#pi^{-} #pi^{+} #gamma} [GeV]; Entries", 1000, 0, 1.0);
TH1D* h_gammagamma     = new TH1D("h_gammagamma", "2#gamma mass; M{#gamma #gamma} [GeV]; Entries", 1000, 0, 1.0);

// pion masses
Double_t Mpi0 = 0.134977;
Double_t Mpic = 0.13957;

//____________________________________________________________________
void ProcessEvent()
{
    vector<TLorentzVector> pim;    // pi-
    vector<TLorentzVector> pip;    // pi+
    vector<TLorentzVector> pi0;    // pi0
    vector<TLorentzVector> gammas; // photons

    // reject event if no vertices reconstructed ...
    if (VerticesRC_->GetEntriesFast() == 0) return;
    
    // and get the position of the reconstructed vertex
    SpdVertexRC* vtx = (SpdVertexRC*)VerticesRC_->At(0);  
    SpdPrimVertexFitPar* vtxPar = dynamic_cast<SpdPrimVertexFitPar*>(vtx->GetFitPars());
    TVector3 vtxPos = vtxPar->GetVertex();

    Int_t nff = 0;
    
    // find π+/π- among tracks and fill their 4-momenta
    for (Int_t i=0; i < Tracks_->GetEntriesFast(); i++) 
    {
        SpdTrackMC* tr = (SpdTrackMC*)Tracks_->At(i);
        
        SpdTrackFitPar* fitpar = tr->GetFitPars();
        if (!fitpar) continue;
        
        SpdTrackState* initpar = fitpar->GetFirstState();
        if (!initpar) continue;
        
        Int_t pdg = fitpar->GetTrackPdgCode();
        TVector3 mom = initpar->GetMomentum();

        TLorentzVector lvpi;
        lvpi.SetXYZM(mom.Px(), mom.Py(), mom.Pz(), Mpic);
        if (pdg ==  211) pip.push_back(lvpi);
        if (pdg == -211) pim.push_back(lvpi);
    }

    // assuming each cluster is produced by a photon, fill 4-momenta of photons
    for (Int_t ip = 0; ip < EcalParticlesRC_->GetEntriesFast(); ++ip) 
    {
        SpdEcalRCParticle* part = (SpdEcalRCParticle*)EcalParticlesRC_->At(ip);
       
        TVector3 pos = part->GetPosition();
        Double_t en = part->GetEnergy();
        TVector3 momReco = pos - vtxPos;
        momReco.SetMag(en);
        
        TLorentzVector lvreco;
        lvreco.SetVectM(momReco, 0.);
        gammas.push_back(lvreco);
    }

    // create all possible combinations of π0
    for (Int_t i1 = 0; i1 < gammas.size(); ++i1) {
        TLorentzVector lv1 = gammas[i1];
        for (Int_t i2 = i1+1; i2 < gammas.size(); ++i2) {
            TLorentzVector lv2 = gammas[i2];
            
            // simple cut to reduce noise
            if (lv1.E() > 0.2 && lv2.E() > 0.2) {
                TLorentzVector lvsum = lv1 + lv2;
                pi0.push_back(lvsum);
            }
        }
    }

    for (auto lv: pi0) {
        h_gammagamma->Fill(lv.Mag());
    }

    for (Int_t im = 0; im < pim.size(); ++im) {
        for (Int_t ip = 0; ip < pip.size(); ++ip) {
            // get all η → π-π+π0 combinations
            for (Int_t i0 = 0; i0 < pi0.size(); ++i0) {
                if (pim[im].E() < 0.05) continue;
                if (pip[ip].E() < 0.05) continue;
                if (pi0[i0].Mag() < 0.115 || pi0[i0].Mag() > 0.155) continue;
                h_mpim_pip_pi0->Fill((pim[im] + pip[ip] + pi0[i0]).Mag());
            }

            // get all η → π-π+γ combinations
            for (Int_t i0 = 0; i0 < gammas.size(); ++i0) {
                if (pim[im].E() < 0.05) continue;
                if (pip[ip].E() < 0.05) continue;
                h_mpim_pip_gamma->Fill((pim[im] + pip[ip] + gammas[i0]).Mag());
            }
        }
    }
}

//___________________________________________________________________________________________
void CheckEtaSimple()
{
    IT = new SpdMCDataIterator();
    
    //----------------------------------------------------------
    // Add input data files
    //----------------------------------------------------------
    // (1) Add single file
    
    //IT->AddSourceFile("recoData.root");
    IT->AddSourceFile("reco_full.root");
    
    // (2) Add data files from specified input directory  
    
    //TString dataDir; 
    //dataDir = gSystem->WorkingDirectory();
    //dataDir += "/indata";
    
    //IT->SetCurrentDataDir(dataDir);
    
    //IT->AddSourceFile("reco_full1.root");
    //IT->AddSourceFile("reco_full2.root");
    //...
    
    //----------------------------------------------------------
    // Activate input data branches that we'll need for reading
    
    IT->ActivateBranch("MCTracks");        
    IT->ActivateBranch("RCVertices");  
    IT->ActivateBranch("RCEcalParticles"); 
    
    //----------------------------------------------------------
    // Iterator initialization
    
    IT->Init();
    
    //----------------------------------------------------------
    // Get data pointers
    
    Tracks_          = IT->GetTracks();         
    VerticesRC_      = IT->GetVerticesRC();     
    EcalParticlesRC_ = IT->GetEcalParticlesRC();
   
    //-----------------------------------------------------------
    // The main action happens here!
    // ----------------------------------------------------------
    while (IT->NextEvent()) 
    {
        ProcessEvent();
    }

    // create a directory for pictures and draw them
    TString fDirname = "pictures_ExampleEta/";
    gSystem->Exec("mkdir -p " + fDirname);

    TCanvas * c1 = new TCanvas("c1", "c1", 1200, 800);
    
    h_mpim_pip_pi0->Draw();
    c1->SaveAs(fDirname + "eta_mass_3pi.png", "png");
    
    h_mpim_pip_gamma->Draw();
    c1->SaveAs(fDirname + "eta_mass_2pi_gamma.png", "png");
    
    h_gammagamma->Draw();
    c1->SaveAs(fDirname + "eta_mass_2gamma.png", "png");

    //gApplication->Terminate();
}





