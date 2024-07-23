#include <TFile.h>
#include <TTree.h>
#include <Riostream.h>
#include <TGeoManager.h>
#include <TClonesArray.h>
#include "FairGeoParSet.h"
#include "SpdKFSimpleRes.h"
#include "SpdBaseParSet.h"
#include <vector>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TProfile2D.h>

TTree* Tree_ = 0;

void check_KFres();
void check_geometry();

TString inFile, inParFile;

//______________________________________________________________________________
void DrawVtx(TString path = "") 
{
   if (!path.IsWhitespace() && !path.EndsWith("/")) path += "/";
    
   inFile    = path + "FittedTracksVtx.root";   
   inParFile = path + "params.root";
   
   TFile* fp = new TFile(inParFile);
   if (fp) {
      SpdPassiveGeoParSet* passive_pars = (SpdPassiveGeoParSet*)fp->Get("PassiveGeoParSet");
      if (passive_pars) passive_pars->print(1);  
      fp->Close();
   }
   
   TFile* f = new TFile(inFile);
   if (!f) return;
   
   Tree_ = (TTree*)f->Get("cbmsim");  
   if (!Tree_) return;
 
   SpdBaseParSet* pars = (SpdBaseParSet*)f->Get("IdealKalmanFitterParams");
   if (pars) pars->print(1);
   
   cout << "\n>>>>>>>>>>>>>>>> DATA FILE & TREE: Ok! <<<<<<<<<<<<<<<<< " << endl;  
   
   check_KFres();
}

//-----------------------------------------------------------------------------
Bool_t AcceptTrack(SpdKFSimpleRes* res, Int_t ntrack) 
{
   
    //if (res->GetIsFitted(ntrack))
    //if (res->GetIsFullyConvergent(ntrack))    
    //if (res->HasNoErrors(ntrack))         
    if (res->GetIsGood(ntrack)) 
    //if (res->GetMCPtheta(ntrack) > 20.  &&  res->GetMCPtheta(ntrack) < 160.)
    //if (res->GetMCPtheta(ntrack) > 100. ||  res->GetMCPtheta(ntrack) < 80.)    
    {
        return true;
    }

    return false;    
}

