
//______________________________________________________________
void ConstructTsSB() {
   //       
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   //SpdCommonGeoMapper::Instance()->SetNGeoSectors(8);
     
   /* +++++++++ TST (BARREL) ++++++++++++ */ 
   
   SpdTsSBGeoMapper* mapper = SpdTsSBGeoMapper::Instance();
   mapper->SetGeoType(1);
   
    /*******************************************************************************************/
    /* module parameters: 1. packing angle (deg: 0,180) 
    *                        (the angle between layer z-axis and module z-axis)
    *                     2. layer shift = 'o' (= 0), 'e' ( = -0.5*(layer width + layer gap) ), 
    *                     3. layer width (cm), 
    *                     4. the gap beetween layers (cm)*/
    /*******************************************************************************************/
   
//   mapper->SetStrawModulePars(0., 'o', 4.7, 4.7);

   //mapper->SetStrawModulePars(0., 'e', 5, 0);
    
//     /*******************************************************************************************/
//     /* layer parameters: 1. packing angle (deg: 0,180) 
//     *                       (the angle between straw z-axis and layer z-axis)
//     *                    2. straw shift = 'o' (= 0), 'e' ( = -0.5*(straw width + straw gap) ), 
//     *                    3. straw width (cm), 
//     *                    4. the gap beetween straws (cm)*/
//     /*******************************************************************************************/

         
   /* pack module by an arbitrary number of a different layers in sequence */
    
//    mapper->SetStrawLayerPars(  0., 'e', 4.7, 5.);
//    mapper->SetStrawLayerPars( 45., 'o', 4.7, 5.);
//    mapper->SetStrawLayerPars( 90., 'e', 4.7, 5.);
//    mapper->SetStrawLayerPars(  0., 'o', 4.7, 5.);
//    mapper->SetStrawLayerPars( 45., 'e', 4.7, 5.); 
//    mapper->SetStrawLayerPars( 90., 'o', 4.7, 5.); 
//    mapper->SetStrawLayerPars(  0., 'e', 4.7, 5.);
//    mapper->SetStrawLayerPars( 45., 'o', 4.7, 5.);

   //++++++++++++++++++
   //   BUILD BARREL  
   //++++++++++++++++++
    
   SpdTsSB* ts_barrel = new SpdTsSB();     
  
   /* ~~~~~~~~~~~~~ DEFAULT PARAMETERS CHANGING ~~~~~~~~~~~~~~~~~ */
 
   SpdParameter* par;
 
   par = SpdTsSBGeoMapper::Instance()->AddParameter("TsSBStrawWireRadius");
   *par = 0.5; // -> cm
   
   //mapper->UnsetMaterials(1);
   
   ts_barrel->ConstructGeometry(); 
      
   /* Print geometry info */
   
   //SpdCommonGeoMapper::Instance()->PrintGeometryList();
   //SpdCommonGeoMapper::Instance()->PrintGeometry();
  
   //SpdTsSBGeoMapper::Instance()->PrintGeoTable(2);
   //SpdTsSBGeoMapper::Instance()->PrintVolPars(150); 
  
   /* DRAW GEOMETRY */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
   geoMan->GetMasterVolume()->Draw("ogl");
   
   //geoMan->GetMasterVolume()->Draw();
 
  // TGeoVolume* vol = SpdTsSBGeoBuilder::Instance()->GetVolume("TsSBModule1");
  // if (vol) vol->Draw("ogl");
   
  // TGeoVolume* vol = SpdTsSBGeoBuilder::Instance()->GetVolume("TsSBLayer1");
  // if (vol) vol->Draw("ogl");
    
  // TGeoVolume* vol = SpdTsSBGeoBuilder::Instance()->GetVolume("TsSBStraw1");
  // if (vol) vol->Draw("ogl");
   
  // return;
  
}


