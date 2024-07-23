

void CreateFileWithParameters(Int_t method /*1 or 2*/, TString filename, Int_t index);

//______________________________________________________________
void TestParticleGenerator2(Int_t nevents = 1)
{
  Int_t Index = 16; //ATTENTION The (arbitrary) value must be > 1 !
   
  // create a file with parameters
  CreateFileWithParameters(1,"parsexample.root",Index);
  
  // create generator 
  SpdParticleGenerator* partgen = new SpdParticleGenerator();
  partgen->SetGenIndex(Index);  
   
  /* >>>>>>>>>>>> set parameters from by the list from the file <<<<<<<<<<<< */
  TFile* f = new TFile("parsexample.root","READ");
  
  SpdPrimGenParSet* pars = (SpdPrimGenParSet*)f->Get("testpars1");
  partgen->LoadParsFrom(pars);
  
  f->Close();
  /* >>>>>>>>>>>> ----------------------------------------------------------  <<<<<<<<<<<< */
  
  // initialize generator 
  partgen->Init();
   
  partgen->Print("");
  
  //return;
  
  //----------------------------------
  // generate events
  //----------------------------------
  
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  
  SpdPrimGenData* gendata = new SpdPrimGenData(partgen->ClassName());
  partgen->SetGenData(gendata);
      
  if (!partgen->IsInit()) return;
 
  for (Int_t i(0); i<nevents; i++) {
      
       partgen->ReadEvent(0);
       gendata->Print("test");
       partgen->RemoveGenData();
  }
  
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "\n";
  
  partgen->Print("");
  
  cout<<"----------------------------------------------------------------------"<<endl<<endl;  
}

//______________________________________________________________
void CreateFileWithParameters(Int_t method, TString filename, Int_t index)
{
   
  SpdPrimGenParSet* data = new SpdPrimGenParSet("testpars1");
   
  if (method == 1) 
  {
      SpdParticleGenerator* partgen = new SpdParticleGenerator();
  
      partgen->SetGenIndex(index); 
  
      partgen->SetPdg(2212);               /* pdg */
      partgen->SetNP(5);                   /* multiplicity (number of particles to generate) */
      partgen->SetMomentum(0,0,-5.);       /* momentum, GeV */     
      partgen->SetVertex(0.1,0.2,0.3);     /* vertex position, cm */  
  
      partgen->FillParsIn(data); // ! fill parameters list !
      
      delete partgen;
  }
  else if (method == 2) 
  {
      data->SetParameter("PDG",   2212,  index); /* pdg */
      data->SetParameter("NP",      10,  index); /* multiplicity (number of particles to generate) */
      data->SetParameter("Px",    100.,  index); /* momentum (px), GeV */   
      data->SetParameter("Py",    0.16,  index); /* momentum (py), GeV */   
      data->SetParameter("Pz",  -10.677, index); /* momentum (pz), GeV */   
      data->SetParameter("Time",   1.,   index); /* vertex time, seconds */   
  }
  
  data->print(1);
  
  // save parameters into the file
  TFile* f = new TFile(filename,"RECREATE");
  data->Write();
  f->Close();
  
  delete data;
  
}



