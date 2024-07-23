
//#define _COMPILE_MACRO_

#if defined(_COMPILE_MACRO_) 

#include <TRint.h>
#include <TStopwatch.h>

#include "FairParRootFileIo.h"

#include "SpdRunSim.h"
#include "SpdRootFileSink.h"

#include "SpdMCEventHeader.h"

#include "SpdFields.hh"
#include "SpdGenerators.hh"

#include "SpdCommonGeoMapper.h"

#include "SpdCave.h"
#include "SpdPipe.h"
#include "SpdHybMagnet.h"

#include "SpdIts.h"
#include "SpdTsTB.h"
#include "SpdTsTEC.h"
#include "SpdEcalTB2.h"
#include "SpdEcalTEC2.h"
#include "SpdRsTB2.h"
#include "SpdRsTEC2.h"
#include "SpdTofB.h"
#include "SpdTofEC.h"
#include "SpdZdc.h"

#include "SpdItsGeoMapperX.h"
#include "SpdTsTBGeoMapper.h"
#include "SpdTsTBGeoBuilder.h"
#include "SpdTsTECGeoMapper.h"

#endif

void CustomMvd(Int_t geo_type);
void CustomTsB(Double_t deg);

//_________________________________________________________________________
void SimuQslMvdPy8(Int_t nEvents = 1)
{
  TString outFile = "run.root";    // Output data file name
  TString parFile = "params.root"; // Output parameters file name
   
  SpdRunSim* run = new SpdRunSim();
  
  run->SetName("TGeant4");       
  //gSystem->Setenv("GEOMPATH","/home/artur/Projects/1/");
  run->SetMaterials("media.geo");
  
  SpdRootFileSink* OutputFile = new SpdRootFileSink(outFile);
  run->SetSink(OutputFile);
  
  TString decayer_config = "DecayConfigPythia8.C";
  run->SetPythiaDecayer(decayer_config);
 
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
  Pipe->SetPipeMaterial("steel",0);  //central beampipe segment
  //Pipe->SetPipeMaterial("steel",1);
  run->AddModule(Pipe);
  
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
  
  SpdTsTB*     ts_barrel   = new SpdTsTB();      /* +++++++++ TS (BARREL) ++++++++++++ */ 
  SpdTsTEC*    ts_ecps     = new SpdTsTEC();     /* +++++++++ TS (ENDCAPS) +++++++++++ */     
  
  run->AddModule(ts_barrel);
  run->AddModule(ts_ecps);  
 
  /* ===== Vertex detector ===== */
  
  //SpdIts* its = new SpdIts(); 
  //run->AddModule(its);
    
  SpdMvd* mvd = new SpdMvd(); 
  run->AddModule(mvd);
  
  /*--------------------------------------------------*/
  /* ++++++++++++  CUSTOMIZE GEOMETRY   +++++++++++++ */
  /*--------------------------------------------------*/
  
  //-------------------------------
  // Choose MVD configuration 
  CustomMvd(1); // geometry type = 1(default), 2 and 3(user-defined)  
   
  //-------------------------------
  // Choose TsTEC configuration 
  //   
  // option = 1: 2 endcaps x 3 modules, 1 module =  8x2 = 16 layers, total = 48 layers (default)
  // option = 2: 2 endcaps x 2 modules, 1 module =  8x2 = 16 layers, total = 32 layers  
  // option = 3: 2 endcaps x 1 modules, 1 module = 24x2 = 48 layers, total = 48 layers (actual!)
   
  SpdTsTECGeoMapper::Instance()->SetGeometryPars(3); 
  
  //-------------------------------
  // Choose TsTB configuration 
  CustomTsB(-1.);

  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
  
  /* --- axial field map --- */
  SpdAxialFieldMap* MagField = new SpdAxialFieldMap("QSolenoidal field");
  MagField->InitData("map_qsolRZ_6cls2cm.bin");
  //MagField->MultiplyField(0.8);

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
      //    PYTHIA 8 GENERATOR
      //============================

      SpdPythia8Generator* P8gen = new SpdPythia8Generator();
      
      // Define beams
      P8gen->SetBeam(2212,2212,27.); // pdg(A), pdg(B), E_cms (CM energy of collision)
      
      // Define start seed (optionally, default: random value)
      //P8gen->SetSeed(75122656); // < 900000000
        
      Int_t option = -1;
      
      if (option == -1) {  
          // Minimum bias 
          P8gen->SetParameters("SoftQCD:singleDiffractive = on");
          P8gen->SetParameters("SoftQCD:doubleDiffractive = on");
          P8gen->SetParameters("SoftQCD:centralDiffractive = on");
          P8gen->SetParameters("SoftQCD:nonDiffractive = on");
          P8gen->SetParameters("SoftQCD:inelastic = on");
          //P8gen->SetParameters("SoftQCD:elastic = on");
      }
      if (option == 0) {
          // Production processes J/psi
          P8gen->SetParameters("Charmonium:gg2ccbar(3S1)[3S1(1)]g = on,off");
          P8gen->SetParameters("Charmonium:gg2ccbar(3S1)[3S1(8)]g = on,off");
          P8gen->SetParameters("Charmonium:qg2ccbar(3S1)[3S1(8)]q = on,off");
          P8gen->SetParameters("Charmonium:qqbar2ccbar(3S1)[3S1(8)]g = on,off");
          P8gen->SetParameters("Charmonium:gg2ccbar(3S1)[1S0(8)]g = on,off");
          P8gen->SetParameters("Charmonium:qg2ccbar(3S1)[1S0(8)]q = on,off");
          P8gen->SetParameters("Charmonium:qqbar2ccbar(3S1)[1S0(8)]g = on,off");
          P8gen->SetParameters("Charmonium:gg2ccbar(3S1)[3PJ(8)]g = on,off");
          P8gen->SetParameters("Charmonium:qg2ccbar(3S1)[3PJ(8)]q = on,off");
          P8gen->SetParameters("Charmonium:qqbar2ccbar(3S1)[3PJ(8)]g = on,off");

          // Jpsi decays to mu+ mu-
          P8gen->SetParameters("443:onMode = off");
          P8gen->SetParameters("443:onIfAny = 13 -13");
      }
      else if (option == 1) {
          // Open charm (heavy flavour states)
          P8gen->SetParameters("HardQCD:gg2ccbar = on");
          P8gen->SetParameters("PhaseSpace:pTHatMin = 1"); // default = 0
      }
      else if (option == 2) {
          // Promt photons   
          P8gen->SetParameters("PromptPhoton:qg2qgamma = on");        // q+g    -> q+gamma
          P8gen->SetParameters("PromptPhoton:qqbar2ggamma = on");     // q+qbar -> g+gamma
          P8gen->SetParameters("PromptPhoton:gg2ggamma = on");        // g+g    -> g+gamma
          P8gen->SetParameters("PromptPhoton:ffbar2gammagamma = on"); // q+qbar -> gamma+gamma
          P8gen->SetParameters("PromptPhoton:gg2gammagamma = on");    // g+g    -> gamma+gamma
      }
      else if (option == 3)
      {
          P8gen->SetParameters("SoftQCD:elastic = on");
      }
      
      //P8gen->SetVerboseLevel(-1);

      primGen->AddGenerator(P8gen);
   

   //-----------------------------------
   run->SetGenerator(primGen);

   primGen->SetBeam(0., 0., 0.1, 0.1); // (X,Y)- position, (X,Y)- (2*delta or sigma) [cm]
   primGen->SmearVertexXY(kTRUE);
   //primGen->SmearGausVertexXY(kTRUE);
    
   primGen->SetTarget(0.,5.);  // Z- position, 2*delta or sigma [cm]
   primGen->SmearVertexZ(kTRUE);
   //primGen->SmearGausVertexZ(kTRUE);
      
   primGen->SetVerboseLevel(-10);
   primGen->SetVerbose(0);
      
  /* ------------------------------------------------ */ 
  /* +++++++++++++++ GLOBAL OPTIONS +++++++++++++++++ */
  /* ------------------------------------------------ */ 
  
  //run->SetStoreTraj(kTRUE);
  
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

  //-------------------------------------------------------------
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is             " << outFile << endl;
  cout << "Parameter file is          " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl;
  cout << endl;
  
  /*--------------------------------------------------*/
  
  //SpdCommonGeoMapper::Instance()->PrintGeometryList();
  SpdCommonGeoMapper::Instance()->PrintGeometry();
  
  //SpdItsGeoMapperX::Instance()->PrintVolPars(); 
  //SpdItsGeoMapperX::Instance()->PrintGeoTable(); 
  //SpdItsGeoMapperX::Instance()->Print("");
  
   //FIXME
   mvd->PrintGeometry("");
  
  /*--------------------------------------------------*/
  
  gApplication->Terminate();
}

