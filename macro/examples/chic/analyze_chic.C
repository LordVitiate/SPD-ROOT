/*
   This is a physics analysis example. 
   
   It shows access to
     -- mc truth,
     -- fitted tracks,
     -- fitted primary vertex,
     -- EMC particles ("separated" showers in EMC).
   
   This macro analyzes chi_c -> gamma J/psi (J/psi-> mu+ mu-) events and provides a set of histograms as an output.
   
   It macro takes the "reco_full.root" as an input. This file is produced by the default 
   reco_event_chic.C (="macro/analysis/RecoEventFull.C") macro. 
   
   For simulation I use slightly modified SimuQslPy8.C named as simu_chic.C:
     -- the primary vertex smearing is changed to Gaussian with an
        increased smearing parameter (it affects the reconstructed photon momentum);
*/
     
void analyze_chic(int nevents_max = 1000, int chic_pdg = 20443) 
{

    // Book output histograms
    TH1D* hangle = new TH1D("hangle", "Angle between cluster and closest MC photon;#alpha [rad];counts", 40, 0, 0.1);
    TH1D* hE = new TH1D("hE", "#chi_{c} photon energy spectrum;E_{#gamma} [GeV];counts", 40, 0, 2);
    TH1D* hdM = new TH1D("hdM", "M(#gamma#mu^{+}#mu^{-}) - M(#mu^{+}#mu^{-}) (#alpha<0.05);M(#gamma#mu^{+}#mu^{-}) - M(#mu^{+}#mu^{-}) [GeV];counts", 100, 0, 1);
    TH1D* hdE = new TH1D("hdE", "#chi_{c} photon energy resolution (#alpha<0.05);#DeltaE [GeV];counts", 40, -1, 1);

    TH1D* hVtxZ = new TH1D( "hVtxZ", "Primary vertex rec. Z;Z [cm];counts", 500, -100, 100);
    TH2D* hVtxXY = new TH2D( "hVtxXY", "Primary vertex rec. XY;X [cm];Y [cm]", 50, -1, 1, 50, -1, 1);


    // Prepare event reader
    SpdMCDataIterator IT;  // Do I need to read pars?
    IT.AddSourceFile("reco.root");
    IT.ActivateBranch("all");
    IT.Init();

    // aliases for frequently used brances
    const TClonesArray* mcparticles      = IT.GetParticles();    
    const TClonesArray* mctracks         = IT.GetTracks();         
    const TClonesArray* rcecalpartilces  = IT.GetEcalParticlesRC();
    const TClonesArray* rcvertices       = IT.GetVerticesRC();

    SpdTrackPropagatorGF trkProp;

    // The main event loop
    int ne_total(0);
    while ( (IT.NextEvent()) && (ne_total < nevents_max) )
    {
        // Find photon candidate for chic -> gamma J/psi as the closest
        // to true photon from SpdEmcParticles.
        // First, fined the refrence photon from MC-truth.
        SpdMCParticle *etac_photon = nullptr;
        for (int ipart=0; ipart<std::min(mcparticles->GetEntriesFast(), 100); ipart++) {
            SpdMCParticle* part = (SpdMCParticle*) mcparticles->At(ipart);
            if ( part->GetPdgCode() != 22 ) continue;
            int mid = part->GetMotherId();
            if ( mid >= 0 ) {
                SpdMCParticle* mother_part = (SpdMCParticle*) mcparticles->At( mid );
                if (mother_part->GetPdgCode() == chic_pdg) {
                    etac_photon = part;
                }
            }
        }
        if ( !etac_photon ) continue;
        TVector3 ptrue = etac_photon->GetStartMom();
//        TVector3 vtrue = etac_photon->GetStartPos(); // Get true photon vertex postion, use if you need resolution


        // Get the primary event vertex
        bool found_vrec = false;
        TVector3 vrec;
        int nvtx = rcvertices->GetEntriesFast();
        for (int ivtx = 0; ivtx < nvtx; ivtx++){
            SpdVertexRC* vtx = (SpdVertexRC*) rcvertices->At(ivtx);
            if ( (vtx) && (vtx->IsPrimary()) ) {
                SpdPrimVertexFitPar* vtxfitpar = dynamic_cast<SpdPrimVertexFitPar*> (vtx->GetFitPars());
                if (!vtxfitpar) continue;
                vrec = vtxfitpar->GetVertex();
                found_vrec = true;
                break;
            }
        }
        if (!found_vrec) continue;

        hVtxZ->Fill( vrec.Z() );
        hVtxXY->Fill( vrec.X(), vrec.Y() );


        // Now find the closet EMC shower (particle)
        double min_angle = 1e10;
        TVector3 pcand;

        for (int ishower=0; ishower<rcecalpartilces->GetEntriesFast(); ishower++) {
            SpdEcalRCParticle* ecal_part = (SpdEcalRCParticle*) rcecalpartilces->At(ishower);
            if (!ecal_part) continue;
            TVector3 shower_pos = ecal_part->GetPosition();
            if (ecal_part->GetEnergy() < 0.4) {
                continue;
            }
            double angle = (shower_pos - vrec).Angle(ptrue);

            if (angle < min_angle) {
                min_angle = angle;
                pcand = (shower_pos - vrec);
                pcand.SetMag( ecal_part->GetEnergy() );
            }

        }
        hangle->Fill( min_angle );
        if ( min_angle < 0.05 ) {
            hE->Fill( pcand.Mag() );
            hdE->Fill( pcand.Mag() - ptrue.Mag() );
        } else {
            continue;
        }
        TLorentzVector p4phot;
        p4phot.SetXYZM( pcand.X(), pcand.Y(), pcand.Z(), 0.);

        // Finally use charged tracks to produce the final variables
        TLorentzVector pmum;
        TLorentzVector pmup;
        bool fitted_pmum = false;
        bool fitted_pmup = false;
        for (int imctrack=0; imctrack<mctracks->GetEntriesFast(); imctrack++) {
            SpdTrackMC* mctrack = (SpdTrackMC*) mctracks->At(imctrack);
            if (!mctrack) continue; // why??

            if ( TMath::Abs( mctrack->GetParticlePdg() ) == 13 ) {
                int partid = mctrack->GetParticleId();
                SpdMCParticle* part = (SpdMCParticle*) mcparticles->At( partid );

                SpdTrackFitPar* fitpars = mctrack->GetFitPars();
                if ( !fitpars ) continue;

                //  Get momentum form the final state fit parameters
                SpdTrackState* trkfinalstate = fitpars->GetFinalState();
                if (!trkfinalstate) continue;
                TVector3 p3 = trkfinalstate->GetMomentum();

                int pdg = mctrack->GetParticlePdg();
                if ( pdg == 13 ) {
                    pmum.SetXYZM(p3.X(), p3.Y(), p3.Z(), 0.10566);
                    fitted_pmum = true;
                }
                if ( pdg == -13 ) {
                    pmup.SetXYZM(p3.X(), p3.Y(), p3.Z(), 0.10566);
                    fitted_pmup = true;
                }
            }
        } // MCtracks
        if (fitted_pmum && fitted_pmup) {
            TLorentzVector pjpsi = pmup + pmum;
            hdM->Fill(  (pjpsi + p4phot).Mag() - pjpsi.Mag() );
        }
    ne_total++;
    } // events

    gStyle->SetOptStat(11);
    gStyle->SetOptFit(1);

    TCanvas* c = new TCanvas("c", "", 1200, 800);
    c->Divide(3,2);

    c->cd(1);
    hangle->Draw();

    c->cd(4);
    hE->Draw();

    c->cd(2);
    hdE->Draw();
    hdE->Fit("gaus");

    c->cd(5);
    hdM->Draw();
    hdM->Fit("gaus", "L", "",  0.3, 0.5);

    c->cd(3);
    hVtxZ->Draw();

    c->cd(6);
    hVtxXY->Draw("colz");

    c->SaveAs("chic.pdf");
}
