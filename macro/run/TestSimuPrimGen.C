
SpdPrimaryGenerator* MakePrimaryGenerator_1();
SpdPrimaryGenerator* MakePrimaryGenerator_2(Int_t nevents);

//_________________________________________________________________________
void TestSimuPrimGen(Int_t nEvents = 1)
{
  TString outFile = "run_tor.root";    // Output data file name
  TString parFile = "params_tor.root"; // Output parameters file name
  
  SpdRunSim* run = new SpdRunSim();
  
  run->SetName("TGeant4");     
  run->SetMaterials("media.geo");
  
  run->SetOutputFile(outFile);        
 
  run->SetMCEventHeader(new SpdMCEventHeader);
 
  /*--------------------------------------------------*/
  /* +++++++++     GEOMETRY (HYBRID)      +++++++++++ */
  /*--------------------------------------------------*/
   
  SpdCommonGeoMapper::Instance()->DefineHybGeometrySet();
  
  /* ++++++++++++++++++ CAVE ++++++++++++++++++ */
  
  FairModule* cave = new SpdCave("CAVE");
  //cave->SetGeometryFileName("cave.geo");
  cave->SetGeometryFileName("cave_precise.geo");
  run->AddModule(cave);
 
  /* ++++++++++++++++++ PIPE ++++++++++++++++++ */
  
  SpdPipe* Pipe = new SpdPipe();
  run->AddModule(Pipe);
  
  /* +++++++++++++++++ MAGNET +++++++++++++++++ */
  
  SpdHybMagnet* Magnet = new SpdHybMagnet();
  run->AddModule(Magnet);
     
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
     
  SpdTsTB*    ts_barrel   = new SpdTsTB();     /* +++++++++ TST (BARREL) ++++++++++++ */ 
  SpdTsTEC*   ts_ecps     = new SpdTsTEC();    /* +++++++++ TST (ENDCAPS) +++++++++++ */     
  SpdEcalTB*  ecal_barrel = new SpdEcalTB();   /* +++++++++ ECALT (BARREL) ++++++++++ */      
  SpdEcalTEC* ecal_ecps   = new SpdEcalTEC();  /* +++++++++ ECALT (ENDCAPS) +++++++++ */  
  SpdRsTB*    rs_barrel   = new SpdRsTB();     /* +++++++++ RST (BARREL) ++++++++++++ */  
  SpdRsTEC*   rs_ecps     = new SpdRsTEC();    /* +++++++++ RST (ENDCAPS) +++++++++++ */

  run->AddModule(ts_barrel);  
  run->AddModule(ecal_barrel);
  run->AddModule(rs_barrel);
  run->AddModule(ts_ecps);   
  run->AddModule(ecal_ecps);
  run->AddModule(rs_ecps);
  
  /* Detector's (bulder, mapper, simulation module) settings */
  
  //SpdTsTBGeoBuilder::Instance()->SetMaxBuildGeoLevel(1); // set build geometry level for TsTB 
  
  /* ===== Vertex detector ===== */
   
//   FairDetector* sts = new SpdSts("STS",kFALSE);
//   //sts->SetGeometryFileName("sts.geo");
//   sts->SetGeometryFileName("sts_vacuum.geo");
//   run->AddModule(sts);

    
  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
  
  
  /* --- const field --- */
  //SpdConstField* MagField = new SpdConstField();  
  //MagField->SetField(0., 0. , 10); //  kG

    
  /* --- field map --- */
  SpdFieldMap* MagField = new SpdFieldMap("Hybrid field");
  MagField->InitData("map_hyb_1T5cm.bin");
  //MagField->SetApproxMethod(0); // 0 (linear,default) or 1 (nearest vertex)
  MagField->MultiplyField(1.0);
  
  
  /* === define field region === */
  //SpdRegion* reg = 0;
  
  //reg = MagField->CreateFieldRegion("box");
  //reg->SetBoxRegion(-1000, 1000, -1000, 1000, -1000, 1000); // (X,Y,Z)_(min,max), cm
  
  //reg = MagField->CreateFieldRegion("tube");
  //reg->SetTubeRegion(0, 1000, -1000, 1000); // (R,Z)_(min,max), cm 
  
  run->SetField(MagField);
  
  MagField->Print();
  
  /*--------------------------------------------------*/ 
  /* ++++++++++ DEFINE PRIMARY GENERATOR ++++++++++++ */ 
  /*--------------------------------------------------*/
 
  //SpdPrimaryGenerator* primGen = MakePrimaryGenerator_1();
  SpdPrimaryGenerator* primGen = MakePrimaryGenerator_2(nEvents);

  run->SetGenerator(primGen);  
  
  /* ------------------------------------------------ */ 
  /* +++++++++++++++ GLOBAL OPTIONS +++++++++++++++++ */
  /* ------------------------------------------------ */ 
  
  run->SetStoreTraj(kTRUE);
  
  SpdCommonGeoMapper::Instance()->UnsetMaterials(1);  // set all materials = vacuum (precise)
 
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
  if (MergePars) parOut->open(parFile.Data());
  else parOut->open(parFile.Data(),"RECREATE");
  
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
   
  run->CreateGeometryFile("geofile_full.root");
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "             << outFile << endl;
  cout << "Parameter file is "          << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl;
  cout << endl;
  
  /*--------------------------------------------------*/
  
  //SpdCommonGeoMapper::Instance()->PrintGeometryList();
  SpdCommonGeoMapper::Instance()->PrintGeometry();
  
  /*--------------------------------------------------*/ 
  
  gApplication->Terminate();
}

