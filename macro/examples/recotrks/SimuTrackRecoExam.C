
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
void SimuTrackRecoExam(Int_t nEvents = 200)
{
//  TString outFile = "runMB_L4exam.root";    // Output data file name
//  TString parFile = "paramsMB_L4exam.root"; // Output parameters file name

  TString outFile = "runMu_L4exam.root";    // Output data file name
  TString parFile = "paramsMu_L4exam.root"; // Output parameters file name
   
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
  run->AddModule(Pipe);
  
  /* +++++++++ MAGNET +++++++++ */

  SpdSolMagnet2 * Magnet = new SpdSolMagnet2();
  run->AddModule(Magnet);
    
  /* +++++++++++++++++ DETECTORS ++++++++++++++ */
  
  SpdTsTB*     ts_barrel   = new SpdTsTB();      /* +++++++++ TS (BARREL) ++++++++++++ */ 
  SpdTsTEC*    ts_ecps     = new SpdTsTEC();     /* +++++++++ TS (ENDCAPS) +++++++++++ */     
//  SpdTofB*     tof_barrel  = new SpdTofB();      /* +++++++++ TOF (BARREL) +++++++++++ */
//  SpdTofEC*    tof_ecps    = new SpdTofEC();     /* +++++++++ TOF (ENDCAPS) ++++++++++ */
//  SpdEcalB*    ecal_barrel = new SpdEcalB();     /* +++++++++ ECAL (BARREL) ++++++++++ */      
//  SpdEcalEC*   ecal_ecps   = new SpdEcalEC();    /* +++++++++ ECAL (ENDCAPS) +++++++++ */  
//  SpdRsTB2*    rs_barrel   = new SpdRsTB2();     /* +++++++++ RS (BARREL) ++++++++++++ */  
//  SpdRsTEC2*   rs_ecps     = new SpdRsTEC2();    /* +++++++++ RS (ENDCAPS) +++++++++++ */ 

//  SpdBbc*      bbc         = new SpdBbc();       /* +++++++++ BEAM-BEAM COUNTER +++++++ */
//  SpdAeg*      aeg         = new SpdAeg();       /* +++++++++ AEROGEL +++++++++++++++++ */
//  SpdZdc*      zdc         = new SpdZdc();       /* +++++++++ Z0 CALORIMETER ++++++++++ */
  
  run->AddModule(ts_barrel);
  run->AddModule(ts_ecps);  

//  run->AddModule(tof_barrel);
//  run->AddModule(tof_ecps);  
//  run->AddModule(ecal_barrel);
//  run->AddModule(ecal_ecps);
//  run->AddModule(rs_barrel);
//  run->AddModule(rs_ecps);  
  
//  run->AddModule(bbc);
//  run->AddModule(aeg);
//  run->AddModule(zdc);
  
  /* ===== Vertex detector ===== */
    
  SpdIts* its = new SpdIts(); 
  run->AddModule(its);
  
  //SpdItsGeoMapperX::Instance()->SetGeometryPars(1);
  //SpdItsGeoMapperX::Instance()->EnableEndcaps(); // enable(1)/disable(0) Inner Tracker (Its) endcaps ; default = enable
  
  //its->SaveEmptyHits();
  
  /*--------------------------------------------------*/
  /* ++++++++++++  CUSTOMIZE GEOMETRY   +++++++++++++ */
  /*--------------------------------------------------*/
  
  CustomIts(1);            // MAPS, 4 layers 
//  CustomIts(-1);           // DSSD, 5 layers 
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
  
//  SpdFieldMap1_8* MagField = new SpdFieldMap1_8("full_map");
//  MagField->InitData("field_full1_8.bin");

  /* --- const field --- */
  SpdConstField* MagField = new SpdConstField();  
  MagField->SetField(0., 0., 10.0); //  kG
//  MagField->SetField(0., 0., 12.0); //  kG

//  SpdAxialFieldMap* MagField = new SpdAxialFieldMap("QSolenoidal field");
//  MagField->InitData("map_qsolRZ_6cls2cm.bin");

  SpdRegion* reg = MagField->CreateFieldRegion("box");
  reg->SetBoxRegion(-330, 330, -330, 330, -386, 386); // (X,Y,Z)_(min,max), cm
  
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
      
      // Define beams
      P8gen->SetBeam(2212,2212,27.); // pdg(A), pdg(B), E_cms (CM energy of collision)
      
      // Define start seed (optionally, default: random value)
      //P8gen->SetSeed(75122656); // < 900000000
        
//      Int_t option = -1;
      Int_t option = 4;               // inline geberator
      
      if (option == -1) {  
          // Minimum bias 
          P8gen->SetParameters("SoftQCD:singleDiffractive = on");
          P8gen->SetParameters("SoftQCD:doubleDiffractive = on");
          P8gen->SetParameters("SoftQCD:centralDiffractive = on");
          P8gen->SetParameters("SoftQCD:nonDiffractive = on");
          P8gen->SetParameters("SoftQCD:inelastic = on");
          //P8gen->SetParameters("SoftQCD:elastic = on");
      }
      if (option == 0) {
          // Production processes J/psi
          P8gen->SetParameters("Charmonium:gg2ccbar(3S1)[3S1(1)]g = on,off");
          P8gen->SetParameters("Charmonium:gg2ccbar(3S1)[3S1(8)]g = on,off");
          P8gen->SetParameters("Charmonium:qg2ccbar(3S1)[3S1(8)]q = on,off");
          P8gen->SetParameters("Charmonium:qqbar2ccbar(3S1)[3S1(8)]g = on,off");
          P8gen->SetParameters("Charmonium:gg2ccbar(3S1)[1S0(8)]g = on,off");
          P8gen->SetParameters("Charmonium:qg2ccbar(3S1)[1S0(8)]q = on,off");
          P8gen->SetParameters("Charmonium:qqbar2ccbar(3S1)[1S0(8)]g = on,off");
          P8gen->SetParameters("Charmonium:gg2ccbar(3S1)[3PJ(8)]g = on,off");
          P8gen->SetParameters("Charmonium:qg2ccbar(3S1)[3PJ(8)]q = on,off");
          P8gen->SetParameters("Charmonium:qqbar2ccbar(3S1)[3PJ(8)]g = on,off");

          // Jpsi decays to mu+ mu-
          P8gen->SetParameters("443:onMode = off");
          P8gen->SetParameters("443:onIfAny = 13 -13");
      }
      else if (option == 1) {
          // Open charm (heavy flavour states)
          P8gen->SetParameters("HardQCD:gg2ccbar = on");
          P8gen->SetParameters("PhaseSpace:pTHatMin = 1"); // default = 0
      }
      else if (option == 2) {
          // Promt photons   
          P8gen->SetParameters("PromptPhoton:qg2qgamma = on");        // q+g    -> q+gamma
          P8gen->SetParameters("PromptPhoton:qqbar2ggamma = on");     // q+qbar -> g+gamma
          P8gen->SetParameters("PromptPhoton:gg2ggamma = on");        // g+g    -> g+gamma
          P8gen->SetParameters("PromptPhoton:ffbar2gammagamma = on"); // q+qbar -> gamma+gamma
          P8gen->SetParameters("PromptPhoton:gg2gammagamma = on");    // g+g    -> gamma+gamma
      }
      else if (option == 3)
      {
          P8gen->SetParameters("SoftQCD:elastic = on");
      }
      else if (option == 4)
      {
         SpdIsotropicGenerator* isogen = new SpdIsotropicGenerator("isogen");
         isogen->SetSeed(73773);           

         isogen->Initialize(13, 1.0, 4);  // mu , momentum (GeV/c), number of particles per event
         isogen->SetVGenOpt(1);           // isotrpically distributed

         primGen->AddGenerator(isogen);

         SpdIsotropicGenerator* isogen1 = new SpdIsotropicGenerator("isogen1");
         isogen1->SetSeed(73774);

         isogen1->Initialize(-13, 1.0, 4);  // mu+ , momentum (GeV/c), number of particles per event
         isogen1->SetVGenOpt(1);            // isotrpically distributed

         primGen->AddGenerator(isogen1);
      }
      
      //P8gen->SetVerboseLevel(-1);
      if (option != 4) primGen->AddGenerator(P8gen);

   //-----------------------------------
   run->SetGenerator(primGen);

   primGen->SetBeam(0., 0., 0.1, 0.1); // (X,Y)- position, (X,Y)- (2*delta or sigma) [cm]
//   primGen->SmearVertexXY(kTRUE);
   primGen->SmearVertexXY(kFALSE);
   //primGen->SmearGausVertexXY(kTRUE);
    
   primGen->SetTarget(0.,5.);  // Z- position, 2*delta or sigma [cm]
//   primGen->SmearVertexZ(kTRUE);
   primGen->SmearVertexZ(kFALSE);
   //primGen->SmearGausVertexZ(kTRUE);
      
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
   //mapper->SetDefaultLadderPars(2, 1, 0,  48*mm);
   //mapper->SetDefaultLadderPars(2, 2, 0,  98*mm);
   //mapper->SetDefaultLadderPars(2, 3, 0, 148*mm);
   //mapper->SetDefaultLadderPars(2, 4, 0, 208*mm);
   mapper->SetDefaultLadderPars(2, 1, 0, 70*mm);
   mapper->SetDefaultLadderPars(2, 2, 0, 127*mm);
   mapper->SetDefaultLadderPars(2, 3, 0, 184*mm);
   mapper->SetDefaultLadderPars(2, 4, 0, 241*mm);
   mapper->SetDefaultLadderPars(2, 5, 0, 298*mm);
   
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


