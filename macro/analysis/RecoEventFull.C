

//______________________________________________________________________________
void RecoEventFull()
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
  //    - tof-barrel-points       "SpdTofBPoint"        TClonesArray[SpdTofBPoint]
  //    - tof-endcaps-points      "SpdTofECPoint"       TClonesArray[SpdTofECPoint]
  //    - ecal-barrel-points      "SpdEcalTB2Point"     TClonesArray[SpdEcalTB2Point]
  //    - ecal-endcaps-points     "SpdEcalTEC2Point"    TClonesArray[SpdEcalTEC2Point]
  //    - rs-barrel-points        "SpdRsTB2Point"       TClonesArray[SpdRsTB2Point]
  //    - rs-endcaps-points       "SpdRsTEC2Point"      TClonesArray[SpdRsTEC2Point]
  //    - bbc-points              "SpdBbcPoint"         TClonesArray[SpdBbcPoint]
  //    - aeg-points              "SpdAegPoint"         TClonesArray[SpdAegPoint]
  //    - zdc-points              "SpdZdcPoint"         TClonesArray[SpdZdcPoint]
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
  //    - mc-tof-hits             SpdTofMCHitProducer 
  //    - mc-ecal-hits            SpdEcalMCHitProducer 
  //    - mc-rs-hits              SpdRsMCHitProducer 
  //    - mc-bbc-hits             SpdBbcMCHitProducer
  //    - mc-aeg-hits             SpdAegMCHitProducer 
  //    - mc-zdc-hits             SpdZdcMCHitProducer 
  //    - mc-tracks               SpdMCTracksFinder (+ fit pars., optionally)
  //    - vertices-fit-pars.(mc)  SpdMCVerticesFitter (mc-vertices update)
  //    - rc-vertices             SpdRCVerticesFinder (+ fit pars. )
  //    - rc-ecal-clusters        SpdEcalRCMaker
  //    - rc-ecal-particles       SpdEcalRCMaker
  //    - mc-ecal-clusters-info   SpdEcalClusterMCInfoMaker
  //    - mc-ecal-particles       SpdEcalClusterMCInfoMaker
  //    - mc-rs-clusters          SpdRsMCClusterMaker
  //    - mc-rs-particles         SpdRsMCClusterMaker
  //    - ts-particles            SpdMCTsParticleProducer
  //    - tof-particles           SpdMCTofParticleProducer
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
  //    - mc-tof-hits             "TofMCHits"           TClonesArray[SpdTofMCHit]
  //    - mc-rs-hits              "RsMCHits"            TClonesArray[SpdRsMCHit]
  //    - mc-bbc-hits             "BbcMCHits"           TClonesArray[SpdBbcMCHit]
  //    - mc-aeg-hits             "AegMCHits"           TClonesArray[SpdAegMCHit]
  //    - mc-zdc-hits             "ZdcMCHits"           TClonesArray[SpdZdcMCHit]
  //    - mc-tracks               "MCTracks"            TClonesArray[SpdTrackMC(+SpdTrackFitPar)]
  //    - rc-vertices             "RCVertices"          TClonesArray[SpdVertexRC(+SpdVertexFitPar)]
  //    - rc-ecal-clusters        "RCEcalClusters"      TClonesArray[SpdEcalRCCluster]
  //    - rc-ecal-particles       "RCEcalParticles"     TClonesArray[SpdEcalRCParticle]
  //    - mc-ecal-clusters-info   "MCEcalClustersInfo"  TClonesArray[SpdEcalClusterMCInfo]
  //    - mc-ecal-particles       "MCEcalParticles"     TClonesArray[SpdEcalMCParticle]
  //    - mc-rs-clusters          "MCRsClusters"        TClonesArray[SpdRsMCCluster]
  //    - mc-rs-particles         "MCRsParticles"       TClonesArray[SpdRsMCParticle]
  //    - ts-particles            "TsParticles"         TClonesArray[SpdTsParticle] 
  //    - tof-particles           "TofParticles"        TClonesArray[SpdTofParticle]
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
  // [TIME-OF-FLIGHT (TOF) SYSTEM (BARREL+CAPS) HIT PRODUCER]
  // Input:  mc-event, mc-particles, 
  //         tof-barrel-points and/or tof-endcaps-points
  // Output: mc-tof-hits
   
  SpdTofMCHitProducer* tof_hits_producer = new SpdTofMCHitProducer();
 
  //tof_hits_producer->SaveHits(false);  // default: true
  
  tof_hits_producer->SetVerboseLevel(1);
   
  Run->AddTask(tof_hits_producer);
  
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
  
  //rs_hits_producer->MakeStripHits(true);
  
  //rs_hits_producer->SaveHits(false);  // default: true
  
  rs_hits_producer->SetVerboseLevel(1);
   
  Run->AddTask(rs_hits_producer);
  
  //--------------------------------------------------------//
  // [BEAM_BEAM COUNTER HIT PRODUCER]
  // Input: mc-event, mc-particles, bbc-points
  // Output: mc-bbc-hits
  
  SpdBbcMCHitProducer* bbc_hits_producer = new SpdBbcMCHitProducer();
 
  //bbc_hits_producer->SaveHits(false); // default: true
  
  bbc_hits_producer->SetVerboseLevel(1);
  
  Run->AddTask(bbc_hits_producer);
  
  //--------------------------------------------------------//
  // [AEROGEL HIT PRODUCER]
  // Input: mc-event, mc-particles, aeg-points
  // Output: mc-aeg-hits
  
  SpdAegMCHitProducer* aeg_hits_producer = new SpdAegMCHitProducer();
 
  //aeg_hits_producer->SaveHits(false); // default: true
  
  aeg_hits_producer->SetVerboseLevel(1);
  
  Run->AddTask(aeg_hits_producer);
  
  //--------------------------------------------------------//
  // [ZERO-DEGREE CALORIMETER HIT PRODUCER]
  // Input: mc-event, mc-particles, zdc-points
  // Output: mc-zdc-hits
  
  SpdZdcMCHitProducer* zdc_hits_producer = new SpdZdcMCHitProducer();
 
  //zdc_hits_producer->SaveHits(false); // default: true
  
  zdc_hits_producer->SetVerboseLevel(1);
  
  Run->AddTask(zdc_hits_producer);
  
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

  vtxs_finder->SetFitSecondaries(false);   // FIT PRIMARY VERTECES ONLY
  
  vtxs_finder->SetVerboseLevel(1);

  Run->AddTask(vtxs_finder);

  // You may use this code to reconstruct vertices based on MC-truth as "PID".
  // See K0 decay for and example standalone use of KFParticle (to be used for
  // reaslistic PID)
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
 /* 
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
  
  Run->AddTask(v0_finder);
*/
// **************************************************************************************************************
// **************************************************************************************************************
  
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
  
  //--------------------------------------------------------//
  // [MC-TRUTH FOR ELECTROMAGNETIC CALORIMETER CLUSTERS]
  // Input: mc-events, mc-particles, 
  //        mc-ecal-hits, rc-ecal-clusters
  // Output: mc-ecal-clusters-info, mc-ecal-particles
  
  SpdEcalClusterMCInfoMaker* ecal_mc_info = new SpdEcalClusterMCInfoMaker();
  
  //ecal_mc_info->SaveInfo(false);           // default = true
  //ecal_mc_info->SaveEcalParticles(false);  // default = true
  
  //ecal_mc_info->CreateEcalParticles(false); // default = true
  //ecal_mc_info->RemoveHitMCTruth(true);     // default = false
  
  ecal_mc_info->SetVerboseLevel(1);
  
  Run->AddTask(ecal_mc_info);
  
  //--------------------------------------------------------//
  // [MC-CLUSTERING AND MC-RECONSTRUCTION FOR RANGE SYSTEM]
  // Input: mc-event, mc-particles, 
  //        mc-rs-hits
  // Output: mc-rs-clusters, mc-rs-particles
  
  SpdRsMCClusterMaker* rs_cl = new SpdRsMCClusterMaker();
  
  //rs_cl->SaveClusters(false);     // default: true
  //rs_cl->SaveRsParticles(false);  // default: true
  
  //rs_cl->CreateRsParticles(false); // default = true
  //rs_cl->RemoveHitMCTruth(true);   // default = false
  
  rs_cl->SetVerboseLevel(1);
  
  Run->AddTask(rs_cl);
  
