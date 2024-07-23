// The script selects all fitted muon tracks
// and puts invariant mass of all pairs to
// a histogram. The tree with dimuon mass
// is also saved.


//______________________________________________________________________________
void CheckJpsi(TString ofilename = "o.root") {


    TString inFile("IdealFittedTracks.root");

    TFile* input_file = new TFile(inFile);
    if (!input_file) return;

    TTree* input_tree = (TTree*)input_file->Get("cbmsim");  
    if (!input_tree) return;

    TBranch* bSpdKFSimpleRes = input_tree->GetBranch("SpdKFSimpleRes.");
    if (!bSpdKFSimpleRes) return;

    SpdKFSimpleRes* fit_results = nullptr;
    input_tree->SetBranchAddress("SpdKFSimpleRes.",&fit_results) ;

    vector<TLorentzVector> vmup;
    vector<TLorentzVector> vmum;

    TLorentzVector pjpsi;

    TFile   fout(ofilename, "RECREATE");
    TTree*  otree = new TTree("t_pjpsi", "t_pjpsi");
    otree->Branch("pjpsi", "pjpsi", &pjpsi);

    TH1D* hjpsi = new TH1D ("hjpsi", "hjpsi", 50, 2.8, 3.4);
    TH1D* hchi2ndf = new TH1D ("hchi2ndf", "hchi2ndf", 100, 0, 10);

    for (int j(0); j < bSpdKFSimpleRes->GetEntries(); j++)   {
        bSpdKFSimpleRes->GetEntry(j);
        vmup.clear();
        vmum.clear();

        // Fit results
        int ntracks = fit_results->GetNTracks();
        for ( int i=0; i<ntracks; i++ ) {
            const SpdKFSimpleResTrack* tr = fit_results->GetTrack(i);

            TVector3 pmc = tr->MomentumMC;
            int pdg = tr->TrackPdg;


            if (!tr->IsFitted) {
                continue;
            }

            if ( abs(pdg) != 13) {
                continue;
            }

            TLorentzVector pmu;
            TVector3 p = tr->MomentumREC;
            pmu.SetXYZM( p.Px(), p.Py(), p.Pz(), 0.139 );

            if ( pdg == 13 ) {
                vmup.push_back( pmu );
            }

            if ( pdg == -13 ) {
                vmum.push_back( pmu );
            }

            hchi2ndf->Fill( tr->Chi2 / tr->NDF );
        }

        for (auto& v1 : vmup) {
            for (auto& v2 : vmum) {
                pjpsi = v1+v2;
                //cout << pjpsi.Mag() << endl;
                hjpsi->Fill( pjpsi.Mag() );
                otree->Fill();
            }
        }

        fit_results->ClearData();
    }

    fout.Write();
    fout.Close();

    cout << "Macro finished successfully." << endl;

    gApplication->Terminate();

}
