
void Config(SpdPythia8Decayer* decayer);

//________________________________________________________________________________________
void DecayConfig() 
{
    cout << "-I- <DecayConfigPythia8:DecayConfig> " << endl;

    // This script uses the external decayer in place of the
    // concrete Monte Carlo native decay mechanisms only for the
    // specific types of decays defined below.

    SpdDecayer* spd_decayer = SpdDecayer::Instance();
    
    if (!spd_decayer->GetDecayer()) spd_decayer->SetDecayer("SpdPythia8Decayer",1);
   
    SpdPythia8Decayer* decayer = dynamic_cast<SpdPythia8Decayer*>(spd_decayer->GetDecayer());
    
    Config(decayer); // FIXME
        
    if (!spd_decayer->IsInit()) spd_decayer->Init(); 
}

//________________________________________________________________________________________
void Config(SpdPythia8Decayer* decayer) 
{     
    if (!decayer) {
        cout << "-F- <DecayConfigPythia8:Config> Decayer not defined" << endl;
        exit(1);
    }
    
    decayer->Init();

    gMC->SetExternalDecayer(decayer);
    
    // ===== Print all decay channels ====== 
    
    //decayer->PrintParticleDecayChannels(413);   // D*+
    //decayer->PrintParticleDecayChannels(411);   // D+
    //decayer->PrintParticleDecayChannels(421);   // D0
    //decayer->PrintParticleDecayChannels(423);   // D*0
    
    //decayer->PrintParticleDecayChannels(-413);  // D*-     (the same as D*+)
    //decayer->PrintParticleDecayChannels(-411);  // D-      (the same as D+)
    //decayer->PrintParticleDecayChannels(-421);  // D0_bar  (the same as D0)
    //decayer->PrintParticleDecayChannels(-423);  // D*0_bar (the same as D*0)
    
    //decayer->PrintParticleDecayChannels(-313);  // K*0_bar
    //decayer->PrintParticleDecayChannels(313);   // K*0 (the same as K*_0_bar)

    // ===== Select decay channels ====== 
    
    // D0 (D0_bar) -> K-, pi+
    decayer->SelectForcedDecay(421,37); // pdg number, channel
    //decayer->SelectForcedDecay(-421,37); 
    
    // D+ (D-) -> K*_00_bar, pi+
    decayer->SelectForcedDecay(411,34);  // pdg number, channel
    //decayer->SelectForcedDecay(-411,34);
    
    // D+ (D-) -> K-, pi+, pi+ 
    decayer->SelectForcedDecay(411,55);  // pdg number, channel
    //decayer->SelectForcedDecay(-411,55);
    
    // D*+ -> D0, pi+
    decayer->SelectForcedDecay(413,0); // pdg number, channel
    
      // just tells concrete monte carlo generator (for example, Geant4)
      // to use external decayer for these type of particles
    
        //gMC->SetUserDecay(211);   // pi+
        //gMC->SetUserDecay(-211);  // pi-
    
    
    // ====== Force selected decay channels ====== 
   
    decayer->ForceSelectedDecays(); 
     
    //  ===== Add particles to database =====
     
    TParticlePDG* part;
    
    part = TDatabasePDG::Instance()->GetParticle(413); // D*+ 
    if (part) { 
        gMC->DefineParticle(part->PdgCode(),part->GetName(),kPTHadron,
                            part->Mass(),part->Charge()/3.,part->Lifetime(),
                            "DStar+",part->Width(), 
                            part->Spin(),part->Parity(),0, 
                            part->Isospin(), 0, 0, 
                            0, 0, false);
    }
    
    part = TDatabasePDG::Instance()->GetParticle(-413); // D*-
    if (part) { 
        gMC->DefineParticle(part->PdgCode(),part->GetName(),kPTHadron,
                            part->Mass(),part->Charge()/3.,part->Lifetime(),
                            "DStar-",part->Width(), 
                            part->Spin(),part->Parity(),0, 
                            part->Isospin(), 0, 0, 
                            0, 0, false);
    }
}

