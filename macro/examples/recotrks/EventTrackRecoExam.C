

//______________________________________________________________________________
void EventTrackRecoExam()
{
  Int_t nEvents = 0; 

//  TString inFile  = "runMB_L4exam.root";       // Input file (MC data)
//  TString parFile = "paramsMB_L4exam.root";    // Input file with parameters
  TString inFile  = "runMu_L4exam.root";       // Input file (MC data)
  TString parFile = "paramsMu_L4exam.root";    // Input file with parameters

  // output
//  TString outFile = "recoMB_L4exam.root";      // Output file
  TString outFile = "recoMu_L4exam.root";      // Output file
  
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
  
  SpdRootFileSink* sink = new SpdRootFileSink(outFile);
  Run->SetSink(sink);
  
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
  
  //===============================================================//
  // [INNER (VERTEX) TRACKING SYSTEM (ITS) HIT PRODUCER]
  // Input: mc-event, mc-particles, its-points
  // Output: mc-its-hits
  
  SpdItsMCHitProducer* its_hits_producer = new SpdItsMCHitProducer();
 
  its_hits_producer->SetHitProductionMetod(1);  // 0,1 (default: 0)  
  
  its_hits_producer->SetVerboseLevel(1);
  
  Run->AddTask(its_hits_producer);
  
  //=============================================================//
  // [STRAW TRACKING SYSTEM (BARREL+CAPS) HIT PRODUCER]
  // Input: mc-event, mc-particles, 
  //        ts-barrel-points and/or ts-endcaps-points
  // Output: mc-ts-hits
   
  SpdTsMCHitProducer* ts_hits_producer = new SpdTsMCHitProducer();
 
  ts_hits_producer->SetHitType('v');  // 'v'=(R) (default), 'w'=(R,Z)
  
  ts_hits_producer->SaveHits(true);  // default: true
  
  ts_hits_producer->SetVerboseLevel(1);
   
  Run->AddTask(ts_hits_producer);

  //===========================================================//
  // [MC-TRACK FINDER]
  // Input: mc-event, mc-particles, 
  //        mc-its-hits and/or mc-ts-hits
  // Output: mc-tracks + track-fit-parameters (optionally)
  
  SpdMCTracksFinder* track_finder = new SpdMCTracksFinder();
  
  //track_finder->SaveTracks(false);  // default: true
  
  SpdTrackFitterGF* track_fitter = track_finder->Fitter();
     
  //track_fitter->SetFitterMaxTrials(2); // default: 2
  //track_fitter->SetStartSeedMethod(1); // (0,1), default: 1
     
  track_fitter->SetVerboseLevel(0);
  track_finder->SetVerboseLevel(1);

  track_finder->CheckMinItsHits(true,1);    // special for reco
  track_finder->CheckMinTsHits(true,1);     // special for reco 
  track_finder->CheckMinHits(true,3);       // special for reco

//  track_finder->CheckMaxPartGeneration(true,1);    // only primary particle
  track_finder->CheckMaxPartGeneration(true,2);  // primary+secondary
//  track_finder->CheckMaxPartGeneration(true,3);  // primary+secondary and so on

  track_finder->CheckMinPartPt(true,0.025);
  track_finder->CheckMinPartMomentum(true,0.075);
  
  Run->AddTask(track_finder);
  
  //==============================================================//
  // [MC-VERTEX FITTER]
  // Input: mc-event, mc-particles, mc-vertices,
  //        mc-tracks + track-fit-parameters
  // Output: vertices-fit-parameters -> mc-vertices
  
  SpdMCVerticesFitter* mcvtxs_fitter = new SpdMCVerticesFitter();
  
  mcvtxs_fitter->SetVerboseLevel(1);
  
//  Run->AddTask(mcvtxs_fitter);
  
  //====================================================================//
  // [REAL VERTEX FINDER (FITTED MC-TRACKS)]
  // Input: mc-event, mc-particles, 
  //        mc-tracks + track-fit-parameters
  // Output: rc-vertices + vertices-fit-parameters 
  //
  //-------------------------------------------------------------------//
  
  SpdRCVerticesFinder* vtxs_finder = new SpdRCVerticesFinder();
  
  // ATTENTION !!! search and fit for primary vertex only !!! ATTENTION
  vtxs_finder->SetFitSecondaries(false); 
  
  vtxs_finder->SetVerboseLevel(1);

//  TVector3 beamPos(0.0, 0.0, 0.0);
//  TVector3 beamDir(0.0, 0.0, 1.0);
//  vtxs_finder->SetBeamLinePosition(beamPos);

  Run->AddTask(vtxs_finder);
  
  //=======================================================================//
  // [REAL SECONDARY VERTEX FINDER (FITTED MC-TRACKS)]
  // Input: mc-event, mc-particles, 
  //        mc-tracks + track-fit-parameters,
  //        primary rc-vertex + primary rc-vertex-fit-parameters
  // Output: secondary rc-vertices + secondary vertices-fit-parameters 
  //---------------------
  //
  // SetMinItsHits(Int_t)          - minimum number of hits in ITS for each track (default = 3) 
  // SetTrackSelOption(Int_t)      - if option: > 0 : hard cut for track selection will be used (default = true)
  // SetUsedTypeOfPV(Int_t)        - type of used primary vertex (PV): sim vertex(0), reco vertex (1 = default)   
  // SetConstrainToPV(Int_t)       - constrain with PV (1 = default), no constrain (0)  
  // SetMinPVTrackChi2(Double_t)   - minimum chi2 track to PV (default = 0.5) 
  // SetMaxTwoTracksChi2(Double_t) - maximum chi2 between tracks (default = 10.)  
  // SetMinimumMass(Double_t)      - minimum mass window edge (default = 0.0 GeV) 
  // SetMaximumMass(Double_t)      - maximum mass window edge (default = 2.5 GeV)  
  //
  
  SpdRCKFpartV0Finder* v0_finder = new SpdRCKFpartV0Finder();
  
  v0_finder->SetMinItsHits(3);
  v0_finder->SetTrackSelOption(0); 
  
  v0_finder->SetUsedTypeOfPV(1);
  v0_finder->SetConstrainToPV(1);
  
  v0_finder->SetMinPVTrackChi2(2.0);
  v0_finder->SetMaxTwoTracksChi2(2.0);
  
  v0_finder->SetMinimumMass(0); 
  v0_finder->SetMaximumMass(10);

  // search for decay mode of K0
  v0_finder->AddVertexCandidate(-211,211);

  v0_finder->SetVerboseLevel(1);
  
//  Run->AddTask(v0_finder);

  //=======================================================================//
  //
  // Track candidate Seed finder which used only Its hits
  // Input:  mc-event, its-hits
  // Output: rc-track-seeds

  SpdRCTrackSeedsFinder* rc_trseed_finder = new SpdRCTrackSeedsFinder();

  rc_trseed_finder->SetVerboseLevel(0);
//  rc_trseed_finder->SetVerboseLevel(1);

  rc_trseed_finder->CheckMinPtSeed(true, 0.025);     // min pt 
  rc_trseed_finder->CheckMinMomSeed(true, 0.075);    // min momentum

//  rc_trseed_finder->SetMaxIterPVZvtx(2);      // max number iter for vertex 

//  rc_trseed_finder->SetMaxDradiusOfHits(0.50);      // max delta radius 
//  rc_trseed_finder->SetMaxDistPVCluster(2.00);      // max dist to cluster

//  rc_trseed_finder->SetMaxResOfLineFit(0.095);      // for DSSD 
  rc_trseed_finder->SetMaxResOfLineFit(0.030);      // for MAPS

  Run->AddTask(rc_trseed_finder);

  //===========================================================//
  //
  // [Reco-TRACK FINDER]
  // Input: mc-event,
  //        seeds and mc-ts-hits (1D 'v'-type hits only!)
  // Output: rc-tracks + track-fit-parameters 

  SpdRCTracksFinder* rc_track_finder = new SpdRCTracksFinder();

  //rc_track_finder->SaveTracks(false);  // default: true

  SpdTrackFitterGF* rc_track_fitter = rc_track_finder->Fitter();
  rc_track_fitter->SetVerboseLevel(0);

  SpdRCTrackFitterGF* rc_track_RCfitter = rc_track_finder->RCFitter();
  rc_track_fitter->SetVerboseLevel(0);

  rc_track_finder->SetVerboseLevel(0);
//  rc_track_finder->SetVerboseLevel(1);

  rc_track_finder->CheckMinItsHits(true,0);    // min mumber of Its hits
  rc_track_finder->CheckMinTsHits(true,0);     // min number of Ts hits 
  rc_track_finder->CheckMinHits(true,3);       // min number of total hits  

//  rc_track_finder->SetMaxFittedTsHits(3);      // max number of Ts hits for full fit 

//  rc_track_finder->SetUseKalmanTree(true);     // default (false)
//  rc_track_finder->SetFullKalmanTrack(true);   // default (false)
//  rc_track_finder->SetRemoveUsedTsHits(true);  // default (false)

  Run->AddTask(rc_track_finder);
  
  //========================================================================// 

  cout << "Start ... " << endl;
  
  Run->Initialize();
    
  //-------------------------------------//     
  cout << "Run ... " << endl;
  
  Run->Run(0,nEvents);
//  Run->Run(0,10);
 
  //-------------------------------------//
  
  sink->Close();
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
}


