
void Standard(Int_t gtype);
void Custom();

//______________________________________________________________
void ConstructTsHB() {
   //       
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdCommonGeoMapper::Instance()->DefineHybGeometrySet();
   
//    SpdPipe* Pipe = new SpdPipe();
//    Pipe->ConstructGeometry();
//    Pipe->Print("");
//    
//    SpdHybMagnet* Magnet = new SpdHybMagnet();
//    Magnet->SetGeometryType(1);       
//    Magnet->ConstructGeometry();   
//    Magnet->Print("");
  
   /* +++++++++ BUILD & TUNE TST BARREL ++++++++++++ */ 
   
   // /* change number of sectors */
   //SpdCommonGeoMapper::Instance()->SetNGeoSectors(9);  // > 2 && < 13
 
   Standard(2);
   //Custom();
   
   /* ~~~~~~~~~~~~~ PRINT GEOMETRY INFO ~~~~~~~~~~~~~~~~~ */
   
   SpdTsTBGeoMapper::Instance()->Print("");          // print all parameters
   SpdTsTBGeoMapper::Instance()->PrintGeoTable(2);   // parameter = geometry level(default value <1: full table)
   SpdTsTBGeoMapper::Instance()->PrintVolPars(110);  // number of volumes to print (default value <1: full table)
  
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
  
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
void Standard(Int_t gtype)
{  
   SpdTsTBGeoMapper* mapper = SpdTsTBGeoMapper::Instance();
   mapper->SetGeoType(gtype);
      
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
   
   TString volpath;
   Double_t pos[3];
   
   volpath = "/cave_1/TsTBModule1_1";
   
   Builder->SetNodePath(volpath);
   Builder->GetDetectorPosition(pos);
   
   cout << "node path: " << volpath 
        << ";  position (" << pos[0] << ",   " << pos[1] << ",   " << pos[2] << ") [cm]"
        << ";  unique id = " << geotable->GetNodeTableNum(volpath) << endl;
    
   TString vpath;
   
   for (Int_t i(0); i<95; i++) {
   
        vpath = volpath + Form("/TsTBLayer%d_1",i+1);

        Builder->SetNodePath(vpath);
        Builder->GetDetectorPosition(pos);

        cout << "node path: " << vpath 
             << ";  position = (" << pos[0] << ",   " << pos[1] << ",   " << pos[2] << ") [cm]"
             << ";  unique id = " << geotable->GetNodeTableNum(vpath) << endl;
   }
   
   if (gtype != 1) return;

//  98   1  95    20 [    1    20]   TsTBLayer100         TsTBModule2          [    96    115]
//  99   1  96    20 [    1    20]   TsTBLayer101         TsTBModule2          [   116    135]
// 100   1  97    20 [    1    20]   TsTBLayer102         TsTBModule2          [   136    155]
// 101   1  98    20 [    1    20]   TsTBLayer103         TsTBModule2          [   156    175]
// 102   1  99    21 [    1    21]   TsTBLayer96          TsTBModule2          [   176    196]
// 103   1 100    21 [    1    21]   TsTBLayer97          TsTBModule2          [   197    217]
// 104   1 101    21 [    1    21]   TsTBLayer98          TsTBModule2          [   218    238]
// 105   1 102    21 [    1    21]   TsTBLayer99          TsTBModule2          [   239    259]

// 106   1 103    20 [   21    40]   TsTBLayer100         TsTBModule3          [   260    279]
// 107   1 104    20 [   21    40]   TsTBLayer101         TsTBModule3          [   280    299]
// 108   1 105    20 [   21    40]   TsTBLayer102         TsTBModule3          [   300    319]
// 109   1 106    20 [   21    40]   TsTBLayer103         TsTBModule3          [   320    339]
// 110   1 107    21 [   22    42]   TsTBLayer96          TsTBModule3          [   340    360]
// 111   1 108    21 [   22    42]   TsTBLayer97          TsTBModule3          [   361    381]
// 112   1 109    21 [   22    42]   TsTBLayer98          TsTBModule3          [   382    402]
// 113   1 110    21 [   22    42]   TsTBLayer99          TsTBModule3          [   403    423]

   cout << endl;
   
   volpath = "/cave_1/TsTBModule2_1";
   
   Int_t N = 99;
   Int_t nmin = 1;
   Int_t nmax = 21;
   
//    volpath = "/cave_1/TsTBModule3_1";
//    
//    Int_t N = 96;
//    Int_t nmin = 22;
//    Int_t nmax = 42;
   
   Builder->SetNodePath(volpath);
   Builder->GetDetectorPosition(pos);
   
   cout << "node path: " << volpath 
        << ";  position (" << pos[0] << ",   " << pos[1] << ",   " << pos[2] << ") [cm]"
        << ";  unique id = " << geotable->GetNodeTableNum(volpath) << endl;
  
   for (Int_t i(nmin); i <= nmax; i++) {
   
        vpath = volpath + Form("/TsTBLayer%d_%d",N,i);

        Builder->SetNodePath(vpath);
        Builder->GetDetectorPosition(pos);

        cout << "node path: " << vpath 
             << ";  position = (" << pos[0] << ",   " << pos[1] << ",   " << pos[2] << ") [cm]"
             << ";  unique id = " << geotable->GetNodeTableNum(vpath) << endl;
   }
   
   cout << endl;
   
   for (Int_t i(96); i<104; i++) {
   
        vpath = volpath + Form("/TsTBLayer%d_%d",i,nmin);

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
   path = geotable->GetNodeGeoPath(1);
   cout << "\t tid = " << geotable->GetNodeTableNum(path) << " path = " << path << endl; 
  
}

//________________________________________________________________________________________________
void Custom()
{
   SpdTsTBGeoMapper* mapper = SpdTsTBGeoMapper::Instance();
   mapper->SetGeoType(1);
  
   mapper->SetStrawModulePars(1);
   
    /*******************************************************************************************/
    /* module parameters: 1. module number, 
    *                     2. packing angle (deg: 0,180) 
    *                        (the angle between layer z-axis and module z-axis)
    *                     3. layer shift = 'o' (= 0), 'e' ( = -0.5*(layer width + layer gap) ), 
    *                     4. layer width (cm), 
    *                     5. the gap beetween layers (cm)*/
    /*******************************************************************************************/
    
     mapper->SetStrawModulePars(1,    0., 'o',  5, 10);  // central module
     mapper->SetStrawModulePars(2,  135., 'e',  5, 20);  // z+ module
     mapper->SetStrawModulePars(3,   45., 'e',  5, 20);  // z- module
   
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
    
         mapper->SetStrawLayerPars(i,  0., 'e', 5.0, 1.);
         mapper->SetStrawLayerPars(i, 45., 'o', 5.0, 1.);
         mapper->SetStrawLayerPars(i, 90., 'e', 5.0, 1.);
         mapper->SetStrawLayerPars(i,  0., 'o', 5.0, 1.);
         mapper->SetStrawLayerPars(i, 45., 'e', 5.0, 1.); 
         mapper->SetStrawLayerPars(i, 90., 'o', 5.0, 1.); 
         mapper->SetStrawLayerPars(i,  0., 'e', 5.0, 1.);
         mapper->SetStrawLayerPars(i, 45., 'o', 5.0, 1.);
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







