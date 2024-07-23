
SpdPythia8* Pythia_;
SpdPythia8Decayer* Decayer_;
SpdPythia8Generator* Generator_;

void TestPythia();
void TestDecayer();
void TestGenerator();

//______________________________________________________________
void TestPythia8(int nevents = 1) 
{
   SpdParticlesAndProcesses:: AddParticlesToDatabasePDG();
    
   //TestPythia();
   
   //TestDecayer();  
   
   TestGenerator();
}

//______________________________________________________________
void TestPythia() 
{
    Pythia_ = new SpdPythia8();
    
    //Pythia_->ListAll();
    //Pythia_->ListChanged();
    
    Pythia_->SetParameter("Beams:idA = 2212");  // first beam
    Pythia_->SetParameter("Beams:idB = 2212");  // fsecond beam
    Pythia_->SetParameter("Beams:eCM = 27.");   // CM energy of collision
    Pythia_->SetParameter("Beams:frameType = 1");
    
    Pythia_->SetParameter("SoftQCD:singleDiffractive = on");
    Pythia_->SetParameter("SoftQCD:doubleDiffractive = on");
    Pythia_->SetParameter("SoftQCD:centralDiffractive = on");
    Pythia_->SetParameter("SoftQCD:nonDiffractive = on");
    Pythia_->SetParameter("SoftQCD:inelastic = on");
    
    Pythia_->Init();
    
    Pythia_->ListChanged();

    for (Int_t i(0); i<10; i++) {
         Pythia_->GenerateEvent();
         Pythia_->Print();
    }
}

//______________________________________________________________
void TestDecayer() 
{
    Decayer_ = new SpdPythia8Decayer();
    Decayer_->Init();
    
    Decayer_->PrintParticleDecayChannels(443);
    
    Decayer_->SelectForcedDecay(443,1); // pdg number, channel (J/psi -> e+ e-)
    Decayer_->SelectForcedDecay(443,2); // pdg number, channel (J/psi -> mu+ mu-) 
    //Decayer_->SelectForcedDecay(443,3); // pdg number, channel (J/psi -> eta_c + gamma)

    //Decayer_->SelectForcedDecay(413,0); 
    
    Decayer_->ForceSelectedDecays();

    Int_t pdg = 443;
    //Int_t pdg = 413;
    
    Double_t p = 10;
    
    TParticlePDG* pdgpart = TDatabasePDG::Instance()->GetParticle(pdg);
    if (!pdgpart) return;
    
    TLorentzVector mom;
    mom.SetXYZM(0,0,p,pdgpart->Mass()); 
    
    TClonesArray* parts = new TClonesArray("TParticle",100);
    
    for (Int_t i(0); i<10; i++) {
         Decayer_->Decay(pdg,&mom);
         Decayer_->ImportParticles(parts);
         Decayer_->GetPythia()->PrintParticles(parts);
    }
    
//     Decayer_->PrintParticleDecayChannels(421);
//     Decayer_->PrintParticleDecayChannels(413);
//     Decayer_->PrintParticleDecayChannels(-413);
//     Decayer_->PrintParticleDecayChannels(411);
//     Decayer_->PrintParticleDecayChannels(-313);
//     Decayer_->PrintParticleDecayChannels(-10311);
    
//     Decayer_->PrintParticleDecayChannels(311);
//     Decayer_->PrintParticleDecayChannels(313);
//     Decayer_->PrintParticleDecayChannels(323);
//     
//     Decayer_->PrintParticleDecayChannels(-311);
//     Decayer_->PrintParticleDecayChannels(-313);
//     Decayer_->PrintParticleDecayChannels(-323);
//     
//     Decayer_->PrintParticleDecayChannels(211);

}

//______________________________________________________________
void TestGenerator() 
{
    Generator_ = new SpdPythia8Generator();
    
    Generator_->SetBeam(2212,2212,27.); // pdg(A), pdg(B), E_cms
    //Generator_->SetSeed(75656);
    
    // Set minimum bias 
    Generator_->SetParameters("SoftQCD:singleDiffractive = on");
    Generator_->SetParameters("SoftQCD:doubleDiffractive = on");
    Generator_->SetParameters("SoftQCD:centralDiffractive = on");
    Generator_->SetParameters("SoftQCD:nonDiffractive = on");
    Generator_->SetParameters("SoftQCD:inelastic = on");
    
    Generator_->Init();
    
    Generator_->SetVerboseLevel(1);
    
    Generator_->Print("");
    
    // Generate events
    const Int_t NE = 3;
    
    for (Int_t i(0); i<NE; i++) 
    {
         cout << "\nEvent: " << i << endl;
         Generator_->ReadEvent();
    }
}

