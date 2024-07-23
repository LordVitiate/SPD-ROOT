
void run_ecal_digit(){
  //  
  Int_t nEvents = 0; 
  
  TString inFile  = "run.root";       // Input file (MC events)
  TString parFile = "params.root";     // Parameter file
  TString outFile = "ecal_hits.root";  // Output file
  
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput = new FairParRootFileIo();
  parInput->open(parFile.Data());
  rtdb->setFirstInput(parInput);
    
  TStopwatch timer;
  timer.Start();
 
  // 0: "all hit's marks (new version) ",  1 "pi0+eta hit's marks (old version)"
  SpdEcalHitProducer* ecalHitProducer = new SpdEcalHitProducer(0);  
  fRun->AddTask(ecalHitProducer);
  
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
}

