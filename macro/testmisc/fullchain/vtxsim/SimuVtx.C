
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

void CustomItsGeometry(Int_t option);
void CustomTsGeometry();

//_________________________________________________________________________
void SimuVtx(Int_t nEvents = 1)
{
  TString outFile = "run.root";    // Output data file name
  TString parFile = "params.root"; // Output parameters file name
   
  SpdRunSim* run = new SpdRunSim();
  
  run->SetName("TGeant4");       
  run->SetMaterials("media.geo");
  run->SetPythiaDecayer("DecayConfig.C");
  run->SetOutputFile(outFile);      
  run->SetMCEventHeader(new SpdMCEventHeader);
  
   /*--------------------------------------------------*/
  /* +++++++++     GEOMETRY (QSOLENOID)      ++++++++ */
  /*--------------------------------------------------*/
  
  SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
  
  /* ++++++++++++++++++ CAVE ++++++++++++++++++ */
  
  FairModule* cave = new SpdCave("CAVE");
  //cave->SetGeometryFileName("cave.geo");          // air (default) FIXME
  //cave->SetGeometryFileName("cave_precise.geo");  // vacuum FIXME
  run->AddModule(cave);
  
  /* ++++++++++++++++++ PIPE ++++++++++++++++++ */
  
  SpdPipe* Pipe = new SpdPipe();
  run->AddModule(Pipe);

    
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
  SpdTsTB*     ts_barrel   = new SpdTsTB();      /* +++++++++ TS (BARREL) ++++++++++++ */ 
  run->AddModule(ts_barrel);  
  
  SpdTsTEC*    ts_ecps     = new SpdTsTEC();     /* +++++++++ TS (ENDCAPS) +++++++++++ */     
  run->AddModule(ts_ecps);   
  
  /* ===== Vertex detector ===== */
    
  SpdIts* its = new SpdIts(); 
  run->AddModule(its);
  
  //its->GetMapper()->UnsetMaterials("vacuum"); FIXME

  /*--------------------------------------------------*/
  /* +++++++++++++ GEOMETRY PARAMETERS ++++++++++++++ */
  /*--------------------------------------------------*/
 
  //-------------------------------------------------
  // c.t. = chip thickness 
  // option: 0 => DSSD (c.t. = 300 mkm; 5 layers), default geometry ; 
  //         1 => DSSD (c.t. = 50 mkm; 5 layers); 
  //         2 => MAPS (c.t. = 50 mkm; 5 layers);
  //         3 => MAPS (c.t. = 50 mkm; 1,2,3 layer) + DSSD (c.t. = 300 mkm; 4,5 layer);
  
  CustomItsGeometry(0);  
   
  CustomTsGeometry();

  /*--------------------------------------------------*/
  /* +++++++++ OPTIONS FOR DATA SAVING ++++++++++++++ */
  /*--------------------------------------------------*/
  
  //its->SaveEmptyHits();
  //ts_barrel->SaveEmptyHits();
  //ts_ecps->SaveEmptyHits();
  
  /*--------------------------------------------------*/
  /* ++++++++++++++   MAGNETIC FIELD   ++++++++++++++ */
  /*--------------------------------------------------*/
  
  /* --- axial field map --- */
  SpdAxialFieldMap* MagField = new SpdAxialFieldMap("QSolenoidal field");
  MagField->InitData("map_qsolRZ_6cls2cm.bin");
  
  //MagField->MultiplyField(0.8); // FIXME

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
      
//       SpdPythia6Generator* P6gen = new SpdPythia6Generator();
//       
//       //-----------------------------------------------------------
//       // Option: (decayer for vertex meson- and baryon- resonances)
//       // 0 = Transport generator + External decayer (particles don't decay in the vertex) 
//       // 1 = Primary generator (DEFAULT value, more safely)
//       
//       P6gen->SetVGenOpt(1);
//         
//       //-----------------------------------------------------------
//       // Set seed:
//       
//       gRandom->SetSeed(0);
//       Int_t seed = gRandom->Integer(Int_t(1e9));
//       P6gen->SetSeed(seed,0);
//        
//       //P6gen->SetSeed(0);
//       //P6gen->SetSeed(144135822, 2);
//       
//       //P6gen->SetSeed(12995,0);
//       //P6gen->SetSeed(19949278, 1);
//       //P6gen->SetSeed(13495);
//       //P6gen->SetSeed(127472);
//       
// 
//       /* --- Set Pythia6 options --- */
//       SpdPythia6* gg = (SpdPythia6*)P6gen->GetGenerator();
//        
//       gg->SetMSEL(1);  // set miminum bias 
// 
//        
//       /* --- Pythia6 initialization --- */
//       P6gen->Initialize("cms","p","p",27/*GeV*/);
//        
//       primGen->AddGenerator(P6gen);
//      
//       primGen->SetVerboseLevel(-10);
//       primGen->SetVerbose(0);
       
  
     //===========================
     //   ISOTROPIC GENERATOR 
     //===========================
   
      //Int_t seed = 0;
      Int_t seed = 216166;
      gRandom->SetSeed(seed);
  
      SpdIsotropicGenerator* isogen1 = new SpdIsotropicGenerator("isogen1");
   
      isogen1->Initialize(13,1.0,10);  // pdg number, momentum (GeV/c), particles per event or LEVEL
      isogen1->SetVerboseLevel(1);
      isogen1->SetVGenOpt(1);  // (+-)1, (+-)2
   
      primGen->AddGenerator(isogen1);
      
      SpdIsotropicGenerator* isogen2 = new SpdIsotropicGenerator("isogen2");
   
      isogen2->Initialize(-13,1.0,10);  // pdg number, momentum (GeV/c), particles per event or LEVEL
      isogen2->SetVerboseLevel(1);
      isogen2->SetVGenOpt(1);  // (+-)1, (+-)2
   
      primGen->AddGenerator(isogen2);
  
      //============================
      
   run->SetGenerator(primGen);

   primGen->SetVerboseLevel(-10);
   primGen->SetVerbose(0);
      
   primGen->SetBeam(0., 0., 0.1, 0.1); // (X,Y)- position, (X,Y)- (2*delta or sigma) [cm]
   primGen->SmearVertexXY(kTRUE);
   
   primGen->SetTarget(0.,5.);  // Z- position, 2*delta or sigma [cm]
   primGen->SmearVertexZ(kTRUE);
 
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

//________________________________________________________________________________________________
void CustomItsGeometry(Int_t option)
{
   if (option < 1) return;
       
   Double_t mm = 0.1;  
   
   SpdItsGeoMapperX* mapper = SpdItsGeoMapperX::Instance();   
       
   //============================================================================
   
   if (option == 1) 
   {
       //----------------------------------------------
       // Number of layers = 5
       // Type of chips (all layers): DSSD (all layers)
       // Chips thickness(all layers): 50 mkm
       //----------------------------------------------
       
       // layers:   
       mapper->SetDefaultLadderPars(3, 3, 0.05*mm);
    
       mapper->SetGeometryPars(3); // set new parameters !
   
       return;
   }

   if (option == 2) 
   {
       //----------------------------------------------
       // Number of layers = 5
       // Type of chips (all layers): MAPS (all layers)
       // Chips thickness (all layers): 50 mkm
       //----------------------------------------------
       
       // layers:
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
   
       return;
   }
   
   if (option == 3) 
   {
       // layers:   
       mapper->SetDefaultLadderPars(3, 1, 2, 15*mm);
       mapper->SetDefaultLadderPars(3, 1, 3, 0.05*mm);
       mapper->SetDefaultLadderPars(3, 1, 4, 0);
       mapper->SetDefaultLadderPars(3, 1, 6, 30);
                                    
       mapper->SetDefaultLadderPars(3, 2, 2, 15*mm);
       mapper->SetDefaultLadderPars(3, 2, 3, 0.05*mm);
       mapper->SetDefaultLadderPars(3, 2, 4, 0);
       mapper->SetDefaultLadderPars(3, 2, 6, 57);
       
       mapper->SetDefaultLadderPars(3, 3, 2, 15*mm);
       mapper->SetDefaultLadderPars(3, 3, 3, 0.05*mm);
       mapper->SetDefaultLadderPars(3, 3, 4, 0);
       mapper->SetDefaultLadderPars(3, 3, 6, 77);
       
       mapper->SetGeometryPars(3,1); // set new parameters !
       
       return;
   }
}

//________________________________________________________________________________________________
void CustomTsGeometry()
{
   //------------------------------------------
   // TsTEC configuration 
   //------------------------------------------  
   // option = 1: 2 endcaps x 3 modules, 1 module =  8x2 = 16 layers, total = 48 layers (default)
   // option = 2: 2 endcaps x 2 modules, 1 module =  8x2 = 16 layers, total = 32 layers  
   // option = 3: 2 endcaps x 1 modules, 1 module = 26x2 = 52 layers, total = 52 layers (actual!)
   
   SpdTsTECGeoMapper::Instance()->SetGeometryPars(3); 
  
   return;
   
   //------------------------------------------
   // TsTB configuration 
   //------------------------------------------   
    
   SpdTsTBGeoBuilder* builder = SpdTsTBGeoBuilder::Instance();
     
   // ---------------------------------------------------
   // AddRejection(volname ,nmin, nmax, lcycle).
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
   
//     builder->AddRejection("Layer", 9, 95);   
//     builder->AddRejection("Layer", 16);
//     builder->AddRejection("Layer", 15, 95);
//     builder->AddRejection("Layer", 35, 45);
//     builder->AddRejection("Layer", 56, 67);
//     builder->AddRejection("Layer", 80, 88);
      
  
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
}



