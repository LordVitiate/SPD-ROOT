#include <Riostream.h>
#include <TCanvas.h>
#include <TH2D.h>
#include <TMarker.h>
#include <TArc.h>
#include <TH2D.h>
#include <TArrayI.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>

SpdSttGeoMapper* mapper = 0;
TCanvas *cv = 0;
TFile* F_ = 0;
TTree* T_ = 0;

Int_t  NEVENT = 2; 

Double_t BarrelRmin(0), BarrelRmax(0);

void DrawGeo(int npad);

void CheckSttHits() 
{
   F_ = new TFile("hits.root");
   if (!F_) return;
   if (!T_) T_ = (TTree*)F_->Get("cbmsim") ;  
   
   cout << "DATA FILE: Ok! " << endl;
   
   TFile* P_ = new TFile("params.root");
   if (!P_) return;
   cout << "FILE WITH GEOMETRY PARAMETERS: Ok! " << endl;
   
   SpdSttGeoPar* pars = 0; 
   pars = (SpdSttGeoPar*)P_->Get("SpdSttGeoPar");  
   if (!pars) return;
     
   //pars->printParams();
   
   if (!pars->GetParameter("barrel_rmin",BarrelRmin)) return;
   if (!pars->GetParameter("barrel_rmax",BarrelRmax)) return;
 
   mapper = new SpdSttGeoMapper();
  
   pars->FillParameters(mapper->GetParameters()); 
   mapper->InitGeometry(pars->GetGeometryType());
   
   //SpdParameter* par = mapper->AddParameter("param");  // CHECK ONLY!!!
   //if (par) *par = 99;
   
   mapper->print(0); // PRINT MAPPER !!!
  
   cout << "INITIALIZE: Ok! " << endl;
    
   cv = new TCanvas("cv", "cv", 900*1.5, 600*1.5);
   cv->Draw();
   cv->Divide(3,2);
   
   for (int i(0); i<6; i++) DrawGeo(i+1);
    
   cv->cd(0);
}

//____________________________________________________
void DrawGeo(int npad) 
{
    if (!mapper) return; 
 
    Int_t NT = mapper->GetTotalTubes();   
    if (NT == 0) return;    
    SpdSttStrawTube* tube = mapper->GetTube(); 
    if (!tube) return;
    
    cv->cd(npad);
 
    TH2D *h2 = new TH2D(Form("geom %d",npad), "", 220, -110, 110, 220, -110, 110);
    h2->SetStats(kFALSE);
    h2->Draw();
    
    TArc *arc2 = new TArc(0, 0, BarrelRmax);
    arc2->Draw("SAME");   
    
    TArc *arc1 = new TArc(0, 0, BarrelRmin);
    arc1->Draw("SAME");   
  
    TMarker *mrk = 0;
    TArc *arc = 0;
    
    Int_t cols[2] = {kYellow,kOrange+1};
    
    Double_t x, y;
    Double_t R = tube->GetTubeOutRad();
    
    for (Int_t i(1); i<=NT; i++) {
         tube = mapper->GetTube(i);
         x = tube->GetXSource();
         y = tube->GetYSource();
          
         arc = new TArc(x, y, R);
         arc->SetFillColor(cols[i%2]);
         arc->Draw("SAME");
         
         mrk = new TMarker(x,y,6);
         mrk->SetMarkerColor(3);
         mrk->Draw("SAME");
    }
    
    // ---------- Draw hits ---------- 
    
    npad--;
    
    int entries = T_->GetEntriesFast();
  
    cout << "__________________________________________________";
    cout << "__________________________________________________" << endl;
    cout << "Number of EVENT   = " << NEVENT << "/" << entries << endl;  
    cout << "Number of SECTION = " << npad << endl;  
    
    if (NEVENT < 0 || NEVENT >= entries) return;
    
    TClonesArray *fH, *fHI, *fHH; 
    
    T_->SetBranchAddress("SpdSttHit",&fH);
    T_->SetBranchAddress("SpdSttHitInfo",&fHI); 
    T_->SetBranchAddress("SpdSttHelixHit",&fHH);   
    
    T_->GetEntry(NEVENT);
    
   
    Int_t nhits = fH->GetEntries();
    Int_t nhits_info = fHI->GetEntries();
    Int_t nhelix_hits = fHH->GetEntries();
    
   
    cout << "Number of: (hits,hinfo,h-hits) = " 
         << nhits << "/"<< nhits_info << "/" <<  nhelix_hits 
         << "\n" << endl;
    
    SpdSttHit* hit;
    SpdSttHitInfo* hit_info;
    SpdSttHelixHit* helix_hit;
    
    int ns(0), nns(0), nhe;
    int nn = nhelix_hits;
     
    for (Int_t i(0); i<nn; i++) {
     
         hhit = (SpdSttHelixHit*)fHH->At(i);
        
         if (hhit && hhit->SectionID() == npad) { 
             
             cout << "[" << i << "] "; hhit->Print();
             
             nhe = hhit->NHits();
             nns += nhe;
             for (Int_t j(0); j<nhe; j++) {
               
                  hit = (SpdSttHit*)fH->At(hhit->HitID(j));
                  hit_info = (SpdSttHitInfo*)fHI->At(i); 
                   
                  if (hit && hit_info) {
                      cout << "   [" << j << "] "; hit->Print();
                      cout << "   [" << j << "] "; hit_info->Print();
                      
                      cout << endl;
                  }
             }
                  
             ns++;
             
             tube = mapper->GetTube(hhit->DetectorID());
             x = tube->GetXSource();
             y = tube->GetYSource();
                    
             arc = new TArc(x, y, R);
             
             //if (nhe > 1) arc->SetFillColor(kMagenta);
             //else arc->SetFillColor(kBlue);
             arc->SetFillColor(kBlue);
             arc->Draw("SAME");
         }
         
    }
    
    cout << "Hits in the section: (h-hits,hits) = " << ns << ", " << nns << "\n " << endl;
     
}





