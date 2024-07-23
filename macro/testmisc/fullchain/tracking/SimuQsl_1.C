
//#define _COMPILE_MACRO_

#if defined(_COMPILE_MACRO_) 

#include <TRint.h>
#include <TStopwatch.h>

#include "FairParRootFileIo.h"

#include "SpdRunSim.h"
#include "SpdMCEventHeader.h"

#include "SpdCommonGeoMapper.h"

#include "SpdFields.hh"
#include "SpdGenerators.hh"

#include "SpdCave.h"
#include "SpdPipe.h"

#include "SpdIts.h"
#include "SpdTsTB.h"
#include "SpdTsTEC.h"

#include "SpdItsGeoMapperX.h"
#include "SpdTsTBGeoBuilder.h"

#endif

//_________________________________________________________________________
void SimuQsl_1(Int_t nEvents = 1)
{
  TString outFile = "run.root";    // Output data file name
  TString parFile = "params.root"; // Output parameters file name
   
  SpdRunSim* run = new SpdRunSim();
  
  run->SetName("TGeant4");       
  run->SetMaterials("media.geo");
  
  run->SetOutputFile(outFile);      
  
  //run->SetPythiaDecayer("DecayConfig.C");
 
  run->SetMCEventHeader(new SpdMCEventHeader);
 
  /*--------------------------------------------------*/
  /* +++++++++     GEOMETRY (QSOLENOID)      ++++++++ */
  /*--------------------------------------------------*/
  
  SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
  
  /* ++++++++++++++++++ CAVE ++++++++++++++++++ */
  
  FairModule* cave = new SpdCave("CAVE");
  //cave->SetGeometryFileName("cave.geo");
  //cave->SetGeometryFileName("cave_precise.geo");
  run->AddModule(cave);
  
  /* ++++++++++++++++++ PIPE ++++++++++++++++++ */
  
  SpdPipe* Pipe = new SpdPipe();
  //Pipe->UnsetMaterials("air");
  run->AddModule(Pipe);
    
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
 
  SpdTsTB*    ts_barrel   = new SpdTsTB();     /* +++++++++ TST (BARREL) ++++++++++++ */ 
  SpdTsTEC*   ts_ecps     = new SpdTsTEC();    /* +++++++++ TST (ENDCAPS) +++++++++++ */     
 
  run->AddModule(ts_barrel);  
  run->AddModule(ts_ecps);   

  /* ===== Vertex detector ===== */
    
  SpdIts* its = new SpdIts(); 
  run->AddModule(its);
  
  //SpdItsGeoMapperX::Instance()->SetGeometryPars(1);
  
  //SpdItsGeoMapperX::Instance()->EnableEndcaps(); // enable(1)/disable(0) Inner Tracker (Its) endcaps ; default = enable
  //its->SaveEmptyHits();

  /*--------------------------------------------------*/
  /* ++++++++++++  CUSTOMIZE GEOMETRY   +++++++++++++ */
  /*--------------------------------------------------*/
  
  // Chose TsTEC configuration 
  //   
  // option = 1: (2x)3 modules, 1 module =  8x2 = 16 layers, total = 48 layers (default)
  // option = 2: (2x)2 modules, 1 module =  8x2 = 16 layers, total = 32 layers  
  // option = 3: (2x)1 modules, 1 module = 26x2 = 52 layers, total = 52 layers (actual!)
   
  SpdTsTECGeoMapper::Instance()->SetGeometryPars(3); 
   
  SpdParameter* parg;
   
  // RsTB geo. parameters
  parg = SpdRsTBGeoMapper::Instance()->AddParameter("RsTBClearance");
  *parg = 0.;
   
  // EcalTB geo. parameters
  parg = SpdEcalTBGeoMapper::Instance()->AddParameter("EcalTBClearance");
  *parg = 0.;
  
  /*--------------------------------------------------*/
  /* ++++++++++++  CUSTOMIZE MATERIALS  +++++++++++++ */
  /*--------------------------------------------------*/

  //SpdParameter* par;

  /* --- alter TsTB straw active media (gas) --- */
  //par = ts_barrel->GetMapper()->AddParameter("TsTBBaseStrawMaterial");
  //if (par) *par = "arco28020x1p5"; // ArCO2, 80/20 %, density x 1.5  
  //if (par) *par = "arco28020x2p0"; // ArCO2, 80/20 %, density x 2.0
  //if (par) *par = "arco27030x1p5"; // ArCO2, 70/30 %, density x 1.5  
  //if (par) *par = "arco27030x2p0"; // ArCO2, 70/30 %, density x 2.0  
  
  /* --- alter TsTEC straw active media (gas) --- */
  //par = ts_ecps->GetMapper()->AddParameter("TsTECBaseStrawMaterial");
  //if (par) *par = "arco28020x1p5"; // ArCO2, 80/20 %, density x 1.5  
  //if (par) *par = "arco28020x2p0"; // ArCO2, 80/20 %, density x 2.0
  //if (par) *par = "arco27030x1p5"; // ArCO2, 70/30 %, density x 1.5  
  //if (par) *par = "arco27030x2p0"; // ArCO2, 70/30 %, density x 2.0  
 
  /* --- reset all module's materials ---*/
  //ts_barrel->GetMapper()->UnsetMaterials("air");  // "air" or "vacuum"
  //ts_ecps->GetMapper()->UnsetMaterials("air");    // "air" or "vacuum"
  //its->GetMapper()->UnsetMaterials("air");        // "air" or "vacuum"
  
  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
  
  /* --- axial field map --- */
  SpdAxialFieldMap* MagField = new SpdAxialFieldMap("QSolenoidal field");
  MagField->InitData("map_qsolRZ_6cls2cm.bin");
  MagField->MultiplyField(0.8);
   
  /* --- const field --- */
  //SpdConstField* MagField = new SpdConstField();  
  //MagField->SetField(0., 0., 8.0); //  kG

  /* --- field map --- */
  //SpdFieldMap* MagField = new SpdFieldMap("Hybrid field");
  
  //MagField->InitData("map_hyb_1T2cm.bin");       // standard "hybrid" field
  //MagField->InitData("map_sol_6cls5cm2.bin");  // "quasi-solenoid"
   
  //MagField->SetApproxMethod(0); // 0 (linear,default) or 1 (nearest vertex)
  //MagField->MultiplyField(1);
  
  /* === define field region === */
  SpdRegion* reg = 0;
  
  //reg = MagField->CreateFieldRegion("box");
  //reg->SetBoxRegion(-1000, 1000, -1000, 1000, -1000, 1000); // (X,Y,Z)_(min,max), cm
  
  reg = MagField->CreateFieldRegion("tube");
  reg->SetTubeRegion(0, 174, -246, 246); // (R,Z)_(min,max), cm 
  
  run->SetField(MagField);
  
  MagField->Print();
  
  /*--------------------------------------------------*/
  /* ++++++++++ DEFINE PRIMARY GENERATORS +++++++++++ */
  /*--------------------------------------------------*/
 
  SpdPrimaryGenerator* primGen = new SpdPrimaryGenerator();

      //============================
      //    PYTHIA 6 GENERATOR
      //============================
      
      SpdPythia6Generator* P6gen = new SpdPythia6Generator();
      
      //-----------------------------------------------------------
      // Option: (decayer for vertex meson- and baryon- resonances)
      // 0 = Transport generator + External decayer (particles don't decay in the vertex) 
      // 1 = Primary generator (DEFAULT value, more safely)
      // 2 = Standart output (accept particles of status = 1,4); max decay vertex distance should be specified
      
      //P6gen->SetVGenOpt(0);
      //P6gen->SetVGenOpt(1); 
      
      // MDVD (max decay vertex distance) > 0 [cm], MDVD < 0 => MDVD = 100 cm (default)
      P6gen->SetVGenOpt(2,0.1 /*MDVD*/);
        
      //-----------------------------------------------------------
      // Set seed:
      
      gRandom->SetSeed(12345);
      Int_t seed = gRandom->Integer(Int_t(1e9));
      P6gen->SetSeed(seed,0);
       
      //P6gen->SetSeed(0);
      //P6gen->SetSeed(144135822, 2);
      
      P6gen->SetSeed(12995,0);
      //P6gen->SetSeed(19949278, 1);
      //P6gen->SetSeed(13495);
      //P6gen->SetSeed(127472);
      

      /* --- Set Pythia6 options --- */
      SpdPythia6* gg = (SpdPythia6*)P6gen->GetGenerator();
       
      gg->SetMSEL(1);  // set miminum bias 
      
      //cout << "AAAAAAAAAAAAA: " << gg->GetMSTJ(22) << " " << gg->GetPARJ(72) << endl;
      
      //gg->SetPARJ(72,1e-8); // 10 mm
      //gg->SetMSTJ(22,3);
      
      /* --- Pythia6 initialization --- */
      P6gen->Initialize("cms","p","p",27/*GeV*/);
       
      primGen->AddGenerator(P6gen);
     
      //primGen->SetVerboseLevel(-10);
      primGen->SetVerbose(0);
       
      //============================
      
   run->SetGenerator(primGen);

   primGen->SetBeam(0., 0., 0.1, 0.1); // (X,Y)- position, (X,Y)- (2*delta or sigma) [cm]
   primGen->SmearVertexXY(kTRUE);  
   primGen->SmearVertexZ(kTRUE);
//   //primGen->SmearGausVertexZ(kTRUE);
 
  /* ------------------------------------------------ */ 
  /* +++++++++++++++ GLOBAL OPTIONS +++++++++++++++++ */
  /* ------------------------------------------------ */ 
  
  //run->SetStoreTraj(kTRUE);
 
  //SpdCommonGeoMapper::Instance()->SaveEmptyHits();
  
  //run->ForceParticleLifetime(-211, 26.033/5.); // pdg, life time [ns]
  //run->ForceParticleLifetime( 211, 26.033/5.); // pdg, life time [ns]
  
  //SpdCommonGeoMapper::Instance()->UnsetMaterials(true);
  
  /* ----------------------------------------------------------------------- */ 
  /* >>>>>>>>>>>>>>>>>>>>>>>>>>> INITALIZE RUN <<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
  /* ----------------------------------------------------------------------- */ 
  
  cout << "\n\t" << "++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "\t"   << "+                                          +" << endl;
  cout << "\t"   << "+           Init Run (start)               +" << endl; 
  cout << "\t"   << "+                                          +" << endl;
  cout << "\t"   << "++++++++++++++++++++++++++++++++++++++++++++\n" << endl;
     
  run->Init();
  
  cout << "\n\t" << "++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "\t"   << "+                                          +" << endl;
  cout << "\t"   << "+           Init Run (finish)              +" << endl; 
  cout << "\t"   << "+                                          +" << endl;
  cout << "\t"   << "++++++++++++++++++++++++++++++++++++++++++++\n" << endl;
 
  //return;
  
  /*--------------------------------------------------*/
  /* +++++++++++++ CREATE RUN PARAMETERS  +++++++++++ */
  /*--------------------------------------------------*/
  
  Bool_t MergePars = kFALSE;
  FairParRootFileIo* parOut = new FairParRootFileIo(MergePars);
  if (MergePars) parOut->open(parFile.Data());
  else parOut->open(parFile.Data(),"RECREATE");
  
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  rtdb->setOutput(parOut);
   
  /* ----------------------------------------------------------------------- */ 
  /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> RUN <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
  /* ----------------------------------------------------------------------- */ 
  
  TStopwatch timer;
  timer.Start();
  
  run->Run(nEvents);
  
  timer.Stop();
 
  /*--------------------------------------------------*/
  /* ++++++++++++  SAVE RUN PARAMETERS  +++++++++++++ */
  /*--------------------------------------------------*/
 
  rtdb->saveOutput();
  
  /*--------------------------------------------------*/
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is             " << outFile << endl;
  cout << "Parameter file is          " << parFile << endl;
  cout << "Seed (prim.gen.) is        " << seed << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl;
  cout << endl;
  
  /*--------------------------------------------------*/
  
  //SpdCommonGeoMapper::Instance()->PrintGeometryList();
  SpdCommonGeoMapper::Instance()->PrintGeometry();
  
  //SpdItsGeoMapperX::Instance()->PrintVolPars(); 
  //SpdItsGeoMapperX::Instance()->PrintGeoTable(); 
  //SpdItsGeoMapperX::Instance()->Print("");
  
  /*--------------------------------------------------*/
  
  gApplication->Terminate();
}

