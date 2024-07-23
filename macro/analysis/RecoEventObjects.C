

//______________________________________________________________________________
void RecoEventObjects() 
{
  Int_t nEvents = 0; 
  
  TString inFile  = "reco_base.root";   // Input file 
  TString outFile = "reco_full2.root";  // Output file 
  
  SpdRunAna* Run = new SpdRunAna();  
  
  FairFileSource* SourceFile = new FairFileSource(inFile);
  Run->SetSource(SourceFile);
  
  SpdRootFileSink* sink = new SpdRootFileSink(outFile);
  
  sink->AddBranchToOutput("MCEvent.");
  sink->AddBranchToOutput("MCParticles");
  sink->AddBranchToOutput("MCVertices");
  sink->AddBranchToOutput("ItsMCHits");
  sink->AddBranchToOutput("TsMCHits");
  sink->AddBranchToOutput("TofMCHits");
  sink->AddBranchToOutput("EcalMCHits");
  sink->AddBranchToOutput("RsMCHits");
  
  //sink->AddBranchToOutput("all");
  
  Run->SetSink(sink);
  
  TStopwatch timer;
  timer.Start();
  
  //===============================================================================================//
  // Input/output data branches.
  //------------------------------------------------------------------------------------------------- 
  //
  // Input [RECO], TREE NAME: "spdsim". 
  //
  //  (full data list):           DATA BRANCH NAME:     BRANCH OBJECT TYPE:   
  //
  //    - simu-event-header       "MCEventHeader."      FairMCEventHeader
  //    - mc-event                "MCEvent."            SpdMCEvent   
  //    - mc-particles            "MCParticles"         TClonesArray[SpdMCParticle]   
  //    - mc-vertices             "MCVertices"          TClonesArray[SpdVertexMC(+SpdVertexFitPar)]   
  //    - mc-its-hits             "ItsMCHits"           TClonesArray[SpdMCSiliconHit]   
  //    - mc-ts-hits              "TsMCHits"            TClonesArray[SpdMCStrawHit(1D,2D)]   
  //    - mc-tof-hits             "TofMCHits"           TClonesArray[SpdTofMCHit]
  //    - mc-ecal-hits            "EcalMCHits"          TClonesArray[SpdEcalMCHit]   
  //    - mc-rs-hits              "RsMCHits"            TClonesArray[SpdRsMCHit]   
  //
  //------------------------------------------------------------------------------------------------- 
  //
  // Output [RECO], TREE NAME: "spdsim". 
  //
  //  (full data list):           TASKS:
  //
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
  //    + all data branches that have been added from input 
  //      
  //  (full data list):           DATA BRANCH NAME:     BRANCH OBJECT TYPE:
  //
  //    - mc-tracks               "MCTracks"            TClonesArray[SpdTrackMC(+SpdTrackFitPar)]
  //    - rc-vertices             "RCVertices"          TClonesArray[SpdVertexRC(+SpdVertexFitPar)]
  //    - rc-ecal-clusters        "RCEcalClusters"      TClonesArray[SpdEcalRCCluster]
  //    - rc-ecal-particles       "RCEcalParticles"     TClonesArray[SpdEcalRCParticle]
  //    - mc-rs-clusters          "MCRsClusters"        TClonesArray[SpdRsMCCluster]
  //    - mc-rs-particles         "MCRsParticles"       TClonesArray[SpdRsMCParticle]
  //    - ts-particles            "TsParticles"         TClonesArray[SpdTsParticle] 
  //    - tof-particles           "TofParticles"        TClonesArray[SpdTofParticle]
  //
  //-------------------------------------------------------------------------------------------------
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
  
  cout << "Start ... " << endl;
  
  Run->Initialize();
  
  //-------------------------------------//     
  cout << "Run ... " << endl;
  
  Run->Run(0, nEvents);
  
  //Run->Run(0, 3);
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


