
//______________________________________________________________________________
void run_tst_digit(){
  //  
  Int_t nEvents = 0; 
  
  TString inFile  = "run_tor.root";       // Input file (MC events)
  TString parFile = "params_tor.root";    // Parameter file
  TString outFile = "tst.root";           // Output file
  
  SpdRunAna* Run = new SpdRunAna();
  
  //Run->LoadGeoParSet(kFALSE);
  
  Run->SetInputFile(inFile);
  Run->SetOutputFile(outFile);
  
  FairRuntimeDb* rtdb = Run->GetRuntimeDb();
  FairParRootFileIo* parInput = new FairParRootFileIo();
  parInput->open(parFile.Data());
  rtdb->setFirstInput(parInput);
  
  TStopwatch timer;
  timer.Start();
 
  //-------------------------------------//
 
  SpdTsTBHitProducer* TsTBHitProducer = new SpdTsTBHitProducer("TsTBParSet");  
  Run->AddTask(TsTBHitProducer);
  
  SpdTsTECHitProducer* TsTECHitProducer = new SpdTsTECHitProducer("TsTECParSet");  
  Run->AddTask(TsTECHitProducer);
  
  //return;
  
  cout << "Start ... " << endl;
  
  //Run->Initialize();
       
  Run->Init();
  Run->Run(0, nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  
  //HitProducer->PrintInput();
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  
//    /* DRAW GEOMETRY */
//    
//   TGeoManager *geoMan = gGeoManager;
//   
//   if (geoMan && geoMan->GetMasterVolume()) {    
//       geoMan->SetVisLevel(2);
//       geoMan->GetMasterVolume()->Draw("ogl");
//       //geoMan->GetMasterVolume()->Draw();
//   }
  
}
