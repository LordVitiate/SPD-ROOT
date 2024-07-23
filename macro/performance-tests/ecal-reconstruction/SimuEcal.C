
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

void CustomIts(Int_t option);
void CustomTsB(Double_t deg);

//_________________________________________________________________________
void SimuEcal(Int_t nEvents = 1, Int_t pdg = 22, TString outFile = "run.root", TString parFile = "params.root")
{

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
  Pipe->SetPipeMaterial("beryllium",0);  // Central beampipe segment
  run->AddModule(Pipe);
  
  /* +++++++++ MAGNET +++++++++ */

  SpdSolMagnet2 * Magnet = new SpdSolMagnet2();
  run->AddModule(Magnet);
    
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
  
  SpdEcalB*    ecal_barrel = new SpdEcalB();     /* +++++++++ ECAL (BARREL) ++++++++++ */      
  SpdEcalEC*   ecal_ecps   = new SpdEcalEC();    /* +++++++++ ECAL (ENDCAPS) +++++++++ */  

  run->AddModule(ecal_barrel);
  run->AddModule(ecal_ecps);

  // Add other modules?

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
  
  SpdFieldMap1_8* MagField = new SpdFieldMap1_8("full_map");
  MagField->InitData("field_full1_8.bin");
  SpdRegion* reg = MagField->CreateFieldRegion("box");
  reg->SetBoxRegion(-330, 330, -330, 330, -386, 386); // (X,Y,Z)_(min,max), cm
  
  run->SetField(MagField);
  
  MagField->Print();
  
  /*--------------------------------------------------*/
  /* ++++++++++ DEFINE PRIMARY GENERATORS +++++++++++ */
  /*--------------------------------------------------*/
 
  SpdPrimaryGenerator* primGen = new SpdPrimaryGenerator();
  

   SpdIsotropicGenerator* isogen = new SpdIsotropicGenerator("isogen");
   isogen->Initialize(pdg, 0.1, 1);  
   isogen->SetPgid(0.5, 14);
//   isogen->SetPVZSmearing(cpars->zsmear);

   primGen->AddGenerator(isogen);


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


