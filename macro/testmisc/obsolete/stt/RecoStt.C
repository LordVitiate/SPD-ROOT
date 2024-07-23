

void RecoStt(){
  //
  Int_t iVerbose = 1;
  
  TString inFile  = "run.root";   // Input file (MC events)
  TString parFile = "params.root"; // Parameter file
  
  TString outFile = "hits.root";   // Output file
 
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  
  Int_t nEvents = 5; 
    
  TStopwatch timer;
  timer.Start();
 
  SpdSttHitProducerIdeal* sttHitProducer = new SpdSttHitProducerIdeal();
  fRun->AddTask(sttHitProducer);
   
//   // ------------------------------------------------------------------------ 
//   // trackfinding 
//   SttSttTrackFinderIdeal* sttTrackFinder = new SttSttTrackFinderIdeal(iVerbose);
//   SttSttFindTracks* sttFindTracks = new SttSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
//   sttFindTracks->AddHitCollectionName("SpdSttHit", "SpdSttPoint");
//   fRun->AddTask(sttFindTracks);


  cout << "Start ... " << endl;
  
  fRun->Init();
  fRun->Run(0, nEvents);
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
}


