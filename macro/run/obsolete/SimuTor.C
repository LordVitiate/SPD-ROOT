
//_________________________________________________________________________
void SimuTor(Int_t nEvents = 1)
{  
  TString outFile = "run_tor.root";    // Output data file name
  TString parFile = "params_tor.root"; // Output parameters file name
  
  SpdRunSim* run = new SpdRunSim();
  
  run->SetName("TGeant4");     
  run->SetMaterials("media.geo"); 
  
  run->SetOutputFile(outFile);        
  
  run->SetMCEventHeader(new SpdMCEventHeader);
 
  /*--------------------------------------------------*/
  /* +++++++++     GEOMETRY (TOROIDAL)    +++++++++++ */
  /*--------------------------------------------------*/
  
  SpdCommonGeoMapper::Instance()->DefineTorGeometrySet(1);
  
  /* ++++++++++++++++++ CAVE ++++++++++++++++++ */
  
  FairModule* cave = new SpdCave("CAVE");
  //cave->SetGeometryFileName("cave.geo");
  cave->SetGeometryFileName("cave_precise.geo");
  run->AddModule(cave);
  
  /* ++++++++++++++++++ PIPE ++++++++++++++++++ */
  
  SpdPipe* Pipe = new SpdPipe();
  run->AddModule(Pipe);
  
  /* +++++++++++++++++ MAGNET +++++++++++++++++ */
  
  SpdTorMagnet* Magnet = new SpdTorMagnet();
  run->AddModule(Magnet);
  
  /* +++++++++++++++++ BASKET +++++++++++++++++ */
  
  //SpdTorBasket* Basket = new SpdTorBasket();  
  //run->AddModule(Basket);
  
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
  
 
  /* ===== Vertex detector ===== */
   
   FairDetector* sts = new SpdSts("STS",kFALSE);
//   //sts->SetGeometryFileName("sts.geo");
   sts->SetGeometryFileName("sts_vacuum.geo");
   run->AddModule(sts);

    
  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
  
  
  /* --- const field --- */
  //SpdConstField* MagField = new SpdConstField();  
  //MagField->SetField(0., 0. , 10); //  kG

  /* --- radial field --- */
  //SpdRadialField* MagField = new SpdRadialField(); 
  //MagField->SetFieldParameter(0,25);  // second parameter: B_max, kG
    
  /* --- field map --- */
  SpdFieldMap* MagField = new SpdFieldMap("Toroidal field");
  MagField->InitData("map_tor_rot.bin");
  //MagField->SetApproxMethod(0); // 0 (linear,default) or 1 (nearest vertex)
  //MagField->MultiplyField(1);
  
  
  /* === define field region === */
  SpdRegion* reg = 0;
  
  //reg = MagField->CreateFieldRegion("box");
  //reg->SetBoxRegion(-1000, 1000, -1000, 1000, -1000, 1000); // (X,Y,Z)_(min,max), cm
  
  reg = MagField->CreateFieldRegion("tube");
  reg->SetTubeRegion(0, 180, -250, 250); // (R,Z)_(min,max), cm 
  
  
  run->SetField(MagField);
  
  MagField->Print();
  
  /*--------------------------------------------------*/
  /* ++++++++++ DEFINE PRIMARY GENERATORS +++++++++++ */
  /*--------------------------------------------------*/
 
  SpdPrimaryGenerator* primGen = new SpdPrimaryGenerator();

      //============================
      //    PYTHIA 6 GENERATOR
      //============================
      
      SpdPythia6Generator* P6gen = new SpdPythia6Generator();
      
      //-----------------------------------------------------------
      // Option: (decayer for vertex meson- and baryon- resonances)
      // 0 = Transport generator + External decayer (particles don't decay in the vertex) 
      // 1 = Primary generator (DEFAULT value, more safely)
      
      P6gen->SetVGenOpt(1);
        
      //-----------------------------------------------------------
      // Set seed:
      
      //P6gen->SetSeed(0);
      //P6gen->SetSeed(144135822, 2);
      
      P6gen->SetSeed(12995,0);
      //P6gen->SetSeed(19949278, 1);
      //P6gen->SetSeed(13495);
      //P6gen->SetSeed(127472);
      

      /* --- Set Pythia6 options --- */
      SpdPythia6* gg = (SpdPythia6*)P6gen->GetGenerator();
       
      gg->SetMSEL(1);  // set miminum bias 

       
      /* --- Pythia6 initialization --- */
      P6gen->Initialize("cms","p","p",26/*GeV*/);
       
      primGen->AddGenerator(P6gen);
      
      //============================

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
