//______________________________________________________________
void ConstructEcalTEC2() 
{
   // 
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
 
   SpdPipe* Pipe = new SpdPipe();
   Pipe->ConstructGeometry();
   
   SpdEcalTEC2* ecaltec = new SpdEcalTEC2();
   
   // customize geometry
   SpdEcalTEC2GeoMapper* mapper = SpdEcalTEC2GeoMapper::Instance();
   
   mapper->SetCellSize(5.5);                 // [cm], X/Y size of endcap cell (default: 5.5) 
   mapper->SetAbsorberLayerThickness(0.05);  // [cm], thickness of absorber layer (material hardcoded as lead) (default: 0.05)
   mapper->SetScintLayerThickness(0.15);     // [cm], thickness of scintillator layer (polystyrene(?)) (default: 0.15)
   
   // additional:
   mapper->SetCellClearance(1.5);      // [cm], clerance between cells in a module (default: 0.05)
   mapper->SetModuleClearance(1.5);    // [cm], clearance between modules (2x2 cells) (default: 0.1)
      
   // construct geometry
   ecaltec->ConstructGeometry();
   
   /* ~~~~~~~~~~~~~ PRINT GEOMETRY INFO ~~~~~~~~~~~~~*/
   
   SpdEcalTEC2GeoMapper::Instance()->Print("");           // print all parameteecal
   
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
  
   //geoMan->GetMasterVolume()->SetVisContainers();
   geoMan->GetMasterVolume()->Draw("ogl");

   //ecaltec->GetEndcap()->Draw("ogl");
   //ecaltec->GetModule()->Draw("ogl");
   //ecaltec->GetCell()->Draw("ogl");
   
   return;

}
