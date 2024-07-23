

//______________________________________________________________________________
void RecoMvdEventFull()
{
  Int_t nEvents = 0; 
  
  TString inFile  = "run.root";       // Input file (MC data)
  TString parFile = "params.root";    // Input file with parameters
  
  TString outFile = "reco_full.root"; // Output file
  
  SpdRunAna* Run = new SpdRunAna();   // Main task
  
  Run->SetContainerStatic(true);      // prevent other parameters request (in case of chain files mode)
  
  FairFileSource* SourceFile = new FairFileSource(inFile);
  Run->SetSource(SourceFile);

  //-----------------------------------------
  // Add other data files here ... 
  // 
  // SourceFile->AddFile("run<...>.root");
  // SourceFile->AddFile("run<...>.root");
  // ...
  //-----------------------------------------
  
  SpdRootFileSink* OutputFile = new SpdRootFileSink(outFile);
  Run->SetSink(OutputFile);
  
  FairRuntimeDb* rtdb = Run->GetRuntimeDb();
  FairParRootFileIo* parInput = new FairParRootFileIo();
  parInput->open(parFile.Data());
  rtdb->setFirstInput(parInput);
  
  TStopwatch timer;
  timer.Start();
  

  //--------------------------------------------------------------------------------------------
  //
  //  Verbose level (all tasks):
  //
  //  (< -1)    : nothing to print 
  //  (> -2) -1 : error & fatal error messages
  //  (> -1)  0 : + task (run) init/finish messages
  //  (>  0)  1 : + [DEFAULT] start event info message 
  //  (>  1)  2 : + finish event info messages (event summary)
  //  (>  2)  3 : + common info messages 
  //  (>  3)    : + other (debug) messages
  //
  //==========================================================================================//
  

  //-------------------------------------------------------//
  // [EVENT MAKER]
  // Input:  simu event header, simu event tracks.
  // Output: mc-event, mc-particles, 
  //         mc-vertices (without fit parameters)
  
  SpdMCEventMaker* event_maker = new SpdMCEventMaker();
  
  //event_maker->SaveEvent(false);     // default: true
  //event_maker->SaveParticles(false); // default: true
  //event_maker->SaveVertices(false);  // default: true
  
  event_maker->SetVerboseLevel(1);
  
  Run->AddTask(event_maker);
  
  //--------------------------------------------------------//
  // [INNER (VERTEX) TRACKING SYSTEM (ITS) HIT PRODUCER]
  // Input: mc-event, mc-particles, its-points
  // Output: mc-its-hits
  
//   SpdItsMCHitProducer* its_hits_producer = new SpdItsMCHitProducer();
//  
//   //its_hits_producer->SaveHits(false); // default: true
//   
//   its_hits_producer->SetVerboseLevel(1);
//   
//   Run->AddTask(its_hits_producer);
  
  //--------------------------------------------------------//
  // [INNER (VERTEX) TRACKING SYSTEM (MVD) HIT PRODUCER]
  // Input: mc-event, mc-particles, mvd-points
  // Output: mc-its-hits
  
  SpdMvdMCHitProducer* mvd_hits_producer = new SpdMvdMCHitProducer();
 
  //mvd_hits_producer->SaveHits(false); // default: true
  
  Double_t mkm = 1.e-4; // mkm -> cm 
  
  // Set hit resolution (layer ID, u_res, v_res), u => z-axis, v = (rphi) 
  // default (10 layers  => u_res = 800 mkm,  v_res = 90 mkm)
   
//     // geometry type: 1
//     mvd_hits_producer->SetHitResolution(1, 800*mkm,  90*mkm); 
//     mvd_hits_producer->SetHitResolution(4, 800*mkm, 105*mkm); 
//     mvd_hits_producer->SetHitResolution(6, 800*mkm, 105*mkm); 
//   
//     // geometry type: 2
//     mvd_hits_producer->SetHitResolution(1, 800*mkm,  90*mkm); 
//     mvd_hits_producer->SetHitResolution(4, 800*mkm,  90*mkm); 
  
  mvd_hits_producer->SetVerboseLevel(1);
  
  Run->AddTask(mvd_hits_producer);
  
  //--------------------------------------------------------//
  // [STRAW TRACKING SYSTEM (BARREL+CAPS) HIT PRODUCER]
  // Input: mc-event, mc-particles, 
  //        ts-barrel-points and/or ts-endcaps-points
  // Output: mc-ts-hits
   
  SpdTsMCHitProducer* ts_hits_producer = new SpdTsMCHitProducer();
 
  ts_hits_producer->SetHitType('v');  // 'v'=(R) (default), 'w'=(R,Z)
  
  //ts_hits_producer->SaveHits(false);  // default: true
  
  ts_hits_producer->SetVerboseLevel(1);
   
  Run->AddTask(ts_hits_producer);
  
  //--------------------------------------------------------//
  // [MC-TRACK FINDER]
  // Input: mc-event, mc-particles, 
  //        mc-its-hits and/or mc-ts-hits
  // Output: mc-tracks + track-fit-parameters (optionally)
  
  SpdMCTracksFinder* track_finder = new SpdMCTracksFinder();
  
  //track_finder->SaveTracks(false);  // default: true
  
     SpdTrackFitterGF* track_fitter = track_finder->Fitter();
     
     //track_fitter->SetFitterMaxTrials(2); // default: 2
     //track_fitter->SetStartSeedMethod(1); // (0,1), default: 1
     
     //track_fitter->StoreImPoints(true);     // default: false
     //track_fitter->StoreImMomentum(true);   // default: true     
     //track_fitter->StoreImPosition(true);   // default: false     
     //track_fitter->StoreImCovariance(true); // default: false   
     
     track_fitter->SetVerboseLevel(1);
      
  track_finder->SetVerboseLevel(1);
  
  Run->AddTask(track_finder);
  
  //========================================================================// 

  cout << "Start ... " << endl;
  
  Run->Initialize();
  
  //-------------------------------------//     
  cout << "Run ... " << endl;
  
  Run->Run(0,nEvents);
  //Run->Run(0,10);
  //Run->Run(0,1);
  
  //-------------------------------------//
  
  //OutputFile->Close();
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  
  //SpdMCEventHelper::PrintMCProcesses();
  
  //SpdMCEventHelper::Instance()->PrintParticles();
  //SpdMCEventHelper::Instance()->PrintVertices();
}


