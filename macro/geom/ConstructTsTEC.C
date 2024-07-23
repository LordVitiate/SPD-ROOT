
void Standard(Int_t goption = 1);
void Custom(Int_t goption = 1);
void Custom_1(Int_t goption = 1);

void TestMapper(Int_t geotype /* 1,2,3 */);
void TestBuilder();

//______________________________________________________________
void ConstructTsTEC() 
{
   //TestMapper(3); return;  
  
   const Int_t goption = 1; // (1 = hybrid, 2 = toroidal)
  
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   if (goption == 1) SpdCommonGeoMapper::Instance()->DefineHybGeometrySet();
 
   //SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
 
//    SpdPipe* Pipe = new SpdPipe();
//    Pipe->ConstructGeometry();
//    Pipe->Print("");
   
//    if (goption == 1) 
//    {
//        SpdHybMagnet* Magnet = new SpdHybMagnet();
//        Magnet->SetGeometryType(1);   // 1, 2    
//        Magnet->ConstructGeometry();   
//        Magnet->Print("");
//    }
//    else {
//        SpdTorMagnet* Magnet = new SpdTorMagnet();
//        Magnet->ConstructGeometry();   
//        Magnet->Print("");
//    }
//  
//    SpdEcalTEC* ecal_ecps = new SpdEcalTEC();
//    ecal_ecps->ConstructGeometry();
// 
//   SpdTsTBGeoBuilder::Instance()->SetMaxBuildGeoLevel(2);
//   SpdTsTB* tstb = new SpdTsTB();
//   tstb->ConstructGeometry();   
//  
//   SpdIts* vxd = new SpdIts(); 
//   vxd->ConstructGeometry();
   
   
   /* ~~~~~~~~~~~~~ BUILD TSTEC ~~~~~~~~~~~ */
   // mapper settings:
   //
   
   //Standard(goption);
   Custom_1(goption);
   
   //SpdTsTECGeoBuilder::Instance()->SetMaxBuildGeoLevel(3); // FIXME!!! (max = 4)
   
   SpdTsTEC* tstec = new SpdTsTEC();
   tstec->ConstructGeometry();
   
   /* ~~~~~~~~~~~~~ PRINT GEOMETRY INFO ~~~~~~~~~~~~~~~~~ */
   
   SpdTsTECGeoMapper::Instance()->Print("");           // print all parameters
   SpdTsTECGeoMapper::Instance()->PrintVolPars(25); 
   SpdTsTECGeoMapper::Instance()->PrintGeoTable(2); 
   
   //TestBuilder();
   
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(3);
  
   geoMan->GetMasterVolume()->SetVisContainers();
   geoMan->GetMasterVolume()->Draw("ogl");
   return;
   
   //geoMan->GetMasterVolume()->SetVisContainers();
   //geoMan->GetMasterVolume()->Draw();
   //return;
   
//     TGeoVolume* vol = SpdTsTECGeoBuilder::Instance()->GetVolume("TsTECSModule1");
//     if (vol) {
//        vol->SetVisContainers();
//        vol->Draw("ogl");
//        return;
//     }
   
   //TGeoVolume* vol = SpdTsTECGeoBuilder::Instance()->GetVolume("TsTECSLayer1");
   //if (vol) vol->Draw("ogl");
   
   //TGeoVolume* vol = SpdTsTECGeoBuilder::Instance()->GetVolume("TsTECStraw2");
   //if (vol) vol->Draw("ogl");
  
}
//______________________________________________________________
void Standard(Int_t goption)
{
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   //                           BUILD TsTEC DETECTOR
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
   SpdTsTECGeoMapper* mapper = SpdTsTECGeoMapper::Instance();
   
   if (goption == 1) // hybrid geometry
   {
       //mapper->SetGeoType(3); 
          
       //mapper->SetGeometryPars(1); // 3x8+3x8 modules*layers (default)
       //mapper->SetGeometryPars(2); // 1+1 modules 
       mapper->SetGeometryPars(3); // 3x24+3x24 modules*layers 
       //mapper->SetGeometryPars(4); // 6x4+6x4 modules*layers 

   }
   else // toroidal geometry  
   { 
       //mapper->SetGeoType(1);  // "pgon" geometry (default)
       mapper->SetGeoType(2);  // "arb" geometry 
   }
}

