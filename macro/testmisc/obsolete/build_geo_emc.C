// ROOT macro. Author: Artur Tkachenko

//______________________________________________________________
void build_geo_emc() 
{
   //       
   FairGeoLoader* loader = new FairGeoLoader("TGeo","Geo Loader");
   FairGeoInterface* GeoInterface =loader->getGeoInterface();
   GeoInterface->setMediaFile("../geometry/media.geo"); 
   GeoInterface->readMedia(); 
   
   //FairGeoMedia* media = GeoInterface->getMedia();
   //media->print();
   
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
     
   //FairModule* stt = new SpdStt();
   //stt->ConstructGeometry();
   
    
   FairModule* emc = new MpdEmc();
   emc->SetGeometryFileName("emc.geo");
   //emc->SetGeometryFileName("emc_tr.geo");
   //emc->SetGeometryFileName("emc_tr_1.geo");
   //emc->SetGeometryFileName("emc_v1.geo");  // very obsolete
   //emc->SetGeometryFileName("emc_tr_xxx.geo");
   
   emc->SetGeometryFileName("emc_tr_400_3.geo");
   emc->ConstructGeometry();

 
   //--------------------------------------------------
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
   geoMan->GetMasterVolume()->Draw();
   
      
}
