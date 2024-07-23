#include "TParticle.h"

void check_urqmd();

TH3D* DP_(0);
TH1D *Theta_(0), *Phi_(0);

void FillH(TParticle* part);
void DrawH(Double_t mark_size = 0.3);

const Double_t R_ = 1.;

//______________________________________________________________
void TestUrqmd() 
{
   gStyle->SetOptStat("e");

   check_urqmd();
}

//______________________________________________________________
void check_urqmd()
{ 
    SpdUrqmdGenerator* gen = new SpdUrqmdGenerator("../input/AuAu_ecm9gev_hydroON_EoS1PT_mbias_1000ev_10.f13.gz");
    
    //--------------------------------------------------------------------------------
    gen->SetVerboseLevel(2); // 0 ("minimal" printing) or 1
    gen->SetKeepEvent(kTRUE); 
    //gen->Initialize(211,1,10000);   
//    gen->Initialize(211,1,5); 
    
    /* ----------- GENERATE EVENT ----------- */
    
    Int_t skip_events = 0;
    while (true) {
      gen->ReadEvent(0);
      if (++skip_events == 1) break;
    }
    
    const TClonesArray* event = gen->GetEvent();
    
    if (event) {
        Int_t size = event->GetEntriesFast();
        for (Int_t i(0); i<size; i++) {
             FillH((TParticle*)event->At(i));
        }
    }
    
    DrawH(1.0 /*size of marker */);
    
    delete gen;
}

//______________________________________________________________
void FillH(TParticle* part)
{
    if (!part) return;
    
    //cout << part->GetPdgCode() << " "  <<  part->GetFirstMother()<< endl;
    
    if (!DP_) {
        DP_ = new TH3D("Direction","", 
        100 , -1.5*R_, 1.5*R_, 150, -1.5*R_, 1.5*R_ , 150, -1.5*R_, 1.5*R_ );
    }
    
    double dd = 0.;
    
    if (!Theta_) {
        Theta_ = new TH1D("Theta","Theta",18,0+dd,180+dd);
        Theta_->SetFillColor(kRed);
        Theta_->SetMinimum(0);
    }
   
    if (!Phi_) {
        Phi_ = new TH1D("Phi","Phi",36,-180+dd,180+dd);
        Phi_->SetFillColor(kRed);
        Phi_->SetMinimum(0);
    }
    
    TVector3 v(part->Px(),part->Py(),part->Pz());
    v.SetMag(R_);
    
    DP_->Fill(v.X(),v.Y(),v.Z());
    Theta_->Fill(v.Theta()*TMath::RadToDeg());
    Phi_->Fill(v.Phi()*TMath::RadToDeg());
}

//______________________________________________________________
void DrawH(Double_t mark_size)
{
    if (DP_) {
        TCanvas* cDP = new TCanvas("Direction","Direction",800,800);
        DP_->SetMarkerStyle(8);
        DP_->SetMarkerSize(mark_size);
        DP_->Draw();
        
        TPolyMarker3D* center = new TPolyMarker3D(1);
        center->SetPoint(0,0,0,0);
        center->SetMarkerStyle(8);
        center->SetMarkerColor(kRed);
        center->SetMarkerSize(2);
        center->Draw("same");
    }
    
    if (Theta_) {
        TCanvas* cTh = new TCanvas("Theta","Theta",1000,700);
        Theta_->Draw();
    }
    
    if (Phi_) {
        TCanvas* cPh = new TCanvas("Phi","Phi",1000,700);
        Phi_->Draw();
    }
}







