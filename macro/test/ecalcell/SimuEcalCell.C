

// ------------------------------------------------------------------------  
//  ===== STANDARD ION TABLE (Z, A) =====
//
//      H  :  1,  1       Si : 14, 28
//      He :  2,  4       P  : 15, 31
//      Li :  3,  7       S  : 16, 32
//      Be :  4,  9       Cl : 17, 35 
//      B  :  5, 11       Ar : 18, 40
//      C  :  6, 12       K  : 19, 39
//      N  :  7, 14       Ca : 20, 40
//      O  :  8, 16       Sc : 21, 45
//      F  :  9, 19       Ti : 22, 48
//      Ne : 10, 20        V : 23, 51
//      Na : 11, 23       Cr : 24, 52
//      Mg : 12, 24       Mn : 25, 55
//      Al : 13, 27       Fe : 26, 56
// ------------------------------------------------------------------------

int  GetPdgCode(const int Z, const int A);
void AddIon(const int pdg);

void SimuEcalCell(int nEvents = 10)
{
  //return;
  
  // -----   Generator & Geometry parameters   ------------------------------

  // --- set momentum 
 
  double mom  =  100;  // GeV/c
  
  // --- set PDG-number: proton (2212), gamma (22), e-(11), ...
  
  double ppdg;  
  
  //ppdg = 2212;
  ppdg = 11;
  //ppdg = GetPdgCode(6,12);
 
  // --- set start point
  
  double vz = -1000; // cm
  
  // ------------------------------------------------------------------------
   
  TString mcEngine = "TGeant4";
  
  //TString outDir = "../output/";
  TString outDir = "";
    
  TString outFile = outDir + "test_data.root";     // Output data file name
  TString outPars = outDir + "test_params.root";   // Output parameters file name


  // -----   Create simulation run   ----------------------------------------
  FairRunSim* run = new FairRunSim();
  
  run->SetName(mcEngine);               // Transport engine
  run->SetOutputFile(outFile);          // Output file
  
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  
  // -----   Set media   ----------------------------------------------------
  run->SetMaterials("media.geo");       // Materials
 
  
  
  // -----   Set Event Header   ---------------------------------------------
  run->SetMCEventHeader(new EcalCellEventHeader);
  
    
  // -----   Create geometry   ----------------------------------------------
  
  FairModule* cave = new SpdCave("CAVE");
  //cave->SetGeometryFileName("cave.geo");
  cave->SetGeometryFileName("cave_precise.geo");
  run->AddModule(cave);
  
  EcalCell* cell = new EcalCell("ECAL_CELL",kTRUE);
  
  //cell->SetGeometryType(2); //0: default; 1,2
  
  cell->SetNlayers(100);
  cell->SetNXYPads(20);
  cell->SetLayerSizeXY(100); // cm
  cell->SetLayer1SizeZ(0.5); // cm, lead
  cell->SetLayer2SizeZ(4.5); // cm, scintilator 

  run->AddModule(cell);
   
  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

 
  // -----   Add generator  -------------------------------------------------
  
  FairParticleGenerator* gen = new FairParticleGenerator();
  
  AddIon(ppdg);
  
  gen->SetPDGType(ppdg);
  gen->SetMultiplicity(1); 
  gen->SetMomentum(0, 0, mom);
  gen->SetVertex(0, 0, vz);
  
  primGen->AddGenerator(gen);
    
  run->SetGenerator(primGen);

  // -------------  Save tracks ---------------------------------------------
  // Store the visualization info of the tracks, 
  // this make the output file very large!!
  // Use it only to display but not for production!
    
  run->SetStoreTraj(kTRUE);
  //run->SetStoreTraj(kFALSE);  
  
  // -----   Initialize simulation run   ------------------------------------
  run->Init();
  
  // -----   Save parameters   ----------------------------------------------

  Bool_t MergePars = kFALSE;
  FairParRootFileIo* parOut = new FairParRootFileIo(MergePars);
  if (MergePars) parOut->open(outPars.Data());
  else parOut->open(outPars.Data(),"RECREATE");
   
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print(); 
   
  // -----   Start run   ----------------------------------------------------
  
  TStopwatch timer;
  timer.Start();
  
  run->Run(nEvents);
  
  timer.Stop();  
  
  // export ROOT geometry on a separate file
  
  //TString geoFile = outDir + "geofile_full.root"; 
  //run->CreateGeometryFile(geoFile);
  
  
  // -----   Finish   -------------------------------------------------------
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Real time " << rtime << " s (" << rtime/3600. << "h), " 
       << "CPU time "  << ctime << " s (" << ctime/3600. << "h)  "
       << endl << endl;
  
  gApplication->Terminate();
}

//_____________________________________________________________________________    
int GetPdgCode(const int Z, const int A) 
{ 
  if (Z == 1 && A == 1) return 2212;    
  return 1000000000 + Z*10000 + A*10; 
}

//_____________________________________________________________________________    
void AddIon(const int pdg) 
{ 
  if (pdg < 1000000000) return;
  
  FairRunSim* run = FairRunSim::Instance();
  if (!run) return;
  
  int Z = (pdg-1000000000)/10000; 
  int A = (pdg-1000000000-10000*Z)/10;
  
  FairIon* ion = new FairIon();
  ion->SetParams(Form("ION_%03d_%03d",Z,A), Z, A, Z);
  
  run->AddNewIon(ion);
}


