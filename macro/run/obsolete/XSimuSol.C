
//_________________________________________________________________________
void XSimuSol(Int_t nEvents = 1)
{  
  TString outFile = "run_sol.root";    // Output data file name
  TString parFile = "params_sol.root"; // Output parameters file name
  
  FairRunSim* run = new FairRunSim();
  
  run->SetName("TGeant4");          
  run->SetMaterials("media.geo");
  
  run->SetOutputFile(outFile);   
 
  run->SetMCEventHeader(new SpdMCEventHeader);
 
  /*--------------------------------------------------*/
  /* +++++++++     GEOMETRY (SOLENOIDAL)    +++++++++ */
  /*--------------------------------------------------*/
   
  SpdCommonGeoMapper::Instance()->DefineSolGeometrySet(1);
  
  /* ++++++++++++++++++ CAVE ++++++++++++++++++ */
  
  FairModule* cave = new SpdCave("CAVE");
  //cave->SetGeometryFileName("cave.geo");
  cave->SetGeometryFileName("cave_precise.geo");
  run->AddModule(cave);
  
  /* ++++++++++++++++++ PIPE ++++++++++++++++++ */
  
  SpdPipe* Pipe = new SpdPipe();
  
  //Pipe->SetPipeMaterial("silicon");
  //Pipe->UnsetMaterials("vacuum2");
  
  run->AddModule(Pipe);
   
  /* ++++++++++++++++++ MAGNET +++++++++++++++++ */
  
  SpdSolMagnet* Magnet = new SpdSolMagnet();
   
  //Magnet->SetMaterial("iron");  
  //Pipe->UnsetMaterials(1);

  run->AddModule(Magnet); 
  
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
  
  SpdTsSB*    ts_barrel   = new SpdTsSB();     /* +++++++++ TSS (BARREL) ++++++++++++ */ 
  SpdTsSEC*   ts_ecps     = new SpdTsSEC();    /* +++++++++ TSS (ENDCAPS) +++++++++++ */     
  SpdEcalSB*  ecal_barrel = new SpdEcalSB();   /* +++++++++ ECALS (BARREL) ++++++++++ */      
  SpdEcalSEC* ecal_ecps   = new SpdEcalSEC();  /* +++++++++ ECALS (ENDCAPS) +++++++++ */  
  SpdRsSB*    rs_barrel   = new SpdRsSB();     /* +++++++++ RSS (BARREL) ++++++++++++ */  
  SpdRsSEC*   rs_ecps     = new SpdRsSEC();    /* +++++++++ RSS (ENDCAPS) +++++++++++ */
  
  run->AddModule(ts_barrel);  
  run->AddModule(ecal_barrel);
  run->AddModule(rs_barrel);
  run->AddModule(ts_ecps);   
  run->AddModule(ecal_ecps);
  run->AddModule(rs_ecps);
  
  /* Detector's (bulder, mapper, simulation module) settings */
   
  SpdTsSBGeoBuilder::Instance()->SetMaxBuildGeoLevel(1);
  
  /* ===== Vertex detector ===== */
   
  FairDetector* sts = new SpdSts("STS",kFALSE);
  //sts->SetGeometryFileName("sts.geo");
  sts->SetGeometryFileName("sts_vacuum.geo");
  run->AddModule(sts);

 
  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
  
  /* --- const field --- */
  SpdConstField* MagField = new SpdConstField();  
  MagField->SetField(0., 0. , 10); //  kG

  /* --- radial field --- */
  //SpdRadialField* MagField = new SpdRadialField(); 
  //MagField->SetFieldParameter(0,10);  // second parameter: B_max, kG
    
 
  /* === define field region === */
  //SpdRegion* reg = 0;
  
  //reg = MagField->CreateFieldRegion("box");
  //reg->SetBoxRegion(-1000, 1000, -1000, 1000, -1000, 1000); // (X,Y,Z)_(min,max), cm
  
  //reg = MagField->CreateFieldRegion("tube");
  //reg->SetTubeRegion(0, 1000, -1000, 1000); // (R,Z)_(min,max), cm 
  
  
  run->SetField(MagField);
  
  MagField->Print();
  /*--------------------------------------------------*/ 
  /* ++++++++++ DEFINE PRIMARY GENERATORS +++++++++++ */ 
  /*--------------------------------------------------*/
 
  SpdPrimaryGenerator* primGen = new SpdPrimaryGenerator(); 
 
      //===========================
      //   ISOTROPIC GENERATOR (1)
      //===========================
      
      SpdIsotropicGenerator* isogen1 = new SpdIsotropicGenerator("isogen1");
      
      isogen1->Initialize(-11,1.0,2);  // pdg number, momentum (GeV/c), particles per event or LEVEL
      isogen1->SetVerboseLevel(1);
      isogen1->SetVGenOpt(-1);  // (+-)1, (+-)2
      
      primGen->AddGenerator(isogen1);
      
      //===========================
      //   ISOTROPIC GENERATOR (2)
      //===========================
         
      SpdIsotropicGenerator* isogen2 = new SpdIsotropicGenerator("isogen2");
      
      isogen2->Initialize(11,1.0,2);  // pdg number, momentum (GeV/c), particles per event or LEVEL
      isogen2->SetVerboseLevel(1);
      isogen2->SetVGenOpt(-1);  // (+-)1, (+-)2 
      
      primGen->AddGenerator(isogen2);
      
      //============================
  
  run->SetGenerator(primGen);  
  
  /* ------------------------------------------------ */ 
  /* +++++++++++++++ GLOBAL OPTIONS +++++++++++++++++ */
  /* ------------------------------------------------ */ 
  
  run->SetStoreTraj(kTRUE);
  
  SpdCommonGeoMapper::Instance()->UnsetMaterials(1);  // set all materials = vacuum (precise)

  //SpdCommonGeoMapper::Instance()->SaveEmptyHits();
  
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
