//#define debug
#include "double_gaussian.C"

TGraphErrors* GetResolutionCosTheta(TH2* h) {
    int nplots = h->GetYaxis()->GetNbins();

    TGraphErrors* gr = new TGraphErrors(nplots);
    cout << "nplots: " << nplots << endl;

    TCanvas c("tc", "tc");
    c.Divide(5, 4);

    for (int i=1; i<=nplots; i++) {
        TH1* th = h->ProjectionX(Form("th%d", i), i, i);
        double y = h->GetYaxis()->GetBinCenter(i);
        if ( th->GetSumOfWeights() > 10) {
            if (i<=20) c.cd(i);
            auto res = fit_double_gaussian(th);
            th->SetMarkerSize(0.5);
            th->SetTitle(Form("cos#theta = %.2f", y));
            gr->SetPoint( i-1, res.mean, y);
            gr->SetPointError( i-1, res.sigma, 0.);
        } else {
            gr->SetPoint( i-1, -99999, y);
            gr->SetPointError( i-1, 0., 0.);
        }
    }

    c.SaveAs("fits.pdf");

    //gr->SetTite();
    gr->SetTitle("(p-p^{mc})/p^{mc} mean value and resolution");
    gr->GetXaxis()->SetTitle("(p-p^{mc})/p^{mc}");
    gr->GetYaxis()->SetTitle("cos#theta");
    gr->SetMarkerStyle(20);


    //gr->GetXaxis()->SetRangeUser( h->GetXaxis()->GetXmin(),
    //                              h->GetXaxis()->GetXmax() );
    return gr;
}

