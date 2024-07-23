
//______________________________________________________________________________
void anrun_ecalt(){
  //  
  Int_t nEvents = 0; 
  
  TString inFile  = "run_tor.root";       // Input file (MC events)
  TString parFile = "params_tor.root";    // Parameter file
  TString outFile = "ecalt_an.root";      // Output file
  
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
  
  SpdEcalTPointAn* Analyzer = new SpdEcalTPointAn();  
  
  //Analyzer->SetParsNameB("EcalTBParSet");
  //Analyzer->SetParsNameEC("EcalTECParSet");
  
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