//______________________________________________________________________________
void check_KFres() 
{
   if (!Tree_) return;  
   
   TBranch* branch = Tree_->GetBranch("SpdKFSimpleRes.");
   if (!branch) return;
   
   int entries = branch->GetEntries();
   
   cout << "Total number of ENTRIES in the TREE = " << entries << endl;    
   
   if (entries == 0) return;
   
   SpdKFSimpleRes* Res = 0;
   
   Tree_->SetBranchAddress("SpdKFSimpleRes.",&Res) ;       

   Int_t NWellFittedEvents = 0;
   Int_t NTotalTracks = 0;
   Int_t NWellFittedTracks = 0;
   Int_t ntracks = 0;
   
   TH1D* hN = new TH1D("Multiplicity (mc)","n_{tracks}/event (mc)",25,0,25);
   TH1D* hn = new TH1D("Multiplicity (mc,selected)","n_{tracks}/event (mc)",25,0,25);
   
   TH1D* hchi2ndf = new TH1D("chi2/ndf","#chi^{2}/ndf ",100,0,2);
   
   // seed values
   TH1D* hp  = new TH1D("p (mc)","p (mc)",100,0,10);
   TH1D* hpt = new TH1D("pt (mc)","p_{T} (mc)",100,0,5);
   TH1D* hpz = new TH1D("pz (mc)","p_{z} (mc)",100,-10,10);
   TH1D* hth = new TH1D("theta (mc)","#theta (mc)",180,0,180);
   TH1D* hfi = new TH1D("phi (mc)","#phi (mc)",180,-180,180);
   
   // resolution
   TH1D* hpr   = new TH1D("dp/p","|dp|/p",50,0,1);
   TH1D* hptr  = new TH1D("dpt/pt","|dp_{T}|/p_{T}",50,0,1);
   TH1D* hpzr  = new TH1D("dpz/pz","|dp_{z}/p_{z}|",50,0,1);
   TH1D* hthr  = new TH1D("dtheta","|d#theta|",180,0,45);
   TH1D* hfir  = new TH1D("dphi","|d#phi|",180,0,180);

   // dp/p
   TProfile*   hppr   = new TProfile("dp/p vs. p "," |dp|/p vs. p ",20,0,10, 0,1.0);
   TProfile*   hpthr  = new TProfile("dp/p vs. theta"," |dp|/p vs. #theta",60,0,180, 0,1.0);
   TProfile*   hpfir  = new TProfile("dp/p vs. phi"," |dp|/p vs. #phi",90,-180,180, 0,1.0);
   TProfile2D* hhppth = new TProfile2D("dp/p(theta,p)","|dp|/p (#theta,p)", 40,0,180, 40,0,10, 0,0.1);
   
   // dpt/pt
   TProfile*   hptpr   = new TProfile("dpt/pt vs. p "," |dp_{T}|/p_{T} vs. p ",20,0,10, 0,1.0);
   TProfile*   hptthr  = new TProfile("dpt/pt vs. theta"," |dp_{T}|/p_{T} vs. #theta",60,0,180, 0,1.0); 
   TProfile*   hptfir  = new TProfile("dpt/pt vs. phi"," |dp_{T}|/p_{T} vs. #phi",90,-180,180, 0,1.0);
   TProfile2D* hhptpth = new TProfile2D("dpt/pt(theta,p)","|dp_{T}|/p_{T} (#theta,p)", 40,0,180, 40,0,10, 0,0.1);
   
   // dpz/pz
   TProfile*   hpzpr   = new TProfile("dpz/pz vs. p "," |dp_{z}/p_{z}| vs. p ",20,0,10, 0,1.0);
   TProfile*   hpzthr  = new TProfile("dpz/pz vs. theta"," |dp_{z}/p_{z}| vs. #theta",60,0,180, 0,1.0);
   TProfile*   hpzfir  = new TProfile("dpz/pz vs. phi"," |dp_{z}|/p_{z} vs. #phi",90,-180,180, 0,1.0);
   TProfile2D* hhpzpth = new TProfile2D("dpz/p(theta,p})","|dp_{z}/p_{z}| (#theta,p)", 40,0,180, 40,0,10, 0,0.1);
   
   // dtheta
   TProfile*   hthpr   = new TProfile("dtheta vs. p ","|d#theta| vs. p ",20,0,1, 0,1.0);
   TProfile*   hththr  = new TProfile("dtheta vs. theta"," |d#theta| vs. #theta",60,0,180, 0,1.0);
   TProfile*   hthfir  = new TProfile("dtheta vs. phi"," |d#theta| vs. #phi",90,-180,180, 0,1.0);
   TProfile2D* hhthpth = new TProfile2D("d#theta(theta,p)","|d#theta}| (#theta,p)", 40,0,180, 40,0,10, 0,0.1);
   
   //chisquare
   TProfile2D* hhchi2pth = new TProfile2D("chisquare(theta,p)","#chi^{2} (#theta,p)", 100,0,180, 100,0,10, 0,2.);
   TProfile*   hchi2pr   = new TProfile("chisquare vs. dp/p","#chi^{2} vs. |dp|/p", 50,0,0.1, 0,2);
   TProfile*   hchi2ptr  = new TProfile("chisquare vs. dpt/pt","#chi^{2} vs. |dp_{T}|/p_{T}", 50,0,0.1, 0,2);
   TProfile*   hchi2pzr  = new TProfile("chisquare vs. dpz/pz","#chi^{2} vs. |dp_{z}/p_{z}|", 50,0,0.1, 0,2);
   TProfile*   hchi2thr  = new TProfile("chisquare vs. d#theta","#chi^{2} vs. |d#theta|", 50,0,5., 0,2);
   
   // additional
   TProfile*   hpptr  = new TProfile("dp/p vs. pt"," |dp|/p vs. p_{T}",20,0, 5, 0,1.0);
   TProfile*   hppzr  = new TProfile("dp/p vs. pz"," |dp|/p vs. |p_{z}|",20,0,10, 0,1.0);
   TProfile*   hptptr = new TProfile("dpt/pt vs. pt"," |dp_{T}|/p_{T} vs. p_{T}",20,0, 5, 0,1.0);
   TProfile*   hptpzr = new TProfile("dpt/pt vs. pz"," |dp_{T}|/p_{T} vs. |p_{z}|",20,0,10, 0,1.0);
   TProfile*   hpzptr = new TProfile("dpz/pz vs. pt"," |dp_{z}/p_{z}| vs. p_{T}",20,0, 5, 0,1.0);
   TProfile*   hpzpzr = new TProfile("dpz/pz vs. pz"," |dp_{z}/p_{z}| vs. |p_{z}|",20,0,10, 0,1.0);
 
   // vertex 
   TH1D* hvr    = new TH1D("Vertex: r (mc)","Vertex:  r (mc)",100,0,3.0);
   TH1D* hvdr   = new TH1D("Vertex: dr","Vertex: |dr| ",100,0,1.); 
   TH1D* hvz    = new TH1D("Vertex: Z (mc)","Vertex: Z (mc)",100,-2.5,2.5);
   TH1D* hvdz   = new TH1D("Vertex: dZ","Vertex: dZ ",200,-1,1); 
   TH1D* hvrt   = new TH1D("Vertex: rt","Vertex: r_{T} ",100,0,0.071);  // 0.071 = ~Sqrt(2*0.05*0.05)
   TH1D* hvdphi = new TH1D("Vertex: dphi","Vertex: |d#phi_{r}| ",180,0,180); 
   
   // hits
   TH1D* hitsh   = new TH1D("itsNH","Its hits/track",10,0,10);
   TH1D* htstbh  = new TH1D("tstbNH","TsTB hits/track",120,0,120);
   TH1D* htstech = new TH1D("tstecNH","TsTEC hits/track",75,0,75);
   TH1D* htotalh = new TH1D("totalNH","Total hits/track",140,0,140);
   
   TProfile2D* hpthh      = new TProfile2D("nh/track(theta,p)","total: hits/track(#theta,p)", 90,0,180, 90,0,10, 0,150);
   TProfile2D* hitspthh   = new TProfile2D("its nh/track(theta,p)","its: hits/track(#theta,p)", 90,0,180, 90,0,10, 0,150);
   TProfile2D* htstbpthh  = new TProfile2D("tstb nh/track(theta,p)","tstb: hits/track(#theta,p)", 90,0,180, 90,0,10, 0,150);
   TProfile2D* htstecpthh = new TProfile2D("tstec nh/track(theta,p)","tstec: hits/track(#theta,p)", 90,0,180, 90,0,10, 0,150);
    
   for (int j(0); j < entries; j++) {
        
        branch->GetEntry(j);
        //cout << "\n------------------------------------------------------------------------------\n";
        
        //Res->PrintData(1);  // 0(default) or 1
        
        if (Res->IsWellFittedEvent()) NWellFittedEvents++;
        //else continue;
        
        ntracks = Res->GetNTracks();
        NTotalTracks += ntracks;
        
        Int_t nm(0);
        
        for (Int_t i(0); i<ntracks; i++) {
             if (AcceptTrack(Res,i))  
             { 
                 NWellFittedTracks++;
                 
                 hchi2ndf->Fill(Res->GetChi2toNDF(i));
                 
                 // seed values
                 hp->Fill(Res->GetMomentumMC(i));
                 hth->Fill(Res->GetMCPtheta(i));
                 hfi->Fill(Res->GetMCPphi(i));
                 hpt->Fill(Res->GetMCPt(i));
                 hpz->Fill(Res->GetMCPz(i));
                 
                 // resolution
                 hpr->Fill(Res->GetMomRes(i));
                 hptr->Fill(Res->GetPtRes(i));
                 hpzr->Fill(Res->GetPzRes(i));
                 hthr->Fill(Res->GetDTheta(i));
                 hfir->Fill(Res->GetDPhi(i));
           
                 // dp/p
                 hppr->Fill(Res->GetMomentumMC(i),Res->GetMomRes(i),1);
                 hpthr->Fill(Res->GetMCPtheta(i),Res->GetMomRes(i),1);
                 hpfir->Fill(Res->GetMCPphi(i),Res->GetMomRes(i),1);
                 hhppth->Fill(Res->GetMCPtheta(i),Res->GetMomentumMC(i),Res->GetMomRes(i));
                 
                 // dpt/pt
                 hptpr->Fill(Res->GetMomentumMC(i),Res->GetPtRes(i),1);
                 hptthr->Fill(Res->GetMCPtheta(i),Res->GetPtRes(i),1);
                 hptfir->Fill(Res->GetMCPphi(i),Res->GetPtRes(i),1);
                 hhptpth->Fill(Res->GetMCPtheta(i),Res->GetMomentumMC(i),Res->GetPtRes(i));
             
                 // dpz/pz
                 hpzpr->Fill(Res->GetMomentumMC(i),Res->GetPzRes(i),1);
                 hpzthr->Fill(Res->GetMCPtheta(i),Res->GetPzRes(i),1);
                 hpzfir->Fill(Res->GetMCPphi(i),Res->GetPzRes(i),1);
                 hhpzpth->Fill(Res->GetMCPtheta(i),Res->GetMomentumMC(i),Res->GetPzRes(i));
 
                 // dtheta
                 hthpr->Fill(Res->GetMomentumMC(i),Res->GetDTheta(i),1);
                 hththr->Fill(Res->GetMCPtheta(i),Res->GetDTheta(i),1);
                 hthfir->Fill(Res->GetMCPphi(i),Res->GetDTheta(i),1);
                 hhthpth->Fill(Res->GetMCPtheta(i),Res->GetMomentumMC(i),Res->GetDTheta(i));
                                  
                 // chisquare
                 hhchi2pth->Fill(Res->GetMCPtheta(i),Res->GetMomentumMC(i),Res->GetChi2toNDF(i));
                 hchi2pr->Fill(Res->GetMomRes(i),Res->GetChi2toNDF(i),1);
                 hchi2ptr->Fill(Res->GetPtRes(i),Res->GetChi2toNDF(i),1);
                 hchi2pzr->Fill(Res->GetPzRes(i),Res->GetChi2toNDF(i),1);
                 hchi2thr->Fill(Res->GetDTheta(i),Res->GetChi2toNDF(i),1);
                 
                 // additional
                 hpptr->Fill(Res->GetMCPt(i),Res->GetMomRes(i),1);
                 hppzr->Fill(TMath::Abs(Res->GetMCPz(i)),Res->GetMomRes(i),1);
                 hptptr->Fill(Res->GetMCPt(i),Res->GetPtRes(i),1);
                 hptpzr->Fill(TMath::Abs(Res->GetMCPz(i)),Res->GetPtRes(i),1);
                 hpzptr->Fill(Res->GetMCPt(i),Res->GetPzRes(i),1);
                 hpzpzr->Fill(TMath::Abs(Res->GetMCPz(i)),Res->GetPzRes(i),1);
                 
                 // hits
                 hitsh->Fill(Res->GetNVhits(i));
                 htstbh->Fill(Res->GetNTBhits(i));
                 htstech->Fill(Res->GetNTEChits(i));
                 htotalh->Fill(Res->GetNTThits(i));
                 
                 hpthh->Fill(Res->GetMCPtheta(i),Res->GetMomentumMC(i),Res->GetNTThits(i));
                 hitspthh->Fill(Res->GetMCPtheta(i),Res->GetMomentumMC(i),Res->GetNVhits(i));
                 htstbpthh->Fill(Res->GetMCPtheta(i),Res->GetMomentumMC(i),Res->GetNTBhits(i));
                 htstecpthh->Fill(Res->GetMCPtheta(i),Res->GetMomentumMC(i),Res->GetNTEChits(i));
                
                 nm++;
             }
        }
        
        hN->Fill(ntracks);
        hn->Fill(nm);
    
        // vertex
        hvr->Fill(Res->GetVertexMCr());
        hvz->Fill(Res->GetVertexMCz());
        hvrt->Fill(Res->GetVertexMCrt());
        
        hvdr->Fill(Res->GetVertexDMag());
        hvdz->Fill(Res->GetVertexDZ());
        hvdphi->Fill(Res->GetVertexDPhi());
        
        Res->ClearData();
   }
   
   gStyle->SetOptStat("ermou");
   
   // seed values
//    TCanvas* cp = new TCanvas("cp","cp",750,850);
//    cp->Divide(2,3);
//    cp->cd(1); hn->SetFillColor(kBlue-7); hn->Draw(); 
//               hN->SetLineColor(kBlack); hN->SetLineWidth(3);  hN->Draw("same"); 
//    cp->cd(2); hp->SetFillColor(kBlue+1);  hp->Draw();
//    cp->cd(3); hpt->SetFillColor(kBlue+2); hpt->Draw();
//    cp->cd(4); hpz->SetFillColor(kBlue+2); hpz->Draw();
//    cp->cd(5); hth->SetFillColor(kBlue+3); hth->Draw();
//    cp->cd(6); hfi->SetFillColor(kBlue+3); hfi->SetMinimum(0); hfi->Draw();
//    cp->cd(0);
   
   // resolution
   TCanvas* cr = new TCanvas("cr","cr",750,850);
   cr->Divide(2,3);
   cr->cd(1); hchi2ndf->SetFillColor(kMagenta+3); hchi2ndf->Draw();
   cr->cd(2); hpr->SetFillColor(kMagenta);     hpr->Draw(); cr->cd(2)->SetLogy();
   cr->cd(3); hptr->SetFillColor(kMagenta+1); hptr->Draw(); cr->cd(3)->SetLogy();
   cr->cd(4); hpzr->SetFillColor(kMagenta+1); hpzr->Draw(); cr->cd(4)->SetLogy();
   cr->cd(5); hthr->SetFillColor(kMagenta+2); hthr->Draw(); cr->cd(5)->SetLogy();
   cr->cd(6); hfir->SetFillColor(kMagenta+2); hfir->Draw(); cr->cd(6)->SetLogy();
   cr->cd(0);

   // dp/p
//    TCanvas* cpr = new TCanvas("cpr","cpr",900,600);
//    cpr->Divide(2,2);
//    cpr->cd(1); hppr->Draw("e1");
//    cpr->cd(2); hpthr->Draw(); cpr->cd(2)->SetLogy();
//    cpr->cd(3); hpfir->Draw(); hpfir->SetMinimum(0);
//    cpr->cd(4); hhppth->Draw("colz");
//    cpr->cd(0);
//    
   // dpt/pt
//    TCanvas* cptr = new TCanvas("cptr","cptr",900,600);
//    cptr->Divide(2,2);
//    cptr->cd(1); hptpr->Draw("e1");
//    cptr->cd(2); hptthr->Draw(); cptr->cd(2)->SetLogy();
//    cptr->cd(3); hptfir->Draw(); hptfir->SetMinimum(0);
//    cptr->cd(4); hhptpth->Draw("colz");
//    cptr->cd(0);
   
   // dpz/pz
//    TCanvas* cpzr = new TCanvas("cpzr","cpzr",900,600);
//    cpzr->Divide(2,2);
//    cpzr->cd(1); hpzpr->Draw("e1");
//    cpzr->cd(2); hpzthr->Draw(); cpzr->cd(2)->SetLogy();
//    cpzr->cd(3); hpzfir->Draw(); hpzfir->SetMinimum(0);
//    cpzr->cd(4); hhpzpth->Draw("colz");
//    cpzr->cd(0);
   
   // dtheta  
//    TCanvas* cthr = new TCanvas("cthr","cthr",900,600);
//    cthr->Divide(2,2);
//    cthr->cd(1); hthpr->Draw("e1");
//    cthr->cd(2); hththr->Draw(); cthr->cd(2)->SetLogy();
//    cthr->cd(3); hthfir->Draw(); hthfir->SetMinimum(0); 
//    cthr->cd(4); hhthpth->Draw("colz");
//    cpzr->cd(0);
   
     
   // dp/p, dpt/pt
   TCanvas* ccpr = new TCanvas("ccpr","ccpr",1200,400);
   ccpr->Divide(2,1);
   ccpr->cd(1); hpthr->Draw();  ccpr->cd(1)->SetLogy();
   ccpr->cd(2); hptthr->Draw(); ccpr->cd(2)->SetLogy();
   ccpr->cd(0);
    
   // vertex
   TCanvas* cv = new TCanvas("cv","cv",750,850);
   cv->Divide(2,3);
   cv->cd(1); hvr->SetFillColor(kGreen+1);   hvr->SetMinimum(0); hvr->Draw();
   cv->cd(2); hvdr->SetFillColor(kGreen+1);  hvdr->Draw();
   cv->cd(3); hvz->SetFillColor(kGreen+2);   hvz->SetMinimum(0); hvz->Draw();
   cv->cd(4); hvdz->SetFillColor(kGreen+2);  hvdz->Draw();
   cv->cd(5); hvrt->SetFillColor(kGreen+3);  hvrt->Draw();
   cv->cd(6); hvdphi->SetFillColor(kGreen);  hvdphi->Draw(); 
   cv->cd(0);
   
   // chisquare
//    TCanvas* cchi2pth = new TCanvas("cchi2pth","cchi2pth",600,600);
//    Double_t dd = 0.5; hhchi2pth->SetMinimum(0.98-dd); hhchi2pth->SetMaximum(0.98+dd);  
//    hhchi2pth->Draw("colz");
//    cchi2pth->cd(0);
   
//   TCanvas* cchi2r = new TCanvas("cchi2r","cchi2r",900,600);
//    cchi2r->Divide(2,2);
//    cchi2r->cd(1); hchi2pr->Draw("e1");
//    cchi2r->cd(2); hchi2ptr->Draw("e1");
//    cchi2r->cd(3); hchi2pzr->Draw("e1");
//    cchi2r->cd(4); hchi2thr->Draw("e1");
//    cchi2r->cd(0); 
 
//     // additional   
//    TCanvas* cr2 = new TCanvas("cr2","cr2",750,850);
//    cr2->Divide(2,3);
//    cr2->cd(1); hpptr->Draw("e1");
//    cr2->cd(2); hppzr->Draw("e1");
//    cr2->cd(3); hptptr->Draw("e1");
//    cr2->cd(4); hptpzr->Draw("e1");
//    cr2->cd(5); hpzptr->Draw("e1");
//    cr2->cd(6); hpzpzr->Draw("e1");
//    cr2->cd(0);
   
   // hits
//    TCanvas* chh = new TCanvas("chh","chh",900,600);
//    chh->Divide(2,2);
//    Double_t hhscale = 1./hitsh->GetEntries();
//    chh->cd(1)->SetLogy(); hitsh->SetFillColor(kRed+2); hitsh->GetXaxis()->CenterLabels(); hitsh->Scale(hhscale); hitsh->Draw("h");
//    chh->cd(2)->SetLogy(); htstbh->SetFillColor(kRed+2); htstbh->GetXaxis()->CenterLabels(); htstbh->Scale(hhscale); htstbh->Draw("h");
//    chh->cd(3)->SetLogy(); htstech->SetFillColor(kRed+2); htstech->GetXaxis()->CenterLabels(); htstech->Scale(hhscale); htstech->Draw("h");
//    chh->cd(4)->SetLogy(); htotalh->SetFillColor(kRed+4); htotalh->GetXaxis()->CenterLabels(); htotalh->Scale(hhscale); htotalh->Draw("h"); 
//    chh->cd(0);
   
//    TCanvas* chhpth = new TCanvas("chhpth","chhpth",900,900);
//    chhpth->Divide(2,2);
//    chhpth->cd(1); hpthh->Draw("colz");
//    chhpth->cd(2); hitspthh->Draw("colz");
//    chhpth->cd(3); htstbpthh->Draw("colz");
//    chhpth->cd(4); htstecpthh->Draw("colz");
//    chhpth->cd(0);
   
   // Vertex stat.
   cout << "\nMOMENTUM: " << hp->GetMean() << " [GeV/c] "<< "\n";
   if (hp) {
       
       Double_t sump(0), q(0.5);
       
       if (hpr->GetQuantiles(1,&sump,&q)) {
           //cout << "\n";
           cout << "Mean (|dp|/p)   = " << 100.*hpr->GetMean() << " [%] "; 
           cout << "Std. (|dp|/p) = " << 100.*hpr->GetRMS() << " [%] "; 
           cout << "Median (|dp|/p) = " << 100.*sump << " [%] \n"; 
       }
       
       if (hptr->GetQuantiles(1,&sump,&q)) {
           //cout << "\n";
           cout << "Mean (|dpt|/pt) = " << 100.*hptr->GetMean() << " [%] "; 
           cout << "Std. (|dpt|/pt) = " << 100.*hptr->GetRMS() << " [%] "; 
           cout << "Median (|dpt|/pt) = " << 100.*sump << " [%] \n"; 
       }
       cout << endl;
       if (hthr) {
           cout << "Mean dθ(p) = " << hthr->GetMean() << " [deg] "; 
           cout << "Std. dθ(p) = " << hthr->GetRMS() << " [deg] \n"; 
       }
         if (hfir) {
           cout << "Mean dφ(p) = " << hfir->GetMean() << " [deg] "; 
           cout << "Std. dφ(p) = " << hfir->GetRMS() << " [deg] \n"; 
       }
   }
   
   // Vertex stat.
   cout << "\nVERTEX: " << "\n";
   if (hvdz) {
       cout << "Mean (dz) = " << hvdz->GetMean()*10 << " [mm] "; 
       cout << "Std. (dz) = " << hvdz->GetRMS()*10 << " [mm] \n"; 
   }
 
   cout << "\nCHI2/NDF:  mean = " << hchi2ndf->GetMean() << " std. = " << hchi2ndf->GetRMS() << "\n";
    
   cout << "\nEVENTS [nice/total]:         " << NWellFittedEvents << "/" << entries 
        << "  (" << 100.*NWellFittedEvents/Double_t(entries) << " %)" << "\n";
   cout <<   "TRACKS [selected/total]:     " << NWellFittedTracks << "/" << NTotalTracks
        << "  (" << 100*NWellFittedTracks/Double_t(NTotalTracks) << " %)" << "\n\n";     
        
}


