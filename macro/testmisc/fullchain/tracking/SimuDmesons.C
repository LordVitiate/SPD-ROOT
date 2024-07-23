
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
#include "SpdHybMagnet.h"

#include "SpdIts.h"
#include "SpdTsTB.h"
#include "SpdTsTEC.h"
#include "SpdEcalTB.h"
#include "SpdEcalTEC.h"
#include "SpdRsTB.h"
#include "SpdRsTEC.h"

#include "SpdItsGeoMapperX.h"
#include "SpdTsTBGeoBuilder.h"

#endif

void CustomizeGeometryIts();
void CustomizeGeometryTsTB();
void CustomizeGeometryTsTEC();

//_________________________________________________________________________
void SimuDmesons(Int_t nEvents = 1)
{
  TString outFile = "run.root";    // Output data file name
  TString parFile = "params.root"; // Output parameters file name
   
  SpdRunSim* run = new SpdRunSim();
  
  run->SetName("TGeant4");       
  run->SetMaterials("media.geo");
  
  run->SetOutputFile(outFile);      
  
  run->SetPythiaDecayer("DecayConfig.C");
 
  run->SetMCEventHeader(new SpdMCEventHeader);
 
  /*--------------------------------------------------*/
  /* +++++++++     GEOMETRY (HYBRID)      +++++++++++ */
  /*--------------------------------------------------*/
  
  SpdCommonGeoMapper::Instance()->DefineHybGeometrySet();
  
  /* ++++++++++++++++++ CAVE ++++++++++++++++++ */
  
  FairModule* cave = new SpdCave("CAVE");
  //cave->SetGeometryFileName("cave.geo");
  cave->SetGeometryFileName("cave_precise.geo");
  run->AddModule(cave);
  
  /* ++++++++++++++++++ PIPE ++++++++++++++++++ */
  
  SpdPipe* Pipe = new SpdPipe();
  run->AddModule(Pipe);
  
  /* +++++++++++++++++ MAGNET +++++++++++++++++ */
  
//   SpdHybMagnet* Magnet = new SpdHybMagnet();
//   //Magnet->SetGeometryType(2); // 1 = "hybrid" (deafult), 2 = "qsolenoid"
//   run->AddModule(Magnet);
  
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
 
  SpdTsTB*    ts_barrel   = new SpdTsTB();     /* +++++++++ TST (BARREL) ++++++++++++ */ 
  SpdTsTEC*   ts_ecps     = new SpdTsTEC();    /* +++++++++ TST (ENDCAPS) +++++++++++ */     
    
  run->AddModule(ts_barrel);
  run->AddModule(ts_ecps);   
    
  /* ===== Vertex detector ===== */
    
  SpdIts* its = new SpdIts(); 
  run->AddModule(its);
  
  SpdItsGeoMapperX::Instance()->EnableEndcaps(); // enable(1)/disable(0) endcaps

  /*--------------------------------------------------*/
  /* ++++++++++++  CASTOMIZE GEOMETRY   +++++++++++++ */
  /*--------------------------------------------------*/
  
  CustomizeGeometryIts();
  CustomizeGeometryTsTB();
  CustomizeGeometryTsTEC();
   
  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
   
  /* --- const field --- */
  //SpdConstField* MagField = new SpdConstField();  
  //MagField->SetField(0., 0. , 10); //  kG

  /* --- field map --- */
  SpdFieldMap* MagField = new SpdFieldMap("Hybrid field");
  
  //MagField->InitData("map_hyb_1T2cm.bin");       // standard "hybrid" field
  MagField->InitData("map_sol_6cls5cm2.bin");  // "quasi-solenoid"
  
  //MagField->SetApproxMethod(0); // 0 (linear,default) or 1 (nearest vertex)
  //MagField->MultiplyField(1);
  
  /* === define field region === */
  SpdRegion* reg = 0;
  
  //reg = MagField->CreateFieldRegion("box");
  //reg->SetBoxRegion(-1000, 1000, -1000, 1000, -1000, 1000); // (X,Y,Z)_(min,max), cm
  
  reg = MagField->CreateFieldRegion("tube");
  reg->SetTubeRegion(0, 250, -300, 300); // (R,Z)_(min,max), cm 
  
  run->SetField(MagField);
  
  MagField->Print();
  
  /*--------------------------------------------------*/
  /* ++++++++++ DEFINE PRIMARY GENERATORS +++++++++++ */
  /*--------------------------------------------------*/
 
  SpdPrimaryGenerator* primGen = new SpdPrimaryGenerator();

      //===========================
      //   ISOTROPIC GENERATOR (1)
      //===========================
      
      SpdIsotropicGenerator* isogen1 = new SpdIsotropicGenerator("isogen1");
      
      // D+
      isogen1->Initialize(411,5.,1);  // pdg number, momentum (GeV/c), particles per event or LEVEL
      isogen1->SetVerboseLevel(1);
      isogen1->SetVGenOpt(-1);  // (+-)1, (+-)2
      
      primGen->AddGenerator(isogen1);

      //===========================
      //   ISOTROPIC GENERATOR (2)
      //===========================
         
      SpdIsotropicGenerator* isogen2 = new SpdIsotropicGenerator("isogen2");
      
      // D-
      isogen2->Initialize(-411,1.5,1);  // pdg number, momentum (GeV/c), particles per event or LEVEL
      isogen2->SetVerboseLevel(1);
      isogen2->SetVGenOpt(-1);  // (+-)1, (+-)2 
      
      primGen->AddGenerator(isogen2);
      
      //===========================
      //   ISOTROPIC GENERATOR (3)
      //===========================
      
      SpdIsotropicGenerator* isogen3 = new SpdIsotropicGenerator("isogen3");
      
      // D0
      isogen3->Initialize(421,5.,1);  // pdg number, momentum (GeV/c), particles per event or LEVEL
      isogen3->SetVerboseLevel(1);
      isogen3->SetVGenOpt(-1);  // (+-)1, (+-)2
      
      primGen->AddGenerator(isogen3);
      
      //============================
    
  run->SetGenerator(primGen);  
  
  primGen->SetVerboseLevel(-10);
  //primGen->SetVerbose(0);
      
  primGen->SetBeam(0., 0., 0.1, 0.1); // (X,Y)- position, (X,Y)- (0.5*delta or sigma) [cm]
  primGen->SmearVertexXY(kTRUE);
  //primGen->SmearGausVertexXY(kTRUE);
   
  primGen->SetTarget(0.,5.);  // Z- position, 0.5*delta or sigma [cm]
  primGen->SmearVertexZ(kTRUE);
  //primGen->SmearGausVertexZ(kTRUE);
 
  /* ------------------------------------------------ */ 
  /* +++++++++++++++ GLOBAL OPTIONS +++++++++++++++++ */
  /* ------------------------------------------------ */ 
  
  //run->SetStoreTraj(kTRUE);
 
  //SpdCommonGeoMapper::Instance()->SaveEmptyHits();
  
  //SpdCommonGeoMapper::Instance()->UnsetMaterials(1);  // set all materials = vacuum (precise)
      
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
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "             << outFile << endl;
  cout << "Parameter file is "          << parFile << endl;
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

//________________________________________________________________________________________________
void CustomizeGeometryTsTB()
{
   //return;
  
   //SpdTsTECGeoMapper::Instance()->SetGeometryPars(2);
  
   SpdTsTBGeoBuilder* builder = SpdTsTBGeoBuilder::Instance();
     
   // ---------------------------------------------------
   // SpdTsTBGeoBuilder::AddRejection(volname ,nmin, nmax, lcycle).
   // Remove nj = n + j*lcycle layers, where 
   // n = [nmin,nmax], j = 0,1,... , lcycle >= n.
   // volname = unique part of volume name placed 
   // before its number (for example SpdTsTBLayer23 ->
   // SpdTsTBLayer, TsTBLayer, TBLayer or Layer).
   // ---------------------------------------------------
   
    Int_t lcycle = 8; // number of different layer (layer cycle)
   
    builder->AddRejection("Layer", 1, 2, lcycle);
    builder->AddRejection("Layer", 3, 4, lcycle);
    //builder->AddRejection("Layer", 5, 6, lcycle);
    builder->AddRejection("Layer", 7, 8, lcycle);
     
   
//     builder->AddRejection("Layer", 16);
//     builder->AddRejection("Layer", 30);
//     builder->AddRejection("Layer", 35, 45);
//     builder->AddRejection("Layer", 56, 67);
//     builder->AddRejection("Layer", 80, 88);
    
}

//________________________________________________________________________________________________
void CustomizeGeometryTsTEC()
{
    
    
}

//________________________________________________________________________________________________
void CustomizeGeometryIts()
{
    
    
}





