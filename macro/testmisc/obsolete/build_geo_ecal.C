// ROOT macro. Author: Artur Tkachenko

//______________________________________________________________
void build_geo_ecal() {
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
   
   //--------------------- EMC ------------------------
   
   MpdEmc* emc = new MpdEmc();
    
   //emc->SetGeometryFileName("emc_tr_xxx.geo");
   emc->SetGeometryFileName("emc_tr_400_3.geo");
   
   emc->ConstructGeometry();
   
   //-------------------- ECAL ------------------------

   SpdEcal* ecal = new SpdEcal();
   ecal->ConstructGeometry();
 
   //--------------------------------------------------
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
   geoMan->GetMasterVolume()->Draw();
   
      
}
