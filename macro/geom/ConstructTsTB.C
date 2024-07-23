
void Standard();

void Custom();

//______________________________________________________________
void ConstructTsTB() 
{
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   /* +++++++++ BUILD & TUNE TST BARREL ++++++++++++ */ 
   
   // /* change number of sectors */
   //SpdCommonGeoMapper::Instance()->SetNGeoSectors(9);  // > 2 && < 13
   
   //Standard();
   Custom();
   
   /* ~~~~~~~~~~~~~ PRINT GEOMETRY INFO ~~~~~~~~~~~~~~~~~ */
   
   //SpdTsTBGeoMapper::Instance()->Print("");           // print all parameters
   //SpdTsTBGeoMapper::Instance()->PrintGeoTable(2);  // parameter = geometry level(default value <1: full table)
   //SpdTsTBGeoMapper::Instance()->PrintVolPars(50);  // number of volumes to print (default value <1: full table)
  
    /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(3);
   
   geoMan->GetMasterVolume()->Draw("ogl");
   
   //geoMan->GetMasterVolume()->Draw();
   
   //TGeoVolume* vol = SpdTsTBGeoBuilder::Instance()->GetVolume("TsTBModule1");
   //if (vol) vol->Draw("ogl");
   
   //TGeoVolume* vol = SpdTsTBGeoBuilder::Instance()->GetVolume("TsTBLayer120");
   //if (vol) vol->Draw("ogl");
    
   //TGeoVolume* vol = SpdTsTBGeoBuilder::Instance()->GetVolume("TsTBStraw1");
   //if (vol) vol->Draw("ogl");
     
}

//________________________________________________________________________________________________
void Standard()
{  
   SpdTsTBGeoMapper* mapper = SpdTsTBGeoMapper::Instance();
   mapper->SetGeoType(1);
  
   // set standard "toroid" module parameters 
   mapper->SetStrawModulePars(1); 
      
   /* ~~~~~~~~~~~~~  MODIFY SOME PARAMETERS ~~~~~~~~~~~~~~~~~ */
   
   /* change minimal straw size [cm] (default: 40 cm) */
   //SpdTsBStraw::SetMinStrawSize(0.1); 
     
   /* change minimal straw wire radius (make it visible better) */
   SpdParameter* par;
   par = SpdTsTBGeoMapper::Instance()->AddParameter("TsTBStrawWireRadius");
   *par = 0.1; 
   
   /* set all materials = vacuum */
   //mapper->UnsetMaterials(1); 
   
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   //                           BUILD BARREL  
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   
   SpdTsTB* ts_barrel = new SpdTsTB();
  
   ts_barrel->ConstructGeometry();
   
   return;
   
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   /* >>>>>>>>>>>>>>>>>>>>>>>> TEST GEOTABLE TEST <<<<<<<<<<<<<<<<<<<<<<<< */
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
   SpdGeoTable* geotable = SpdTsTBGeoMapper::Instance()->GetGeoTable();
  
   // >>>>>>>>>> (TEST ) NODE PARAMETERS (TEST) <<<<<<<<<<
   SpdTsTBGeoBuilder* Builder =  SpdTsTBGeoBuilder::Instance();
   
   TString volpath = "/cave_1/TsTBModule1_1";
   Double_t pos[3];
    
   Builder->SetNodePath(volpath);
   Builder->GetDetectorPosition(pos);
   
   cout << "node path: " << volpath 
        << ";  position (" << pos[0] << ",   " << pos[1] << ",   " << pos[2] << ") [cm]"
        << ";  unique id = " << geotable->GetNodeTableNum(volpath) << endl;
    
   TString vpath ;
   
   for (Int_t i(0); i<120; i++) {
   
        vpath = volpath + Form("/TsTBLayer%d_1",i+1);

        Builder->SetNodePath(vpath);
        Builder->GetDetectorPosition(pos);

        cout << "node path: " << vpath 
             << ";  position = (" << pos[0] << ",   " << pos[1] << ",   " << pos[2] << ") [cm]"
             << ";  unique id = " << geotable->GetNodeTableNum(vpath) << endl;
   }
   // <<<<<<<<<< (TEST) NODE PARAMETERS  (TEST) <<<<<<<<<<
 
   //return;
   
   Long_t  tid;   // node unique id
   TString path;  // node full path
    
   path = "/cave_1/TsTBModule1_6/TsTBLayer2_1/TsTBStraw11_163";
   
   cout << "\n" << path << endl;
   tid = geotable->GetNodeTableNum(path);
   path = geotable->GetNodeGeoPath(tid);
   cout << "\t tid = " << tid << " path = " << path << endl;  
 
   path = "/cave_1/TsTBModule1_6/TsTBLayer2_1";
   
   cout << "\n" << path << endl;
   tid = geotable->GetNodeTableNum(path);
   path = geotable->GetNodeGeoPath(tid);
   cout << "\t tid = " << tid << " path = " << path << endl;
   
   path = "/cave_1/TsTBModule3_8";
   
   cout << "\n" << path << endl;
   tid = geotable->GetNodeTableNum(path);
   path = geotable->GetNodeGeoPath(tid);
   cout << "\t tid = " << tid << " path = " << path << endl;
   
   path = "/cave_1/TsTBModule1_8/TsTBLayer9_1";
   
   cout << "\n" << path << endl;
   tid = geotable->GetNodeTableNum(path);
   path = geotable->GetNodeGeoPath(tid);
   cout << "\t tid = " << tid << " path = " << path << endl;
   
   //---------------------------------------------------------------------------------------------
   // search for TOP level volume id
   //
   path = "/cave_1";
   
   cout << "\n" << path << endl;
   tid = geotable->GetNodeTableNum(path);
   path = geotable->GetNodeGeoPath(tid);
   cout << "\t tid = " << tid << " path = " << path << endl;
   
   tid = 1;
   path = geotable->GetNodeGeoPath(tid);
   cout << "\t tid = " << geotable->GetNodeTableNum(path) << " path = " << path << endl; 
   
   //---------------------------------------------------------------------------------------------
   // 6275   3 1274    1 [    1     1]   TsTBStraw89Shell     TsTBStraw89          [  1275   1275]
   // 1217   2 1078    1 [    2     2]   TsTBStraw89          TsTBLayer127         [  4632   4632]
   //  137   1 134    21 [   22    42]   TsTBLayer127         TsTBModule3          [   419    439]
   //    2   0   2     8 [    1     8]   TsTBModule3          cave                 [    17     24]
   
   path = "/cave_1/TsTBModule3_6/TsTBLayer127_25/TsTBStraw89_2/TsTBStraw89Shell_1";
   
   cout << "\n" << path << endl;
   tid = geotable->GetNodeTableNum(path);
   path = geotable->GetNodeGeoPath(tid);
   cout << "\t tid = " << tid << " path = " << path << endl;
   cout << endl;
}

