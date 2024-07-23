// #if !defined(__CINT__) || defined(__MAKECINT__)
// 
// #include "TString.h"
// #include "TStopwatch.h"
// #include "TROOT.h"
// #include "TSystem.h"
// #include "TMath.h"
// #include "TRint.h"
// 
// #include "FairRunSim.h"
// #include "FairRuntimeDb.h"
// #include "FairParRootFileIo.h"
// #include "FairTrajFilter.h"
// #include "FairPrimaryGenerator.h"
// #include "FairCave.h"
// #include "FairModule.h"
// 
// #include "SpdSttGeoMapper.h"
// #include "SpdMCEventHeader.h"
// #include "SpdConstField.h"
// #include "SpdCave.h"
// #include "SpdSts.h"
// #include "SpdStt.h"
// #include "SpdEcal.h"
// #include "MpdEmc.h"
// #include "SpdFieldMap.h"
// #include "Pythia6Generator.h"
// 
// #include <iostream>
// using namespace std;
// 
// #endif

//_________________________________________________________________________
void xsimu(Int_t nEvents = 1, TString mcEngine = "TGeant4")
{
   
  TString outFile = "run.root";    // Output file name
  TString parFile = "params.root"; // Parameter file name
  
  FairRunSim* run = new FairRunSim();
  
  run->SetName("TGeant4");     
  run->SetOutputFile(outFile);        
  run->SetMaterials("media.geo");      
 
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  
  run->SetMCEventHeader(new SpdMCEventHeader);
 
  /*--------------------------------------------------*/
  /* ++++++++++++++      GEOMETRY      ++++++++++++++ */
  /*--------------------------------------------------*/
  
  FairModule* cave = new SpdCave("CAVE");
  //cave->SetGeometryFileName("cave.geo");
  cave->SetGeometryFileName("cave_precise.geo");
  run->AddModule(cave);
  
  SpdBarrel* Barrel = new SpdBarrel();
  Barrel->SetBuildGeoOption("toroid");
  run->AddModule(Barrel);
  
  /* PIPE */
  
  SpdPassiveModule* pipe = new SpdPipe();
  pipe->UnsetMaterials(1);
  run->AddModule(pipe);
  
//   
//   /* STS  (vertex detector) */
//   FairDetector* sts = new SpdSts("STS",kTRUE);
//   sts->SetGeometryFileName("sts_v3.geo");
//   run->AddModule(sts);
//  
//   /* STT  (central tracker) */
// 
//   SpdSttGeoMapper* stt_mapper = SpdSttGeoMapper::Instance();
//   
//   stt_mapper->SetScale(10); 
//   stt_mapper->SetTubeStep(2); 
//   stt_mapper->SetTubeSections(6);
//   
//   stt_mapper->InitGeometry(1);
// 
//   FairModule* stt = new SpdStt("STT",kTRUE);
//   run->AddModule(stt); 
//   
//   /* Calorimeter EMC (central) + ECAL (lateral) */
//   
//   SpdEcalGeoMapper* ecal_mapper = SpdEcalGeoMapper::Instance();
//   ecal_mapper->InitGeometry(1);
//   
//   FairModule* emc = new MpdEmc();
//   emc->SetGeometryFileName("emc_tr_400_3.geo");
//   run->AddModule(emc);
//   
//   FairModule* ecal = new SpdEcal();
//   run->AddModule(ecal);
     
 
  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
  SpdRegion* reg = 0;
  
  SpdConstField* fMagField = new SpdConstField();  
  fMagField->SetField(0., 0. , 25); //  kG

  //SpdRadialField* fMagField = new SpdRadialField(); 
  //fMagField->SetFieldParameter(0,25);  // second parameter: B_max, kG
  
      /* define field region */
  
      reg = fMagField->CreateFieldRegion("box");
      reg->SetBoxRegion(-1000, 1000, -1000, 1000, -1000, 1000); // (X,Y,Z)_(min,max), cm
  
      //reg = fMagField->CreateFieldRegion("tube");
      //reg->SetTubeRegion(0, 1000, -1000, 1000); // (R,Z)_(min,max), cm 
   
   /* SET FIELD MAP */
   
  //SpdFieldMap* fMagField = new SpdFieldMap("karta_200_30","A");
  //fMagField->Init();
   
  run->SetField(fMagField);
  
  fMagField->Print();
  
  /*--------------------------------------------------*/
  /* ++++++++++++++  PRIMARY GENERATOR ++++++++++++++ */
  /*--------------------------------------------------*/
 
  SpdPrimaryGenerator* primGen = new SpdPrimaryGenerator();

  Pythia6Generator* P6gen = new Pythia6Generator();
  
  //================
  //    PYTHIA 6
  //================
  
  //-----------------------------------------------------------
  // Option: (decayer for vertex meson- and baryon- resonances)
  // 0 = Pythia 
  // 1 = Transport generator (DEFAULT value)
  
  P6gen->SetVGenOpt(1);
    
  //-----------------------------------------------------------
  // Set seed:
  
  //P6gen->SetSeed(0);
  P6gen->SetSeed(12995,0);
  //P6gen->SetSeed(19949278, 1);
  //P6gen->SetSeed(13495);
  //P6gen->SetSeed(127472);
  

  /* --- Pythia6 options --- */
  TPythia6* gg = (TPythia6*)P6gen->GetGenerator();
   
  gg->SetMSEL(1);  // set miminum bias 

   
  /* --- Pythia6 initialization --- */
  P6gen->Initialize("cms","p","p",20/*GeV*/);
   
  primGen->AddGenerator(P6gen);
  
  //================
  //   ISOTROPIC 
  //================
  
//   IsotropicGenerator* isogen = new IsotropicGenerator();
//   
//   isogen->Init(11,2,2);  // pdg number, momentum (GeV/c), particles per event or LEVEL
//   isogen->SetVerboseLevel(1);
//   isogen->SetVGenOpt(-1);  // (+-)1, (+-)2, 
//   
//   isogen->SetSeed(3921153342);
//   
//   //isogen->SetSpherical(60, 90, 30, 70); // theta[min,max], phi[min,max]
//  
//   
//   primGen->AddGenerator(isogen);
  
  /*============= SET PRIMARY GENERATOR ==============*/

  run->SetGenerator(primGen);  
  
  /*--------------------------------------------------*/
  
  run->SetStoreTraj(kTRUE);
    
  run->Init();
  
  /*--------------------------------------------------*/
  /* ++++++++++++++  RUNTIME DATABASE  ++++++++++++++ */
  /*--------------------------------------------------*/
 

  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print(); 
  
  /* ----------------------------------------------------------------------- */ 
  /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> RUN <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
  /* ----------------------------------------------------------------------- */ 
  
  TStopwatch timer;
  timer.Start();
  
  run->Run(nEvents);
 
  timer.Stop();
 
  /*--------------------------------------------------*/
  
  
  run->CreateGeometryFile("geofile_full.root");
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "             << outFile << endl;
  cout << "Parameter file is "          << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl;
  cout << endl;
  
  gApplication->Terminate();
}


