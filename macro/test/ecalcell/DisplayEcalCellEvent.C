
//_______________________________________________________________________________
void DisplayEcalCellEvent(Int_t nevent = 0)
{
  TString  InputFile  = "test_data.root";
  TString  ParFile    = "test_params.root";
  TString  OutFile    = "test_draw.root";
  
  if (gSystem->AccessPathName(InputFile)) {
      cout << "No input data-file: " <<  InputFile << endl;
      return;
  }
  
  if (gSystem->AccessPathName(ParFile)) {
      cout << "No input pars-file: " <<  ParFile << endl;
      return;
  }
 
  SpdRunAna *fRun = new SpdRunAna();
  fRun->SetInputFile(InputFile.Data());
  fRun->SetOutputFile(OutFile.Data());

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);
  
  SpdEventManager* Man = new SpdEventManager(1);
  SpdMCTracks* Tracks  = new SpdMCTracks("GeoTracks");

  Man->AddTask(Tracks);
  
  Man->Init();    

  Man->GotoEvent(nevent);

}
