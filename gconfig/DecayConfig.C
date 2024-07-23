/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

void Config1(SpdPythia6Decayer* decayer);
void Config2(TPythia6Decayer* decayer);
void Config3(TPythia6Decayer* decayer);

//________________________________________________________________________________________
void DecayConfig() 
{
    cout << "-I- <DecayConfig> " << endl;

    // This script uses the external decayer in place of the
    // concrete Monte Carlo native decay mechanisms only for the
    // specific types of decays defined below.

    // Access the external decayer singleton and initialize it  
    
    //TPythia6Decayer* decayer = TPythia6Decayer::Instance();
    //Config3(decayer);
    
    //SpdPythia6Decayer* decayer = new SpdPythia6Decayer();
    //Config3(decayer);
    
    SpdDecayer* spd_decayer = SpdDecayer::Instance();
    
    if (!spd_decayer->GetDecayer()) spd_decayer->SetDecayer("SpdPythia6Decayer",1);
   
    SpdPythia6Decayer* decayer = dynamic_cast<SpdPythia6Decayer*>(spd_decayer->GetDecayer());

//    if (decayer) {
//      
//        //decayer->SetSeed(123456789);
//      
//        TPythia6& pythia6 = *(decayer->GetPythia());
//        
//      // .... make Pythia6 settings here if necessary ...
//        
//  activate decays       
//
//         pythia6.SetMDCY(pythia6.Pycomp(211),1,1);   // pi+
//         pythia6.SetMDCY(pythia6.Pycomp(-211),1,1);  // pi-
//        
//  force user decayer for particles of a such type   
//
//         gMC->SetUserDecay(211);  // pi+
//         gMC->SetUserDecay(-211); // pi-
//        
//    }
    
    Config1(decayer); // FIXME
    
    //exit(1);
        
    if (!spd_decayer->IsInit()) spd_decayer->Init(); 
}

//________________________________________________________________________________________
void Config1(SpdPythia6Decayer* decayer) 
{    
    if (!decayer) {
        cout << "-F- <DecayConfig:Config1> Decayer not defined" << endl;
        exit(1);
    }
        
    decayer->Init();

    gMC->SetExternalDecayer(decayer);
    
    // ===== Print all decay channels ====== 
    
    //decayer->PrintParticleDecayChannels(421);   // D0
    //decayer->PrintParticleDecayChannels(411);   // D+
    //decayer->PrintParticleDecayChannels(-313);  // K*0_bar
    
    //decayer->PrintParticleDecayChannels(-421); // D0_bar (the same as D0)
    //decayer->PrintParticleDecayChannels(-411); // D- (the same as D+)
    //decayer->PrintParticleDecayChannels(313);  // K*0 (the same as K*_0_bar)

    // ===== Select decay channels ====== 
    
    // -> Fix vertices:
   
    // D0 (D0_bar) -> K-, pi+
    decayer->SelectForcedDecay(421,17); // pdg number, channel
    //decayer->SelectForcedDecay(-421,17); 
    
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
    
    // ====== Force selected decay channels ====== 
    
    decayer->ForceSelectedDecays();
}

//________________________________________________________________________________________
void Config2(TPythia6Decayer* decayer) 
{   
    /* EXAMPLE:  Force the J/PSI decay channel e+e- */
      
    if (!decayer) {
        cout << "-F- <DecayConfig:Config1> Decayer not defined" << endl;
        exit(1);
    }
    
//    Int_t products[2], mult[2];    
//    Int_t npart = 2;
// 
//    /* decay products */ 
//    products[0] =  11;
//    products[1] = -11;
//    
//    /* minimal multiplicity in final state */
//    mult[0] = 1;
//    mult[1] = 1;
//          
//    /* force J/PSI decay channel */
//    decayer->ForceParticleDecay(443,products,mult,npart); 

   decayer->Init();

   gMC->SetExternalDecayer(decayer);
}

