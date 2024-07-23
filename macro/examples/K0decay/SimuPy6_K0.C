
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
void SimuPy6_K0(Int_t nEvents = 100)
{
  TString outFile = "run_K0.root";    // Output data file name
  TString parFile = "params_K0.root"; // Output parameters file name
   
  SpdRunSim* run = new SpdRunSim();
  
  run->SetName("TGeant4");       
  run->SetMaterials("media.geo");
  
  SpdRootFileSink* OutputFile = new SpdRootFileSink(outFile);
  run->SetSink(OutputFile);
  
  //TString decayer = "DecayConfig.C";
  TString decayer = "DecayConfigSpecV0.C";
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
      //    PYTHIA 6 GENERATOR
      //============================
      
      SpdPythia6Generator* P6gen = new SpdPythia6Generator();
      
      /* --- Set seed --- */
      gRandom->SetSeed(0);
      //Int_t seed = gRandom->Integer(Int_t(1e9));
      //P6gen->SetSeed(seed,0);
      P6gen->SetSeed(127471);
      
      /* --- Set Pythia6 options --- */
      SpdPythia6* gg = (SpdPythia6*)P6gen->GetGenerator();
       
      gg->SetMSEL(1);  // set miminum bias 
       
      /* --- Pythia6 initialization --- */
      P6gen->Initialize("cms","p","p",27/*GeV*/);
       
      primGen->AddGenerator(P6gen);
     
      primGen->SetVerboseLevel(-10);
      primGen->SetVerbose(0);
       
      //------------------------------------------------
      // add inline generator with K^0
      //------------------------------------------------
      
      SpdIsotropicGenerator* isogen = new SpdIsotropicGenerator("isogen");
      isogen->SetSeed(39113);           // 1-nd

      isogen->Initialize(310, 1.5, 1);  // Ks^0
      isogen->SetVGenOpt(1);  // (+-)1, (+-)2

      primGen->AddGenerator(isogen);

   //------------------------------
      
   run->SetGenerator(primGen);

   primGen->SetBeam(0., 0., 0.1, 0.1); // (X,Y)- position, (X,Y)- (2*delta or sigma) [cm]
//   primGen->SetBeam(0., 0., 0.0, 0.0); // (X,Y)- position, (X,Y)- (2*delta or sigma) [cm]
   primGen->SmearVertexXY(kTRUE);
//   primGen->SmearVertexXY(kFALSE);
   //primGen->SmearGausVertexXY(kTRUE);
    
   primGen->SetTarget(0.,5.);  // Z- position, 2*delta or sigma [cm]
//   primGen->SetTarget(0.,0.);  // Z- position, 2*delta or sigma [cm]
   primGen->SmearVertexZ(kTRUE);
//   primGen->SmearVertexZ(kFALSE);
   //primGen->SmearGausVertexZ(kTRUE);
 
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


