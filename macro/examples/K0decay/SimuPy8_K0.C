
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
#include "SpdEcalTB2.h"
#include "SpdEcalTEC2.h"
#include "SpdRsTB2.h"
#include "SpdRsTEC2.h"

#include "SpdItsGeoMapperX.h"
#include "SpdTsTBGeoBuilder.h"

#endif

//_________________________________________________________________________
void SimuPy8_K0(Int_t nEvents = 100)
{
  TString outFile = "run_K0_1.root";    // Output data file name
  TString parFile = "params_K0_1.root"; // Output parameters file name
   
  SpdRunSim* run = new SpdRunSim();
  
  run->SetName("TGeant4");       
  run->SetMaterials("media.geo");
  
  SpdRootFileSink* OutputFile = new SpdRootFileSink(outFile);
  run->SetSink(OutputFile);
    
  TString decayer = "DecayConfigPythia8.C";
  run->SetPythiaDecayer(decayer);
 
  run->SetMCEventHeader(new SpdMCEventHeader);
  
   /*--------------------------------------------------*/
  /* +++++++++     GEOMETRY (QSOLENOID)      ++++++++ */
  /*--------------------------------------------------*/
  
  SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
  
  /* ++++++++++++++++++ CAVE ++++++++++++++++++ */
  
  FairModule* cave = new SpdCave("CAVE");
  run->AddModule(cave);
  
  /* ++++++++++++++++++ PIPE ++++++++++++++++++ */
  
  //SpdPipe* Pipe = new SpdPipe();
  //run->AddModule(Pipe);
    
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
 
  SpdTsTB*     ts_barrel   = new SpdTsTB();      /* +++++++++ TS (BARREL) ++++++++++++ */ 
  SpdTsTEC*    ts_ecps     = new SpdTsTEC();     /* +++++++++ TS (ENDCAPS) +++++++++++ */     

  run->AddModule(ts_barrel);
  run->AddModule(ts_ecps);  
  
  /* ===== Vertex detector ===== */
    
  SpdIts* its = new SpdIts(); 
  run->AddModule(its);
  
  /*--------------------------------------------------*/
  /* ++++++++++++  CUSTOMIZE GEOMETRY   +++++++++++++ */
  /*--------------------------------------------------*/

  SpdTsTECGeoMapper::Instance()->SetGeometryPars(3); 
  
  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
  
  /* --- axial field map --- */
//  SpdAxialFieldMap* MagField = new SpdAxialFieldMap("QSolenoidal field");
//  MagField->InitData("map_qsolRZ_6cls2cm.bin");
//  MagField->MultiplyField(0.8);

  /* --- const field --- */
  SpdConstField* MagField = new SpdConstField();  
  MagField->SetField(0., 0., 10.0); //  kG

  /* === define field region === */
  SpdRegion* reg = 0;
  
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
      
      // Define beams pdg(A), pdg(B), CM energy of collision
      P8gen->SetBeam(2212,2212,27.); // p+p, E_cms
      
      // Define start seed (optionally, default: random value)
      //P8gen->SetSeed(75656);
      
      // Define processes
      //P8gen->SetParameters("SoftQCD:elastic = on");
      P8gen->SetParameters("SoftQCD:singleDiffractive = on");
      P8gen->SetParameters("SoftQCD:doubleDiffractive = on");
      P8gen->SetParameters("SoftQCD:centralDiffractive = on");
      P8gen->SetParameters("SoftQCD:nonDiffractive = on");
      P8gen->SetParameters("SoftQCD:inelastic = on");
      
      //P8gen->SetVerboseLevel(-1);

      primGen->AddGenerator(P8gen);
      
      //------------------------------------------------
      // add inline generator with K^0
      //------------------------------------------------
      
      SpdIsotropicGenerator* isogen1 = new SpdIsotropicGenerator("isogen1");
      isogen1->SetSeed(39113);           // 1-nd

      isogen1->Initialize(310, 1.5, 1);  // Ks^0
      isogen1->SetVGenOpt(1);            // (+-)1, (+-)2

      primGen->AddGenerator(isogen1);

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
  cout << "Output file is             " << outFile << endl;
  cout << "Parameter file is          " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl;
  cout << endl;
  
  /*--------------------------------------------------*/
  
  gApplication->Terminate();
}


