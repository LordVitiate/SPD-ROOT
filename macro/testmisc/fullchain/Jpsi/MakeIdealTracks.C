

void MakeIdealTracks() 
{
  //  
  Int_t nEvents = 0; 
  
  TString inFile  = "run_tor.root";       // Input file (MC events)
  TString parFile = "params_tor.root";    // Parameter file
  TString outFile = "IdealTracks.root";  // Output file
  
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
 
  SpdIdealTrackFinder* TrackFinder = new SpdIdealTrackFinder();
  
  // set geometry
  //TrackFinder->SetTrackingGeoModules(); // Its,TsTB,TsTEC
  
  TrackFinder->AddModule(12); // Its
  TrackFinder->AddModule(2);  // TsTB
  TrackFinder->AddModule(3);  // TsTEC
  
  // set cuts
  TrackFinder->SetMinMomentum(1e-2);  // momentum cut;  default = 1e-2 (10 Mev); absiolute min. value = 4e-3 (4 MeV)
  TrackFinder->SetMinGammaBeta(0.5);  // p/m cut; default = 0.5; absiolute min. value = 0.05
  TrackFinder->SetMinVertexNHits(1);  // its hits number cut; default = 1; absiolute min. value = 0;  
  TrackFinder->SetMinTrackerNHits(2); // tstb+tstec hits number cut; default = 2; absiolute min. value = 2;  
  
  // set hits resolution
  Double_t mkm = 1e-4;
 
  // ------ INNER TRACKER -----
 
  TrackFinder->SetHitResolution(kSpdIts,  'p',  50*mkm,  50*mkm); // ENDCAP: POINT  (u=x,v=y)
  TrackFinder->SetHitResolution(kSpdIts,  'm',  50*mkm,  50*mkm); // BARREL: MAPS   (u,v)
  TrackFinder->SetHitResolution(kSpdIts,  's',  50*mkm,  50*mkm); // BARREL: DSSD   (u,v)
  
  // ----- STRAW TRACKER -----
  
  // --- barrel ---
  TrackFinder->SetHitResolution(kSpdTsTB, 'w', 150*mkm);                  // Wire(1D)    (r,z)
  //TrackFinder->SetHitResolution(kSpdTsTB, 'w', 150*mkm, 2. /*cm*/);       // Wire(1D,2D) (r,z)
 
  //--- endcaps ---
  TrackFinder->SetHitResolution(kSpdTsTEC,'w', 150*mkm);                  // Wire(1D)    (r,z)
  //TrackFinder->SetHitResolution(kSpdTsTEC,'w', 150*mkm, 2. /*cm*/);       // Wire(1D,2D) (r,z)
  
  //------------------------------------//
  Run->AddTask(TrackFinder);
  
  //return;
  
  cout << "Start ... " << endl;
  
  Run->LoadGeoParSet(kFALSE);
  Run->Initialize();
       
  Run->Run(0, nEvents);
  
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

