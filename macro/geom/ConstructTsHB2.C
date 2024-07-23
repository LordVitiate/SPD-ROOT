
void Standard(Int_t gtype);
void Custom();

//______________________________________________________________
void ConstructTsHB2() 
{
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   //SpdCommonGeoMapper::Instance()->DefineHybGeometrySet();
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();  

//    SpdPipe* Pipe = new SpdPipe();
//    Pipe->ConstructGeometry();
//    Pipe->Print("");
//    
//    SpdHybMagnet* Magnet = new SpdHybMagnet();
//    Magnet->SetGeometryType(1);       
//    Magnet->ConstructGeometry();   
//    Magnet->Print("");
   
   //SpdIts* vxd = new SpdIts(); 
   //vxd->ConstructGeometry();
  
   /* +++++++++ TUNE TST BARREL ++++++++++++ */ 
   
   //Custom();
   
   /* +++++++++ BUILD TST BARREL ++++++++++++ */ 
   
   //SpdTsTBGeoBuilder::Instance()->SetMaxBuildGeoLevel(4);
   
   SpdTsTB* ts_barrel = new SpdTsTB();   
   ts_barrel->ConstructGeometry();
   
   /* ~~~~~~~~~~~~~ PRINT GEOMETRY INFO ~~~~~~~~~~~~~~~~~ */
   
   SpdTsTBGeoMapper::Instance()->Print("");          // print all parameters
   SpdTsTBGeoMapper::Instance()->PrintGeoTable(2);   // parameter = geometry level(default value <1: full table)
   SpdTsTBGeoMapper::Instance()->PrintVolPars(110);  // number of volumes to print (default value <1: full table)
   
   
   //SpdTsTBGeoBuilder::Instance()->PrintRejection(1);
  
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
  
  geoMan->GetMasterVolume()->SetVisContainers();
  geoMan->GetMasterVolume()->Draw("ogl");
   
   //geoMan->GetMasterVolume()->Draw();
   
//    TGeoVolume* vol = SpdTsTBGeoBuilder::Instance()->GetVolume("TsTBModule1");
//    if (vol) {
//        vol->SetVisContainers();
//        vol->Draw("ogl");
//    }
   
   //TGeoVolume* vol = SpdTsTBGeoBuilder::Instance()->GetVolume("TsTBLayer3");
   //if (vol) vol->Draw("ogl");
   
   //TGeoVolume* vol = SpdTsTBGeoBuilder::Instance()->GetVolume("TsTBStraw1");
   //if (vol) vol->Draw("ogl");
  
}


//________________________________________________________________________________________________
void Custom()
{
    SpdTsTBGeoMapper* mapper = SpdTsTBGeoMapper::Instance();
   
    //mapper->SetGeoType(2);
    
    /* ~~~~~~~~~~~~~  MODIFY SOME PARAMETERS ~~~~~~~~~~~~~~~~~ */
   
    SpdParameter* par;
    
    /* remove airgap beetween modules (need to remove toroidal magnet!) */
    //par = mapper->AddParameter("TsTBMakeOffset2");
    //*par = kFALSE;
    
    // change module size long Z-axis
//     par = mapper->AddParameter("TsTBLen2");   // cm, default = 236
//     *par = 236.;
//    
//     // change module OUTER size
//     par = mapper->AddParameter("TsTBSize2");  // cm, default = 160
//     *par = 160.;
//     
//     // change module RADIAL size
//     par = mapper->AddParameter("TsTBWidth2"); // cm, default = 95
//     *par = 95.;
 
    /*******************************************************************************************/
    /* module parameters: 1. module number, 
    *                     2. packing angle (deg: 0,180) 
    *                        (the angle between layer z-axis and module z-axis)
    *                     3. layer shift = 'o' (= 0), 'e' ( = -0.5*(layer width + layer gap) ), 
    *                     4. layer width (cm), 
    *                     5. the gap beetween layers (cm)*/
    /*******************************************************************************************/
       
    mapper->SetStrawModulePars(1, 0., 'o',  1, 0);
    
    //mapper->SetStrawModulePars(1, 0., 'o',  1, 3);
    
    //mapper->SetStrawModulePars(1, 0., 'e',  5, 0);  
    
    //mapper->SetStrawModulePars(1, 0., 'o',  5, 0);  
    
    /*******************************************************************************************/
    /* layer parameters: 1. module number
    *                    2. packing angle (deg: 0,180) 
    *                       (the angle between straw z-axis and layer z-axis)
    *                    3. straw shift = 'o' (= 0), 'e' ( = -0.5*(straw width + straw gap) ), 
    *                    4. straw width (cm), 
    *                    5. the gap beetween straws (cm)*/
    /*******************************************************************************************/
  
    /* !!!!!!!!!!!!! DEFAULT STRAW PACKING CONFIGURATION !!!!!!!!!!!!!!!! */
    
//         mapper->SetStrawLayerPars(1,  90., 'e', 1.0, 0.);
//         mapper->SetStrawLayerPars(1,  90., 'o', 1.0, 0.);
//         mapper->SetStrawLayerPars(1,  45., 'e', 1.0, 0.);
//         mapper->SetStrawLayerPars(1,  45., 'o', 1.0, 0.);
//         mapper->SetStrawLayerPars(1,   0., 'e', 1.0, 0.); 
//         mapper->SetStrawLayerPars(1,   0., 'o', 1.0, 0.); 
//         mapper->SetStrawLayerPars(1, -45., 'e', 1.0, 0.);
//         mapper->SetStrawLayerPars(1, -45., 'o', 1.0, 0.);
//     
    
         
    /* pack module with an arbitrary number of a different layers in sequence */
    
//     mapper->SetStrawLayerPars(1, 90., 'e', 5.0, 1.);
//     mapper->SetStrawLayerPars(1, 90., 'o', 5.0, 1.);
//     mapper->SetStrawLayerPars(1, 45., 'e', 5.0, 1.);
//     mapper->SetStrawLayerPars(1, 45., 'o', 5.0, 1.);
//     mapper->SetStrawLayerPars(1,  0., 'e', 5.0, 1.); 
//     mapper->SetStrawLayerPars(1,  0., 'o', 5.0, 1.); 
//     mapper->SetStrawLayerPars(1,-45., 'e', 5.0, 1.);
//     mapper->SetStrawLayerPars(1, 45., 'o', 5.0, 1.);

//         mapper->SetStrawLayerPars(1,  90., 'e', 1.0, 0.);
//         mapper->SetStrawLayerPars(1,  90., 'o', 1.0, 0.);
//         mapper->SetStrawLayerPars(1,  0., 'e', 1.0, 0.);
//         mapper->SetStrawLayerPars(1,  0., 'o', 1.0, 0.);
   

   /* change minimal straw size [cm] (default = 40 cm) */
   
   SpdTsBStraw::SetMinStrawSize(0.1); 

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
    
  
}







