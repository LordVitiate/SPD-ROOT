
//______________________________________________________________
void ConstructTorGeo() {
   //       
   SpdCommonGeoMapper::Instance()->OpenGeometry();
   
   //FairGeoMedia* media = FairGeoLoader::Instance()->getGeoInterface()->getMedia();
   //media->print();
   
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   //   0: pipe only
   //   1: standard geometry (geotype = 1)
   //   2: "sectional" geometry (geotype = 2)
   // -11: passives only (geotype = 1)
   // -21: passives only (geotype = 2)
   //   3: hybrid compatible geometry
   
   SpdCommonGeoMapper::Instance()->DefineTorGeometrySet(1);
   
   //SpdCommonGeoMapper::Instance()->SetNGeoSectors(5);
 
   /* +++++++++ PIPE +++++++++ */
   
   SpdPipe* Pipe = new SpdPipe();
   
   //Pipe->SetPipeType("tor");
   //Pipe->SetPipeType("sol");
   //Pipe->SetPipeType("general");
   
   //Pipe->SetPipeMaterial("silicon");
   //Pipe->UnsetMaterials(1);
   
   /* +++++++++ MAGNET +++++++++ */
   
   SpdTorMagnet* Magnet = new SpdTorMagnet();
   
   //Magnet->SetCoilsMaterial("silicon");  // see avaliable materials: geometry/media.geo
   //Magnet->UnsetMaterials(1);
   
   /* +++++++++ BASKET +++++++++ */
   
   //SpdTorBasket* Basket = new SpdTorBasket(); 
   
   /* +++++++++ DETECTORS +++++++++ */
  
      SpdEcalTB*  ecal_barrel = new SpdEcalTB();   /* +++++++++ ECALT (BARREL) ++++++++++ */      
      SpdEcalTEC* ecal_ecps   = new SpdEcalTEC();  /* +++++++++ ECALT (ENDCAPS) +++++++++ */  
      SpdRsTB*    rs_barrel   = new SpdRsTB();     /* +++++++++ RST (BARREL) ++++++++++++ */  
      SpdRsTEC*   rs_ecps     = new SpdRsTEC();    /* +++++++++ RST (ENDCAPS) +++++++++++ */
      SpdTsTB*    ts_barrel   = new SpdTsTB();     /* +++++++++ TST (BARREL) ++++++++++++ */ 
      SpdTsTEC*   ts_ecps     = new SpdTsTEC();    /* +++++++++ TST (ENDCAPS) +++++++++++ */ 
  
   /* ===== Vertex detector ===== */
   
    FairDetector* sts = new SpdSts("STS",kFALSE);
    sts->SetGeometryFileName("sts.geo");
    sts->ConstructGeometry();
    
//    SpdTsTBGeoBuilder::Instance()->SetMaxBuildGeoLevel(2);
   
//    SpdCommonGeoMapper::Instance()->UnsetMaterials(1);
//    
    SpdCommonGeoMapper::Instance()->ConstructGeometry(); 
  
    SpdCommonGeoMapper::Instance()->PrintGeometryList();
    SpdCommonGeoMapper::Instance()->PrintGeometry();
//    
//    //SpdTsTBGeoMapper::Instance()->PrintVolPars(); 
//    SpdTsTBGeoMapper::Instance()->PrintGeoTable(); 
  
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