void ana(int events_max = 100000, TString output_filename = "ana.root") {

    TFile fout(output_filename, "RECREATE");
    TH2D* hCosThetaPhi_gen = new TH2D("hCosThetaPhi_gen", ";cos(#theta);#phi", 10, -1, 1, 10, -TMath::Pi(), TMath::Pi());
    TH2D* hCosThetaPhi_rec = new TH2D("hCosThetaPhi_rec", ";cos(#theta);#phi", 10, -1, 1, 10, -TMath::Pi(), TMath::Pi());

    TH1D* hRes = new TH1D("hRes", "Resolution (4#pi average);#Deltap/p", 500, -0.5, 0.5);

    TH2D* hResCosTheta = new TH2D("hResCosTheta", "Resolution Vs. cos#theta;(p-p^{mc})/p^{mc};cos(#theta)", 100, -0.2, 0.2, 20, -1, 1); // Auxiliary histo
    TH2D* hNItsHitsCosTheta = new TH2D("hNItsHitsCosTheta", "#hits in the inner tracker;cos(#theta);N_{Its hits}", 40, -1, 1, 24, 0, 6);
    TH2D* hNStrawHitsCosTheta = new TH2D("hNStrawHitsCosTheta", "#hits in straw;cos(#theta);N_{Straw hits}", 40, -1, 1, 80, 0, 80);


    TH1D* hChi2 = new TH1D("hChi2", ";#chi^{2}", 500, 0, 100);
    TH2D* hNDofChi2 = new TH2D("hNDofChi2", ";NDOF;#chi^{2}", 80, 0, 80, 100, 0, 100);

    // Add histograms
    // chi2 for different cos theta
    // resolution for different cos theta
    // nhits for different cos theta for inner tracker and straw

    SpdMCDataIterator* IT = new SpdMCDataIterator();
    
    IT->AddSourceFile("reco.root");

    IT->ActivateBranch("all");
    IT->Init();

    TDatabasePDG *pdgdb = new TDatabasePDG();
    
    //----------------------------------------------------------
    // Get data pointers
    
    const SpdSetParSet* Pars_ = 0;
    Pars_ = IT->GetParameters();   // some magic...

    SpdTrackPropagatorGF trkProp;
    trkProp.Init();
    
    Int_t ne_total = 0;
     
    while ( (IT->NextEvent()) && (ne_total < events_max) ) {
        ne_total++;
#ifdef debug
        cout << "*********** New event! ***********" << endl;
#endif
        const TClonesArray* mcparticles = IT->GetParticles();

        TVector3 pmc;
        for (int ipart=0; ipart<mcparticles->GetEntriesFast(); ipart++) {
            SpdMCParticle* part = (SpdMCParticle*) mcparticles->At( ipart );
            if (part) {
                if ( (part->GetVProcess() == 0) || (part->GetGeneration() == 1) ) {
                    pmc = part->GetStartMom();
                    hCosThetaPhi_gen->Fill( pmc.CosTheta(), pmc.Phi() );
                }
            }
        }

        // count fitted tracks associated with the primary vertex
        const TClonesArray* tracksmc = IT->GetTracks();         
        std::vector<int> pvtracks_ids;
        for (int itrackmc=0; itrackmc<tracksmc->GetEntriesFast(); itrackmc++) {
            SpdTrackMC* trackmc = (SpdTrackMC*) tracksmc->At(itrackmc);
            if (!trackmc) continue; // why??

            int part_id = trackmc->GetParticleId();
            SpdMCParticle* part = (SpdMCParticle*) mcparticles->At(part_id);
            if  (!part) continue;
            //cout << "pdg: " << part->GetPdgCode() << endl;

            if ( (part->GetVProcess() == 0) || (part->GetGeneration() == 1) ) {
                // fill reco
                SpdTrackFitPar* fitpars = trackmc->GetFitPars();
                if (!fitpars) continue;

                // parameters in vertex
                SpdTrackState* fstate = fitpars->GetFirstState();
                if (!fstate) continue;

                hChi2->Fill(fitpars->GetChi2());
                hNDofChi2->Fill(fitpars->GetNDF(), fitpars->GetChi2());
                hNItsHitsCosTheta->Fill( pmc.CosTheta(), trackmc->GetNHitsIts());
                hNStrawHitsCosTheta->Fill( pmc.CosTheta(), trackmc->GetNHitsTsB() + trackmc->GetNHitsTsEC());

                // GetMomentum!
                SpdTrackState trkextstate;
                trkProp.InitTrack( part->GetPdgCode() );
                trkProp.ExtrapolateToLine( TVector3(0, 0, 0), TVector3(0, 0, 1), *fstate, trkextstate);
                //TVector3 p = trkextstate.GetMomentum();
                TVector3 p = fstate->GetMomentum();
                hCosThetaPhi_rec->Fill( p.CosTheta(), p.Phi() );

                double h = (p.Mag() - pmc.Mag())/pmc.Mag();
                hRes->Fill( (p.Mag() - pmc.Mag())/pmc.Mag() );
                hResCosTheta->Fill( (p.Mag() - pmc.Mag())/pmc.Mag(), pmc.CosTheta() );

            }
        }

    } // event loop

    cout << "Events processed: " << ne_total << endl;
    
    fout.cd();
    TH2D* hCosThetaPhi_eff = dynamic_cast<TH2D*>(hCosThetaPhi_rec->Clone("hCosThetaPhi_eff"));
    hCosThetaPhi_eff->Divide(hCosThetaPhi_rec, hCosThetaPhi_gen, 1, 1, "B");
    hCosThetaPhi_eff->SetTitle("Efficiency in cos#theta and #phi bins");


    gStyle->SetOptStat(0);

    auto c = new TCanvas("c", "c", 800, 600);
    c->Divide(3, 2);

    c->cd(1);
    hCosThetaPhi_eff->Draw("colz");

    c->cd(4);
    fit_double_gaussian(hRes);

    c->cd(2);
    hResCosTheta->Draw("colz");

    c->cd(5);
    auto gr = GetResolutionCosTheta(hResCosTheta);
    c->cd(5);
    gr->Draw("AP");

    c->cd(3);
    hNItsHitsCosTheta->Draw("colz");

    c->cd(6);
    hNStrawHitsCosTheta->Draw("colz");

    c->SaveAs("ana.pdf");

    fout.Write();
    fout.Close();
}

