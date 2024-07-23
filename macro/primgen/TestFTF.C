
void createfile_SpdFTF(Int_t nevents, Double_t Epar /*GeV*/);

void generate_SpdFTF_readmode(Int_t nevents);
void generate_SpdFTF_standard(Int_t nevents, Double_t Epar /*GeV*/);

void PrintEvents(TString fname = "FTF.root");

//______________________________________________________________
void TestFTF(Int_t nevents = 5) 
{
    //createfile_SpdFTF(15,10.);
  
    //generate_SpdFTF_readmode(nevents);
    
    generate_SpdFTF_standard(nevents,10.);
}

//______________________________________________________________
void createfile_SpdFTF(Int_t nevents, Double_t Epar)
{
  // OUTPUT data file name
  TString outputfname = gSystem->Getenv("VMCWORKDIR");
  outputfname += "/input";
  outputfname += "/FTF.root";
  
  // create the generator in the "standard" mode
  SpdFtfGenerator* ftfgen = new SpdFtfGenerator(0 /*mode*/);
      
  // make settings
  ftfgen->SetSeed(0 /*seed*/, 10 /*events_to_skip+1*/); // generator point
  
  ftfgen->SetConfig("PP.mac");                        // config file name, Epar will be treated as sqrt(s) [GeV]
  //ftfgen->SetConfig("PbarP.mac");                       // config file name, Epar will be treated as Plab [GeV/c]
  
  ftfgen->SetFilePath(outputfname);                     // set output data file
  
  ftfgen->SetRemoveDataFile(kFALSE);  // don't delete output file after the generation
  
  // initialize generator
  ftfgen->Initialize(nevents,Epar);   // number of events, [moment in Lab. CS, GeV/c /OR/ sqrt(s) per 2 hadrons, GeV]
  
  if (!ftfgen->IsInit()) return;
  
  //----------------------------------
  // generate events
  //----------------------------------
  
  for (Int_t i(0); i<nevents; i++) ftfgen->ReadEvent(0);
 
  ftfgen->Print("");
  
  delete ftfgen;
}

//______________________________________________________________
void generate_SpdFTF_readmode(Int_t nevents)
{
  // INPUT data file name
  TString inputfname = gSystem->Getenv("VMCWORKDIR");
  inputfname += "/input";
  inputfname += "/FTF.root";
  
  // create the generator in the "read" mode (= -1)
  SpdFtfGenerator* ftfgen = new SpdFtfGenerator(-1 /*mode*/);
      
  // make settings
  ftfgen->SetSeed(0 /*any number*/, 0 /*events_to_skip+1*/); // generator point
  ftfgen->SetFilePath(inputfname);                           // input data file
  
  // initialize generator
  ftfgen->Initialize(nevents);   // number of events
  
  if (!ftfgen->IsInit()) return;
  
  //----------------------------------
  // generate events
  //----------------------------------
  
  ftfgen->ReadEvent(0);
  return;
  
  SpdPrimGenData* gendata = new SpdPrimGenData(ftfgen->ClassName());
  ftfgen->SetGenData(gendata);
  
  for (Int_t i(0); i<nevents; i++) {
       ftfgen->ReadEvent(0);
       gendata->Print("read mode");
       ftfgen->RemoveGenData();
  }
  
  ftfgen->Print("");
  
  //PrintEvents(inputfname);
} 

//______________________________________________________________
void generate_SpdFTF_standard(Int_t nevents, Double_t Epar)
{
  // create the generator in the "standard" mode (= 0)
  SpdFtfGenerator* ftfgen = new SpdFtfGenerator(0 /*mode*/);
  
  // make settings  
  ftfgen->SetSeed(0 /*seed*/, 0 /*events_to_skip+1*/); // generator point
  
  ftfgen->SetConfig("PP.mac");                       // config file name, Epar will be treated as sqrt(s) [GeV]
  //ftfgen->SetConfig("PbarP.mac");                      // config file name, Epar will be treated as Plab [GeV/c]
    
  // initialize generator
  ftfgen->Initialize(nevents,Epar); // number of events, [moment in Lab. CS, GeV/c /OR/ sqrt(s) per 2 hadrons, GeV]
  
  if (!ftfgen->IsInit()) return;
      
  //----------------------------------
  // generate events
  //----------------------------------
  
  if (!ftfgen->IsInit()) return;
  
  SpdPrimGenData* gendata = new SpdPrimGenData(ftfgen->ClassName());
  ftfgen->SetGenData(gendata);
  
  for (Int_t i(0); i<nevents; i++) {
       ftfgen->ReadEvent(0);
       gendata->Print("standard mode");
       ftfgen->RemoveGenData();
  }
  
  ftfgen->Print("");
  
  delete ftfgen;
} 

//______________________________________________________________
void PrintEvents(TString FileName)
{
  if (gSystem->AccessPathName(FileName)) 
  {  
      cout << "-I- <PrintEvents> No such file: " << FileName << endl;
      return;
  }
  cout << "-I- <PrintEvents> Input file: " << FileName << endl;
  
  TFile* InputFile = new TFile(FileName);
  TTree* InputTree = (TTree*)InputFile->Get("data");
  
  if (!InputTree) return;
  
  TClonesArray* Particles = 0;
  InputTree->SetBranchAddress("Particles",&Particles);
  
  int entries = InputTree->GetEntries();
  
  cout << "Total number of EVENTS in the TREE = " << entries << endl;    
  if (entries == 0) return;
  
  Int_t np = 0;
  TParticle* p;
  
  for (Int_t i(0); i < entries; i++) {
    
       InputTree->GetEntry(i);
       np = Particles->GetEntriesFast();
    
       cout << "Event: " << i << ",  particles = " << np << endl;
       
       for (Int_t j(0); j < np; j++) {
         
            p = (TParticle*)Particles->At(j);
            if (!p) continue;
              
            printf("\t%-5d %2d/%-5d  %12d %14.6f %14.6f %14.6f %10.3e %10.3e %10.3e %10.3e\n",
                   j, p->GetFirstMother(), p->GetSecondMother(), 
                   p->GetPdgCode(), 
                   p->Px(), p->Py(), p->Pz(), 
                   p->Vx(), p->Vy(), p->Vz(), p->T()); 
       }    
  }
  
  InputFile->Close();
}

