//______________________________________________________________
void ConstructRsTB2() {
   // 
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
 
   //SpdPipe* Pipe = new SpdPipe();
   //Pipe->ConstructGeometry();
   //Pipe->Print("");
   
   SpdRsTB2* rstb = new SpdRsTB2();
   rstb->ConstructGeometry();
   
   /* ~~~~~~~~~~~~~ PRINT GEOMETRY INFO ~~~~~~~~~~~~~~~~~ */
   
   SpdRsTB2GeoMapper::Instance()->Print("");           // print all parameters
   
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(3);
  
   //geoMan->GetMasterVolume()->SetVisContainers();
   geoMan->GetMasterVolume()->Draw("ogl");
   return;

}
