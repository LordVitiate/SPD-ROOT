


void cl_emc(){
  //
  Int_t nEvents = 0; 
  
  TString inFile  = "emc_hits.root";    // Input file (MC events)
  TString outFile = "emc_cl.root";      // Output file (MC events)
  TString parFile = "params.root";      // Parameter file
 
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput = new FairParRootFileIo();
  parInput->open(parFile.Data());
  rtdb->setFirstInput(parInput);
  
  TStopwatch timer;
  timer.Start();
 
  EmcClManager* clman = new EmcClManager();
  
  //---------------------------------------------------------------
  // parameters: 
  //
  // 1 - total number of pars. 
  // 2 - method number
  
  //---------------------------------------------------------------
  // METHOD = 1 :
  // 3 - energy threshold for each element deposit (MUST BE FLOAT VALUE!!!)
  // 4 - minimal number of elements in the cluster (MUST BE FLOAT VALUE!!!) 
  //---------------------------------------------------------------
  
  //clman->SetCLPars(3, 1, 0.001, 0.); 
  
  //---------------------------------------------------------------
  // METHOD = 2 :
  // 3 - energy threshold for each element deposit (MUST BE FLOAT VALUE!!!)
  // 4 - time threshold (ns) for each element (MUST BE FLOAT VALUE!!!)
  // 5 - minimal number of elements in the cluster (MUST BE FLOAT VALUE!!!) 
  //---------------------------------------------------------------
  
  clman->SetCLPars(4, 2, 0.001, -1., 1.); 
  
  fRun->AddTask(clman);

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
  
  //TH2D* h = clman->GetHitsMap();
  //if (h) h->Draw("colz");
  
  clman->SaveCLinfo("emc_clinfo.txt");
}


