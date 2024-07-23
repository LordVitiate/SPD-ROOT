
//#define _COMPILE_MACRO_

#if defined(_COMPILE_MACRO_) 

#include <TRint.h>
#include <TStopwatch.h>

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

void CustomIts(Int_t option);
void CustomTsB(Double_t deg);

//_________________________________________________________________________
void SimuQslPy6(Int_t nEvents = 1)
{
  TString outFile = "run.root";    // Output data file name
  TString parFile = "params.root"; // Output parameters file name
   
  SpdRunSim* run = new SpdRunSim();
  
  run->SetName("TGeant4");       
  //gSystem->Setenv("GEOMPATH","/home/artur/Projects/1/");
  run->SetMaterials("media.geo");
  
  SpdRootFileSink* OutputFile = new SpdRootFileSink(outFile);
  run->SetSink(OutputFile);
  
  TString decayer_config = "DecayConfig.C";
  //TString decayer_config = "DecayConfigSpecV0.C";
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
  run->AddModule(Pipe);
  
  /* +++++++++ MAGNET +++++++++ */

  SpdHybMagnet* Magnet = new SpdHybMagnet();
  run->AddModule(Magnet);
    
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
  SpdTsTB*     ts_barrel   = new SpdTsTB();      /* +++++++++ TS (BARREL) ++++++++++++ */ 
  SpdTsTEC*    ts_ecps     = new SpdTsTEC();     /* +++++++++ TS (ENDCAPS) +++++++++++ */     
  SpdTofB*     tof_barrel  = new SpdTofB();      /* +++++++++ TOF (BARREL) +++++++++++ */
  SpdTofEC*    tof_ecps    = new SpdTofEC();     /* +++++++++ TOF (ENDCAPS) ++++++++++ */
  SpdEcalTB2*  ecal_barrel = new SpdEcalTB2();   /* +++++++++ ECAL (BARREL) ++++++++++ */      
  SpdEcalTEC2* ecal_ecps   = new SpdEcalTEC2();  /* +++++++++ ECAL (ENDCAPS) +++++++++ */  
  SpdRsTB2*    rs_barrel   = new SpdRsTB2();     /* +++++++++ RS (BARREL) ++++++++++++ */  
  SpdRsTEC2*   rs_ecps     = new SpdRsTEC2();    /* +++++++++ RS (ENDCAPS) +++++++++++ */
  
  SpdBbc*      bbc         = new SpdBbc();       /* +++++++++ BEAM-BEAM COUNTER +++++++ */
  SpdAeg*      aeg         = new SpdAeg();       /* +++++++++ AEROGEL +++++++++++++++++ */
  SpdZdc*      zdc         = new SpdZdc();       /* +++++++++ Z0 CALORIMETER ++++++++++ */
  
  run->AddModule(ts_barrel);  
  run->AddModule(ts_ecps);   
  run->AddModule(tof_barrel);
  run->AddModule(tof_ecps); 
  run->AddModule(ecal_barrel);
  run->AddModule(ecal_ecps);
  run->AddModule(rs_barrel);
  run->AddModule(rs_ecps);
  
  run->AddModule(bbc);
  run->AddModule(aeg);
  run->AddModule(zdc);

  /* ===== Vertex detector ===== */
    
  SpdIts* its = new SpdIts(); 
  run->AddModule(its);
  
  //SpdItsGeoMapperX::Instance()->SetGeometryPars(1);
  //SpdItsGeoMapperX::Instance()->EnableEndcaps(); // enable(1)/disable(0) Inner Tracker (Its) endcaps ; default = enable
  
  //its->SaveEmptyHits();
  
  /*--------------------------------------------------*/
  /* ++++++++++++  CUSTOMIZE GEOMETRY   +++++++++++++ */
  /*--------------------------------------------------*/
  
  //SpdTsTBGeoBuilder::Instance()->SetMaxBuildGeoLevel(2); 
  //SpdTsTECGeoBuilder::Instance()->SetMaxBuildGeoLevel(2); 
  
  // Choose TsTEC configuration 
  //   
  // option = 1: 2 endcaps x 3 modules, 1 module =  8x2 = 16 layers, total = 48 layers (default)
  // option = 2: 2 endcaps x 2 modules, 1 module =  8x2 = 16 layers, total = 32 layers  
  // option = 3: 2 endcaps x 1 modules, 1 module = 24x2 = 48 layers, total = 48 layers (actual!)
   
  SpdTsTECGeoMapper::Instance()->SetGeometryPars(3); 

  CustomIts(-1);
  CustomTsB(-1.);
  
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
      //    PYTHIA 6 GENERATOR
      //============================
      
      SpdPythia6Generator* P6gen = new SpdPythia6Generator();
      
      gRandom->SetSeed(0);
      Int_t seed = gRandom->Integer(Int_t(1e9));
      P6gen->SetSeed(seed,0);
       
      //P6gen->SetSeed(0);
      //P6gen->SetSeed(144135822, 2);
      
      //P6gen->SetSeed(12995,0);
      //P6gen->SetSeed(19949278, 1);
      //P6gen->SetSeed(13495);
      //P6gen->SetSeed(127472);
      

      /* --- Set Pythia6 options --- */
      SpdPythia6* gg = (SpdPythia6*)P6gen->GetGenerator();
       
      gg->SetMSEL(1);  // set miminum bias 

       
      /* --- Pythia6 initialization --- */
      P6gen->Initialize("cms","p","p",27/*GeV*/);
       
      primGen->AddGenerator(P6gen);
     
      primGen->SetVerboseLevel(-10);
      primGen->SetVerbose(0);
       
      //============================
  
   run->SetGenerator(primGen);

   primGen->SetBeam(0., 0., 0.1, 0.1); // (X,Y)- position, (X,Y)- (2*delta or sigma) [cm]
   primGen->SmearVertexXY(kTRUE);
   //primGen->SmearGausVertexXY(kTRUE);
    
   primGen->SetTarget(0.,5.);  // Z- position, 2*delta or sigma [cm]
   primGen->SmearVertexZ(kTRUE);
   //primGen->SmearGausVertexZ(kTRUE);
 
  /* ------------------------------------------------ */ 
  /* +++++++++++++++ GLOBAL OPTIONS +++++++++++++++++ */
  /* ------------------------------------------------ */ 
  
  //run->SetStoreTraj(kTRUE);
 
  //SpdCommonGeoMapper::Instance()->SaveEmptyHits();
  
  //run->ForceParticleLifetime(-211, 26.033/5.); // pdg, life time [ns]
  //run->ForceParticleLifetime( 211, 26.033/5.); // pdg, life time [ns]
  
  //SpdCommonGeoMapper::Instance()->UnsetMaterials(false);
  
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
  
  //run->CreateGeometryFile("geofile_full.root");
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  //-------------------------------------------------------------
  // PYTHIA 6 output info.
  // Option:
  // 1 - statistics on number of events and cross-sections;
  // 2 - decay widths and branching ratios;
  // 3 - allowed incoming partons/particles at hard interaction;
  // 4 - user-defined limits on kinematical variables;
  // 5 - summary of status codes and parameter values;
  // 6 - list of implemented processes;
  // 7 - sums over R-violating branching ratios.
  //-------------------------------------------------------------
  
  //((SpdPythia6*)P6gen->GetGenerator())->Pystat(1);
  
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

//___________________________________________________________________________
void CustomIts(Int_t option)
{
   if (option < 1) return;
   
   Double_t mm = 0.1;  
   
   SpdItsGeoMapperX* mapper = SpdItsGeoMapperX::Instance(); 
   
   //----------------------------------------------
   // Number of layers = 4
   // Type of chips (all layers): MAPS 
   // Chips thickness (all layers): 50 mkm
   //----------------------------------------------
    
   // layers:
   mapper->SetDefaultLadderPars(2, 1, 0, 70*mm);
   mapper->SetDefaultLadderPars(2, 2, 0, 127*mm);
   mapper->SetDefaultLadderPars(2, 3, 0, 184*mm);
   mapper->SetDefaultLadderPars(2, 4, 0, 241*mm);
   //mapper->SetDefaultLadderPars(2, 5, 0, 298*mm);
   
   mapper->SetDefaultLadderPars(2, 2, 15*mm);
   mapper->SetDefaultLadderPars(2, 3, 0.05*mm);
   
   // chips:
   mapper->SetDefaultChipPars(1, 0, 14.336*mm);
   mapper->SetDefaultChipPars(1, 1, 14.336*mm);
   mapper->SetDefaultChipPars(1, 2, 0.664*mm);
   mapper->SetDefaultChipPars(1, 3, 0);
   mapper->SetDefaultChipPars(1, 4, 512);
   mapper->SetDefaultChipPars(1, 5, 512);
   mapper->SetDefaultChipPars(1, 6, 1);
   
   mapper->SetGeometryPars(2,1); // set new parameters !
   
   mapper->SetNLayers(4); //!!!
}

//________________________________________________________________________________________________
void CustomTsB(Double_t deg)
{
   if (deg < 0 || deg > 90) return;
   
   SpdTsTBGeoMapper* mapper = SpdTsTBGeoMapper::Instance();
   
   Double_t d = 1.0;    // straw diameter;
   Double_t gap1 = 0.;  // gap between straws in the sublayer
   Double_t gap2 = 0.;  // gap between the sublayers in the double layer
   Double_t gap3 = 0.;  // gap between the double layers in the module
   
   Double_t w = SpdTsTBGeoMapper::GetLayerWidthFromStrawDiameter(d, gap1, gap2);
   mapper->SetStrawModulePars(1, w, gap3);
   
   mapper->SetStrawLayerPars(1,   0., d, gap1, gap2);   // z layer
   mapper->SetStrawLayerPars(1, +deg, d, gap1, gap2);   // u layer
   mapper->SetStrawLayerPars(1, -deg, d, gap1, gap2);   // v layer
}
