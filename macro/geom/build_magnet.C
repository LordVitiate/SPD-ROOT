

void pipe();

void magnet_tor();
void magnet_sol();
void magnet_hyb();
void magnet_qsl();

//________________________________________________________________________
void build_magnet() 
{
   FairGeoLoader* loader = new FairGeoLoader("TGeo","Geo Loader");
   FairGeoInterface* GeoInterface = loader->getGeoInterface();
   GeoInterface->setMediaFile("../geometry/media.geo"); 
   GeoInterface->readMedia(); 
   
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   //--------------------------------------------------
   
   //pipe();
   
   //magnet_tor();
   //magnet_sol();
   //magnet_hyb();
   magnet_qsl();
   
   //--------------------------------------------------
   
   //return;
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
   geoMan->GetMasterVolume()->Draw("ogl");
   
   SpdCommonGeoMapper::Instance()->PrintGeometryList();
}

//________________________________________________________________________
void magnet_tor()
{
   SpdTorMagnet* Magnet = new SpdTorMagnet();
   
   Magnet->SetGeometryType(1);             // type = 1 (default) or type = 2
   
   //Magnet->SetAxialDistance(65.);               // [cm], distance >= 10 cm
   //Magnet->SetCoilsWidth(2.);                   // [cm], width >= 1 cm && (width > size(X,Y) - rounding)
   //Magnet->SetCoilsThickness(0.1);              // [cm], thickness >= 0.1 cm
   //Magnet->SetCoilsInnerRounding(0);            // [cm], rounding <= (size(X,Y)/2 - width) && (r > 0)  
   //Magnet->StretchCoilsAlongR(100/*dr, cm*/);   // [cm], new R_size >= width + rounding
   //Magnet->PurseCoilsAlongZ(30 /*dz, cm*/);     // [cm], new Z_size >= width + rounding
   //Magnet->SetCoilsMaterial("silicon");  // see avaliable materials in geometry/media.geo
   
   //Magnet->UnsetMaterials();
   
   Magnet->ConstructGeometry();
   
   //Magnet->Print();
   
   //Magnet->GetCoil(1)->Draw();
}

//________________________________________________________________________
void magnet_sol()
{
   SpdSolMagnet* Magnet = new SpdSolMagnet();
   
   Magnet->SetGeometryType(1);   // type = 1 (default) or type = 2
   
   //Magnet->SetWidth(50);         // [cm], width >= 1 cm 
   //Magnet->SetMaterial("iron");
   
   Magnet->ConstructGeometry();
   
   //Magnet->Print();
   
   //Magnet->GetSection()->Draw();
   
   //gGeoManager->GetMasterVolume()->Draw();
}

//________________________________________________________________________
void magnet_hyb()
{
   SpdHybMagnet* Magnet = new SpdHybMagnet();
   
   Magnet->SetGeometryType(1);            // geometry type = 1: hybrid, 
   //Magnet->SetGeometryType(2);            // geometry type = 2: qsolenoid-large
      
   Magnet->ConstructGeometry();
   
   //Magnet->Print("");
}

//________________________________________________________________________
void magnet_qsl()
{
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();  // default geometry type = 3:  qsolenoid-actual
   
   SpdHybMagnet* Magnet = new SpdHybMagnet();
  
   Magnet->ConstructGeometry();
   
   //Magnet->Print("");
}

//________________________________________________________________________
void pipe()
{
   SpdPipe* Pipe = new SpdPipe();
   
   //Pipe->Print();
   
   //Pipe->SetPipeType("tor");
   //Pipe->SetPipeType("sol");
   //Pipe->SetPipeType("general");
   //Pipe->SetPipeType("hybrid");
   
   //Pipe->SetPipeMaterial("silicon");
   
   //Pipe->UnsetMaterials("vacuum");
 
   Pipe->ConstructGeometry();
   
   //Pipe->Print();
}

