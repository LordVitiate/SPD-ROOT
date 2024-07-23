

void ConstructRsTB() 
{
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   //SpdCommonGeoMapper::Instance()->DefineHybGeometrySet();
   SpdCommonGeoMapper::Instance()->DefineQSolGeometrySet();
   
   //SpdPipe* Pipe = new SpdPipe();
   //Pipe->ConstructGeometry();
   //Pipe->Print("");
   
   //SpdRsTBGeoMapper::Instance()->SetGeoType(1);
   
   //SpdParameter* par = SpdRsTBGeoMapper::Instance()->AddParameter("RsTBClearance");
   //*par = 0.;
   
   SpdRsTB* rs = new SpdRsTB();
   
   rs->ConstructGeometry();
   
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
  
   geoMan->GetMasterVolume()->Draw("ogl");
}

