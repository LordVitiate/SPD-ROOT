//______________________________________________________________
void ConstructBbc() 
{
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
 
   //SpdPipe* Pipe = new SpdPipe();
   //Pipe->ConstructGeometry();
   //Pipe->Print("");
   
   SpdBbc* bbc = new SpdBbc();
   bbc->ConstructGeometry();
   
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(3);
  
   //geoMan->GetMasterVolume()->SetVisContainers();
   geoMan->GetMasterVolume()->Draw("ogl");
   return;

}
