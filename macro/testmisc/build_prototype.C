
//______________________________________________________________
void build_prototype() 
{
   //       
   FairGeoLoader* loader = new FairGeoLoader("TGeo","Geo Loader");
   FairGeoInterface* GeoInterface =loader->getGeoInterface();
   GeoInterface->setMediaFile("../geometry/media.geo"); 
   GeoInterface->readMedia(); 
   
   //FairGeoMedia* media = GeoInterface->getMedia();
   //media->print();
   
   //--------------------- CAVE -----------------------
   
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdBarrel* Barrel = new SpdBarrel();
 
   Barrel->SetBuildGeoOption("toroid");
 
   Barrel->ConstructGeometry();
   
   //FairModule* Magnet = new SpdMagnet("MAGNET");
   //Magnet->ConstructGeometry();
   
  
   //--------------------------------------------------
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(1);
   geoMan->GetMasterVolume()->Draw("ogl");

}
   