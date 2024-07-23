
SpdPythia6Generator* Gen_ = 0;

void GenerateEvent();

//______________________________________________________________
void TestPythia6(int nevents = 1) 
{
   Gen_ = new SpdPythia6Generator();
   
   Gen_->SetKeepEvent(kTRUE);
   
   Gen_->SetVerboseLevel(-2);  // option = -2: no print
   
   //-----------------------------------------------------------
   // Option: (decayer for vertex meson- and baryon- resonances)
   // 0 = Transport generator + External decayer (particles don't decay in the vertex) 
   // 1 = Primary generator (DEFAULT value, more safely)
   // 2 = Standart output (accept particles of status = 1,4); max decay vertex distance should be specified
   
   //Gen_->SetVGenOpt(0);
   Gen_->SetVGenOpt(1);
   
   // MDVD (max decay vertex distance) > 0 [cm], MDVD < 0 => MDVD = 100 cm (default)
   //Gen_->SetVGenOpt(2,0.1 /*MDVD*/);
      
   Gen_->SwitchTestMode(1);
    
   //Gen_->SetSeed(0);
   Gen_->SetSeed(12995,0);
   //Gen_->SetSeed(19949278, 1);
   //Gen_->SetSeed(13495);
   //Gen_->SetSeed(127472);
   
   
   /* J/psi event */
//   Gen_->SetSeed(12995,10);  // [0,10: J/psi] [340: D-meson]
//    SpdPythia6* gg = (SpdPythia6*)Gen_->GetGenerator();
//    gg->SetMSEL(0);  // set miminum bias
//    gg->SetMSUB(86,1); //gg -> J/Psi g
//    Gen_->Initialize("cms","p","p",1000/*GeV*/);
//   
   
   Gen_->Initialize("cms","p","p",20/*GeV*/);
   
   /* GENERATE EVENT */
   Int_t N = nevents;
   
   for (int i(0); i<N; i++) {
        GenerateEvent();
        //if (i % 100000 == 0) 
        //cout << " current event : " << i << endl;
   }
   
   Gen_->Print("");
   
   delete Gen_;
}

//______________________________________________________________________
void GenerateEvent()
{
  if (!Gen_) return;
 
  Gen_->ReadEvent();
  
  const TClonesArray* event = Gen_->GetEvent();
  TParticle* part;
  
  if (event) cout << "<GenerateEvent> Particles (stored): " << event->GetEntriesFast() << endl;
  else cout << "<GenerateEvent> Particles (stored): " << 0 << endl;
 
  return;
  
  if (event) {
      Int_t size = event->GetEntriesFast();
      for (Int_t i(0); i<size; i++) {
           part = (TParticle*)event->At(i);
           cout << i << " " << part->GetPdgCode() << " " << part->GetName() << endl;    
      }
  }
  
}








