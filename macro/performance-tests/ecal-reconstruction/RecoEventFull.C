

//______________________________________________________________________________
void RecoEventFull(TString inFile = "run.root", TString outFile = "reco.root", TString parFile = "params.root")
{

  Int_t nEvents = 0; 
  
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
//  event_maker->SetParticleMaxGeneration(2);
//  event_maker->SetVertexMaxGeneration(1);
  //event_maker->SaveEvent(false);     // default: true
  //event_maker->SaveParticles(false); // default: true
  //event_maker->SaveVertices(false);  // default: true
  event_maker->SetVerboseLevel(1);
  Run->AddTask(event_maker);
  
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
  // [REAL ECAL CLUSTERIZATION AND 
  //            CLUSTER PARAMETERS RECONSTRUCTION]
  // Input: mc-event, mc-ecal-hits
  // Output: rc-ecal-clusters, rc-ecal-particles
  
  SpdEcalRCMaker* ecal_rc = new SpdEcalRCMaker();
  //ecal_rc->SaveClusters(false);
  //ecal_rc->SaveParticles(false);  
  ecal_rc->SetVerboseLevel(1);
  //ecal_rc->SetClusterMinDist(recopars.minDist);
  //ecal_rc->SetCellEnergyThreshold(recopars.cellThreshold);
  //ecal_rc->SetReconstructionOption(recopars.recoOption);
  Run->AddTask(ecal_rc);
  

  //--------------------------------------------------------//
  // [MC-TRUTH FOR ELECTROMAGNETIC CALORIMETER CLUSTERS]
  // Input: mc-events, mc-particles, 
  //        mc-ecal-hits, rc-ecal-clusters
  // Output: mc-ecal-clusters-info, mc-ecal-particles
  
  SpdEcalClusterMCInfoMaker* ecal_mc_info = new SpdEcalClusterMCInfoMaker();
  //ecal_mc_info->CreateEcalParticles(false); // default = true
  //ecal_mc_info->RemoveHitMCTruth(true);     // default = false
  ecal_mc_info->SetVerboseLevel(1);
  Run->AddTask(ecal_mc_info);

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
}
