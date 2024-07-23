
void pipe();

void magnet_tor();
void magnet_sol();
void magnet_hyb();

void pipe_volume();

//________________________________________________________________________
void build_pipe() 
{
   FairGeoLoader* loader = new FairGeoLoader("TGeo","Geo Loader");
   FairGeoInterface* GeoInterface = loader->getGeoInterface();
   GeoInterface->setMediaFile("../geometry/media.geo"); 
   GeoInterface->readMedia(); 
   
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   //--------------------------------------------------
   
   pipe();
   //pipe_volume(); return;
   
   //--------------------------------------------------
   
   //return;
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(3);
   geoMan->GetMasterVolume()->Draw("ogl");
   
   SpdCommonGeoMapper::Instance()->PrintGeometryList();
}

//________________________________________________________________________
void pipe()
{
   SpdPipe* Pipe = new SpdPipe();
   
   //Pipe->Print();
   
   //Pipe->SetPipeType("tor");
   //Pipe->SetPipeType("sol");
   //Pipe->SetPipeType("general");
   //Pipe->SetPipeType("hyb");
   
   //Pipe->SetPipeMaterial("silicon");
   
   Pipe->UnsetMaterials("vacuum");
 
   Pipe->ConstructGeometry();
   
   //Pipe->Print();
}

//________________________________________________________________________
void pipe_volume()
{
  gSystem->Load("libGeom.so");
  
  SpdPipe* Pipe = new SpdPipe();
   
  Pipe->ConstructGeometry();
   
  TGeoVolume* vol;
  
  vol = const_cast<TGeoVolume*>(Pipe->GetHybPipeISpace());
  //vol = const_cast<TGeoVolume*>(Pipe->GetHybPipeC());       
  //vol = const_cast<TGeoVolume*>(Pipe->GetHybPipeLZminus());
  //vol = const_cast<TGeoVolume*>(Pipe->GetHybPipeLZPlus());  
  
  if (vol) vol->Draw("ogl");
  
}
