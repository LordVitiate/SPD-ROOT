//______________________________________________________________
void TestParticleGenerator(Int_t nevents = 1)
{
  // create generator 
  SpdParticleGenerator* partgen = new SpdParticleGenerator();
      
  // initialize generator 
  
  partgen->EnableExternalDecayer();
  
  Int_t inicase = 3;
  
  if (inicase == 1) 
  {
      partgen->Initialize(2212,            /* pdg */
                          0., 0., 5.,      /* momentum, GeV */
                          5,               /* multiplicity (number of particles to generate) */
                          0.1, 0.1, 0.1,   /* vertex, cm */
                          1e-3             /* start time, s */
                     );   
  }
  else if (inicase == 2)
  {  
      partgen->SetPdg(2212);               /* pdg */
      partgen->SetNP(3);                   /* multiplicity (number of particles to generate) */
      partgen->SetTime(1e-3);              /* start time, s */
      
      //partgen->SetMomentum(0,0,-5.);       /* momentum, GeV */     
      partgen->SetMomThetaPhi(5,45,0);    /* momentum [GeV], theta [deg], phi [deg] */
      
      partgen->SetVertex(0.1,0.2,0.3);     /* vertex, cm */
      
      partgen->Init();
  }
  else if (inicase == 3)
  {  
      partgen->Initialize(2, 4,            /*  Z, A (ion: helium) */      
                          0., 0., 5.,      /* momentum, GeV */
                          5,               /* multiplicity (number of particles to generate) */
                          0.1, 0.1, 0.1,   /* vertex, cm */
                          1e-3             /* start time, s */
                     ); 
  }
  else return;
   
  if (!partgen->IsInit()) return;
  
  //----------------------------------
  // generate events
  //----------------------------------
  
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  
  SpdPrimGenData* gendata = new SpdPrimGenData(partgen->ClassName());
  partgen->SetGenData(gendata);
  
  for (Int_t i(0); i<nevents; i++) {
       partgen->ReadEvent(0);
       gendata->Print("test");
       partgen->RemoveGenData();
  }
  
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "\n";
  
  partgen->Print("");
  
} 