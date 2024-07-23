
void reco_emc(){
  //  
  Int_t nEvents = 0; 
  
  TString inFile  = "test.root";       // Input file (MC events)
  TString parFile = "params.root";     // Parameter file
  TString outFile = "emc_hits.root";   // Output file
 
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput = new FairParRootFileIo();
  parInput->open(parFile.Data());
  rtdb->setFirstInput(parInput);
    
  TStopwatch timer;
  timer.Start();
 
  MpdEmcHitProducerA* emcHitProducer = new MpdEmcHitProducerA();
  fRun->AddTask(emcHitProducer);
  
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
  
  if (!emcHitProducer->IsTested()) return;
  
  TH1D* h0 = emcHitProducer->fHgtheta0;
  h0->SetFillColor(kBlack);
  TH1D* h1 = emcHitProducer->fHgtheta1;
  h1->SetFillColor(kRed);
  
  h0->GetXaxis()->SetTitle("#theta(#gamma_{#pi_{0}}), deg");
  h0->GetYaxis()->SetTitle("Entries");
  h0->SetMinimum(0);
  h0->Draw();
  h1->Draw("same");
  
  gStyle->SetOptStat("ermuo");
  
  
  
}