//_______________________________________________________________________________
//_______________________________________________________________________________
//_______________________________________________________________________________
SpdPrimaryGenerator* MakePrimaryGenerator_1()
{
   SpdPrimaryGenerator* primGen = new SpdPrimaryGenerator(); 
 
   //===========================
   //   ISOTROPIC GENERATOR (1)
   //===========================
   
   SpdIsotropicGenerator* isogen1 = new SpdIsotropicGenerator("isogen (1)");
   
   isogen1->Initialize(-11,0.05,1);  // pdg number, momentum (GeV/c), particles per event or LEVEL
   isogen1->SetVerboseLevel(1);
   isogen1->SetVGenOpt(-1);  // (+-)1, (+-)2
   
   primGen->AddGenerator(isogen1);
   
   //============================
   //    PYTHIA 6 GENERATOR (1)
   //============================
   
   SpdPythia6Generator* P6gen = new SpdPythia6Generator("pythia6 (1)");
   
   //-----------------------------------------------------------
   // Option: (decayer for vertex meson- and baryon- resonances)
   // 0 = Transport generator + External decayer (particles don't decay in the vertex) 
   // 1 = Primary generator (DEFAULT value, more safely)
   
   P6gen->SetVGenOpt(0);
     
   //-----------------------------------------------------------
   // Set seed:
   
   //P6gen->SetSeed(0);
   //P6gen->SetSeed(144135822, 2);
   
   P6gen->SetSeed(12995,0);
   //P6gen->SetSeed(19949278, 1);
   //P6gen->SetSeed(13495);
   //P6gen->SetSeed(127472);
   

   /* --- Set Pythia6 options --- */
   SpdPythia6* gg_1 = (SpdPythia6*)P6gen->GetGenerator();
    
   gg_1->SetMSEL(1);  // set miminum bias 

    
   /* --- Pythia6 initialization --- */
   P6gen->Initialize("cms","p","p",26/*GeV*/);
    
   primGen->AddGenerator(P6gen);
   
   //===========================
   //   ISOTROPIC GENERATOR (2)
   //===========================
      
   SpdIsotropicGenerator* isogen2 = new SpdIsotropicGenerator("isogen (2)");
   
   isogen2->Initialize(11,0.05,1);  // pdg number, momentum (GeV/c), particles per event or LEVEL
   isogen2->SetVerboseLevel(1);
   isogen2->SetVGenOpt(-1);  // (+-)1, (+-)2 
   
   primGen->AddGenerator(isogen2);

   
   //============================
   //    PYTHIA 6 GENERATOR (2)
   //============================
   
   SpdPythia6Generator* P6gen_2 = new SpdPythia6Generator("pythia6 (2)");
   
   //-----------------------------------------------------------
   // Option: (decayer for vertex meson- and baryon- resonances)
   // 0 = Transport generator + External decayer (particles don't decay in the vertex) 
   // 1 = Primary generator (DEFAULT value, more safely)
   
   P6gen_2->SetVGenOpt(1);
     
   //-----------------------------------------------------------
   // Set seed:
   
   //P6gen_2->SetSeed(0);
   //P6gen_2->SetSeed(144135822, 2);
   
   P6gen_2->SetSeed(12995,0);
   //P6gen_2->SetSeed(19949278, 1);
   //P6gen_2->SetSeed(13495);
   //P6gen_2->SetSeed(127472);
   

   /* --- Set Pythia6 options --- */
   SpdPythia6* gg_2 = (SpdPythia6*)P6gen_2->GetGenerator();
    
   gg_2->SetMSEL(1);  // set miminum bias 

    
   /* --- Pythia6 initialization --- */
   P6gen_2->Initialize("cms","p","p",26/*GeV*/);
    
   primGen->AddGenerator(P6gen_2);    
   
   
//==============================================================================
     
   //primGen->SetFairMode();
   
   primGen->SetVerboseLevel(-10);
   primGen->SetVerbose(1);
   
   primGen->SetBeam(0., 0., 0.1, 0.1); // X-,Y- position, 0.5*delta or sigma [cm]
   primGen->SmearVertexXY(kTRUE);
   //primGen->SmearGausVertexXY(kTRUE);
   
   primGen->SetTarget(0.,5.);  // Z- position, 0.5*delta or sigma [cm]
   primGen->SmearVertexZ(kTRUE);
   //primGen->SmearGausVertexZ(kTRUE);
   
   return primGen;
}  

