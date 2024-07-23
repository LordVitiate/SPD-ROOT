
//______________________________________________________________
void ConstructHybGeo() {
   //       
   SpdCommonGeoMapper::Instance()->OpenGeometry();
   
   //FairGeoMedia* media = FairGeoLoader::Instance()->getGeoInterface()->getMedia();
   //media->print();
   
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   Int_t HybGeoSetType = 2;  // 1, 2
   
   SpdCommonGeoMapper::Instance()->DefineHybGeometrySet(HybGeoSetType);
   
   //SpdCommonGeoMapper::Instance()->SetNGeoSectors(4);
 
   /* +++++++++ PIPE +++++++++ */
   
   SpdPipe* Pipe = new SpdPipe();
   
   //Pipe->SetPipeType("tor");
   //Pipe->SetPipeType("sol");
   //Pipe->SetPipeType("general");
   
   //Pipe->SetPipeMaterial("silicon");
   //Pipe->UnsetMaterials(1);
   
   /* +++++++++ MAGNET +++++++++ */
   
   SpdHybMagnet* Magnet = new SpdHybMagnet(); 
   
   //Magnet->SetGeometryType(1);             // type = 1 (hybrid) or type = 2 (qsolenoid)
         
   //Magnet->SetCoilsMaterial("silicon");  // see avaliable materials: geometry/media.geo
   //Magnet->UnsetMaterials(0);

  
//    /* +++++++++ DETECTORS +++++++++ */
//   
       SpdEcalTB*  ecal_barrel = new SpdEcalTB();   /* +++++++++ ECALT (BARREL) ++++++++++ */      
       SpdEcalTEC* ecal_ecps   = new SpdEcalTEC();  /* +++++++++ ECALT (ENDCAPS) +++++++++ */  
       SpdRsTB*    rs_barrel   = new SpdRsTB();     /* +++++++++ RST (BARREL) ++++++++++++ */  
       SpdRsTEC*   rs_ecps     = new SpdRsTEC();    /* +++++++++ RST (ENDCAPS) +++++++++++ */
       SpdTsTB*    ts_barrel   = new SpdTsTB();     /* +++++++++ TST (BARREL) ++++++++++++ */ 
      
       if (HybGeoSetType != 1) {
       
           SpdTsTEC* ts_ecps = new SpdTsTEC();    /* +++++++++ TST (ENDCAPS) +++++++++++ */
          
           // Use as TsTEC
           //Tracker* tracker = new Tracker(); 
           //tracker->SetDefaultGeometryParameters(2);
           //tracker->ConstructGeometry();
       }
  
      SpdIts* vxd = new SpdIts();    /* +++++++++ VERTEX TRACKER ++++++++++ */
      
      
   // SpdTsTECGeoMapper::Instance()->SetGeometryPars(1); // 1,2 
   // SpdTsTBGeoBuilder::Instance()->SetMaxBuildGeoLevel(2);
   
//   SpdCommonGeoMapper::Instance()->UnsetMaterials(1);   
    SpdCommonGeoMapper::Instance()->ConstructGeometry(); 
  
    //SpdCommonGeoMapper::Instance()->PrintGeometryList();
    //SpdCommonGeoMapper::Instance()->PrintGeometry();
//    
//   SpdTsTBGeoMapper::Instance()->PrintVolPars(); 
//   SpdTsTBGeoMapper::Instance()->PrintGeoTable(); 
  
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