//______________________________________________________________
void Custom(Int_t goption)
{
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   //                           BUILD TsTEC DETECTOR
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
   if (goption != 1) return;
   
   SpdTsTECGeoMapper* mapper = SpdTsTECGeoMapper::Instance();
        
   // set hybrid geometry
   mapper->SetGeoType(3);
   
   /* +++++++++++++++++ custom geometry +++++++++++++++++ */
   
   //SpdTsECStraw::SetMinStrawSize(20./*cm*/);
   
   //---------------------------------------------------------------
   // set straw radius; STRAW: type, parnum(0), radius > 0 (default = 0.5 cm (type 1), 0.4 cm (type 2))
   mapper->SetDefaultStrawPars(2, 0, 2 /*cm*/); 
   
   // set straw shell width; STRAW: type, parnum(1), shell width > 0 (default = 0.036 mm)
   mapper->SetDefaultStrawPars(2, 1, 0.5); 
   
   // set straw wire radius; STRAW: type, parnum(2), wire radius > 0 (default = 0.01 mm)
   mapper->SetDefaultStrawPars(2, 2, 0.2); 
     
   //---------------------------------------------------------------
   // set layer straw type; LAYER: type, parnum(0), straw type, 1 or 2 (default = 1)
   //mapper->SetDefaultLayerPars(1, 0, 2);    
   //mapper->SetDefaultLayerPars(2, 0, 2); 
   
   // set layer straw z-gap; LAYER: type, parnum(1), straw z-gap (default = 0)
   mapper->SetDefaultLayerPars(1, 1, 2. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 1, 2. /*cm*/); 
   
   // set layer straw r-gap; LAYER: type, parnum(2), straw r-gap  >= 0 (default = 0)
   mapper->SetDefaultLayerPars(1, 2, 1. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 2, 1. /*cm*/); 
   
   // set layer packing option; LAYER: type, parnum(3), value: 0 = 'eo', 1 = 'oe'
   mapper->SetDefaultLayerPars(1, 3, 0 /*0,1*/);    
   mapper->SetDefaultLayerPars(2, 3, 1 /*0,1*/); 
   
   // set layer dR; LAYER: type, parnum(4), straw dR >= 0 (default =  1 cm)
   mapper->SetDefaultLayerPars(1, 4, 5. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 4, 5. /*cm*/); 
   
   // set layer dB; LAYER: type, parnum(5), straw dB  >= 0 (default =  0.1 cm)
   mapper->SetDefaultLayerPars(1, 5, 2. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 5, 2. /*cm*/);
   
   // set layer radial size (R); LAYER: type, parnum(6), radius > bore size (default =  170 cm)
   mapper->SetDefaultLayerPars(1, 6, 160. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 6, 160. /*cm*/);
   
   // set layer bore size (B); LAYER: type, parnum(7), bore size > 0 (default =  10 cm)
   mapper->SetDefaultLayerPars(1, 7, 10. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 7, 10. /*cm*/);
   
   //---------------------------------------------------------------
   // set number of layers in the module; MODULE: type, parnum(1), number of layers
   mapper->SetDefaultModulePars(1, 1, 3);  
   mapper->SetDefaultModulePars(2, 1, 3);     
   
   // set module layer z-gap; MODULE: type, parnum(2), layer z-gap (default = 0)
   mapper->SetDefaultModulePars(1, 2, 45. /*cm*/); 
   mapper->SetDefaultModulePars(2, 2, 45. /*cm*/);
   
   // set local rotation angle step dphi = 45*(+-0,1,2,3); MODULE: type, parnum(3), (default = 1)
   mapper->SetDefaultModulePars(1, 3,  1);  
   mapper->SetDefaultModulePars(2, 3, -1); 
    
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   // set module/layer/straw parameters; TYPE: [module], [layer], [straw]
   mapper->SetParameters(1, 1, 2); 
   mapper->SetParameters(2, 2, 2); 
     
   // add modules: [forward(1)/backward(-1)], [module type], [number of modules], [angle (deg)]
   mapper->AddIdentModules( 1, 1, 1,   0. /*deg*/);
   mapper->AddIdentModules(-1, 2, 1, -45. /*deg*/);
   
   //mapper->AddForwardModule(1,0,0);
   //mapper->AddBackwardModule(2,0,45.);
   //mapper->RecalculateDistance();
}
//______________________________________________________________
void Custom_1(Int_t goption)
{
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   //                           BUILD TsTEC DETECTOR
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
   if (goption != 1) return;
   
   SpdTsTECGeoMapper* mapper = SpdTsTECGeoMapper::Instance();
        
   // set hybrid geometry
   mapper->SetGeoType(3);
   
   /* +++++++++++++++++ custom geometry +++++++++++++++++ */
   
   //SpdTsECStraw::SetMinStrawSize(20./*cm*/);
   
   //---------------------------------------------------------------
   // set straw radius; STRAW: type, parnum(0), radius > 0 (default = 0.5 cm (type 1), 0.4 cm (type 2))
   mapper->SetDefaultStrawPars(2, 0, 5. /*cm*/); 
   
   // set straw shell width; STRAW: type, parnum(1), shell width > 0 (default = 0.036 mm)
   mapper->SetDefaultStrawPars(2, 1, 1.); 
   
   // set straw wire radius; STRAW: type, parnum(2), wire radius > 0 (default = 0.01 mm)
   mapper->SetDefaultStrawPars(2, 2, 0.5); 
     
   //---------------------------------------------------------------
   // set layer straw type; LAYER: type, parnum(0), straw type, 1 or 2 (default = 1)
   //mapper->SetDefaultLayerPars(1, 0, 2);    
   //mapper->SetDefaultLayerPars(2, 0, 2); 
   
   // set layer straw z-gap; LAYER: type, parnum(1), straw z-gap (default = 0) // FIXME!!!
   mapper->SetDefaultLayerPars(1, 1, -100. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 1, -100. /*cm*/); 
   
   // set layer straw r-gap; LAYER: type, parnum(2), straw r-gap  >= 0 (default = 0)
   mapper->SetDefaultLayerPars(1, 2, 0. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 2, 10. /*cm*/); 
   
   // set layer packing option; LAYER: type, parnum(3), value: 0 = 'eo', 1 = 'oe'
   mapper->SetDefaultLayerPars(1, 3, 0 /*0,1*/);    
   mapper->SetDefaultLayerPars(2, 3, 1 /*0,1*/); 
   
   // set layer dR; LAYER: type, parnum(4), straw dR >= 0 (default =  1 cm)
   mapper->SetDefaultLayerPars(1, 4, 5. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 4, 5. /*cm*/); 
   
   // set layer dB; LAYER: type, parnum(5), straw dB  >= 0 (default =  0.1 cm)
   mapper->SetDefaultLayerPars(1, 5, 5. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 5, 5. /*cm*/);
   
   // set layer radial size (R); LAYER: type, parnum(6), radius > bore size (default =  170 cm)
   mapper->SetDefaultLayerPars(1, 6, 160. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 6, 160. /*cm*/);
   
   // set layer bore size (B); LAYER: type, parnum(7), bore size > 0 (default = 10 cm)
   mapper->SetDefaultLayerPars(1, 7, 5. /*cm*/);    
   mapper->SetDefaultLayerPars(2, 7, 5. /*cm*/);
   
   //---------------------------------------------------------------
   // set number of layers in the module; MODULE: type, parnum(1), number of layers
   mapper->SetDefaultModulePars(1, 1, 1);  
   mapper->SetDefaultModulePars(2, 1, 1);     
   
   // set module layer z-gap; MODULE: type, parnum(2), layer z-gap (default = 0)
   mapper->SetDefaultModulePars(1, 2, 45. /*cm*/); 
   mapper->SetDefaultModulePars(2, 2, 45. /*cm*/);
   
   // set local rotation angle step dphi = 45*(+-0,1,2,3); MODULE: type, parnum(3), (default = 1)
   mapper->SetDefaultModulePars(1, 3,  1);  
   mapper->SetDefaultModulePars(2, 3, -1); 
    
   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   // set module/layer/straw parameters; TYPE: [module], [layer], [straw]
   mapper->SetParameters(1, 1, 2); 
   mapper->SetParameters(2, 2, 2); 
     
   // add modules: [forward(1)/backward(-1)], [module type], [number of modules], [angle (deg)]
   mapper->AddIdentModules( 1, 1, 1,  0. /*deg*/);
   mapper->AddIdentModules(-1, 2, 1, 90. /*deg*/);
   
//    mapper->AddForwardModule(1,0,0);
//    mapper->AddForwardModule(1,0,0);
//    mapper->AddBackwardModule(2,0,45.);
//    mapper->AddBackwardModule(2,0,45.);
//    mapper->RecalculateDistance();
}


