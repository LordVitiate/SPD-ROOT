
void check_isotropic();
void check_pythia6();

TH3D* DP_(0);
TH1D *Theta_(0), *Phi_(0);

void FillH(TParticle* part);
void DrawH(Double_t mark_size = 0.3);

const Double_t R_ = 1.;

//______________________________________________________________
void TestIsotropic() 
{
   gStyle->SetOptStat("e");

   check_isotropic();
   //check_pythia6();
}

//______________________________________________________________
void check_isotropic()
{ 
    SpdIsotropicGenerator* gen = new SpdIsotropicGenerator();
    
    //--------------------------------------------------------------------------------
    // Initialization parameters:
    //
    // [1] pdg number, 
    // [2] momentum (GeV/c),
    // [3] particles per event (with option = 1, 2) or DENSITY LEVEL[1,7] (with option = -1, -2)
    //
    // Options (via SetVGenOpt), default option = 2 :
    //     1: isotropic (oridinary):  theta[min,max], phi[min,max] via SetSpherical
    //     2: isotripic (solid angle):  theta[min,max], solid angle [phi,theta] via SetSolid
    //    -1: pseudoisotropic (oridinary):  theta[min,max], phi[min,max] via SetSpherical
    //    -2: pseudoisotropic (solid angle):  theta[min,max], solid angle [phi,theta] via SetSolid
    //
    // PSEUDOISOTROPIC DISTRIBUTION (option = -1, -2):
    //
    //     Particles: 12, 42, 162, 642, 2562, 10241, 40962 (v[i] = 4*v[i-1] - 6)
    //     Angular resolution: a = 63.435, a/2, a/4, a/8, a/16, a/32, a/64 (~1 deg)
    //--------------------------------------------------------------------------------
    
    gen->SetVerboseLevel(1); // 0 ("minimal" printing) or 1
   
    //----------------------------------------------------------------
    // OPTION = 1, 2 should be set!
    // pi-, p = 2.5 GeV/c, 100 particles 
    //----------------------------------------------------------------
    gen->Initialize(-211,2.5,100);    
    gen->SetVGenOpt(1);      // isotropic in the ranges
    //gen->SetVGenOpt(2);    // isotropic in the solid angle
    
    //----------------------------------------------------------------
    // OPTION = -1, -2 should be set!
    // pi-, p = 2.5 GeV/c, level = 5 (2562 particles per event in 4pi !)
    // REAL NUMBER OF PARTICLES GENERATED DEPENDS ON RANGES!
    //----------------------------------------------------------------
    //gen->Initialize(-211,2.5,5);    
    //gen->SetVGenOpt(-1);   // pseudoisotripic in the ranges 
    //gen->SetVGenOpt(-2);   // pseudoisotripic in the solid angle
  
    // >>>>> ordinary: will be used if OPTION = 1 or -1
    // theta[min,max], phi[min,max]
    //gen->SetSpherical(20, 40, 50, 70);  
    
    // >>>>> solid angle: will be used if OPTION = 2 or -2
    // theta[min,max], solid angle (direction) = [phi,theta]
    //gen->SetSolid(10, 20, 45, 45);      
    
    
    //-------------------------------------------------------
    gen->SetKeepEvent(1);  // keep particles in the generator 
    
    /* ----------- GENERATE EVENT ----------- */
    
    gen->ReadEvent();
    
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
void check_pythia6()
{
   SpdPythia6Generator* gen = new SpdPythia6Generator();
   
   gen->SetKeepEvent(kTRUE);
   
   gen->SetVGenOpt(1);  // 0 or 1
    
   //gen->SetSeed(0);
   //gen->SetSeed(12995,0);
   gen->SetSeed(19949278, 1);
   //gen->SetSeed(13495);
   //gen->SetSeed(127472);
 
   gen->Initialize("cms","p","p",20/*GeV*/);
  
   /* GENERATE EVENT */
    
   gen->ReadEvent();
    
   const TClonesArray* event = gen->GetEvent();
   if (event) {
       Int_t size = event->GetEntriesFast();
       for (Int_t i(0); i<size; i++) {
            FillH((TParticle*)event->At(i));
       }
   }
   
   DrawH(0.5 /* marker size */);
    
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







