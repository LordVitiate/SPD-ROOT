
void SetParameters_1(SpdMultiParticleGenerator* partgen);
void SetParameters_2(SpdMultiParticleGenerator* partgen);

//______________________________________________________________
void TestMultiParticleGenerator(Int_t nevents = 1)
{
  // create generator 
  SpdMultiParticleGenerator* partgen = new SpdMultiParticleGenerator();
      
  // initialize generator 
   
  //SetParameters_1(partgen);
  SetParameters_2(partgen); 
  
  partgen->Init();
 
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

//______________________________________________________________
void SetParameters_1(SpdMultiParticleGenerator* partgen)
{
  partgen->EnableExternalDecayer();
  
  partgen->SetNP(4);  /* set number of particles in the list */

  partgen->SetParticle(0, 0., 0., -5., 2212);  /* particle number, momentum[3], pdg */  
  partgen->SetVertex(0, 0.1, 0.2, 0.3);        /* particle number, vertex[3], cm */
  partgen->SetTime(0, 1e-3);                   /* start time, s */
  
  Double_t px, py, pz;
  //SpdMultiParticleGenerator::Momentum(5., 45., 0., px, py, pz); /* momentum [GeV], theta [deg], phi [deg] */
  SpdMultiParticleGenerator::Momentum(5., px, py, pz); /* momentum [GeV], random direction! */
  
  partgen->SetParticle(1, px, py, pz, 443);    /* particle number, momentum[3], pdg */  
  partgen->SetVertex(1, -0.1, 0.1, -0.1);      /* particle number, vertex[3], cm */
  partgen->SetTime(1, 1e-4);                   /* start time, s */
  
  Int_t pdg = SpdMultiParticleGenerator::IonPdg(3,7);
  
  partgen->SetParticle(2, 1., 1., 1., pdg);    /* particle number, momentum[3], pdg */  
  partgen->SetVertex(2, 0., 0., 1);            /* particle number, vertex[3], cm */
  partgen->SetTime(2, 1e-2);                   /* start time, s */
  
  partgen->SetParticle(3, 1., 1., 1., 2, 4);   /* particle number, momentum[3], Z, A */  
  partgen->SetVertex(3, 0., 0., 1);            /* particle number, vertex[3], cm */
  partgen->SetTime(3, 1e-2);                   /* start time, s */
}

//______________________________________________________________
void SetParameters_2(SpdMultiParticleGenerator* partgen)
{
   Int_t np = 8;
   
   partgen->SetNP(np);  /* set number of particles in the list */
   
   Double_t px, py, pz;
   
   Double_t dphi = 360./np;
   
   for (Int_t i(0); i<np; i++) {
    
        /* momentum [GeV], theta [deg], phi [deg] */
        SpdMultiParticleGenerator::Momentum(5., 90., i*dphi, px, py, pz);
       
        /* particle number, momentum[3], pdg */  
        partgen->SetParticle(i,px, py, pz, 13); 
   }
}


