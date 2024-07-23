
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
void SimuV0IdealExam(Int_t nEvents = 200)
{
   TString outFile = "run_torV0exam.root";     // Output data file name
   TString parFile = "params_torV0exam.root";  // Output parameters file name
   
   SpdRunSim* run = new SpdRunSim();
  
   run->SetName("TGeant4");       
   run->SetMaterials("media.geo");
  
   run->SetOutputFile(outFile);      
  
   TString decayer = "DecayConfigSpecV0.C";
   run->SetPythiaDecayer(decayer);
 
   run->SetMCEventHeader(new SpdMCEventHeader);
 
   /*--------------------------------------------------*/
   /* +++++++++     GEOMETRY (HYBRID)      +++++++++++ */
   /*--------------------------------------------------*/
  
   SpdCommonGeoMapper::Instance()->DefineHybGeometrySet();

//   SpdCommonGeoMapper::Instance()->UnsetMaterials(1);   // all geom is vacuum
  
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
   SpdConstField* MagField = new SpdConstField();  
   MagField->SetField(0., 0. , 10.0); //  kG

   /* --- field map --- */
//   SpdFieldMap* MagField = new SpdFieldMap("Hybrid field");
  
  // MagField->InitData("map_hyb_1T2cm.bin");       // standard "hybrid" field
//   MagField->InitData("map_sol_6cls5cm2.bin");  // "quasi-solenoid"
  
  // MagField->SetApproxMethod(0); // 0 (linear,default) or 1 (nearest vertex)
  // MagField->MultiplyField(1);

//   MagField->MultiplyField(1);
  
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

   //============================
   //    PYTHIA 6 GENERATOR
   //============================
      
   SpdPythia6Generator* P6gen = new SpdPythia6Generator();
      
   //-----------------------------------------------------------
   // Option: (decayer for vertex meson- and baryon- resonances)
   // 0 = Transport generator + External decayer (particles don't decay in the vertex) 
   // 1 = Primary generator (DEFAULT value, more safely)
      
   P6gen->SetVGenOpt(1);

   //-----------------------------------------------------------
   // Set seed:
   gRandom->SetSeed(54545);
      
   Int_t seed = gRandom->Integer(Int_t(1e9));
   //P6gen->SetSeed(seed);
   P6gen->SetSeed(13495);

   /* --- Set Pythia6 options --- */
   SpdPythia6* gg = (SpdPythia6*)P6gen->GetGenerator();
       
   gg->SetMSEL(1);  // set miminum bias 
       
   /* --- Pythia6 initialization --- */
   P6gen->Initialize("cms","p","p",27/*GeV*/);
       
   primGen->AddGenerator(P6gen);

   //===========================
   //   ISOTROPIC GENERATOR (1)
   //===========================

   SpdIsotropicGenerator* isogen = new SpdIsotropicGenerator("isogen");
      
//   isogen->Initialize(310, 1.5, 1);  // Ks^0
//   isogen->Initialize(421, 1.5, 1);  // D^0
//   isogen->Initialize(411, 1.5, 1);  // D^+
   isogen->Initialize(413, 1.5, 1);  // D*+
   
   isogen->SetVerboseLevel(1);
   isogen->SetSeed(39211); 
//   isogen->SetVGenOpt(-1);  // (+-)1, (+-)2
   isogen->SetVGenOpt(1);  // (+-)1, (+-)2

//   isogen->SetSpherical(70.0, 110.0, 0.0, 360.0); // theta[min,max], phi[min,max]
   primGen->AddGenerator(isogen);

   /* run generator */

   run->SetGenerator(primGen);  
  
   primGen->SetVerboseLevel(-10);
//   primGen->SetVerbose(0);
      
//   primGen->SetBeam(0., 0., 0.1, 0.1); // (X,Y)- position, (X,Y)- (0.5*delta or sigma) [cm]
   primGen->SetBeam(0., 0., 0.0, 0.0); // (X,Y)- position, (X,Y)- (0.5*delta or sigma) [cm]
   primGen->SmearVertexXY(kFALSE);
//   primGen->SmearVertexXY(kTRUE);
//   primGen->SmearGausVertexXY(kTRUE);
   
//   primGen->SetTarget(0.,5.);  // Z- position, 0.5*delta or sigma [cm]
   primGen->SetTarget(0., 0.);  // Z- position, 0.5*delta or sigma [cm]
   primGen->SmearVertexZ(kFALSE);
   // primGen->SmearVertexZ(kTRUE);
   // primGen->SmearGausVertexZ(kTRUE);
 
   /* ------------------------------------------------ */ 
   /* +++++++++++++++ GLOBAL OPTIONS +++++++++++++++++ */
   /* ------------------------------------------------ */ 
  
   //run->SetStoreTraj(kTRUE);
 
//   SpdCommonGeoMapper::Instance()->SaveEmptyHits();
  
   //SpdCommonGeoMapper::Instance()->UnsetMaterials(1);  // set all materials = vacuum (precise)
      
   /* ----------------------------------------------------------------------- */ 
   /* >>>>>>>>>>>>>>>>>>>>>>>>>>> INITALIZE RUN <<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
   /* ----------------------------------------------------------------------- */ 
  
   cout << "\n\t" << "++++++++++++++++++++++++++++++++++++++++++++"   << endl;
   cout << "\t"   << "+                                          +"   << endl;
   cout << "\t"   << "+           Init Run (start)               +"   << endl; 
   cout << "\t"   << "+                                          +"   << endl;
   cout << "\t"   << "++++++++++++++++++++++++++++++++++++++++++++\n" << endl;
     
   run->Init();
  
   cout << "\n\t" << "++++++++++++++++++++++++++++++++++++++++++++"   << endl;
   cout << "\t"   << "+                                          +"   << endl;
   cout << "\t"   << "+           Init Run (finish)              +"   << endl; 
   cout << "\t"   << "+                                          +"   << endl;
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





