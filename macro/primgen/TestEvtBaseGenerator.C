

SpdGenerator* InitPythia6();
SpdGenerator* InitFTF(Int_t nevents, Double_t Elab = 25 /*GeV*/);
SpdGenerator* InitUrqmd(TString data);

//______________________________________________________________
void TestEvtBaseGenerator(Int_t ngevents = 5, Int_t nevents = 10)
{
   SpdGenerator* ingen = 0;
   
   ingen = InitPythia6();
   //ingen = InitFTF(nevents);
   
   //ingen = InitUrqmd("../input/AuAu_ecm9gev_hydroON_EoS1PT_mbias_1000ev_10.f13.gz");
   
   if (!ingen) return;
   
   //ingen->SetVerboseLevel(-10);
   
   ingen->CreateAsciiFile(nevents,"EvtBaseEvents");
   //ingen->CreateRootFile(nevents,""EvtBaseEvents");
   
   //ingen->CreateRootFile(1,"UrqmdEvents");
   //ingen->CreateAsciiFile(1,"UrqmdEvents");
   
   return;
   
   //----------------------------------------------------  
  
   SpdEvtBaseGenerator* gen = new SpdEvtBaseGenerator(); 
   
   //gen->SetFile("EvtBaseEvents.root");
   gen->SetFile("EvtBaseEvents.txt");
   
   gen->Initialize(ngevents,5); // number of events (generate,skip)
   
   //gen->Print("");
   
   if (!gen->IsInit()) return;
  
   //----------------------------------
   // generate events
   //----------------------------------
  
   SpdPrimGenData* gendata = new SpdPrimGenData(gen->ClassName());
   gen->SetGenData(gendata);
    
   for (Int_t i(0); i<ngevents; i++) {
        gen->ReadEvent(0);
        cout << "\n>>>>>>>>>>>>>>>>>>>>> Event " << i << "\n\n";
        gendata->Print("evtbase");
        gen->RemoveGenData();
   }

   gen->Print("");
}

//______________________________________________________________
SpdGenerator* InitPythia6() 
{ 
   SpdPythia6Generator* P6gen = new SpdPythia6Generator("pythia6 (1)");
   
   P6gen->SetVGenOpt(0); // if opt = 0 particles don't decay in the vertex, 
                         // so external decayer is enable automatically
   P6gen->SetSeed(0);    // set seed
  
   SpdPythia6* gg_1 = (SpdPythia6*)P6gen->GetGenerator();  
   gg_1->SetMSEL(1);    // set miminum bias 

   P6gen->Initialize("cms","p","p",26/*GeV*/);
   
   P6gen->Print("");
   
   if (!P6gen->IsInit()) return 0;
   
   return P6gen;
}

//______________________________________________________________
SpdGenerator* InitFTF(Int_t nevents, Double_t Elab)
{
  // create the generator in the "standard" mode (= 0)
  SpdFtfGenerator* ftfgen = new SpdFtfGenerator(0 /*mode*/);
  
  // make settings  
  ftfgen->SetSeed(0 /*seed*/, 0 /*events_to_skip+1*/); // generator's start point
  ftfgen->SetConfig("PbarP.mac");                      // config file name
    
  // initialize generator
  ftfgen->Initialize(nevents,Elab); // number of events, energy in Lab. CS
  
  if (!ftfgen->IsInit()) return 0;
  
  return ftfgen;      
}
//______________________________________________________________
SpdGenerator* InitUrqmd(TString data)
{
   SpdUrqmdGenerator* gen = new SpdUrqmdGenerator("../input/AuAu_ecm9gev_hydroON_EoS1PT_mbias_1000ev_10.f13.gz");
    
   gen->SetVerboseLevel(2); // 0 ("minimal" printing) or 1
   gen->SetKeepEvent(kFALSE); 
   
   return gen;
}

