
//______________________________________________________________
void ConstructTrackerGeo() {
   //       
   SpdCommonGeoMapper::Instance()->OpenGeometry();
   
   //FairGeoMedia* media = FairGeoLoader::Instance()->getGeoInterface()->getMedia();
   //media->print();
   
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   //SpdPipe* Pipe = new SpdPipe();
   //Pipe->ConstructGeometry();
   
   SpdHybMagnet* Magnet = new SpdHybMagnet();
   Magnet->ConstructGeometry();
  
   Tracker* tracker = new Tracker();
   
   //tracker->SetBaseMaterial("vacuum2");
   //tracker->SetActiveMaterial("air");
  
   ///tracker->SetSlicing(10);   
   
   /* TRACKER PARAMETERS: Zmin, Zmax, Rmin, Rmax, NSlices */
   
   //tracker->SetDefaultGeometryParameters(2);
   
   tracker->SetGeoParameters(118, 318, 65, 160, 95);              // geometry type = 1 "barrel + endcaps"
   //tracker->SetGeoEndcapsParameters(118, 318, 20, 160, 50);     // geometry type = 2 "z-sliced endcaps"
   //tracker->SetGeoBarrelParameters(118, 318, 20, 160, 50);      // geometry type = 3 "r-sliced endcaps"
  
   tracker->ConstructGeometry();
   
   //SpdCommonGeoMapper::Instance()->PrintGeometryList();
   
   tracker->Print("");
  
   /* DRAW GEOMETRY */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
   geoMan->GetMasterVolume()->Draw("ogl");
   //geoMan->GetMasterVolume()->Draw();
   
}
