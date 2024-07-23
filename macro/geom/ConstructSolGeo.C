//#define _COMPILE_MACRO_

#if defined(_COMPILE_MACRO_) 

#include <TRint.h>
#include <TStopwatch.h>
#include <TGeoManager.h>
#include <TFile.h>

#include "SpdCommonGeoMapper.h"

#include "SpdCave.h"
#include "SpdPipe.h"
#include "SpdSolMagnet.h"

#include "SpdSts.h"
#include "SpdTsSB.h"
#include "SpdTsSEC.h"
#include "SpdEcalSB.h"
#include "SpdEcalSEC.h"
#include "SpdRsSB.h"
#include "SpdRsSEC.h"

#endif

//______________________________________________________________
void ConstructSolGeo() {
   //       
  
   SpdCommonGeoMapper::Instance()->OpenGeometry();
   
   //FairGeoMedia* media = FairGeoLoader::Instance()->getGeoInterface()->getMedia();
   //media->print();
   
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   //   0: pipe only
   //   1: standard geometry (geotype = 1)
   //   2: additional geometry (geotype = 2)
   // -11: passives only (geotype = 1)
   // -21: passives only (geotype = 2)
   //   3: hybrid compatible geometry
   
   SpdCommonGeoMapper::Instance()->DefineSolGeometrySet(1);
   
   //SpdCommonGeoMapper::Instance()->SetNGeoSectors(12);
   
   /* +++++++++ PIPE +++++++++ */
   
   SpdPassiveModule* Pipe = new SpdPipe();
   
   //Pipe->SetPipeType("tor");
   //Pipe->SetPipeType("sol");
   //Pipe->SetPipeType("general");  // default
   
   //Pipe->SetBaseMaterial("silicon");
   //Pipe->UnsetMaterials(1);
  
   /* +++++++++ MAGNET +++++++++ */
   
   SpdSolMagnet* Magnet = new SpdSolMagnet();
   
   //Magnet->SetGeometryType(1);   // type = 1 (default) or type = 2
   
   //Magnet->SetWidth(50);         // [cm], width >= 1 cm 
   //Magnet->SetMaterial("iron");  // material
   
   /* +++++++++ DETECTORS +++++++++ */
   
   SpdTsSB*    ts_barrel   = new SpdTsSB();       /* +++++++++ TS (BARREL) ++++++++++++ */
   SpdTsSEC*   ts_ecps     = new SpdTsSEC();      /* +++++++++ TS (ENDCAPS) +++++++++++ */ 
   SpdEcalSB*  ecal_barrel = new SpdEcalSB();     /* +++++++++ ECAL (BARREL) ++++++++++ */
   SpdEcalSEC* ecal_ecps   = new SpdEcalSEC();    /* +++++++++ ECAL (ENDCAPS) +++++++++ */
   SpdRsSB*    rs_barrel   = new SpdRsSB();       /* +++++++++ RS (BARREL) ++++++++++++ */
   SpdRsSEC*   rs_ecps     = new SpdRsSEC();      /* +++++++++ RS (ENDCAPS) +++++++++++ */
//   
//    /* ===== Vertex detector ===== */
//    
   FairDetector* sts = new SpdSts("STS",kFALSE);
   sts->SetGeometryFileName("sts.geo");
   sts->ConstructGeometry();
   
   //SpdTsSBGeoBuilder::Instance()->SetMaxBuildGeoLevel(2);
  
   //SpdCommonGeoMapper::Instance()->UnsetMaterials(1);
   
   SpdCommonGeoMapper::Instance()->ConstructGeometry(); 
 
   SpdCommonGeoMapper::Instance()->PrintGeometryList();
   SpdCommonGeoMapper::Instance()->PrintGeometry();
   
   //SpdTsSBGeoMapper::Instance()->PrintVolPars(); 
   //SpdTsSBGeoMapper::Instance()->PrintGeoTable(); 
   
  
   /* DRAW GEOMETRY */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
   geoMan->GetMasterVolume()->Draw("ogl");
   //geoMan->GetMasterVolume()->Draw();
   

   //return;
   
   /* Save Geometry into the file */
   
   TFile* file = new TFile("geofile_full.root","RECREATE");
   file->cd();
   gGeoManager->Write();
   file->Close();
}
