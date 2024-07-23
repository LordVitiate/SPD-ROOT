
void cl_ecal(){
  //
  Int_t nEvents = 0; 
  
  TString inFile  = "ecal_hits.root";    // Input file (MC events)
  TString outFile = "ecal_cl.root";      // Output file (MC events)
  TString parFile = "params.root";       // Parameter file
 
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput = new FairParRootFileIo();
  parInput->open(parFile.Data());
  rtdb->setFirstInput(parInput);
  
  TStopwatch timer;
  timer.Start();
 
  SpdEcalClManager* clman = new SpdEcalClManager();
  
  clman->SetClusteringMC(2);  // 0 = "no mc-clusters"; 1 = pi0+eta; 2 = all particles 
  
  // CHOOSE CLUSTERING (TRUE(1)/FALSE(0)); ECAL PARTS (BARREL, CAP Z+, CAP Z-)
  
  clman->SwitchCL(1,1,1); 
  
  // SAVE HISTOGRAMMS (TRUE(1)/FALSE(0));  ECAL PARTS (BARREL, CAP Z+, CAP Z-)
  
  clman->SwitchHistosCL(1,1,1);
    
  //---------------------------------------------------------------
  // parameters: 
  //
  //---------------------------------------------------------------
  // METHOD = 1 :
  // 1 - energy threshold for each element deposit 
  // 2 - minimal number of elements in the cluster 
  //---------------------------------------------------------------
  
  //clman->SetCLPars_1(0.001, 0); // -> FOR ALL PARTS (BARREL & CAPS) AT ONCE!
  
  //clman->SetTypeCLPars_1( 0, 0.001, 0); // BARREL (FIRST PARAMETER =  0)
  //clman->SetTypeCLPars_1(-1, 0.001, 0); // CAP Z- (FIRST PARAMETER = -1)
  //clman->SetTypeCLPars_1( 1, 0.001, 0); // CAP Z+ (FIRST PARAMETER =  1)
  
  //---------------------------------------------------------------
  // METHOD = 2 :
  // 1 - energy threshold for each element deposit 
  // 2 - time threshold (ns) for each element 
  // 3 - minimal number of elements in the cluster  
  //---------------------------------------------------------------
  
  //clman->SetCLPars_2(0.001, -1., 1.);   // -> FOR ALL PARTS (BARREL & CAPS) AT ONCE!
  
  clman->SetTypeCLPars_2( 0, 0.001, -1., 1);  // BARREL (FIRST PARAMETER =  0)
  clman->SetTypeCLPars_2(-1, 0.001, -1., 1);  // CAP Z- (FIRST PARAMETER = -1)
  clman->SetTypeCLPars_2( 1, 0.001, -1., 1);  // CAP Z+ (FIRST PARAMETER =  1)
  
  
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
  
  clman->SaveCLinfo("ecal_clinfo.txt");
}

