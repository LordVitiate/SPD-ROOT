

void RunTracking(Int_t stage = 1, Int_t nevents = 1)
{
  if (stage == 1) {  
      gInterpreter->LoadMacro("tracking/Simu_tr1.C");
      gInterpreter->ProcessLine(Form("Simu_tr1(%d)",nevents));
  }
  else if (stage == 2) {  
      gInterpreter->LoadMacro("tracking/MakeIdealTracks.C");
      gInterpreter->ProcessLine("MakeIdealTracks()");
  }
  else if (stage == 3) {  
      gInterpreter->LoadMacro("tracking/CheckIdealTracks.C");
      gInterpreter->ProcessLine("CheckIdealTracks()");
  }
  else if (stage == 4) {  
      gInterpreter->LoadMacro("tracking/FitIdealTracks.C");
      gInterpreter->ProcessLine("FitIdealTracks()");
  }
  else if (stage == 5) {  
      gInterpreter->LoadMacro("tracking/CheckFittedTracks.C");
      gInterpreter->ProcessLine("CheckFittedTracks()");
  }
}