//______________________________________________________________
void TestMapper(Int_t geotype /*1,2,3*/) 
{
   SpdTsTECGeoMapper* mapper = SpdTsTECGeoMapper::Instance();
 
   if (geotype == 3) 
   {
       // set hybrid geometry settings
       SpdCommonGeoMapper::Instance()->DefineHybGeometrySet(2); // 1 (no geometry), 2 (default)
       mapper->InitGeometry(3); // 3 (1,2 => 0)
   }
   else if (geotype == 1 || geotype == 2) 
   {
       // toroidal geomery settings 
       mapper->InitGeometry(geotype); // 1,2 (3 => 0)
   }

   mapper->Print("");
   mapper->PrintVolPars(); 
   mapper->PrintGeoTable(); 
   
   //SpdParSet* params = new SpdParSet();
   //mapper->FillParametersIn(params);
   //params->print(1);  
}

//______________________________________________________________
void TestBuilder()
{
   SpdTsTECGeoBuilder* builder = SpdTsTECGeoBuilder::Instance();
   //builder->Print("");
   
   SpdGeoMapper* mapper = builder->GetMapper();
   //mapper->Print("");
   
   cout << "GEOMETRY TYPE: " << mapper->GetGeoType() << endl;
   
   if (mapper->GetGeoType() != 1) return;
}

