//______________________________________________________________
void ConstructTof() 
{
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
 
   //SpdPipe* Pipe = new SpdPipe();
   //Pipe->ConstructGeometry();
   //Pipe->Print("");
   
   // 1 - simple geometry
   // 2 - geometry based on MRPC modules (default)
   //SpdTofBGeoMapper::Instance()->SetGeoType(1);
   //SpdTofECGeoMapper::Instance()->SetGeoType(1);
   
   SpdTofB* tofb = new SpdTofB();
   tofb->ConstructGeometry();
   
   SpdTofEC* tofec = new SpdTofEC();
   tofec->ConstructGeometry();
   
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(3);
  
   //geoMan->GetMasterVolume()->SetVisContainers();
   geoMan->GetMasterVolume()->Draw("ogl");
   return;

}
