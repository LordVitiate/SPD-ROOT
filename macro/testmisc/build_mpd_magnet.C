
//______________________________________________________________
void build_mpd_magnet() {
   //       
   FairGeoLoader* loader = new FairGeoLoader("TGeo","Geo Loader");
   FairGeoInterface* GeoInterface = loader->getGeoInterface();
   GeoInterface->setMediaFile("../geometry/media.geo"); 
   GeoInterface->readMedia(); 
   
   //FairGeoMedia* media = GeoInterface->getMedia();
   //media->print();
   
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   FairDetector* sts = new SpdSts("STS",kFALSE);
   
   sts->SetGeometryFileName("mpd_magnet_v4_0.geo");
  
   sts->ConstructGeometry();
   
   //--------------------------------------------------
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
   //geoMan->GetMasterVolume()->Draw();
   geoMan->GetMasterVolume()->Draw("ogl");
}