//________________________________________________________________________________________________
void Custom()
{
   SpdTsTBGeoMapper* mapper = SpdTsTBGeoMapper::Instance();
   mapper->SetGeoType(1);
   
   // set standard "toroid" module parameters 
   mapper->SetStrawModulePars(1);
   
    /*******************************************************************************************/
    /* module parameters: 1. module number, 
    *                     2. packing angle (deg: 0,180) 
    *                        (the angle between layer z-axis and module z-axis)
    *                     3. layer shift = 'o' (= 0), 'e' ( = -0.5*(layer width + layer gap) ), 
    *                     4. layer width (cm), 
    *                     5. the gap beetween layers (cm)*/
    /*******************************************************************************************/
    
     mapper->SetStrawModulePars(1,    0., 'o',  10, 10);  // central module
     mapper->SetStrawModulePars(2,  135., 'e',  10, 20);  // z+ module
     mapper->SetStrawModulePars(3,   45., 'e',  10, 20);  // z- module
   
//     //mapper->SetStrawModulePars(1,  0., 'e', 5, 5);
//     //mapper->SetStrawModulePars(2,  0., 'e', 5, 5);
//     //mapper->SetStrawModulePars(3,  0., 'e', 5, 5);
//     
//     /*******************************************************************************************/
//     /* layer parameters: 1. module number
//     *                    2. packing angle (deg: 0,180) 
//     *                       (the angle between straw z-axis and layer z-axis)
//     *                    3. straw shift = 'o' (= 0), 'e' ( = -0.5*(straw width + straw gap) ), 
//     *                    4. straw width (cm), 
//     *                    5. the gap beetween straws (cm)*/
//     /*******************************************************************************************/
//     
    for (int i(1); i<=3; i++) { // -> modules
         
         /* pack module by an arbitrary number of a different layers in sequence */
    
         mapper->SetStrawLayerPars(i,  0., 'e', 10.0, 1.);
         mapper->SetStrawLayerPars(i, 45., 'o', 10.0, 1.);
         mapper->SetStrawLayerPars(i, 90., 'e', 10.0, 1.);
         mapper->SetStrawLayerPars(i,  0., 'o', 10.0, 1.);
         mapper->SetStrawLayerPars(i, 45., 'e', 10.0, 1.); 
         mapper->SetStrawLayerPars(i, 90., 'o', 10.0, 1.); 
         mapper->SetStrawLayerPars(i,  0., 'e', 10.0, 1.);
         mapper->SetStrawLayerPars(i, 45., 'o', 10.0, 1.);
    }
//     for (int i(1); i<=3; i++) { // -> modules
// 
//          /* pack module by an arbitrary number of a different layers in sequence */
//
//          mapper->SetStrawLayerPars(i,  90., 'e', 5.0, 0.);
//          mapper->SetStrawLayerPars(i,  90., 'o', 5.0, 0.);
//          mapper->SetStrawLayerPars(i,  45., 'e', 5.0, 0.);
//          mapper->SetStrawLayerPars(i,  45., 'o', 5.0, 0.);
//          mapper->SetStrawLayerPars(i,   0., 'e', 5.0, 0.); 
//          mapper->SetStrawLayerPars(i,   0., 'o', 5.0, 0.); 
//          mapper->SetStrawLayerPars(i, -45., 'e', 5.0, 0.);
//          mapper->SetStrawLayerPars(i, -45., 'o', 5.0, 0.);
//     }

   
   /* ~~~~~~~~~~~~~  MODIFY SOME PARAMETERS ~~~~~~~~~~~~~~~~~ */
   
   /* change minimal straw size [cm] (default: 40 cm) */
   //SpdTsBStraw::SetMinStrawSize(0.1); 
   
   /* change minimal straw wire radius (make it visible better) */
   SpdParameter* par;
   par = SpdTsTBGeoMapper::Instance()->AddParameter("TsTBStrawWireRadius");
   *par = 0.5;  
   
   /* set all materials = vacuum */
   //mapper->UnsetMaterials(1); 
   
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   //                           BUILD BARREL  
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   
   SpdTsTB* ts_barrel = new SpdTsTB();   
   
   ts_barrel->ConstructGeometry();
}

