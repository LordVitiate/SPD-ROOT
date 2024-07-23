
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

//_________________________________________________________________________
void simu(Int_t nEvents = 10, int seed = 0)
{
  TString outFile = "simu.root";    // Output data file name
  TString parFile = "params.root"; // Output parameters file name

  gRandom->SetSeed(seed);

  SpdRunSim* run = new SpdRunSim();
  
  run->SetName("TGeant4");       
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
  // Central beampipe segment material
  //  Pipe->SetPipeMaterial("aluminium",0);  // default     
  //  Pipe->SetPipeMaterial("steel",0);      // central beampipe segment
  //  Pipe->SetPipeMaterial("beryllium",0);  // central beampipe segment
  //  Pipe->SetPipeMaterial("steel",1);      // outer parts of the beam pipe, aluminium is the default
  run->AddModule(Pipe);
  
  /* +++++++++ MAGNET +++++++++ */

  //SpdSolMagnet2 * Magnet = new SpdSolMagnet2();
  //run->AddModule(Magnet);
    
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
  
  SpdTsTB*     ts_barrel   = new SpdTsTB();      /* +++++++++ TS (BARREL) ++++++++++++ */ 
  SpdTsTEC*    ts_ecps     = new SpdTsTEC();     /* +++++++++ TS (ENDCAPS) +++++++++++ */     
  
  run->AddModule(ts_barrel);
  run->AddModule(ts_ecps);  
 
  /* ===== Vertex detector ===== */
  
    
  SpdMvd* mvd = new SpdMvd(); 
  run->AddModule(mvd);
  
  /*--------------------------------------------------*/
  /* ++++++++++++  CUSTOMIZE GEOMETRY   +++++++++++++ */
  /*--------------------------------------------------*/
  
  //-------------------------------
  // Choose MVD configuration 
  CustomMvd(1); // geometry type = 1(default), 2 and 3(user-defined), 3-layers  // ~TDR
//  CustomMvd(2); // geometry type = 1(default), 2 and 3(user-defined), 2-layers  
//  CustomMvd(3); // geometry type = 1(default), 2 and 3(user-defined), 1-layer  // Likely optimal
   
  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
  
  SpdFieldMap1_8* MagField = new SpdFieldMap1_8("full_map");
  MagField->InitData("field_full1_8.bin");
  MagField->SetApproxMethod(1);
  SpdRegion* reg = MagField->CreateFieldRegion("box");
  reg->SetBoxRegion(-330, 330, -330, 330, -386, 386); // (X,Y,Z)_(min,max), cm
  
  run->SetField(MagField);
  
  MagField->Print();
  
  /*--------------------------------------------------*/
  /* ++++++++++ DEFINE PRIMARY GENERATORS +++++++++++ */
  /*--------------------------------------------------*/
 
  SpdPrimaryGenerator* primGen = new SpdPrimaryGenerator();
  

  SpdIsotropicGenerator* IGen = new SpdIsotropicGenerator("IGen");
  IGen->Initialize(13, 1.5, 1);  
  IGen->SetSeed(1); 
  IGen->SetVerboseLevel(1);
  IGen->SetVGenOpt(1);  // (+-)1, (+-)2

  primGen->AddGenerator(IGen);

   //-----------------------------------
   run->SetGenerator(primGen);

   primGen->SetBeam(0., 0., 0.1, 0.1); // (X,Y)- position, (X,Y)- (2*delta or sigma) [cm]
   primGen->SmearGausVertexXY(kTRUE);
    
   primGen->SetTarget(0., 30.);  // Z- position, 2*delta or sigma [cm]
   primGen->SmearGausVertexZ(kTRUE);
      
   primGen->SetVerboseLevel(-10);
   primGen->SetVerbose(0);
      
  /* ------------------------------------------------ */ 
  /* +++++++++++++++ GLOBAL OPTIONS +++++++++++++++++ */
  /* ------------------------------------------------ */ 
  
  //run->SetStoreTraj(kTRUE);
 
  //SpdCommonGeoMapper::Instance()->SaveEmptyHits();
  
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

  //run->CreateGeometryFile("geofile_full.root");
 
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
  
//   l1 = mapper->DefineLayerCopy(l0,12.0);          mapper->SetLayerActivity(l1,true);
//   l1 = mapper->DefineLayerCopy(l0,12.5);

//   l1 = mapper->DefineLayerCopy2(l0,19.0,160.0);   mapper->SetLayerActivity(l1,true);
//   l1 = mapper->DefineLayerCopy2(l0,19.5,160.0);
}


