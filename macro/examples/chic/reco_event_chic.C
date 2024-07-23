

//______________________________________________________________________________
void reco_event_chic()
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
  
  //==========================================================================================//
  // Input/output data branches.
  //==========================================================================================//
  //
  // Input [SIMU], TREE NAME: "spdsim". 
  //
  //  (full data list):           DATA BRANCH NAME:     BRANCH OBJECT TYPE:
  //
  //    - simu-event header       "MCEventHeader."      SpdMCEventHeader
  //    - simu-event mctracks     "SpdMCTrack"          TClonesArray[SpdMCTrack]
  //    - its-points              "SpdItsPoint"         TClonesArray[SpdItsPoint]
  //    - ts-barrel-points        "SpdTsTB2Point"       TClonesArray[SpdTsTB2Point]
  //    - ts-endcaps-points       "SpdTsTEC2Point"      TClonesArray[SpdTsTEC2Point]
  //    - ecal-barrel-points      "SpdEcalTB2Point"     TClonesArray[SpdEcalTB2Point]
  //    - ecal-endcaps-points     "SpdEcalTEC2Point"    TClonesArray[SpdEcalTEC2Point]
  //    - rs-barrel-points        "SpdRsTB2Point"       TClonesArray[SpdRsTB2Point]
  //    - rs-endcaps-points       "SpdRsTEC2Point"      TClonesArray[SpdRsTEC2Point]
  //
  //-------------------------------------------------------------------------------------------- 
  //
  // Output [RECO], TREE NAME: "spdsim". 
  //
  //  (full data list):           TASKS:
  //
  //    - simu-event-header       - 
  //    - mc-event                SpdMCEventMaker
  //    - mc-particles            SpdMCEventMaker
  //    - mc-vertices             SpdMCEventMaker
  //    - mc-its-hits             SpdItsMCHitProducer
  //    - mc-ts-hits              SpdTsMCHitProducer 
  //    - mc-ecal-hits            SpdEcalMCHitProducer 
  //    - mc-rs-hits              SpdRsMCHitProducer 
  //    - mc-tracks               SpdMCTracksFinder (+ fit pars., optionally)
  //    - vertices-fit-pars.(mc)  SpdMCVerticesFitter (mc-vertices update)
  //    - rc-vertices             SpdRCVerticesFinder (+ fit pars. )
  //    - rc-ecal-clusters        SpdEcalRCMaker
  //    - rc-ecal-particles       SpdEcalRCMaker
  //      
  //  (full data list):           DATA BRANCH NAME:     BRANCH OBJECT TYPE:
  //
  //    - simu-event-header       "MCEventHeader."      FairMCEventHeader
  //    - mc-event                "MCEvent."            SpdMCEvent
  //    - mc-particles            "MCParticles"         TClonesArray[SpdMCParticle]
  //    - mc-vertices             "MCVertices"          TClonesArray[SpdVertexMC(+SpdVertexFitPar)]
  //    - mc-its-hits             "ItsMCHits"           TClonesArray[SpdMCSiliconHit]
  //    - mc-ts-hits              "TsMCHits"            TClonesArray[SpdMCStrawHit(1D/2D)]
  //    - mc-ecal-hits            "EcalMCHits"          TClonesArray[SpdEcalMCHit]
  //    - mc-rs-hits              "RsMCHits"            TClonesArray[SpdRsMCHit]
  //    - mc-tracks               "MCTracks"            TClonesArray[SpdTrackMC(+SpdTrackFitPar)]
  //    - rc-vertices             "RCVertices"          TClonesArray[SpdVertexRC(+SpdVertexFitPar)]
  //    - rc-ecal-clusters        "RCEcalClusters"      TClonesArray[SpdEcalRCCluster]
  //    - rc-ecal-particles       "RCEcalParticles"     TClonesArray[SpdEcalRCParticle]
  //
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
  
  SpdItsMCHitProducer* its_hits_producer = new SpdItsMCHitProducer();
 
  //its_hits_producer->SetHitProductionMetod(1);  // 0,1 (default: 0)  
  
  //its_hits_producer->SaveHits(false); // default: true
  
  its_hits_producer->SetVerboseLevel(1);
  
  Run->AddTask(its_hits_producer);
  
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
  // [ELECTROMAGNETIC CALORIMETER (BARREL+CAPS) HIT PRODUCER]
  // Input: mc-event, mc-particles, 
  //        ecal-barrel-points and/or ecal-endcaps-points
  // Output: mc-ecal-hits
  
  SpdEcalMCHitProducer* ecal_hits_producer = new SpdEcalMCHitProducer();
  
  //ecal_hits_producer->SaveHits(false);  // default: true
  
  ecal_hits_producer->SetVerboseLevel(1);
   
  Run->AddTask(ecal_hits_producer);
 
  //--------------------------------------------------------//
  // [MUON-HADRON RANGE SYSTEM (BARREL+CAPS) HIT PRODUCER]
  // Input: mc-event, mc-particles, 
  //        rs-barrel-points and/or rs-endcaps-points
  // Output: mc-rs-hits
  
  SpdRsMCHitProducer* rs_hits_producer = new SpdRsMCHitProducer();
  
  //rs_hits_producer->SaveHits(false);  // default: true
  
  rs_hits_producer->SetVerboseLevel(1);
   
  Run->AddTask(rs_hits_producer);
  
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
     
     track_fitter->SetVerboseLevel(1);
      
  track_finder->SetVerboseLevel(1);
  
  Run->AddTask(track_finder);
  
  //--------------------------------------------------------//
  // [MC-VERTEX FITTER]
  // Input: mc-event, mc-particles, mc-vertices,
  //        mc-tracks + track-fit-parameters
  // Output: vertices-fit-parameters -> mc-vertices
  
  SpdMCVerticesFitter* mcvtxs_fitter = new SpdMCVerticesFitter();
  
  mcvtxs_fitter->SetVerboseLevel(1);
  
  Run->AddTask(mcvtxs_fitter);
  
  //--------------------------------------------------------//
  // [REAL VERTEX FINDER (FITTED MC-TRACKS)]
  // Input: mc-event, mc-particles, 
  //        mc-tracks + track-fit-parameters
  // Output: rc-vertices + vertices-fit-parameters 
  
  SpdRCVerticesFinder* vtxs_finder = new SpdRCVerticesFinder();
 
  //vtxs_finder->SaveVertices(false);  
  
  vtxs_finder->SetMinTrackPVchi2(0.);
  
  vtxs_finder->SetVerboseLevel(1);

  Run->AddTask(vtxs_finder);
  
  //--------------------------------------------------------//
  // [REAL ECAL CLUSTERIZATION AND 
  //            CLUSTER PARAMETERS RECONSTRUCTION]
  // Input: mc-event, mc-ecal-hits
  // Output: rc-ecal-clusters, rc-ecal-particles
  
  SpdEcalRCMaker* ecal_rc = new SpdEcalRCMaker();
 
  //ecal_rc->SaveClusters(false);
  //ecal_rc->SaveParticles(false);  
  
  ecal_rc->SetVerboseLevel(1);

  Run->AddTask(ecal_rc);
  
  //========================================================================// 


  cout << "Start ... " << endl;
  
  Run->Initialize();
  
  
  //-------------------------------------//     
  cout << "Run ... " << endl;
  
  Run->Run(0,nEvents);
  //Run->Run(0,10);
  //Run->Run(0,1);
  
  //-------------------------------------//
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  
  //SpdMCEventHelper::PrintMCProcesses();
 
  //SpdMCEventHelper* smceh = SpdMCEventHelper::Instance();
  //smceh->PrintParticles();
}


