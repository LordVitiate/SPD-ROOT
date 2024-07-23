
// example of K0 finder using KFParticle package 
//-------------------------------------------------

SpdMCDataIterator* IT = 0;

//----------------------------------
// pointers to current data objects

const SpdMCEvent*     Event_       = 0;   // Main mc-data object
const TClonesArray*   Particles_   = 0;   // List of mc-particles 
const TClonesArray*   Vertices_    = 0;   // List of mc-vertices
const TClonesArray*   Tracks_      = 0;   // List of mc-tracks 

const TClonesArray*   VerticesRC_  = 0;   // List of rc-vertices 

//-----------------------------------
Float_t fMinChi2PVadd = 50.0;             // additional chi2 track to PV cut
Float_t fMinLdLcut    = 15.0;             // additional L/dL cut 

//____________________________________________________________________
void DumpEvent()
{
    //if (Event_)      Event_->PrintEvent();
    //if (Particles_)  SpdMCEventHelper::Instance()->PrintParticles(2);
    //if (Vertices_)   SpdMCEventHelper::Instance()->PrintVertices(1);
    //if (Tracks_)     SpdMCEventHelper::Instance()->PrintTracks();
    
    //if (VerticesRC_) SpdMCEventHelper::Instance()->PrintVerticesRC();
    //if (VerticesRC_) SpdMCEventHelper::Instance()->PrintVerticesRC2(-1,"fitpars");
}

