
void restore_field();
void restore_primgen();
void restore_modules(); 

TString inParFile;
TFile* fp;

//________________________________________________________________________________
void RestoreInput(TString stype = "") 
{
 // Input data & parameters files
       if (stype.IsWhitespace()) inParFile = "params.root";
  else if (stype == "tor" || stype == "hyb") inParFile = "params_tor.root";
  else if (stype == "sol") inParFile = "params_sol.root";

  else {
      cout << "<CheckOutputData> Unknown data type: " << stype << endl;
      return;
  }
  
  fp = new TFile(inParFile);
  if (!fp) return;
  
  //fp->ls();
  
  //restore_field();
  restore_primgen();
  //restore_modules();
}

//________________________________________________________________________________
void restore_field() 
{
  /* RESTORE FIELD FROM THE SET OF PARAMETERS */
  
  if (!fp) return;
  
  // load set of field parameters
  SpdFieldPar* pars = (SpdFieldPar*)fp->Get("SpdFieldPar");
  if (!pars) return;
  
  pars->printParams();  // print new set of field parameters
  
  // create field 
  SpdFieldCreator* creator = new SpdFieldCreator();
  SpdFieldMap* field = (SpdFieldMap*)creator->createFairField(pars);
   
  field->Print(); // print field info
  
  // create NEW set of field parameters
  SpdFieldPar* new_pars = new SpdFieldPar();
  
  // fill parameters
  field->FillParContainer(new_pars);
   
  //pars->printParams(); // print new set of field parameters 
}

//________________________________________________________________________________
void restore_primgen() 
{
  /* RESTORE PRIMARY GENERATOR & DECAYER FROM THE SET OF PARAMETERS */
  
  // load set of primary generator parameters from file
  SpdPrimGenParSet *gen_pars = (SpdPrimGenParSet*)fp->Get("PrimGenParSet");
  if (!gen_pars) return;
  
  //gen_pars->print(1); // print set of generator parameters 
  
  // search for DECAYER & load decayer parameters
  Int_t dstorage_index(-1000);
  SpdDecayer* decayer = 0;
  if (gen_pars->GetParameter("DecayerStorageIndex",dstorage_index,0))
  {
      decayer = new SpdDecayer(dstorage_index);
      decayer->LoadParsFrom(gen_pars);
  }
  
  // create PRIMARY GENERATOR
  SpdPrimaryGenerator* primgen = new SpdPrimaryGenerator("","");
  
  // load primary generator(s) parameters
  primgen->LoadParsFrom(gen_pars);
  
  //primgen->Print(""); // print primary generator info before initialization
    
        Int_t gen_index = 2;
        SpdGenerator* gen = primgen->Generator(gen_index);
        
        /* ++++++ FIRST METHOD: LOAD & INIT PYTHIA PARAMETERS DIRECTLY ++++++ */
        
        if (gen && TString("SpdPythia6Generator") == gen->ClassName()) {       
            SpdPythia6* pythia6 = ((SpdPythia6Generator*)gen)->GetGenerator();
            pythia6->SetStorageIndex(gen_index);
            pythia6->LoadParsFrom(gen_pars); // load parameters & init Pythia6 
        }
        
        if (gen && TString("SpdPythia8Generator") == gen->ClassName()) {       
            SpdPythia8* pythia8 = ((SpdPythia8Generator*)gen)->GetGenerator();
            pythia8->SetStorageIndex(gen_index);
            pythia8->LoadParsFrom(gen_pars); // load parameters & init Pythia8 
        }
        
        /* ++++++ SECOND METHOD: LOAD & INIT PYTHIA PARAMETERS VIA PRIMARY GENERATOR ++++++ */
        
        if (gen && TString("SpdPythia6Generator") == gen->ClassName()) {    
            primgen->LoadPythia6Parameters(gen_pars);
        }
        
        if (gen && TString("SpdPythia8Generator") == gen->ClassName()) {    
            primgen->LoadPythia8Parameters(gen_pars);
        }
       
   
  // initialize PRIMARY GENERATOR and DECAYER
  Bool_t init = primgen->Init();
  
  if (!init) cout << "<restore_primgen> Initaialization: Failed " << endl;
  else cout << "<restore_primgen> Initaialization: Ok " << endl;
  
  primgen->Print(""); // print primary generator info after initialization
  
  // create new set of primary generator parameters
  SpdPrimGenParSet* gen_pars_new = new SpdPrimGenParSet("SpdPrimGenParSet",
                                                        "NEW PRIMARY GENERATOR PARAMETERS");
  //fill primary generator parameters
  primgen->FillParsIn(gen_pars_new);
  
  //fill decayer generator parameters
  if (decayer) decayer->FillParsIn(gen_pars_new);
  
  delete primgen;
  delete decayer;
 
  //gen_pars_new->print(1); // print new set of  generator parameters 
}

//________________________________________________________________________________
void restore_modules() 
{
  /* RESTORE LIST OF MODULES FROM THE SET OF PARAMETERS */
   
  // load materials and create geometry top level (cave)
  SpdCommonGeoMapper::Instance()->OpenGeometry();
  
  SpdPassiveGeoParSet* paspars = (SpdPassiveGeoParSet*)fp->Get("PassiveGeoParSet");
   
  SpdCave* Cave = new SpdCave();
  Cave->LoadParsFrom(paspars);
  Cave->ConstructGeometry();

  // load full set of modules (passive and active)
  FairBaseParSet* geoset = (FairBaseParSet*)fp->Get("FairBaseParSet");
  
  //SpdCommonGeoMapper::Instance()->ConstructGeometry(); // !construct default geometry!
  //return;
  
  // get list of passives and construct each passive module
  Int_t nmod;
  SpdPassiveModule** mod = SpdCommonGeoMapper::Instance()->GetListOfPassives(nmod);
  for (Int_t i(0); i<nmod; i++) {
       mod[i]->ConstructGeometry();
       //mod[i]->Print("");
  }
  
  // get list of detectors, load parameters and construct each detector
  Int_t ndet;
  SpdDetector** det = SpdCommonGeoMapper::Instance()->GetListOfDetectors(ndet);
  
  SpdParSet* pars;
  for (Int_t i(0); i<ndet; i++) {
  
       pars = (SpdParSet*)fp->Get(det[i]->GetParametersType());
             
       if (!pars) continue;   
       
       det[i]->LoadParsFrom(pars);
       
       //pars->print(1);
       
       //if (det[i]->IsMapperDefined()) 
           det[i]->ConstructGeometry();
     
       //det[i]->Print("");
  }
  
  //SpdCommonGeoMapper::Instance()->ConstructGeometry(); // !construct parametric geometry!
  
  //return;
 
  /* DRAW GEOMETRY */
   
  TGeoManager *geoMan = gGeoManager;
  
  if (geoMan && geoMan->GetMasterVolume()) {    
      geoMan->SetVisLevel(2);
      geoMan->GetMasterVolume()->Draw("ogl");
      //geoMan->GetMasterVolume()->Draw();
  }

}


   
