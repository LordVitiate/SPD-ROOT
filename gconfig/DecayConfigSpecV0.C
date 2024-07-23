/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

void Config(SpdPythia6Decayer* decayer);

//________________________________________________________________________________________
void DecayConfig() 
{
    cout << "-I- <DecayConfigSpecV0:DecayConfig> " << endl;

    // This script uses the external decayer in place of the
    // concrete Monte Carlo native decay mechanisms only for the
    // specific types of decays defined below.

    SpdDecayer* spd_decayer = SpdDecayer::Instance();
    
    if (!spd_decayer->GetDecayer()) spd_decayer->SetDecayer("SpdPythia6Decayer",1);
   
    SpdPythia6Decayer* decayer = dynamic_cast<SpdPythia6Decayer*>(spd_decayer->GetDecayer());
    
    Config(decayer); // FIXME
        
    if (!spd_decayer->IsInit()) spd_decayer->Init(); 
}

//________________________________________________________________________________________
void Config(SpdPythia6Decayer* decayer) 
{    
     if (!decayer) {
        cout << "-F- <DecayConfigSpecV0:Config> Decayer not defined" << endl;
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
    
    // -> Fix vertices:
   
    // D0 (D0_bar) -> K-, pi+
    decayer->SelectForcedDecay(421,17); // pdg number, channel
    //decayer->SelectForcedDecay(-421,17); 
    
    // D*+ -> D0, pi+
    decayer->SelectForcedDecay(413,1); // pdg number, channel

/*    
    // D+ (D-) -> K*0_bar, pi+ 
    decayer->SelectForcedDecay(411,24);  // pdg number, channel
    //decayer->SelectForcedDecay(-411,24);
    
    // D+ (D-) -> K-, pi+, pi+ 
    decayer->SelectForcedDecay(411,47);  // pdg number, channel
    //decayer->SelectForcedDecay(-421,47);
    
    // -> Fix secondary vertices (if necessary!):
    
    // K*0_bar (K*0) -> K+, pi- 
    decayer->SelectForcedDecay(-313,1);  // pdg number, channel
    
      // just tells concrete monte carlo generator (for example, Geant4)
      // to use external decayer for these type of particles
      gMC->SetUserDecay(-313); // K*0_bar
      gMC->SetUserDecay(313);  // K*0
*/

    TParticlePDG* part;
    
    part = TDatabasePDG::Instance()->GetParticle(413);
    if (part) { 
        gMC->DefineParticle(part->PdgCode(),part->GetName(),kPTHadron,
                            part->Mass(),part->Charge()/3.,part->Lifetime(),
                            "DStar+",part->Width(), 
                            part->Spin(),part->Parity(),0, 
                            part->Isospin(), 0, 0, 
                            0, 0, false);
    }
    
    part = TDatabasePDG::Instance()->GetParticle(-413);
    if (part) { 
        gMC->DefineParticle(part->PdgCode(),part->GetName(),kPTHadron,
                            part->Mass(),part->Charge()/3.,part->Lifetime(),
                            "DStar-",part->Width(), 
                            part->Spin(),part->Parity(),0, 
                            part->Isospin(), 0, 0, 
                            0, 0, false);
    }
    // ====== Force selected decay channels ====== 
    
    decayer->ForceSelectedDecays();

}

