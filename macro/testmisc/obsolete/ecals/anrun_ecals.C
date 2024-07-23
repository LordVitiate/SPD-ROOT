
//______________________________________________________________________________
void anrun_ecals(){
  //  
  Int_t nEvents = 0; 
  
  TString inFile  = "run_sol.root";       // Input file (MC events)
  TString parFile = "params_sol.root";    // Parameter file
  TString outFile = "ecals_an.root";      // Output file
  
  SpdRunAna* Run = new SpdRunAna();
  
  Run->SetInputFile(inFile);
  Run->SetOutputFile(outFile);
  
  FairRuntimeDb* rtdb = Run->GetRuntimeDb();
  FairParRootFileIo* parInput = new FairParRootFileIo();
  parInput->open(parFile.Data());
  rtdb->setFirstInput(parInput);
  
  TStopwatch timer;
  timer.Start();
 
  //-------------------------------------//
  
  SpdEcalSPointAn* Analyzer = new SpdEcalSPointAn(); 
  
  //Analyzer->SetParsNameB("EcalSBParSet");
  //Analyzer->SetParsNameEC("EcalSECParSet");
  
  Run->AddTask(Analyzer);
  
  cout << "Start ... " << endl;
  
       Run->Init();
       Run->Run(0, nEvents);
  
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