//___________________________________________________________________________
void CustomMvd(Int_t geo_type)
{
   if (geo_type < 1) return;

   SpdMvdGeoMapper* mapper = SpdMvdGeoMapper::Instance(); 
   
   if (geo_type == 1) { mapper->SetGeoType(1); return; }
   if (geo_type == 2) { mapper->SetGeoType(2); return; }
  
   mapper->SetGeoType(3);
   
   mapper->ClearGeometry();
   
   // here we can redefine active material (by default = "argon") 
   //mapper->SetActiveMaterial("copper");
   
   //---------------------------------------
   // BUILD LAYERS
   
   Int_t l0, l1;
   
   l0 = mapper->DefineLayer(5.0,80.0);             mapper->SetLayerActivity(l0,true);   
   
        mapper->AddSublayer(l0,0.01750,"FR4");
        mapper->AddSublayer(l0,0.00190,"copper");
        mapper->AddSublayer(l0,0.01350,"kapton2");
        mapper->AddSublayer(l0,0.40000,"argon");
        mapper->AddSublayer(l0,0.00055,"copper");
        mapper->AddSublayer(l0,0.02400,"kapton2");
   
   l1 = mapper->DefineLayerCopy(l0,5.5);  
   l1 = mapper->DefineLayerCopy(l0,6.0);   
  
   l1 = mapper->DefineLayerCopy(l0,12.0);          mapper->SetLayerActivity(l1,true);
   l1 = mapper->DefineLayerCopy(l0,12.5);

   l1 = mapper->DefineLayerCopy2(l0,19.0,160.0);   mapper->SetLayerActivity(l1,true);
   l1 = mapper->DefineLayerCopy2(l0,19.5,160.0);
}

//________________________________________________________________________________________________
void CustomTsB(Double_t deg)
{
   if (deg < 0 || deg > 90) return;
   
   SpdTsTBGeoMapper* mapper = SpdTsTBGeoMapper::Instance();
   
   //SpdParameter* par;
   // remove airgap beetween modules 
   //par = mapper->AddParameter("TsTBMakeOffset2");
   //*par = false;
   
   mapper->SetStrawModulePars(1,   0., 'e', 1.0, 0);
   
   mapper->SetStrawLayerPars(1,    0., 'e', 1.0, 0.);
   mapper->SetStrawLayerPars(1,    0., 'o', 1.0, 0.);
   mapper->SetStrawLayerPars(1,   deg, 'e', 1.0, 0.); 
   mapper->SetStrawLayerPars(1,   deg, 'o', 1.0, 0.);
   mapper->SetStrawLayerPars(1,  -deg, 'e', 1.0, 0.); 
   mapper->SetStrawLayerPars(1,  -deg, 'o', 1.0, 0.);  
}


