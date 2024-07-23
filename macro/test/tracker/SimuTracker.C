
void AddParticleGenerator(SpdPrimaryGenerator* pGen);
void AddMultiParticleGenerator(SpdPrimaryGenerator* pGen);
  
//____________________________________________________________________________________________
void SimuTracker(int nEvents = 1)
{
  //TString outDir = "../output/";
  TString outDir = "";
    
  TString outFile = outDir + "test_data.root";     // Output data file name
  TString outPars = outDir + "test_params.root";   // Output parameters file name
  
  SpdRunSim* run = new SpdRunSim();
  
  run->SetName("TGeant4");       
  run->SetMaterials("media.geo");
  
  run->SetOutputFile(outFile);      
 
  run->SetMCEventHeader(new SpdMCEventHeader);
  
  /*--------------------------------------------------*/
  /* ++++++++++++++++   GEOMETRY   ++++++++++++++++++ */
  /*--------------------------------------------------*/
  
  FairModule* cave = new SpdCave("CAVE");
  //cave->SetGeometryFileName("cave.geo");
  cave->SetGeometryFileName("cave_precise.geo");
  run->AddModule(cave);
  
  Tracker* tracker = new Tracker();
  
  /* ******************************* !!!!!!! SET TRACKER PARAMETERS !!!!!!! ******************************** */
  
  //tracker->SetMaxSimuStep(1);  // cm
  
  //tracker->SetBaseMaterial("vacuum2");
  //tracker->SetActiveMaterial("air");
 
  //tracker->SetSlicing(10);      // set number of slices
  
  /* TRACKER PARAMETERS: Zmin, Zmax, Rmin, Rmax, NSlices */
  
  //tracker->SetGeoParameters(118, 318, 65, 160, 40);
  tracker->SetGeoEndcapsParameters(118, 318, 0, 160, 80);
  //tracker->SetGeoBarrelParameters(118, 318, 0, 160, 40);
  
  /* ******************************* !!!!!!! SET TRACKER PARAMETERS !!!!!!! ******************************** */
  
  run->AddModule(tracker);
  
  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
  

  SpdFieldMap* MagField = new SpdFieldMap("Hybrid field");
  MagField->InitData("map_hyb_1T5cm.dat");
  //MagField->InitData("map_hyb_2T5cm.dat");
 
  //MagField->MultiplyField(2);
  
  /* === define field region === */
  //SpdRegion* reg = 0;
  //reg = MagField->CreateFieldRegion("tube");
  //reg->SetTubeRegion(0, 250, -300, 300); // (R,Z)_(min,max), cm 
  
  run->SetField(MagField);
  
  MagField->Print();
   
  /*--------------------------------------------------*/
  /* ++++++++++ DEFINE PRIMARY GENERATORS +++++++++++ */
  /*--------------------------------------------------*/
  
  SpdPrimaryGenerator* primGen = new SpdPrimaryGenerator();
  
//       FairParticleGenerator* gen = new FairParticleGenerator();
//   
//       gen->SetPDGType(-11);
//       gen->SetMultiplicity(1); 
//       gen->SetMomentum(0, 0.01, 0);
//       gen->SetVertex(0, 0, 0);
//  
//       primGen->AddGenerator(gen);
      
  //AddParticleGenerator(primGen);
  AddMultiParticleGenerator(primGen);
    
  run->SetGenerator(primGen);

  /* ------------------------------------------------ */ 
  /* +++++++++++++++ GLOBAL OPTIONS +++++++++++++++++ */
  /* ------------------------------------------------ */ 
  
  run->SetStoreTraj(kTRUE);
 
  SpdCommonGeoMapper::Instance()->SaveEmptyHits();
    
  /* ----------------------------------------------------------------------- */ 
  /* >>>>>>>>>>>>>>>>>>>>>>>>>>> INITALIZE RUN <<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
  /* ----------------------------------------------------------------------- */ 
  
  cout << "\n\t" << "++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "\t"   << "+                                          +" << endl;
  cout << "\t"   << "+           Init Run (start)               +" << endl; 
  cout << "\t"   << "+                                          +" << endl;
  cout << "\t"   << "++++++++++++++++++++++++++++++++++++++++++++\n" << endl;
     
  run->Init();
  
  cout << "\n\t" << "++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "\t"   << "+                                          +" << endl;
  cout << "\t"   << "+           Init Run (finish)              +" << endl; 
  cout << "\t"   << "+                                          +" << endl;
  cout << "\t"   << "++++++++++++++++++++++++++++++++++++++++++++\n" << endl;

  /*--------------------------------------------------*/
  /* +++++++++++++ CREATE RUN PARAMETERS  +++++++++++ */
  /*--------------------------------------------------*/
  
  Bool_t MergePars = kFALSE;
  FairParRootFileIo* parOut = new FairParRootFileIo(MergePars);
  if (MergePars) parOut->open(outPars.Data());
  else parOut->open(outPars.Data(),"RECREATE");
  
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  rtdb->setOutput(parOut);
   
  /* ----------------------------------------------------------------------- */ 
  /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> RUN <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
  /* ----------------------------------------------------------------------- */ 
  
  TStopwatch timer;
  timer.Start();
  
  run->Run(nEvents);
  
  timer.Stop();
 
  /*--------------------------------------------------*/
  /* ++++++++++++  SAVE RUN PARAMETERS  +++++++++++++ */
  /*--------------------------------------------------*/
 
  rtdb->saveOutput();
  
  /*--------------------------------------------------*/
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "             << outFile << endl;
  cout << "Parameter file is "          << outPars << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl;
  cout << endl;
  
  /*--------------------------------------------------*/
  
  //SpdCommonGeoMapper::Instance()->PrintGeometryList();
  tracker->Print("");
  
  /*--------------------------------------------------*/
  
  gApplication->Terminate();
}

