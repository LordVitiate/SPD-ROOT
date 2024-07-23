//______________________________________________________________________________
void ReadDataSimple(TString inFile = "reco_full.root")
{
  Int_t nEvents  = 0; 
  
  SpdRunAna* Run = new SpdRunAna();
  Run->SetSource(new FairFileSource(inFile));
  
  TStopwatch timer;
  timer.Start();

  //-------------------------------------------------------------//

  SpdMCDataReaderSimple* event_reader = new SpdMCDataReaderSimple();
  
  //-------------------------------------------------------------//
    
    // default = true
    event_reader->PrintEvent(false);
    
    // option = 1, 2, (< 1 : nothing will be printed); default = 0
    event_reader->PrintParticles(-1);
    
    // option = 1, 2, (< 1 : nothing will be printed); default = 0
    event_reader->PrintVertices(-1);
    
    //-------------------------------------------------------------
    
    // option = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintItsHits(-2); 
    
    // option = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintTsHits(-2);
    
    // option = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintTofHits(-2);
    
    // option = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintEcalHits(-2); 
    
    // option = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintRsHits(-2);
    
    // option = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintBbcHits(-2);
    
    // option = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintAegHits(-2);
    
    // option = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintZdcHits(-2);
    
    //-------------------------------------------------------------
 
    // option = 1, 2, (< 1 : nothing will be printed); default = 0
    event_reader->PrintTracks(-1);
    
    // option = 1, 2, (< 1 : nothing will be printed); default = 0
    event_reader->PrintVerticesRC(-1);
    
    // option = 1, 2, (< 1 : nothing will be printed); default = 0
    event_reader->PrintTracksRC(-1);
    
    //-------------------------------------------------------------
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintEcalClustersRC(-2);
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintEcalParticlesRC(-2);
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintEcalClustersMCInfo(-2);
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintEcalParticlesMC(-2);
    
    //-------------------------------------------------------------
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintRsClustersMC(-2);
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintRsParticlesMC(-2);
    
    //-------------------------------------------------------------
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintTsParticles(-2);
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintTofParticles(-2);
    
  // opt = 2 (< 2 : nothing will be printed); default = 0
    event_reader->PrintAegParticles(-2);

  //-------------------------------------------------------------//
    
  Run->AddTask(event_reader);
  
  //-------------------------------------------------------------//
  
  cout << "Initialize ... " << endl;
  
  Run->Initialize();
       
  cout << "Run ... " << endl;
  
  Run->Run(0, nEvents);
  //Run->Run(0,1);
  
  //-------------------------------------//
  
  // !!!ATTENTION!!!
  event_reader->PrintParameters(0); 
  
  SpdSetParSet* pars = event_reader->GetParameters();
  if (pars) {
      SpdPrimGenParSet* pp = pars->GetPrimGenPars();  
      if (pp) pp->print(1);
  }
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  
  /* DRAW GEOMETRY */
   
  TGeoManager *geoMan = gGeoManager;
  
  if (geoMan && geoMan->GetMasterVolume()) {    
      geoMan->SetVisLevel(2);
      geoMan->GetMasterVolume()->Draw("ogl");
      //geoMan->GetMasterVolume()->Draw();
  }  
}