//   ///--------------------------------------------------------//
//   // [MC-PRODUCER FOR TS-PARTICLES]
//   // Input: mc-event, mc-particles, mc-tracks(+ fit pars.)
//   // Output: ts-particles
//   
//   SpdMCTsParticleProducer* mcts_part = new SpdMCTsParticleProducer();
// 
//   //mcts_part->SaveParticles(false);  
//   
//   mcts_part->SetVerboseLevel(1);
// 
//   Run->AddTask(mcts_part);
//   
//   ///--------------------------------------------------------//
//   // [MC-PRODUCER FOR TOF-PARTICLES]
//   // Input: mc-event, mc-particles, mc-tracks(+ fit pars.), mc-tof-hits
//   // Output: tof-particles
//   
//   SpdMCTofParticleProducer* mctof_part = new SpdMCTofParticleProducer();
// 
//   //mctof_part->SaveParticles(false);  
//   
//   mctof_part->SetVerboseLevel(1);
// 
//   Run->AddTask(mctof_part);
  
  //========================================================================// 


   SpdMCAegParticleProducer* mcaeg_part = new SpdMCAegParticleProducer();
// 
   //mctof_part->SaveParticles(false);  
   
   mcaeg_part->SetVerboseLevel(1);
// 
   Run->AddTask(mcaeg_part);
  

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