//___________________________________________________________________________________________
void K0decay(Int_t opt = 0) 
{
    IT = new SpdMCDataIterator();
    
    //----------------------------------------------------------
    // Add input data files
    //----------------------------------------------------------
    // Add single file which is located in the /macro directory  
    
    if (opt == 0) IT->AddSourceFile("reco_full_K0.root");
    else IT->AddSourceFile("reco_full2_K0.root");
    
    //----------------------------------------------------------
    // Activate input data branches for reading
    
    IT->ActivateBranch("MCEvent.");
    IT->ActivateBranch("MCParticles");
    IT->ActivateBranch("MCVertices");
    IT->ActivateBranch("MCTracks");
    
    IT->ActivateBranch("RCVertices");      
 
    //IT->ActivateBranch("all");
    
    //----------------------------------------------------------
    // Iterator initialization
    
    IT->Init();
    
    //----------------------------------------------------------
    // Get data pointers
    
    Event_       = IT->GetEvent();
    Particles_   = IT->GetParticles();  
    Vertices_    = IT->GetVertices(); 
    Tracks_      = IT->GetTracks();         

    VerticesRC_  = IT->GetVerticesRC();     

    // histos
    //-------
    Char_t name[100];
    
    snprintf(name,100,"Invariant mass of V0  (no selection)"); 
    TH1D* hmass1 = new TH1D("hmass1", name, 100, 0, 1.0);

    snprintf(name,100,"Invariant mass of V0 (#chi^{2}_{PV} > %3.1f)",fMinChi2PVadd); 
    TH1D* hmass2 = new TH1D("hmass2", name, 100, 0, 1.0);

    snprintf(name,100,"Invariant mass of V0 (L/dL > %3.1f)",fMinLdLcut); 
    TH1D* hmass3 = new TH1D("hmass3", name, 100, 0, 1.0);

    snprintf(name,100,"Invariant mass of V0 (#chi^{2}_{PV} > %3.1f + L/dL > %3.1f)",fMinChi2PVadd,fMinLdLcut); 
    TH1D* hmass4 = new TH1D("hmass4", name, 100, 0, 1.0);

    TH1D* hlength = new TH1D("hlength", "Decay Length of V0", 100, 0, 25.);
    TH1D* hctau   = new TH1D("hctau", "Life time of V0", 100, 0, 25.);
    
    const Int_t NEvents = IT->GetNEvents();

    std::vector<Float_t> a1;
    std::vector<Float_t> a2;
    
    a1.reserve(10000);
    a2.reserve(10000);
     
    //------------------------------------------------------------
    Int_t ne_total(0);
    Int_t nevent(-1);

    Int_t nvertices, nvdaughters;
    
    SpdVertexRC*      vertex;
    SpdVertexFitPar2* vpars;
    
    Float_t K0mass;
    Float_t K0decayLength, K0decayLengthErr;
    Float_t K0ctau;
    
    Float_t chi2Trk1PV, chi2Trk2PV;

    while (IT->NextEvent()) 
    {
        ++nevent;
        
        cout << " >>>>>>>>>>>>>>>>>>>>>>> Current event >>>>>>>>>>>>>>>>>>>>: " << nevent << endl; 

        nvertices = VerticesRC_->GetEntriesFast();

        //cout << " nvertices = " << nvertices  << endl;
        
        for (Int_t i(1); i<nvertices; i++) {
        
             vertex = (SpdVertexRC*)VerticesRC_->At(i);
             
             vpars = dynamic_cast<SpdVertexFitPar2*>(vertex->GetFitPars());
             if (!vpars) continue;
             
             //cout << "\t vertex: " << i << endl;
        
             nvdaughters = vpars->GetNDaughters();
        
             if (nvdaughters != 2) continue;
        
             K0mass = vpars->GetMass();
             
             K0decayLength    = abs(vpars->GetLength()); 
             K0decayLengthErr = vpars->GetLengthErr();
             
             chi2Trk1PV = vpars->GetTrkChi2PV(0);
             chi2Trk2PV = vpars->GetTrkChi2PV(1);
             
             K0ctau = abs(vpars->GetLifetime());
             
             hmass1->Fill(K0mass);

              if (
                     (chi2Trk1PV > fMinChi2PVadd)
                  && (chi2Trk2PV > fMinChi2PVadd)
                 )   
                  hmass2->Fill(K0mass);

              // L/dL 
              if (
                  (K0decayLength/K0decayLengthErr > fMinLdLcut)
                 )  
                  hmass3->Fill(K0mass);

              if (
                     (chi2Trk1PV > fMinChi2PVadd) 
                  && (chi2Trk2PV > fMinChi2PVadd)
                  && (K0decayLength/K0decayLengthErr >  fMinLdLcut)
                 )
                  hmass4->Fill(K0mass);

              hlength->Fill(K0decayLength);
              hctau->Fill(K0ctau);
              
              if (
                     (chi2Trk1PV > fMinChi2PVadd) 
                  && (chi2Trk2PV > fMinChi2PVadd)
                  && (K0decayLength/K0decayLengthErr >  fMinLdLcut)
                 ) 
              {
                 a1.push_back(vpars->GetQtAlfa2());
                 a2.push_back(vpars->GetQtAlfa1());
              }
        }

        DumpEvent();
        
        ne_total++;
    }   

    cout << "\n>>>>>> Events (total): " << ne_total << endl;
    
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

    // Armenteros Podolanski plot
    //-------------------------------
    
    TCanvas* c1 = new TCanvas("c1", "", 1200, 800);
    c1->Divide(1,1);

    TGraph *gr = new TGraph(a1.size(),a1.data(),a2.data());
    c1->cd(1);

    gr->SetTitle("Armenteros Podolanski");
    gr->GetXaxis()->SetLimits(-1.0, 1.0);
    gr->GetXaxis()->SetTitle("(p^{+}_{L}-p^{-}_{L}) / (p^{+}_{L}+p^{-}_{L})");

    gr->GetYaxis()->SetLimits( 0.0, 0.3);
    gr->GetYaxis()->SetTitle("P_{T}, GeV/c");

    gr->SetMarkerStyle(20);
    gr->SetMarkerColor(1);
    gr->SetLineColor(0);

    gr->SetMaximum(0.30);

    gr->Draw();
  
}



