//______________________________________________________________________________
void RecoEventVertex_K0()
{
  Int_t nEvents = 0; 
  
  TString inFile  = "reco_base_K0.root";   // Input file 
  TString outFile = "reco_full2_K0.root";  // Output file 
  
  SpdRunAna* Run = new SpdRunAna();  
  
  Run->KeepParameters(false); // don't save tasks parameters and geometry (optionaly)!
  
  FairFileSource* SourceFile = new FairFileSource(inFile);
  Run->SetSource(SourceFile);
  
  SpdRootFileSink* sink = new SpdRootFileSink(outFile);
  
  sink->AddBranchToOutput("MCEvent.");
  sink->AddBranchToOutput("MCParticles");
  sink->AddBranchToOutput("MCVertices");
  sink->AddBranchToOutput("MCTracks"); 
  sink->AddBranchToOutput("RCVertices"); 
      
  //sink->AddBranchToOutput("all");
  
  Run->SetSink(sink);
  
  TStopwatch timer;
  timer.Start();
  
  //===============================================================================================//
  // [REAL SECONDARY VERTEX FINDER (FITTED MC-TRACKS)]
  // Input: mc-event, mc-particles, 
  //        mc-tracks + track-fit-parameters,
  //        primary rc-vertex + primary rc-vertex-fit-parameters
  // Output: secondary rc-vertices + secondary vertices-fit-parameters 
  //---------------------
  //
  // SetMinItsHits(Int_t)          - minimum number of hits in ITS for each track (default = 3) 
  // SetTrackSelOption(Int_t)      - if option: > 0 : hard cut for track selection will be used (default = 1)
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
  
  Run->AddTask(v0_finder);
  
  //========================================================================// 

  cout << "Start ... " << endl;
  
  Run->Initialize();
  
  //-------------------------------------//     
  cout << "Run ... " << endl;
  
  Run->Run(0,nEvents);
  //Run->Run(0,1);
  
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