//_______________________________________________________________________________
//_______________________________________________________________________________
//_______________________________________________________________________________
SpdPrimaryGenerator* MakePrimaryGenerator_2(Int_t nevents)
{
   SpdPrimaryGenerator* primGen = new SpdPrimaryGenerator(); 
      
   //primGen->SetFairMode();
   
   primGen->SetVerboseLevel(-10);
   primGen->SetVerbose(1);
   
   primGen->SetBeam(0., 0., 0.1, 0.1); // (X,Y)- position, (X,Y)- (0.5*delta or sigma) [cm]
   primGen->SmearVertexXY(kTRUE);
   //primGen->SmearGausVertexXY(kTRUE);
   
   primGen->SetTarget(0.,5.);  // Z- position, 0.5*delta or sigma [cm]
   primGen->SmearVertexZ(kTRUE);
   //primGen->SmearGausVertexZ(kTRUE);
   
   //==============================================================================
   
   //===========================
   //   MULTIPARTCLE GENERATOR 
   //===========================
   
   SpdMultiParticleGenerator* mpartgen = new SpdMultiParticleGenerator("multipartgen");
    
   Int_t np = 8;
   Double_t px, py, pz;
   Double_t dphi = 360./np;
   
   mpartgen->SetNP(np);  /* set number of particles in the list */
   
   for (Int_t i(0); i < np; i++) {
    
        /* momentum [GeV], theta [deg], phi [deg] */
        SpdMultiParticleGenerator::Momentum(0.1, 90., i*dphi, px, py, pz);
        
        //if (i == 3) {
        //    mpartgen->SetParticle(i, px, py, pz, 2, 4, 1, 1e-8); /* particle number, momentum [GeV/c], Z, A */
        //    mpartgen->SetVertex(i,0.345, 0.562, 4.718); /* particle number, primary vertex [cm] */
        //    mpartgen->SetTime(i,1e-9);                  /* particle number, start time [s] */
        //    continue;
        //}
       
        /* particle number, momentum[3], pdg */  
        mpartgen->SetParticle(i, px, py, pz, 11); /* particle number, momentum [GeV/c], pdg */
        
        //mpartgen->SetVertex(i,0.1,0.1,0.1); /* particle number, primary vertex [cm] */
        //mpartgen->SetTime(i,1e-9);          /* particle number, start time [s] */
   }
   
   //mpartgen->EnableExternalDecayer();  /* activate external decayer */
   //mpartgen->SaveParticles(0);       /* save only the first particle in the list */ 
   
   mpartgen->Init();
   
   primGen->AddGenerator(mpartgen);
   
   //return primGen;
   
   //===========================
   //   PARTCLE GENERATOR (1)
   //===========================
   
   // create generator 
   SpdParticleGenerator* partgen1 = new SpdParticleGenerator("partgen (1)");
      
   partgen1->Initialize(443,             /* pdg (J/psi)*/
                        0., 0., 5.,      /* momentum, GeV */
                        3,               /* multiplicity (number of particles to generate) */
                        0.1, 0.1, 0.1,   /* vertex, cm */
                        1e-3             /* start time, s */
                       ); 
   
   partgen1->EnableExternalDecayer();    /* activate external decayer (it's needed for J/psi!) */
   
   primGen->AddGenerator(partgen1);
   
   //===========================
   //   PARTCLE GENERATOR (2)
   //===========================
   
   // create generator 
   SpdParticleGenerator* partgen2 = new SpdParticleGenerator("partgen (2)");
   
   partgen2->SetPdg(443);                /* pdg */
   partgen2->SetNP(3);                   /* multiplicity (number of particles to generate) */
   partgen2->SetTime(1e-3);              /* start time, s */
      
   //partgen2->SetMomentum(0,0,-5.);     /* momentum, GeV */     
   partgen2->SetMomThetaPhi(5.,180.,0.); /* momentum [GeV], theta [deg], phi [deg] */
      
   partgen2->SetVertex(-0.1,-0.1,-0.1);  /* vertex, cm */
        
   partgen2->EnableExternalDecayer();    /* activate external decayer (it's needed for J/psi!) */
   
   partgen2->Init();
    
   primGen->AddGenerator(partgen2);
   
   //===========================
   //   PARTCLE GENERATOR (3)
   //===========================
   
   // create generator 
   SpdParticleGenerator* partgen3 = new SpdParticleGenerator("partgen (3)");
   
   partgen3->Initialize(6, 12,           /* Z, A (ion: helium)  */      
                        0., 0., 5.,      /* momentum, GeV */
                        3,               /* multiplicity (number of particles to generate) */
                        0.1, 0.1, 0.1,   /* vertex, cm */
                        1e-3             /* start time, s */
                     ); 
   
   primGen->AddGenerator(partgen3);
   
   //return primGen;
 
   //===========================
   //   ISOTROPIC GENERATOR (1)
   //===========================
   
   SpdIsotropicGenerator* isogen1 = new SpdIsotropicGenerator("isogen (1)");
   
   isogen1->Initialize(-11,0.05,1);  // pdg number, momentum (GeV/c), particles per event or LEVEL
   isogen1->SetVerboseLevel(1);
   isogen1->SetVGenOpt(-1);  // (+-)1, (+-)2
   
   primGen->AddGenerator(isogen1);
   
   //============================
   //    PYTHIA 6 GENERATOR 
   //============================
   
   SpdPythia6Generator* P6gen = new SpdPythia6Generator("pythia6");
   
   //-----------------------------------------------------------
   // Option: (decayer for vertex meson- and baryon- resonances)
   // 0 = Transport generator + External decayer (particles don't decay in the vertex) 
   // 1 = Primary generator (DEFAULT value, more safely)
   
   P6gen->SetVGenOpt(0);
     
   //-----------------------------------------------------------
   // Set seed:
   
   P6gen->SetSeed(0);
   //P6gen->SetSeed(144135822, 2);
   
   //P6gen->SetSeed(12995,0);
   //P6gen->SetSeed(19949278, 1);
   //P6gen->SetSeed(13495);
   //P6gen->SetSeed(127472);
   
   SpdPythia6* gg = (SpdPythia6*)P6gen->GetGenerator();
   
   Int_t pyopt = 0;
   
   /* --- Set Pythia6 options --- */
   if (pyopt == 0) 
   {     
      gg->SetMSEL(0);  
      gg->SetMSUB(86,1); //gg -> J/Psi g
   }
   else if (pyopt == 1)
   { 
     //gg->SelectParSet(0);  // don't implement any external settings (default)
     gg->SelectParSet(1);    // it's equivalent to  SetMSEL(1);  (set miminum bias)
   }
   else if (pyopt == 2)
   { 
     P6gen->SetVGenOpt(0);
     // psi' generation 
     gg->SetMSEL(0);
     gg->SetMSUB(86,1);
     gg->SetKFPR(86,1,100443); 
     
     gg->SetParameter("MSEL",Int_t(0));
     gg->SetParameter("MSUB[86]",Int_t(1));
     gg->SetParameter("KFPR[1][86]",Int_t(100443));
   }
   
   /* --- Pythia6 initialization --- */
   P6gen->Initialize("cms","p","p",100/*GeV*/); 
   primGen->AddGenerator(P6gen);
   
   //===========================
   //   ISOTROPIC GENERATOR (2)
   //===========================
      
   SpdIsotropicGenerator* isogen2 = new SpdIsotropicGenerator("isogen (2)");
   
   isogen2->Initialize(11,0.05,1);  // pdg number, momentum (GeV/c), particles per event or LEVEL
   isogen2->SetVerboseLevel(1);
   isogen2->SetVGenOpt(-1);  // (+-)1, (+-)2 
   
   primGen->AddGenerator(isogen2);
   
   //===========================
   //   FAIR PARTICLE GENERATOR 
   //===========================

//    FairParticleGenerator* fpgen = new FairParticleGenerator();
//  
//    fpgen->SetPDGType(2212);
//    fpgen->SetMultiplicity(5); 
//    fpgen->SetMomentum(0, 0, 5);
//    fpgen->SetVertex(0, 0, 0);
//   
//    primGen->AddGenerator(fpgen);
   
   //===========================
   //   FTF GENERATOR 
   //===========================
   
   SpdFtfGenerator* ftfgen = new SpdFtfGenerator(0);
  
   // make some settings  
   ftfgen->SetSeed(0); 
   ftfgen->SetConfig("PbarP.mac");  // config file name
    
   // initialize generator
   Double_t Elab = 10; // energy in Lab. CS, GeV
   ftfgen->Initialize(nevents,Elab); 
  
   primGen->AddGenerator(ftfgen);
   
   //===========================
   //   EVTBASE GENERATOR 
   //===========================
   
   SpdEvtBaseGenerator* evtgen = new SpdEvtBaseGenerator(); 
   
   //evtgen->SetFile("SpdPythia6_events.root");
   evtgen->SetFile("SpdPythia6_events.txt");
   
   evtgen->Initialize(nevents,0); // number of events (generate,skip)
   
   if (evtgen->IsInit()) primGen->AddGenerator(evtgen);
   
   //==============================================================================
   
   return primGen;
}    