//____________________________________________________________________________________________
void AddParticleGenerator(SpdPrimaryGenerator* pGen)
{
  SpdParticleGenerator* partgen = new SpdParticleGenerator();
      
  // initialize generator 
  
  //partgen->EnableExternalDecayer();
  
  partgen->SetPdg(11);               /* pdg */
  partgen->SetNP(1);                   /* multiplicity (number of particles to generate) */
  
  //partgen->SetTime(1e-3);              /* start time, s */
  
  //partgen->SetMomentum(0,0.01,0);       /* momentum, GeV */     
  partgen->SetMomThetaPhi(0.05,90,90);    /* momentum [GeV], theta [deg], phi [deg] */
  
  //partgen->SetVertex(0.1,0.2,0.3);     /* vertex, cm */
  
  partgen->Init();
  
  if (partgen->IsInit()) pGen->AddGenerator(partgen);
  else delete partgen;
}

//____________________________________________________________________________________________
void AddMultiParticleGenerator(SpdPrimaryGenerator* pGen)
{
   SpdMultiParticleGenerator* mpartgen = new SpdMultiParticleGenerator("multipartgen");
    
   Int_t np = 8;
   Double_t px, py, pz;
   Double_t dphi = 360./np;
   
   mpartgen->SetNP(2*np);  /* set number of particles in the list */
   
   for (Int_t i(0); i < np; i++) {
    
        /* momentum [GeV], theta [deg], phi [deg] */
        SpdMultiParticleGenerator::Momentum(0.05, 80., i*dphi, px, py, pz);
        
        /* particle number, momentum[3], pdg */  
        mpartgen->SetParticle(i, px, py, pz, 11); /* particle number, momentum [GeV/c], pdg */
        mpartgen->SetParticle(np+i, -px, -py, -pz, -11); /* particle number, momentum [GeV/c], pdg */
        
        //mpartgen->SetVertex(i,0.1,0.1,0.1); /* particle number, primary vertex [cm] */
        //mpartgen->SetTime(i,1e-9);          /* particle number, start time [s] */
   }
   
   //mpartgen->EnableExternalDecayer();  /* activate external decayer */
   mpartgen->SaveParticles(0);         /* save only the first particle in the list */ 
  
   mpartgen->Init();
    
   if (mpartgen->IsInit()) pGen->AddGenerator(mpartgen);
   else delete mpartgen;
}



