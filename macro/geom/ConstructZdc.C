//______________________________________________________________
void ConstructZdc() 
{
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
 
   //SpdPipe* Pipe = new SpdPipe();
   //Pipe->ConstructGeometry();
   //Pipe->Print("");
 
   // ATTENTION
   SpdZdcGeoMapper* mapper = SpdZdcGeoMapper::Instance();
   SpdParameter* par = mapper->AddParameter("ZdcMinDist");
   *par = 50.;
   
   SpdZdc* zdc = new SpdZdc();
   zdc->ConstructGeometry();
   
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(3);
  
   //geoMan->GetMasterVolume()->SetVisContainers();
   geoMan->GetMasterVolume()->Draw("ogl");
   return;

}
