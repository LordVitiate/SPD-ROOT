

SpdPrimaryGenerator* MakePrimaryGenerator(Int_t nevents, Int_t nskipevents);

//_________________________________________________________________________
void SimuEvtBase(Int_t nEvents = 1, Int_t nSkipEvents = 0)
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
  
  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
  
  /* --- field map --- */
  SpdFieldMap* MagField = new SpdFieldMap("Hybrid field");
  MagField->InitData("map_hyb_1T2cm.bin");
  
  run->SetField(MagField);
  
  MagField->Print();
  
  /*--------------------------------------------------*/ 
  /* ++++++++++ DEFINE PRIMARY GENERATOR ++++++++++++ */ 
  /*--------------------------------------------------*/
 
  SpdPrimaryGenerator* primGen = MakePrimaryGenerator(nEvents,nSkipEvents);

  if (!primGen) return;
  
  run->SetGenerator(primGen);  
  
  /* ------------------------------------------------ */ 
  /* +++++++++++++++ GLOBAL OPTIONS +++++++++++++++++ */
  /* ------------------------------------------------ */ 
  
  run->SetStoreTraj(kTRUE);
  
  //SpdCommonGeoMapper::Instance()->UnsetMaterials(1);  // set all materials = vacuum (precise)
 
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
  //SpdCommonGeoMapper::Instance()->PrintGeometry();
  
  /*--------------------------------------------------*/ 
  
  gApplication->Terminate();
}

//_______________________________________________________________________________
SpdPrimaryGenerator* MakePrimaryGenerator(Int_t nevents, Int_t nskipevents)
{
   //===========================
   //   EVTBASE GENERATOR 
   //===========================
   
   SpdEvtBaseGenerator* evtgen = new SpdEvtBaseGenerator(); 
  
   evtgen->SetFile("EvtBaseInput.txt");
   
   evtgen->Initialize(nevents,nskipevents); // number of events (generate,skip)
   
   if (!evtgen->IsInit()) 
   {
       cout << "<MakePrimaryGenerator> Bad initialization " << endl;
       delete evtgen;
       return 0; 
   }
   
   evtgen->Print("");
   
   //==============================================================================

   SpdPrimaryGenerator* primGen = new SpdPrimaryGenerator(); 
      
   primGen->SetVerboseLevel(-10);
   primGen->SetVerbose(1);
   
   //primGen->SetTarget(0.,5.);    // Z- position, 0.5*delta [cm]
   //primGen->SmearVertexZ(kTRUE);
   
   primGen->AddGenerator(evtgen);
   
   return primGen;
}


