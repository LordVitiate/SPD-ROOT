// ROOT macro. Author: Artur Tkachenko

//______________________________________________________________
void build_geo() {
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
   
   FairModule* Pipe = new SpdPipe();
   Pipe->ConstructGeometry();
   
   FairModule* ecal = new SpdEcal();
   ecal->ConstructGeometry();
    
   FairModule* emc = new MpdEmc();
   emc->SetGeometryFileName("emc_tr_400_3.geo");
   emc->ConstructGeometry();
   
   FairModule* hmrs = new SpdHmrs();
   hmrs->ConstructGeometry();
   
   SpdSttGeoMapper* mapper = SpdSttGeoMapper::Instance();
   
   mapper->SetScale(10);        // scale (1 = 1 mm, 10 = 1cm), 
   mapper->SetTubeStep(3);      // radial distance between tubes (in tube size)
   mapper->SetTubeSections(6);  // number of sections in each tube
   
   mapper->InitGeometry(1);      // init geometry of type = 1
  
   
   FairModule* stt = new SpdStt();
   stt->ConstructGeometry();
   
   FairModule* sts = new SpdSts();
   sts->SetGeometryFileName("sts.geo");
   sts->ConstructGeometry();   
   
 
   //--------------------------------------------------
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
   geoMan->GetMasterVolume()->Draw();
   
      
}

