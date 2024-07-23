
//______________________________________________________________
void ConstructQslGeo()
{
   //       
   SpdCommonGeoMapper::Instance()->OpenGeometry();
   
   //FairGeoMedia* media = FairGeoLoader::Instance()->getGeoInterface()->getMedia();
   //media->print();
   
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
 
   /* +++++++++ PIPE +++++++++ */
   
   SpdPipe* Pipe = new SpdPipe();
   
   //Pipe->SetPipeMaterial("silicon");
   //Pipe->UnsetMaterials(1);
   
   /* +++++++++ MAGNET +++++++++ */

   SpdHybMagnet* Magnet = new SpdHybMagnet();
       
   /* +++++++++ DETECTORS +++++++++ */
   
   SpdRsTB2*    rs_barrel   = new SpdRsTB2();     /* +++++++++ RST (BARREL) ++++++++++++ */  
   SpdRsTEC2*   rs_ecps     = new SpdRsTEC2();    /* +++++++++ RST (ENDCAPS) +++++++++++ */
   SpdEcalTB2*  ecal_barrel = new SpdEcalTB2();   /* +++++++++ ECALT (BARREL) ++++++++++ */      
   SpdEcalTEC2* ecal_ecps   = new SpdEcalTEC2();  /* +++++++++ ECALT (ENDCAPS) +++++++++ */  
   SpdTsTB*     ts_barrel   = new SpdTsTB();      /* +++++++++ TST (BARREL) ++++++++++++ */ 
   SpdTsTEC*    ts_ecps     = new SpdTsTEC();     /* +++++++++ TST (ENDCAPS) +++++++++++ */
   SpdTofB*     tofb        = new SpdTofB();      /* +++++++++ TOF (BARREL) ++++++++++++ */
   SpdTofEC*    tofec       = new SpdTofEC();     /* +++++++++ TOF (ENDCAPS) +++++++++++ */
   SpdIts*      vxd         = new SpdIts();       /* +++++++++ VERTEX TRACKER ++++++++++ */
   
   SpdBbc*      bbc         = new SpdBbc();       /* +++++++++ BEAM-BEAM COUNTER +++++++ */
   SpdAeg*      aeg         = new SpdAeg();       /* +++++++++ AEROGEL +++++++++++++++++ */
   SpdZdc*      zdc         = new SpdZdc();       /* +++++++++ Z0 CALORIMETER ++++++++++ */
   
   /*--------------------------------------------------*/
   /* ++++++++++++  CASTOMIZE GEOMETRY   +++++++++++++ */
   /*--------------------------------------------------*/
   
   SpdParameter* par;
   
   //--------------------------------------------------------------------------
   // Chose TsTEC configuration 
   //   
   // option = 1: (2x)3 modules, 1 module =  8x2 = 16 layers, total = 48 layers (default)
   // option = 2: (2x)2 modules, 1 module =  8x2 = 16 layers, total = 32 layers  
   // option = 3: (2x)1 modules, 1 module = 26x2 = 52 layers, total = 52 layers (actual!)

   SpdTsTECGeoMapper::Instance()->SetGeometryPars(3); 

   /*++++++++++++++++++++++++++++++*/
   
   //SpdTsTBGeoBuilder::Instance()->SetMaxBuildGeoLevel(2); 
   //SpdTsTECGeoBuilder::Instance()->SetMaxBuildGeoLevel(2); 
   
//    SpdCommonGeoMapper::Instance()->UnsetMaterials(1);    
    SpdCommonGeoMapper::Instance()->ConstructGeometry(); 
  
    //SpdCommonGeoMapper::Instance()->PrintGeometryList();
    //SpdCommonGeoMapper::Instance()->PrintGeometry();
    
//   SpdTsTECGeoMapper::Instance()->PrintVolPars(); 
//   SpdTsTBGeoMapper::Instance()->PrintGeoTable(); 
    
    //SpdTsTECGeoBuilder::Instance()->SetMaxBuildGeoLevel(0); 
    //SpdTsTBGeoBuilder::Instance()->SetMaxBuildGeoLevel(0); 
  
  
  
   /* DRAW GEOMETRY */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
   geoMan->GetMasterVolume()->Draw("ogl");
   //geoMan->GetMasterVolume()->Draw();
   
   // return;
   
   /* Save Geometry into the file */
   
//    TFile* file = new TFile("geofile_full.root","RECREATE");
//    file->cd();
//    gGeoManager->Write();
//    file->Close();

   //geoMan->Export("geofile_full.root");  // save as *.root
   //geoMan->Export("geofile_full.gdml");  // save as *.gdml
 
}