//________________________________________________________________________________________
void Config3(TPythia6Decayer* decayer) 
{    
   // The following just tells pythia6 to not decay particles only to certain channels.
    
   if (!decayer) {
       cout << "-F- <DecayConfig:Config1> Decayer not defined" << endl;
       exit(1);
   } 
    
   decayer->SetForceDecay(TPythia6Decayer::kAll);

   //--------------------------------------------------------------------
   // Tell the concrete monte carlo to use the external decayer.  
   // The external decayer will be used for:
   //
   // 1) particle decays not defined in concrete monte carlo;
   //
   // 2) particles for which the concrete monte carlo is told
   //    to use the external decayer for its type via:
   //
   //    gMC->SetUserDecay(pdgId);
   //
   //    If this is invoked, the external decayer will be used for particles 
   //    of type pdgId even if the concrete monte carlo has a decay mode 
   //    already defined for that particle type.
   //--------------------------------------------------------------------
    
   TPythia6& pythia6 = *(TPythia6::Instance());
 
   pythia6.SetMDCY(pythia6.Pycomp(313),1,1);   // K*0
   pythia6.SetMDCY(pythia6.Pycomp(-313),1,1);  // K*0bar
   pythia6.SetMDCY(pythia6.Pycomp(323),1,1);   // K*+
   pythia6.SetMDCY(pythia6.Pycomp(-323),1,1);  // K*+bar
      
   // - activate decay in pythia
   // - force external decayer for particle of a such type
   
   pythia6.SetMDCY(pythia6.Pycomp(311),1,1);   // K0
   pythia6.SetMDCY(pythia6.Pycomp(-311),1,1);  // K0bar
 
   gMC->SetUserDecay(311);  // K0
   gMC->SetUserDecay(-311); // K0_bar
   
   pythia6.SetMDCY(pythia6.Pycomp(443),1,1);   // J/psi
   
   gMC->SetUserDecay(443); // J/psi
   
//    /* mu-, mu+, pi+, pi-, K+, K-, K0L, Xi-, J/psi */
//    Int_t pdgnf[10] = { 13, -13, 211, -211, 321, -321, 130, 3312, -3312, 443 };
//    
//    for (Int_t i = 0; i < 9; i++ ) 
//    {
//         Int_t ipdg = pdgnf[i];
//    
//         // Force the decay to be done w/external decayer
//         if (TString(gMC->GetName()) == "TGeant3") gMC->SetUserDecay(ipdg);
// 
//         Int_t j = pythia6.Pycomp(ipdg);
//         
//         cout << ">>> " << ipdg << " " << j 
//              << " " << pythia6.GetMDCY(j,1) 
//              << " " << pythia6.GetMDCY(j,2)
//              << " " << pythia6.GetMDCY(j,3)
//              << endl;
//             
//         // Activate decay in pythia     
//         pythia6.SetMDCY(pythia6.Pycomp(ipdg),1,1); 
//         
//         cout << "<<< " << ipdg << " "  << j 
//              << " " << pythia6.GetMDCY(j,1) 
//              << " " << pythia6.GetMDCY(j,2)
//              << " " << pythia6.GetMDCY(j,3)
//              << "\n\n";
//    }
   
   //--------------------------------------------------------------------
   // rho0, rho+, rho-, D+, D-, D0, D0bar 
   // have decay modes defined in  TGeant3::DefineParticles, 
   // but for these particles those decay modes are overridden to make 
   // use of pythia6.
   //--------------------------------------------------------------------
    
   /* rho0, rho+, rho-, D+, D-, D0, D0bar */
   //Int_t pdghq[7] = { 113, 213, -213, 411, -411, 421, -421 };
   
   /*  D0, Lambda, Lambda_bar */
   //Int_t pdghq[3] = { 421, 3122, -3122 };
       
//    for (Int_t i = 0; i < 3; i++) 
//    {
//         Int_t ipdg = pdghq[i];
//         
//         // Force the decay to be done w/external decayer
//         if (TString(gMC->GetName()) == "TGeant3") gMC->SetUserDecay(ipdg); 
//         
//         // Activate decay in pythia
//         pythia6.SetMDCY(pythia6.Pycomp(ipdg),1,1); 
//    }
   
   //--------------------------------------------------------------------
   // Set pi0 to be stable in pythia6 so that Geant3 can handle decay.
   // In general, TGeant3 is set up through TGeant3gu::gudcay to pass
   // all pythia6 decay products back to the G3 transport mechanism if they
   // have a lifetime > 1.E-15 sec for further transport.
   // Since the pi0 lifetime is less than this, if pi0 is produced as a decay
   // product in pythia6, e.g. KL0 -> pi0 pi+ pi-, the pi0 will be immediately
   // decayed by pythia6 to 2 gammas, and the KL0 decay product list passed
   // back to the transport mechanism will be "gamma gamma pi+ pi-", i.e.  
   // the pi0 will not be visible in the list of secondaries passed back to 
   // the transport mechanism and will not be pushed to the stack for possible 
   // storage to the stdhep output array.
   // To avoid this, the pi0 is set to stable in pythia6, and its decay
   // will be handled by Geant3. 
   // pythia6.SetMDCY(pythia6.Pycomp(111),1,0);
   //--------------------------------------------------------------------
 
   // The following will print the decay modes
   
   //pythia6.Pyupda(1,6);  
}


