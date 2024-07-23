/* 
   This is a physics analysis example using SpdMCDataIterator class. 
   
   It shows access to
     -- mc truth,
     -- fitted tracks,
     -- EMC particles ("separated" showers in EMC),
     -- track extrapolation (to match the proper MC particle).
   
   This macro analyzes J/psi -> mu+ mu- events and provides a set of histograms as an output.
   
   It macro takes the "reco_full.root" as an input. This file is produced by the default
   reco_event_jpsi.C (="macro/analysis/RecoEventFull.C") macro.    
*/

#include "fit_dimu.C"

void analyze_jpsi(int events_max = 1000, int lepton_pdg = 13) 
{
    // default is muon
    double lepton_mass = 0.10566;
    if (lepton_pdg == 11) {
        lepton_mass = 0.511e-3;
    }

    SpdMCDataIterator* IT = 0;
    const SpdSetParSet* Pars_ = 0;

    //----------------------------------
    // pointers to current data objects

    const TClonesArray*   mcparticles     = 0;    //! List of mc-particles 
    const TClonesArray*   mctracks        = 0;    //! List of mc-tracks 
    const TClonesArray*   rcecalpartilces = 0;    //! List of ECAL rs-particles 

    IT = new SpdMCDataIterator();
    
    IT->AddSourceFile("reco.root");

    IT->ActivateBranch("all");
     
    IT->Init();

    // The object to extrapolate muon track to RS
    // based on the hits there
    RsTrackFinder     RTF;
    RTF.verbose_lvl = 0;
    
    //----------------------------------------------------------
    // Get data pointers
    
    Pars_ = IT->GetParameters(); 
    
    mcparticles      = IT->GetParticles();    
    mctracks         = IT->GetTracks();         
    rcecalpartilces  = IT->GetEcalParticlesRC();
   
    TH1D* hpmup     = new TH1D("hpmup", "Momentum of #mu^{+}; p(#mu^{+}) [GeV]", 50, 0, 5);
    TH1D* hMmumu    = new TH1D("hMmumu", "Dimuon mass; M_{#mu#mu} [GeV]", 50, 2.6, 3.6);
    TH1D* hjpsi_pT  = new TH1D("hjpsi_pT", "J/#psi p_{T} spectrum; p_{T} [GeV]", 100, 0, 5);
    TH1D* hMuonEcal = new TH1D("hMuonEcal", "Lepton shower in ecal (d<10 cm);E [GeV]", 100, 0, 1);
    TH1D* hd        = new TH1D("hd", "Distance from extr. track to the closest shower;d [cm]", 100, 0, 100);
    TH2D* hCosMupCosMum = new TH2D("hCosMupCosMum", "cos#theta_{#mu^{+}} Vs. cos#theta_{#mu^{-}};cos#theta_{#mu^{+}};cos#theta_{#mu^{-}}", 50, -1, 1, 50, -1, 1);
    
    SpdTrackPropagatorGF trkProp;
    trkProp.Init();
    //trkProp.SetMaterialEffects(false);
    
    Int_t ne_total(0);
     
    while ( (IT->NextEvent()) && (ne_total < events_max) ) {
        TLorentzVector pmum;
        TLorentzVector pmup;
        bool fitted_pmum = false;
        bool fitted_pmup = false;

        RTF.ReadHits( IT->GetRsHits() );

        // Loop over tracks, select mu+ and mu- from J/psi
        for (int imctrack=0; imctrack<mctracks->GetEntriesFast(); imctrack++) {
            SpdTrackMC* mctrack = (SpdTrackMC*) mctracks->At(imctrack);
            if (!mctrack) continue; // why??

            if ( TMath::Abs( mctrack->GetParticlePdg() ) == lepton_pdg ) {

                // Check if the lepton originates from J/psi
                int partid = mctrack->GetParticleId();
                SpdMCParticle* part = (SpdMCParticle*) mcparticles->At( partid );

                int mid = part->GetMotherId();
                if ( mid < 0 ) continue;
                SpdMCParticle* mother_part = (SpdMCParticle*) mcparticles->At( mid );
                if (mother_part->GetPdgCode() != 443) continue;

                SpdTrackFitPar* fitpars = mctrack->GetFitPars();
                if ( !fitpars ) continue;

                //  Get momentum in the primary vertex (final state)
                SpdTrackState* trkfinalstate = fitpars->GetFinalState();
                if (!trkfinalstate) continue;
                TVector3 p3 = trkfinalstate->GetMomentum();

                int pdg = mctrack->GetParticlePdg();
                if ( pdg == lepton_pdg ) {
                    pmum.SetXYZM(p3.X(), p3.Y(), p3.Z(), lepton_mass);
                    fitted_pmum = true;
                }
                if ( pdg == -lepton_pdg ) {
                    pmup.SetXYZM(p3.X(), p3.Y(), p3.Z(), lepton_mass);
                    fitted_pmup = true;
                    hpmup->Fill( p3.Mag() );
                }
                trkProp.InitTrack( pdg );

                // ------- Faind the closest particle in ECal, fill muon nergy losses ------
                SpdTrackState* trklaststate = fitpars->GetLastState();
                if (!trklaststate) continue;
                double dmin = 1e10;
                SpdEcalRCParticle*  closest_emc_particle = nullptr;
                for (int ishower=0; ishower<rcecalpartilces->GetEntriesFast(); ishower++) {
                    SpdEcalRCParticle* ecal_part = (SpdEcalRCParticle*) rcecalpartilces->At(ishower);
                    if (!ecal_part) continue;

                    TVector3 pos = ecal_part->GetPosition();
                    if (ecal_part->GetEnergy() < 0.1) {
                      continue;
                    }

                    SpdTrackState trkextstate;
                    trkProp.ExtrapolateToPoint( pos, *trklaststate, trkextstate);
                    TVector3 extpos = trkextstate.GetPosition();
                    double d = (pos - extpos).Mag();
                    if ( d < dmin ) {
                        dmin = d;
                        closest_emc_particle = ecal_part;
                    }
                }

                if ( closest_emc_particle ) {
                    hd->Fill(dmin);
                    if ( dmin < 10 ) {
                        hMuonEcal->Fill( closest_emc_particle->GetEnergy() );
                    }
                }

                // ------ Extrapolate track in RS based on hits there ------
                // Commented by  default, quite slow
                //  RTF.SetPdg( part->GetCharge() > 0 ? -13 : 13 );
                //  RsRecTrack rstrk = RTF.ExtrapolateTrack(trklaststate);
                //  cout << "Muon extrapolation length is " << rstrk.length << endl;
            }
        } // MCtracks
        if (fitted_pmum && fitted_pmup) {
            TLorentzVector pjpsi = pmup + pmum;
            hMmumu->Fill( pjpsi.Mag() );
            hjpsi_pT->Fill( pjpsi.Pt() );
            hCosMupCosMum->Fill( pmup.CosTheta(), pmum.CosTheta() );
        }
        ne_total++;
    }

    cout << "Events processed: " << ne_total << endl;

    gStyle->SetOptStat(11);

    TCanvas* c = new TCanvas("c", "", 1200, 800);
    c->Divide(3,2);

    c->cd(1);
    hpmup->Draw();

    c->cd(2);
    hMmumu->Draw();
    FitDimuSpectrum( hMmumu );

    c->cd(3);
    hjpsi_pT->Draw();

    c->cd(4);
    hCosMupCosMum->Draw("colz");

    c->cd(5);
    hd->Draw();

    c->cd(6);
    hMuonEcal->Draw();

    c->SaveAs("jpsi.pdf");
}

