
//_______________________________________________________________________________
void DisplayTorEvent(Int_t nevent = 0)
{
  TString  InputFile  = "run_tor.root";
  TString  ParFile    = "params_tor.root";
  TString  OutFile    = "draw_tor.root";
  
  if (gSystem->AccessPathName(InputFile)) {
      cout << "No input data-file: " <<  InputFile << endl;
      return;
  }
  
  if (gSystem->AccessPathName(ParFile)) {
      cout << "No input pars-file: " <<  ParFile << endl;
      return;
  }
 
  SpdRunAna* Run = new SpdRunAna();
  
  Run->SetInputFile(InputFile.Data());
  Run->SetOutputFile(OutFile.Data());

  FairRuntimeDb* rtdb = Run->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);
  
  SpdEventManager* Man = new SpdEventManager(2);
  
  SpdMCTracks* Tracks = new SpdMCTracks("GeoTracks");
  
  Man->AddTask(Tracks);
  Man->Init();
  
  Man->GotoEvent(nevent);
}
