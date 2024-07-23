
// example analizing of reco tracks 
//------------------------------------

#include <TTree.h>
#include <TFile.h>

SpdMCDataIterator* IT = 0;

//----------------------------------
// pointers to current data objects

const SpdMCEvent*     Event_       = 0;   // Main mc-data object
//const TClonesArray*   Particles_   = 0;   // List of mc-particles 
//const TClonesArray*   Vertices_    = 0;   // List of mc-vertices

const TClonesArray*   TracksMC_      = 0;   // List of mc-tracks 
const TClonesArray*   TracksRC_      = 0;   // List of mc-tracks 
const TClonesArray*   TrackSeedsRC_  = 0;   // List of rc-tracks 

//const TClonesArray*   VerticesRC_  = 0;   // List of rc-vertices 
//const TClonesArray*   VerticesMC_  = 0;   // List of rc-vertices 

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
void AnaRecoTrack01(Int_t opt = 0) 
{
    IT = new SpdMCDataIterator();
    
    //----------------------------------------------------------
    // Add input data files
    //----------------------------------------------------------
    // Add single file which is located in the /macro directory  
    
//    IT->AddSourceFile("recoMB_L4exam.root");
    IT->AddSourceFile("recoMu_L4exam.root");

//    IT->AddSourceFile("");


//    if (opt == 0) IT->AddSourceFile("reco_full_V2.root");
//    else IT->AddSourceFile("reco_full2_K0.root");
    
    //----------------------------------------------------------
    // Activate input data branches for reading
    
//    IT->ActivateBranch("MCEvent.");
//    IT->ActivateBranch("MCParticles");
//    IT->ActivateBranch("MCVertices");
//    IT->ActivateBranch("MCTracks");
//    IT->ActivateBranch("RCVertices");      
 
    IT->ActivateBranch("all");
    
    IT->ActivateBranch("RCTracks");
    IT->ActivateBranch("RCTrackSeeds");

    //----------------------------------------------------------
    // Iterator initialization
    
    IT->Init();
    
    //----------------------------------------------------------
    // Get data pointers
    
    Event_       = IT->GetEvent();

//    Particles_   = IT->GetParticles();  
//    Vertices_    = IT->GetVertices(); 

    TracksMC_      = IT->GetTracks();        
    TracksRC_      = IT->GetTracksRC();         
    TrackSeedsRC_  = IT->GetTrackSeedsRC();         

//    VerticesRC_  = IT->GetVerticesRC();     
//    VerticesMC_  = IT->GetVerticesMC();     
//    VerticesMC_  = IT->GetVertices();     

    typedef  std::pair<Int_t,Int_t> spd_point;

    // histos
    //-------
    Char_t name[100];

    //-------- multiplicity ----------------------------
    
    snprintf(name,100,"Reco track multiplicity"); 
    TH1D* htrk1 = new TH1D("htrk1", name, 40, 0, 40.0);

    snprintf(name,100,"Ideal reco track multiplicity"); 
    TH1D* htrk2 = new TH1D("htrk2", name, 40, 0, 40.0);

   //-------- reco track momentum --------------------------   

    snprintf(name,100,"Reco track momentum"); 
    TH1D* htrk3 = new TH1D("htrk3", name, 50, 0, 5.0);

    snprintf(name,100,"Ideal Reco track momentum"); 
    TH1D* htrk4 = new TH1D("htrk4", name, 50, 0, 5.0);

    //------------------------------------------------------------

    snprintf(name,100,"#Delta #theta"); 
    TH1D* htrk5 = new TH1D("htrk5", name, 100, -0.0025, 0.0025);

    snprintf(name,100,"#Delta #phi"); 
    TH1D* htrk6 = new TH1D("htrk6", name, 100, -0.05, 0.05);

    snprintf(name,100,"#Delta P/P"); 
    TH1D* htrk7 = new TH1D("htrk7", name, 100, -0.05, 0.05);

    snprintf(name,100,"Ts hits, used for reco "); 
    TH1D* htrk8 = new TH1D("htrk8", name, 100, 0.0, 100);

    snprintf(name,100,"Ts hits, used for reco "); 
    TH1D* htrk9 = new TH1D("htrk9", name, 100, 0.0, 100);

    // track efficiency
    //------------------

    snprintf(name,100,"Ideal reco tracks, Pt ");
    TH1D* htrk10 = new TH1D("htrk10", name, 50, 0.0, 1.0);      // mu
//    TH1D* htrk10 = new TH1D("htrk10", name, 50, 0.0, 1.5);

    snprintf(name,100,"Efficiency vs Pt ");
    TH1D* htrk11 = new TH1D("htrk11", name, 50, 0.0, 1.0);      // mu
//    TH1D* htrk11 = new TH1D("htrk11", name, 50, 0.0, 1.5);

    snprintf(name,100,"Ideal reco tracks, P ");
    TH1D* htrk13 = new TH1D("htrk13", name, 50, 0.0, 4.0);

    snprintf(name,100,"Efficiency vs P ");
    TH1D* htrk14 = new TH1D("htrk14", name, 50, 0.0, 4.0);

    //---------------------------------------------------

    snprintf(name,100,"Ideal reco tracks, #theta ");
    TH1D* htrk20 = new TH1D("htrk20", name, 60, 0.0, 3.2);

    snprintf(name,100,"Efficiency vs #theta ");
    TH1D* htrk21 = new TH1D("htrk21", name, 60, 0.0, 3.2);

    snprintf(name,100,"Ideal reco tracks, #phi ");
    TH1D* htrk23 = new TH1D("htrk23", name, 120, -3.2, 3.2);

    snprintf(name,100,"Efficiency vs #phi ");
    TH1D* htrk24 = new TH1D("htrk24", name, 120, -3.2, 3.2);

    TEfficiency* pEff1 = new TEfficiency("eff1","Efficiency vs #theta;#theta;#epsilon",100, 0.0,3.2); 
    TEfficiency* pEff2 = new TEfficiency("eff2","Efficiency vs #phi;#phi;#epsilon",100, -3.2,3.2); 
    TEfficiency* pEff3 = new TEfficiency("eff3","Efficiency vs P_{T}; P_{T}, GeV/c;#epsilon",50., 0.0,1.0); 
    TEfficiency* pEff4 = new TEfficiency("eff4","Efficiency vs P; P;#epsilon",50., 0.0,2.5); 

    // Fake estimation
    //-------------------
    snprintf(name,100,"Ideal reco tracks, Pt ");
    TH1D* htrk30 = new TH1D("htrk30", name, 50, 0.0, 1.0);      // mu

    snprintf(name,100,"Fake rate vs Pt ");
    TH1D* htrk31 = new TH1D("htrk31", name, 50, 0.0, 1.0);      // mu

    snprintf(name,100,"Ideal reco tracks, P ");
    TH1D* htrk32 = new TH1D("htrk32", name, 50, 0.0, 4.0);

    snprintf(name,100,"Fake rate vs P ");
    TH1D* htrk33 = new TH1D("htrk33", name, 50, 0.0, 4.0);

    //----------------------------------------------------

    snprintf(name,100,"Ideal reco tracks, #theta ");
    TH1D* htrk40 = new TH1D("htrk40", name, 60, 0.0, 3.2);

    snprintf(name,100,"Fake rate vs #theta ");
    TH1D* htrk41 = new TH1D("htrk41", name, 60, 0.0, 3.2);

    snprintf(name,100,"Ideal reco tracks, #phi ");
    TH1D* htrk42 = new TH1D("htrk42", name, 120, -3.2, 3.2);

    snprintf(name,100,"Fake rate vs #phi ");
    TH1D* htrk43 = new TH1D("htrk43", name, 120, -3.2, 3.2);

    TEfficiency* pFake1 = new TEfficiency("fake1","Fake rate vs #theta;#theta;fake",40, 0.0,3.2); 
    TEfficiency* pFake2 = new TEfficiency("fake2","Fake rate vs #phi;#phi;fake",40, -3.2,3.2); 
    TEfficiency* pFake3 = new TEfficiency("fake3","Fake rate vs P_{T}; P_{T}, GeV/c;fake",40., 0.0,1.0); 
    TEfficiency* pFake4 = new TEfficiency("fake4","Fake rate vs P; P, GeV/c;fake",40., 0.0,2.5); 

    //---------------------------------------------------

    const Int_t NEvents = IT->GetNEvents();

    //------------------------------------------------------------
    Int_t ne_total(0);
    Int_t nevent(-1);

//    Bool_t   onlyPrim = true;        // true - used only primary particles (else all)
    Bool_t   onlyPrim = false;        // false also for inline generator
    Double_t ptmin = 0.025;          // min pt for selection
    Double_t pmin  = 0.075;          // min momentum for selection
    Double_t minRts= 0.4;            // min common Ts hits for identical tracks

//    while (IT->NextEvent() && ne_total<10) 
    while (IT->NextEvent()) 
    {
        ++nevent;

//        if (nevent == 0) continue;
        
        cout << " Event = " << nevent << " Reco tracks (Seeds) = " 
             << TracksRC_->GetEntriesFast() 
             << " (" << TrackSeedsRC_->GetEntriesFast() << ")"
             << " Ideal MC = " << TracksMC_->GetEntriesFast() 
             << endl;

        // ideal reco
        //------------
        Int_t nTrkMc = 0; 
        for (int imc=0; imc < TracksMC_->GetEntriesFast(); imc++)
        {
           SpdTrackMC* mc_tracks = (SpdTrackMC*)TracksMC_->At(imc);  //
           if (!mc_tracks) continue;

           SpdTrackFitPar* fitpars = mc_tracks->GetFitPars();
           if ( !fitpars ) continue;

           SpdTrackState* trkfirststate = fitpars->GetFirstState();
           if (!trkfirststate) continue;

           if (mc_tracks->GetVertexId() != 0 && onlyPrim) continue;   //  only primary particles

           TVector3 p3 = trkfirststate->GetMomentum();

           if (p3.Perp() < ptmin) continue;
           if (p3.Mag()  < pmin) continue;

           ++nTrkMc;
           htrk4->Fill( float(p3.Mag()) );                    // momentum
        }
        htrk2->Fill( float(nTrkMc) );

        // reco 
        //------
        Int_t nTrkRc = 0; 
        for (int irc=0; irc < TracksRC_->GetEntriesFast(); irc++) 
        {
           SpdTrackRC* rc_tracks = (SpdTrackRC*)TracksRC_->At(irc);  
           if (!rc_tracks) continue;

           SpdTrackFitPar* fitpars = rc_tracks->GetFitPars();
           if ( !fitpars ) continue;

           SpdTrackState* trkfirststate = fitpars->GetFirstState();
           if (!trkfirststate) continue;

           TVector3 p3 = trkfirststate->GetMomentum();

           if (p3.Perp() < ptmin) continue;
           if (p3.Mag()  < pmin) continue;

           ++nTrkRc;

           htrk3->Fill( float(p3.Mag()) );                   // momentum    
        }
        htrk1->Fill( float(nTrkRc) );

        // estimation of reco track efficiency:
        // loop over ideal reco track and try to find reco track
        // which has the same Its hits and ratio Ts hits > 0.50
        //-------------------------------------------------------
        Int_t totMC, findRC, fakeTrk;
        totMC = findRC = fakeTrk;

        TVector3 pMc;
        TVector3 pRc;
    
        std::vector<Int_t> vIts1;
        std::vector<Int_t> vTs1;

        std::vector<Int_t> vIts2;
        std::vector<Int_t> vTs2;

        std::vector<Int_t> vIts3;
        std::vector<Int_t> vTs3;

        Int_t mcTrk, rcTrk;
        mcTrk = rcTrk = 0;
        Bool_t bPassed;
        Int_t passNtrk = 0;

        for (int imc=0; imc < TracksMC_->GetEntriesFast(); imc++)  // loop over MC track
        {
           SpdTrackMC* mc_tracks = (SpdTrackMC*)TracksMC_->At(imc);  
           if (!mc_tracks) continue;

           SpdTrackFitPar* mcpars = mc_tracks->GetFitPars();
           if ( !mcpars ) continue;

           SpdTrackState* mcfirststate = mcpars->GetFirstState();
           if (!mcfirststate) continue;

           pMc = mcpars->GetFirstState()->GetMomentum();             // mom

           if (mc_tracks->GetVertexId() != 0 && onlyPrim) continue;  // primary tracks
          
           if (pMc.Perp() < ptmin) continue;                         // pt selection
           if (pMc.Mag()  < pmin) continue;                          // p selection

           ++mcTrk;

           htrk10->Fill( float(pMc.Perp()) );
           htrk20->Fill( float(pMc.Theta()) );
           htrk13->Fill( float(pMc.Mag()) );
           htrk23->Fill( float(pMc.Phi()) );

           bPassed = false;

           vIts1.clear();
            vTs1.clear();
           const std::vector<spd_point>& mcPoints = mc_tracks->GetTrackPoints();           
           for (Int_t i(0); i<mcPoints.size(); ++i)
           {
              if (mcPoints[i].second==10) vIts1.push_back(mcPoints[i].first);
              if (mcPoints[i].second==20)  vTs1.push_back(mcPoints[i].first);
           }

           if (vIts1.size() < 2) continue; 
           if (vTs1.size()  < 5) continue; 

           // loop over reco tracks
           //-----------------------
           for (int irc=0; irc < TracksRC_->GetEntriesFast(); irc++)    
           {
              SpdTrackRC* rc_tracks = (SpdTrackRC*)TracksRC_->At(irc);  
              if (!rc_tracks) continue;

              SpdTrackFitPar* rcpars = rc_tracks->GetFitPars();
              if ( !rcpars ) continue;

              SpdTrackState* rcfirststate = rcpars->GetFirstState();
              if (!rcfirststate) continue;

              pRc = rcfirststate->GetMomentum();

              vIts2.clear();
              vIts3.clear();
               vTs2.clear();
               vTs3.clear();
              const std::vector<spd_point>& rcPoints = rc_tracks->GetTrackPoints();           
              for (Int_t i(0); i<rcPoints.size(); ++i)
              {
                 Int_t ip = rcPoints[i].first;

                 if (rcPoints[i].second == 10) 
                 {
                    vIts2.push_back(rcPoints[i].first);
                    if (std::find(vIts1.begin(),vIts1.end(),ip) != vIts1.end()) vIts3.push_back(ip);
                 }
  
                 if (rcPoints[i].second == 20)
                 {
                    vTs2.push_back(rcPoints[i].first);
                    if (std::find(vTs1.begin(),vTs1.end(),ip) != vTs1.end()) vTs3.push_back(ip);
                 }
              }

              // identical tracks
              //-----------------
              if (vIts3.size() <= 0 ) continue;        // no common Its hits
              if (vTs3.size() <= 0 )  continue;        // no common Ts hits
              
//              Double_t ratio = (Double_t)vTs3.size()/(Double_t)vTs1.size();

//              if (vIts1.size()==2 && vIts3.size() < 2 && ratio < minRts) continue;              
//              if (vIts1.size()==3 && vIts3.size() < 2 && ratio < minRts) continue;              
//              if (vIts1.size()==4 && vIts3.size() < 3 && ratio < minRts) continue;              
//              if (vIts1.size()==5 && vIts3.size() < 3 && ratio < minRts) continue;              

              Int_t nTot1, nTot2, nTot3;
              Double_t r1, r2;
              r1 = r2 = 0.0;

              nTot1 = vIts1.size()+vTs1.size();
              nTot2 = vIts2.size()+vTs2.size();
              nTot3 = vIts3.size()+vTs3.size();

              if (nTot1 > 0) r1 = (Double_t)nTot3/nTot1;
              if (nTot2 > 0) r2 = (Double_t)nTot3/nTot2;

              if (vIts3.size() < 2)
              { 
                 if (vTs1.size() < 8) continue;
                 if (r2 < minRts && r1 < minRts) continue;
              }
              else
              {
                 if (r2 < minRts && r1 < minRts) continue;
              }

              // reco difference
              //-----------------
              Double_t dTheta = pRc.Theta()-pMc.Theta();
              Double_t dPhi   = pRc.Phi()-pMc.Phi();
              Double_t dR = sqrt(dTheta*dTheta+dPhi*dPhi);

              htrk5->Fill( float( 180.*(pRc.Theta()-pMc.Theta()) ) );
              htrk6->Fill( float( 180.*(pRc.Phi()-pMc.Phi())     ) );
              htrk7->Fill( float((pRc.Mag()-pMc.Mag())/pMc.Mag()) );

              htrk8->Fill( float(vTs1.size()) );
              htrk9->Fill( float(vTs2.size()) );

              // for efficiency
              //----------------
              bPassed = true;

              htrk11->Fill( float(pMc.Perp()) );
              htrk21->Fill( float(pMc.Theta()) );
              htrk14->Fill( float(pMc.Mag()) );
              htrk24->Fill( float(pMc.Phi()) );

             break;    
           } 

           // efficiency
           //------------ 
           pEff1->Fill(bPassed,pMc.Theta());
           pEff2->Fill(bPassed,pMc.Phi());
           pEff3->Fill(bPassed,pMc.Perp());
           pEff4->Fill(bPassed,pMc.Mag());

        }

        // Fake rate estimation
        //---------------------
        for (int irc=0; irc < TracksRC_->GetEntriesFast(); irc++)  // loop over RC track
        {
           SpdTrackRC* rc_tracks = (SpdTrackRC*)TracksRC_->At(irc);  
           if (!rc_tracks) continue;

           SpdTrackFitPar* rcpars = rc_tracks->GetFitPars();
           if ( !rcpars ) continue;

           SpdTrackState* rcfirststate = rcpars->GetFirstState();
           if (!rcfirststate) continue;

           pRc = rcpars->GetFirstState()->GetMomentum();                // mom
          
           if (pRc.Perp() < ptmin) continue;                     // pt selection
           if (pRc.Mag()  < pmin) continue;                      // p selection

           htrk30->Fill( float(pRc.Perp()) );
           htrk40->Fill( float(pRc.Theta()) );
           htrk32->Fill( float(pRc.Mag()) );
           htrk42->Fill( float(pRc.Phi()) );

           bPassed = false;

           vIts1.clear();
            vTs1.clear();
           const std::vector<spd_point>& rcPoints = rc_tracks->GetTrackPoints();           
           for (Int_t i(0); i<rcPoints.size(); ++i)
           {
              if (rcPoints[i].second==10) vIts1.push_back(rcPoints[i].first);
              if (rcPoints[i].second==20)  vTs1.push_back(rcPoints[i].first);
           }

           // loop over ideal reco tracks
           //-------------------------------
           for (int imc=0; imc < TracksMC_->GetEntriesFast(); imc++)    
           {
              SpdTrackMC* mc_tracks = (SpdTrackMC*)TracksMC_->At(imc);  
              if (!mc_tracks) continue;

              SpdTrackFitPar* mcpars = mc_tracks->GetFitPars();
              if ( !mcpars ) continue;

              SpdTrackState* mcfirststate = mcpars->GetFirstState();
              if (!mcfirststate) continue;

              if (mc_tracks->GetVertexId() != 0 && onlyPrim) continue;  // primary tracks

              pMc = mcfirststate->GetMomentum();

//              if (pMc.Perp() < ptmin) continue;                     // pt selection
//              if (pMc.Mag()  < pmin) continue;                      // p selection

              vIts2.clear();
              vIts3.clear();
               vTs2.clear();
               vTs3.clear();
              const std::vector<spd_point>& mcPoints = mc_tracks->GetTrackPoints();           
              for (Int_t i(0); i<mcPoints.size(); ++i)
              {
                 Int_t ip = mcPoints[i].first;

                 if (mcPoints[i].second == 10) 
                 {
                    vIts2.push_back(mcPoints[i].first);
                    if (std::find(vIts1.begin(),vIts1.end(),ip) != vIts1.end()) vIts3.push_back(ip);
                 }
  
                 if (mcPoints[i].second == 20)
                 {
                    vTs2.push_back(mcPoints[i].first);
                    if (std::find(vTs1.begin(),vTs1.end(),ip) != vTs1.end()) vTs3.push_back(ip);
                 }
              }

              // identical tracks
              //-----------------
              if (vIts3.size() <= 0 ) continue;

              Double_t ratio = (Double_t)vTs3.size()/(Double_t)vTs2.size();

              if (vIts1.size()==2 && vIts3.size() < 2 && ratio < minRts) continue;              
              if (vIts1.size()==3 && vIts3.size() < 2 && ratio < minRts) continue;              
              if (vIts1.size()==4 && vIts3.size() < 3 && ratio < minRts) continue;              
              if (vIts1.size()==5 && vIts3.size() < 3 && ratio < minRts) continue;       

              // for fake rate

              bPassed = true;

              break;
           }

           if (!bPassed)
           {
              htrk31->Fill( float(pRc.Perp()) );
              htrk41->Fill( float(pRc.Theta()) );
              htrk33->Fill( float(pRc.Mag()) );
              htrk43->Fill( float(pRc.Phi()) );
           }

           pFake1->Fill(!bPassed,pRc.Theta());
           pFake2->Fill(!bPassed,pRc.Phi());
           pFake3->Fill(!bPassed,pRc.Perp());
           pFake4->Fill(!bPassed,pRc.Mag());

        }
  
        DumpEvent();

        ne_total++;
    }   

    cout << "\n>>>>>> Events (total): " << ne_total << endl;
    
    //----------------------------------------------------
    //gStyle->SetOptStat(0);
    gStyle->SetFrameLineWidth(3);

    TLegend *leg[20];
    Double_t xl1=0.50, yl1=0.75, xl2=xl1+0.20, yl2=yl1+0.10;

    //--------- multiplicity and track momentum -------------------
 
    TCanvas* c = new TCanvas("c", "", 1200, 800);     
    c->Divide(2,2);     

    c->cd(1);     
    htrk1->SetLineWidth(2.);
    htrk1->GetXaxis()->SetTitleSize(0.05);
    htrk1->GetXaxis()->SetTitle("Number tracks");
    htrk1->Draw();     

    c->cd(2);     
    htrk2->SetLineWidth(2.);
    htrk2->GetXaxis()->SetTitleSize(0.05);
    htrk2->GetXaxis()->SetTitle("Number tracks");
    htrk2->Draw();     

    c->cd(3);
    htrk3->SetLineWidth(2.);
    htrk3->GetXaxis()->SetTitleSize(0.05);
    htrk3->GetXaxis()->SetTitle("GeV/c");
    htrk3->Draw();

    c->cd(4);
    htrk4->SetLineWidth(2.);
    htrk4->GetXaxis()->SetTitleSize(0.05);
    htrk4->GetXaxis()->SetTitle("GeV/c");
    htrk4->Draw();

    //------ dTheta, dPhi, dP and Ts hits ------------------

    TCanvas* c1 = new TCanvas("c1", "", 1200, 800);
    c1->Divide(2,2);

    c1->cd(1);
    htrk5->SetLineWidth(2.);
    htrk5->GetXaxis()->SetTitleSize(0.05);
    htrk5->GetXaxis()->SetTitle("Degree");
    htrk5->Draw();

    c1->cd(2);
    htrk6->SetLineWidth(2.);
    htrk6->GetXaxis()->SetTitleSize(0.05);
    htrk6->GetXaxis()->SetTitle("Degree");
    htrk6->Draw();

    c1->cd(3);
    htrk7->SetLineWidth(2.);
    htrk7->GetXaxis()->SetTitleSize(0.05);
    htrk7->Draw();

    c1->cd(4);

    Int_t yMax = TMath::Max(htrk8->GetMaximum(),htrk9->GetMaximum());

    Double_t xl01=0.20, yl01=0.75, xl02=xl01+0.20, yl02=yl01+0.10;
    leg[19] = new TLegend(xl01,yl01,xl02,yl02); 

    snprintf(name,50,"Ideal reco");
    leg[19]->AddEntry(htrk8,name,"L");

    htrk8->SetLineWidth(2.);
    htrk8->GetXaxis()->SetTitleSize(0.05);
    htrk8->GetXaxis()->SetTitle("Ts hits");
    htrk8->GetYaxis()->SetRangeUser(0.0, 1.05*yMax);
    htrk8->Draw();

    snprintf(name,50,"Real reco");
    leg[19]->AddEntry(htrk9,name,"L");

    htrk9->SetLineWidth(2.);
    htrk9->SetLineColor(2);
    htrk8->GetYaxis()->SetRangeUser(0.0, 1.05*yMax);

    htrk9->Draw("same");

    leg[19]->Draw();

    //------------- track pt and momentum efficiency ------------------

    TCanvas* c2 = new TCanvas("c2", "", 1200, 800);
    c2->Divide(2,2);

    c2->cd(1);

    leg[0] = new TLegend(xl1,yl1,xl2,yl2);    

    snprintf(name,50,"Ideal reco");
    leg[0]->AddEntry(htrk10,name,"L");

    htrk10->SetLineWidth(2.);
    htrk10->GetXaxis()->SetTitleSize(0.05);
    htrk10->GetXaxis()->SetTitle("GeV/c");
    htrk10->Draw();

    snprintf(name,50,"Ideal (+Reco)");
    leg[0]->AddEntry(htrk11,name,"L");

    htrk11->SetLineWidth(2.);
    htrk11->SetLineColor(2);
    htrk11->Draw("same");

    leg[0]->Draw();

    c2->cd(2);
    TH1D* htrk12 = (TH1D*)htrk11->Clone();

    htrk12->Divide(htrk10);
    htrk12->GetXaxis()->SetTitle("P_{T}, GeV/c");
    htrk12->GetYaxis()->SetRangeUser(0.0, 1.5);
    htrk12->Draw();

    c2->cd(3);

    leg[1] = new TLegend(xl1,yl1,xl2,yl2);    

    snprintf(name,50,"Ideal reco");
    leg[1]->AddEntry(htrk13,name,"L");

    htrk13->SetLineWidth(2.);
    htrk13->GetXaxis()->SetTitleSize(0.05);
    htrk13->GetXaxis()->SetTitle("GeV/c");

    snprintf(name,50,"Ideal (+Reco)");
    leg[1]->AddEntry(htrk14,name,"L");

    htrk13->Draw();

    htrk14->SetLineWidth(2.);
    htrk14->SetLineColor(2);

    htrk14->Draw("same");

    leg[1]->Draw();

    c2->cd(4);
    TH1D* htrk15 = (TH1D*)htrk14->Clone();
    htrk15->Divide(htrk13);
    htrk15->GetXaxis()->SetTitle("GeV/c");
    htrk15->GetYaxis()->SetRangeUser(0.0, 1.5);

    htrk15->Draw();

    //-------  efficiency vs theta and phi -----------------------

    TCanvas* c3 = new TCanvas("c3", "", 1200, 800);
    c3->Divide(2,2);

    c3->cd(1);

    leg[2] = new TLegend(xl1,yl1,xl2,yl2);    

    snprintf(name,50,"Ideal reco");
    leg[2]->AddEntry(htrk20,name,"L");


    htrk20->SetLineWidth(2.);
    htrk20->GetXaxis()->SetTitleSize(0.05);
    htrk20->GetXaxis()->SetTitle("#theta");
    htrk20->Draw();

    snprintf(name,50,"Ideal (+Reco)");
    leg[2]->AddEntry(htrk21,name,"L");

    htrk21->SetLineWidth(2.);
    htrk21->SetLineColor(2);
    htrk21->Draw("same");

    leg[2]->Draw();

    c3->cd(2);

    TH1D* htrk22 = (TH1D*)htrk21->Clone();

    htrk22->Divide(htrk20);
    htrk22->GetYaxis()->SetRangeUser(0.0, 1.5);

    htrk22->GetXaxis()->SetTitleSize(0.05);
    htrk22->GetXaxis()->SetTitle("#theta");

    htrk22->Draw();

    c3->cd(3);

    leg[3] = new TLegend(xl1,yl1,xl2,yl2);    

    snprintf(name,50,"Ideal reco");
    leg[3]->AddEntry(htrk23,name,"L");

    htrk23->SetLineWidth(2.);
    htrk23->GetXaxis()->SetTitleSize(0.05);
    htrk23->GetXaxis()->SetTitle("#phi");

    htrk23->GetYaxis()->SetRangeUser(0.0, 150.);
    htrk23->Draw();

    snprintf(name,50,"Ideal (+Reco)");
    leg[3]->AddEntry(htrk24,name,"L");

    htrk24->SetLineWidth(2.);
    htrk24->SetLineColor(2);
    htrk24->GetYaxis()->SetRangeUser(0.0, 150.);
    htrk24->Draw("same");

    leg[3]->Draw();

    c3->cd(4);
    TH1D* htrk25 = (TH1D*)htrk24->Clone();

    htrk25->Divide(htrk23);
    htrk25->GetYaxis()->SetRangeUser(0.0, 1.5);

    htrk25->GetXaxis()->SetTitleSize(0.05);
    htrk25->GetXaxis()->SetTitle("#phi");

    htrk25->Draw();


    //------------ efficiency with error bar --------------------

    TCanvas* c4 = new TCanvas("c4", "", 1200, 800);
    c4->Divide(2,2);

    c4->cd(1);
    pEff3->SetFillColor(2);
    pEff3->Draw("AP");
//    pEff3->Draw("A4");

    c4->cd(2);
    pEff4->SetFillColor(4);
    pEff4->Draw("AP");
//    pEff3->Draw("A4");

    c4->cd(3);
    pEff1->SetFillColor(4);
    pEff1->Draw("AP");
//    pEff1->Draw("A4");

    c4->cd(4);
    pEff2->SetFillColor(4);
//    pEff2->Draw("AP");
    pEff2->Draw("A4");

   // Fake rate histos
   //------------------

    TCanvas* c5 = new TCanvas("c5", "", 1200, 800);
    c5->Divide(2,2);

    c5->cd(1);

    leg[10] = new TLegend(xl1,yl1,xl2,yl2);    

    snprintf(name,50,"Ideal reco");
    leg[10]->AddEntry(htrk30,name,"L");

    htrk30->SetLineWidth(2.);
    htrk30->GetXaxis()->SetTitleSize(0.05);
    htrk30->GetXaxis()->SetTitle("GeV/c");
    htrk30->Draw();

    snprintf(name,50,"Ideal (+Reco)");
    leg[10]->AddEntry(htrk31,name,"L");

    htrk31->SetLineWidth(2.);
    htrk31->SetLineColor(2);
    htrk31->Draw("same");

    leg[10]->Draw();

    c5->cd(2);
    TH1D* htrk35 = (TH1D*)htrk31->Clone();

    htrk35->Divide(htrk30);
    htrk35->GetXaxis()->SetTitle("P_{T}, GeV/c");
    htrk35->GetYaxis()->SetRangeUser(0.0, 1.5);
    htrk35->Draw();

    // momentum

    c5->cd(3);

    leg[11] = new TLegend(xl1,yl1,xl2,yl2);

    snprintf(name,50,"Ideal reco");
    leg[11]->AddEntry(htrk32,name,"L");

    htrk32->SetLineWidth(2.);
    htrk32->GetXaxis()->SetTitleSize(0.05);
    htrk32->GetXaxis()->SetTitle("GeV/c");
    htrk32->Draw();

    snprintf(name,50,"Ideal (+Reco)");
    leg[11]->AddEntry(htrk33,name,"L");

    htrk33->SetLineWidth(2.);
    htrk33->SetLineColor(2);
    htrk33->Draw("same");

    leg[11]->Draw();

    c5->cd(4);
    TH1D* htrk36 = (TH1D*)htrk33->Clone();

    htrk36->Divide(htrk32);
    htrk36->GetXaxis()->SetTitle("P_{T}, GeV/c");
    htrk36->GetYaxis()->SetRangeUser(0.0, 1.5);
    htrk36->Draw();

    // fake rate theta and phi
    //-------------------------
    TCanvas* c6 = new TCanvas("c6", "", 1200, 800);
    c6->Divide(2,2);

    c6->cd(1);

    leg[12] = new TLegend(xl1,yl1,xl2,yl2);    

    snprintf(name,50,"Ideal reco");
    leg[12]->AddEntry(htrk40,name,"L");

    htrk40->SetLineWidth(2.);
    htrk40->GetXaxis()->SetTitleSize(0.05);
    htrk40->GetXaxis()->SetTitle("#theta");
    htrk40->Draw();

    snprintf(name,50,"Ideal (+Reco)");
    leg[12]->AddEntry(htrk41,name,"L");

    htrk41->SetLineWidth(2.);
    htrk41->SetLineColor(2);
    htrk41->Draw("same");

    leg[12]->Draw();

    c6->cd(2);

    TH1D* htrk45 = (TH1D*)htrk41->Clone();

    htrk45->Divide(htrk40);
    htrk45->GetYaxis()->SetRangeUser(0.0, 1.5);

    htrk45->GetXaxis()->SetTitleSize(0.05);
    htrk45->GetXaxis()->SetTitle("#theta");

    htrk45->Draw();

    c6->cd(3);

    leg[13] = new TLegend(xl1,yl1,xl2,yl2);    

    snprintf(name,50,"Ideal reco");
    leg[13]->AddEntry(htrk42,name,"L");

    htrk42->SetLineWidth(2.);
    htrk42->GetXaxis()->SetTitleSize(0.05);
    htrk42->GetXaxis()->SetTitle("#phi");

    htrk42->GetYaxis()->SetRangeUser(0.0, 150.);
    htrk42->Draw();

    snprintf(name,50,"Ideal (+Reco)");
    leg[13]->AddEntry(htrk43,name,"L");

    htrk43->SetLineWidth(2.);
    htrk43->SetLineColor(2);
    htrk43->GetYaxis()->SetRangeUser(0.0, 150.);
    htrk43->Draw("same");

    leg[13]->Draw();

    c6->cd(4);
    TH1D* htrk46 = (TH1D*)htrk43->Clone();

    htrk46->Divide(htrk42);
    htrk46->GetYaxis()->SetRangeUser(0.0, 1.5);

    htrk46->GetXaxis()->SetTitleSize(0.05);
    htrk46->GetXaxis()->SetTitle("#phi");

    htrk46->Draw();

    // fake rate with error bar

    TCanvas* c7 = new TCanvas("c7", "", 1200, 800);
    c7->Divide(2,2);

    c7->cd(1);
    pFake3->SetFillColor(2);
    pFake3->Draw("AP");

    c7->cd(2);
    pFake4->SetFillColor(4);
    pFake4->Draw("AP");

    c7->cd(3);
    pFake1->SetFillColor(4);
    pFake1->Draw("AP");

    c7->cd(4);
    pFake2->SetFillColor(4);
//    pEff2->Draw("AP");
    pFake2->Draw("A4");



}



