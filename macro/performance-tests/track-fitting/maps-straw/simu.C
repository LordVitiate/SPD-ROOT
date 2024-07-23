
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

//_________________________________________________________________________
void simu(Int_t nEvents = 10)
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
  Pipe->SetPipeMaterial("beryllium",0);  // Central beampipe segment
  run->AddModule(Pipe);
  
  /* +++++++++ MAGNET +++++++++ */

  SpdSolMagnet2 * Magnet = new SpdSolMagnet2();
  run->AddModule(Magnet);
    
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
  
  SpdTsTB*     ts_barrel   = new SpdTsTB();      /* +++++++++ TS (BARREL) ++++++++++++ */ 
  SpdTsTEC*    ts_ecps     = new SpdTsTEC();     /* +++++++++ TS (ENDCAPS) +++++++++++ */     
  SpdTofB*     tof_barrel  = new SpdTofB();      /* +++++++++ TOF (BARREL) +++++++++++ */
  SpdTofEC*    tof_ecps    = new SpdTofEC();     /* +++++++++ TOF (ENDCAPS) ++++++++++ */
  SpdEcalB*    ecal_barrel = new SpdEcalB();     /* +++++++++ ECAL (BARREL) ++++++++++ */      
  SpdEcalEC*   ecal_ecps   = new SpdEcalEC();    /* +++++++++ ECAL (ENDCAPS) +++++++++ */  
  SpdRsTB2*    rs_barrel   = new SpdRsTB2();     /* +++++++++ RS (BARREL) ++++++++++++ */  
  SpdRsTEC2*   rs_ecps     = new SpdRsTEC2();    /* +++++++++ RS (ENDCAPS) +++++++++++ */ 

  SpdBbc*      bbc         = new SpdBbc();       /* +++++++++ BEAM-BEAM COUNTER +++++++ */
  SpdAeg*      aeg         = new SpdAeg();       /* +++++++++ AEROGEL +++++++++++++++++ */
  SpdZdc*      zdc         = new SpdZdc();       /* +++++++++ Z0 CALORIMETER ++++++++++ */
  
  run->AddModule(ts_barrel);
  run->AddModule(ts_ecps);  
  //run->AddModule(tof_barrel);
  //run->AddModule(tof_ecps);  
  //run->AddModule(ecal_barrel);
  //run->AddModule(ecal_ecps);
  //run->AddModule(rs_barrel);
  //run->AddModule(rs_ecps);  
  //
  //run->AddModule(bbc); // pink
  //run->AddModule(aeg); // green
  //run->AddModule(zdc);
  
  /* ===== Vertex detector ===== */
    
  SpdIts* its = new SpdIts(); 
  run->AddModule(its);
  
  //SpdItsGeoMapperX::Instance()->SetGeometryPars(1);
  //SpdItsGeoMapperX::Instance()->EnableEndcaps(); // enable(1)/disable(0) Inner Tracker (Its) endcaps ; default = enable
  
  //its->SaveEmptyHits();
  
  /*--------------------------------------------------*/
  /* ++++++++++++  CUSTOMIZE GEOMETRY   +++++++++++++ */
  /*--------------------------------------------------*/
  
  CustomIts(1);  // 1 - MAPS, 2 - DSSD

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
  MagField->SetApproxMethod(1);
   //MagField->MultiplyField(0.8);
  // this shoul not be needed, to check
  SpdRegion* reg = MagField->CreateFieldRegion("box");
  reg->SetBoxRegion(-330, 330, -330, 330, -386, 386); // (X,Y,Z)_(min,max), cm

  
  //SpdAxialFieldMap* MagField = new SpdAxialFieldMap("QSolenoidal field");
  //MagField->InitData("map_qsolRZ_6cls2cm.bin");
  //SpdRegion* reg = MagField->CreateFieldRegion("tube");
  //reg->SetTubeRegion(0, 174, -246, 246); // (R,Z)_(min,max), cm 

  
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
  
  //SpdItsGeoMapperX::Instance()->PrintVolPars(); 
  //SpdItsGeoMapperX::Instance()->PrintGeoTable(); 
  //SpdItsGeoMapperX::Instance()->Print("");
  
  /*--------------------------------------------------*/
  
  gApplication->Terminate();
}

//______________________________________________________________
void CustomIts(Int_t option) {
   Double_t mm = 0.1;  
   
   SpdItsGeoMapperX* mapper = SpdItsGeoMapperX::Instance(); 
   
   // See spdgeometry/its/SpdItsGeoMapperX.cxx for details
   //------------------------------------------------------------------------
   // List of default layer/ladder parameters (geom = [1,5], layer = [1,5]):
   //----------------------------------------------------------------------------
   // par[0] = Rlayer    : the distance from z axis to ladder center (LAYER radius)
   // par[1] = Lz        : ladder size along z axis
   // par[2] = Lphi      : ladder size along phi axis
   // par[3] = Lr        : radial size of ladder 
   // par[4] = angle     : local rotation angle for ladder
   // par[5] = Phi       : global rotation angle for LAYER
   // par[6] = Nladders  : number of ladders inside the LAYER (will be calculated automatically for geometry = 1,2)
   //-------------------------------------------------------------------------
   //static void SetDefaultLadderPars(Int_t geometry, Int_t layer, Int_t par, Double_t value);
   //static void SetDefaultLadderPars(Int_t geometry, Int_t par, Double_t value); // all layers
 
   //-------------------------------------------------------------------------
   // List of default ladder/chip parameters (geom = [1,5], layer = [1,5]): 
   //-------------------------------------------------------------------------
   // par[0] = lz        : chip size along z-axis
   // par[1] = lphi      : chip phi-size  
   // par[2] = dlz       : gap size between chips along z-axis
   // par[3] = dlphi     : gap size between chips along phi-axis
   // par[4] = ncz       : number of chip cells (channels) along z-axis
   // par[5] = ncphi     : number of chip cells (channels) along phi-axis
   // par[6] = type      : chip type (0: NO INNER STRUCTURE, 1: MAPS, 2: DSSD, >2: USER DEFINED)
   //-------------------------------------------------------------------------
   // static void SetDefaultChipPars(Int_t geometry, Int_t layer, Int_t par, Double_t value);
   // static void SetDefaultChipPars(Int_t geometry, Int_t par, Double_t value); // all layers
   
   //============================================================================

   if (option == 1) 
   {
       mapper->SetGeometryPars(1, 1); // set new parameters !
       mapper->EnableEndcaps(0);
       return;
   }

   
   if (option == 2)
   {
       mapper->SetGeometryPars(3,2); // set new parameters !
       mapper->EnableEndcaps(1);
       return;
   }
 
}
