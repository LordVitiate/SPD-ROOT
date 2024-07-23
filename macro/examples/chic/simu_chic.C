
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
void simu_chic(Int_t nEvents = 10, UInt_t seed = 0)
{
  TString outFile = "run.root";    // Output data file name
  TString parFile = "params.root"; // Output parameters file name
   
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
  
  SpdPipe* Pipe = new SpdPipe();
  run->AddModule(Pipe);
  
  /* +++++++++ MAGNET +++++++++ */

  SpdHybMagnet* Magnet = new SpdHybMagnet();
  run->AddModule(Magnet);
    
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
 
  SpdTsTB*     ts_barrel   = new SpdTsTB();      /* +++++++++ TS (BARREL) ++++++++++++ */ 
  SpdTsTEC*    ts_ecps     = new SpdTsTEC();     /* +++++++++ TS (ENDCAPS) +++++++++++ */     
  SpdEcalTB2*  ecal_barrel = new SpdEcalTB2();   /* +++++++++ ECAL (BARREL) ++++++++++ */      
  SpdEcalTEC2* ecal_ecps   = new SpdEcalTEC2();  /* +++++++++ ECAL (ENDCAPS) +++++++++ */  
  SpdRsTB2*    rs_barrel   = new SpdRsTB2();     /* +++++++++ RS (BARREL) ++++++++++++ */  
  SpdRsTEC2*   rs_ecps     = new SpdRsTEC2();    /* +++++++++ RS (ENDCAPS) +++++++++++ */ 
  
  run->AddModule(ts_barrel);
  run->AddModule(ts_ecps);  
  run->AddModule(ecal_barrel);
  run->AddModule(ecal_ecps);
  run->AddModule(rs_barrel);
  run->AddModule(rs_ecps);  
  
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
  SpdAxialFieldMap* MagField = new SpdAxialFieldMap("QSolenoidal field");
  MagField->InitData("map_qsolRZ_6cls2cm.bin");
  //MagField->MultiplyField(0.8);

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
      P8gen -> SetBeam(2212,2212,27.); // p+p, E_cms
      
      // Define start seed (optionally, default: random value)
      P8gen -> SetSeed(seed);
      
      P8gen -> SetParameters("Charmonium:gg2ccbar(3PJ)[3PJ(1)]g = off,on,off");
      P8gen -> SetParameters("Charmonium:qg2ccbar(3PJ)[3PJ(1)]q = off,on,off");
      P8gen -> SetParameters("Charmonium:qqbar2ccbar(3PJ)[3PJ(1)]g = off,on,off");
      P8gen -> SetParameters("Charmonium:gg2ccbar(3PJ)[3S1(8)]g = off,on,off");
      P8gen -> SetParameters("Charmonium:qg2ccbar(3PJ)[3S1(8)]q = off,on,off");
      P8gen -> SetParameters("Charmonium:qqbar2ccbar(3PJ)[3S1(8)]g = off,on,off");

      // chi_c decays to gamma + J/psi
      P8gen -> SetParameters("20443:oneChannel = 1 1 0 22 443");

      // J/psi decays to mu+ mu-
      P8gen -> SetParameters("443:onMode = off");
      P8gen -> SetParameters("443:onIfAny = 13 -13");

      //P8gen->SetVerboseLevel(-1);

      primGen->AddGenerator(P8gen);
   
   //-----------------------------------
   run->SetGenerator(primGen);

   primGen->SetBeam(0., 0., 0.05, 0.05); // (X,Y)- position, (X,Y)- (2*delta or sigma) [cm]
//   primGen->SmearVertexXY(kTRUE);
   primGen->SmearGausVertexXY(kTRUE);
    
   primGen->SetTarget(0.,20.);  // Z- position, 2*delta or sigma [cm]
//   primGen->SmearVertexZ(kTRUE);
   primGen->SmearGausVertexZ(kTRUE);
      
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
  
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is             " << outFile << endl;
  cout << "Parameter file is          " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl;
  cout << endl;

  /*--------------------------------------------------*/
  
  gApplication->Terminate();
}


