

void FitIdealTracks() 
{
  //  
  Int_t nEvents = 0; 
  
  TString inFile  = "IdealTracks.root";  // Input file (MC trakcs)
  TString parFile = "params_tor.root";   // Parameter file
  TString outFile = "IdealFittedTracks.root";   // Output file
  
  SpdRunAna* Run = new SpdRunAna();
  
  Run->SetInputFile(inFile);
  Run->SetOutputFile(outFile);
  
  FairRuntimeDb* rtdb = Run->GetRuntimeDb();
  FairParRootFileIo* parInput = new FairParRootFileIo();
  parInput->open(parFile.Data());
  rtdb->setFirstInput(parInput);
  
  TStopwatch timer;
  timer.Start();
 
  //-------------------------------------//
  // Make task                           //
  //-------------------------------------//
 
  SpdIdealKalmanFitter* KalmanFitter = new SpdIdealKalmanFitter();
  
  KalmanFitter->SetVerboseLevel(2); // level = <1 (no prints), 1(default), 2, 3, 4
  
  KalmanFitter->SetTrackingGeoModules(); // add Its,TsTB,TsTEC modules to analysis
  
  //KalmanFitter->AddModule(12); // Its
  //KalmanFitter->AddModule(2);  // TsTB
  //KalmanFitter->AddModule(3);  // TsTEC
  
  // standard options: 
  KalmanFitter->SetSeedMomentum(1.);           // Start vertex momentum for fit, GeV/c (default = 1 GeV/c) 
  
  KalmanFitter->SetHitRepresentation(kSpdIts,pkPixel);   // options: pkPixel, pkSpacepoint
  KalmanFitter->SetHitRepresentation(kSpdTsTB,pkWire);   // options: pkWire, pkWirePoint, pkSpacepoint
  KalmanFitter->SetHitRepresentation(kSpdTsTEC,pkWire);  // options: pkWire, pkWirePoint, pkSpacepoint
  
  // advanced options:
  KalmanFitter->SetVertexFindingMethod(1);     // <1 (vertex will not be found), (default = 1)   
  KalmanFitter->SetVertexFindingAngleCut(3.0); // skip tracks with theta 90+-cut degree (default = 3[deg])
  
  KalmanFitter->SetStartSeedMethod(2);         // set [SeedMethod]: choose start value initialization method (1, 2 = default)
  
  KalmanFitter->SetSmearSeedValues(true);      // [SeedMethod = 1,2] start values smearing flag (default = true)
  KalmanFitter->SetSmearingMomDelta(0.1);      // [SeedMethod = 1,2] track start momentum smearing (+-)value, GeV/c (default = 0.1 GeV/c) 
  KalmanFitter->SetSmearingAngleDelta(3.0);    // [SeedMethod = 1] track start angle smearing  (+-)value, deg (default = 3.0 deg)
  KalmanFitter->SetSmearingVertexDelta(0.5);   // [SeedMethod = 1] track start vertex position smearing (+-)value, cm (default = 0.5 cm)
  KalmanFitter->SetNPointsVertexUse(0);        // [SeedMethod = 2] <1 (default): number of points is the maximum possible (usually 5-6 )

  // Material effects option: <  1 : fit without materials;
  //                          >= 1 : fit taking into accont material effects (default);
  
  KalmanFitter->SetMaterialEffectsOption(1);

  // method to update covariance matrix (default = 1)  
  KalmanFitter->SetFitterOption(1);      
  
  //------------------------------------//
  Run->AddTask(KalmanFitter);
  
  //return;
  
  cout << "Start ... " << endl;
  
  Run->Initialize();
  
  //return;
       
  Run->Run(0, nEvents);
  //Run->Run(0,1000);
  //Run->Run(0,10);
   
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
    
  gApplication->Terminate();
}
