
void DefaultGeometryTight();
void DefaultGeometryTightBasket();
void DefaultGeometryForceCellSize();

void CustomizeGeometry();

//______________________________________________________________
void ConstructEcalTB2() 
{
   // 
   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
 
    //SpdPipe* Pipe = new SpdPipe();
    //Pipe->ConstructGeometry();
    //Pipe->Print("");
   
    //SpdEcalTEC2* ecaltec = new SpdEcalTEC2();
    //ecaltec->ConstructGeometry();
    //ecaltec->Print("");
   
   SpdEcalTB2* ecaltb = new SpdEcalTB2();
  
   //CustomizeGeometry();

   ecaltb->ConstructGeometry(); 
  
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(3);
  
   //geoMan->GetMasterVolume()->SetVisContainers();
   geoMan->GetMasterVolume()->Draw("ogl");
  
   //ecaltb->GetBasket()->Draw("ogl");
   
   TGeoVolume* module; 
   module = ecaltb->GetModule(0);
   
   //if (module) module->SetVisContainers();
   //if (module) module->Draw("ogl");
   
   ecaltb->Print("");
}

//______________________________________________________________
void CustomizeGeometry()
{
   // 1. recommended default: fixed cell length, automatically calculated cell width
    
   //DefaultGeometryTight();      
   
   // 2. trimming cell length (cell length automatically calculated to fit into basket), 
   //    automatically calculated cell width
    
   //DefaultGeometryTightBasket(); 
   
   // 3. force cell size
    
   //DefaultGeometryForceCellSize(); 
}

//______________________________________________________________
void DefaultGeometryTight() 
{
   //
   // Fill baskets(barrel) with modules tightly (automatically selecting cell transverse size)
   // Module length and number of layers are constant
   // 
   SpdEcalTB2GeoMapper* mapper = SpdEcalTB2GeoMapper::Instance();
   
   //rough guideline: size will be chosen as close as possible to these values
   mapper->SetCellZSize(5.5);        // [cm] z-size of cell
   mapper->SetCellInnerPhiSize(5.5); // [cm] φ-size of the cell face closer to beam axis
   mapper->SetNLayers(200);
   mapper->SetAbsorberLayerThickness(0.05);
   mapper->SetScintLayerThickness(0.15);
   
   //additional
   mapper->SetModuleClearance(0.1); // [cm] clerance between modules (default: 0.1)
   mapper->SetCellClearance(0.05);  // [cm] clerance between cells in a module (default: 0.05)
}

//______________________________________________________________
void DefaultGeometryTightBasket() 
{
   //
   // Fill baskets(barrel) with modules tightly (automatically selecting cell transverse size)
   // Trim cell length to be inside basket and select number of layers accordingly    
   //
    
   SpdEcalTB2GeoMapper* mapper = SpdEcalTB2GeoMapper::Instance();
   mapper->SetForceInsideBasket(true);
   mapper->SetCellZSize(5.5);         // [cm] z-size of cell
   mapper->SetCellInnerPhiSize(5.5);  // [cm] φ-size of the cell face closer to beam axis
   mapper->SetAbsorberLayerThickness(0.05);
   mapper->SetScintLayerThickness(0.15);
   
   //additional
   mapper->SetModuleClearance(0.1); // [cm] clerance between modules (default: 0.1)
   mapper->SetCellClearance(0.05);  // [cm] clerance between cells in a module (default: 0.05)
}

//______________________________________________________________
void DefaultGeometryForceCellSize() 
{
   //  
   // Fill baskets(barrel) with modules, using the given cell size
   // 
   SpdEcalTB2GeoMapper* mapper = SpdEcalTB2GeoMapper::Instance();
   mapper->SetForceCellSize(true);
   
   //strict values: size will be forced to be this value, but the ECAL may have lots of empty space - choose with caution
   mapper->SetCellZSize(5.5);        // [cm] z-size of cell
   mapper->SetCellInnerPhiSize(5.1); // [cm] φ-size of the cell face closer to beam axis
   mapper->SetCellOuterPhiSize(6.6); // [cm] φ-size of the cell face closer to beam axis
  
   mapper->SetNLayers(200);
   mapper->SetAbsorberLayerThickness(0.05); // [cm]
   mapper->SetScintLayerThickness(0.15);    // [cm]
  
   //additional
   mapper->SetModuleClearance(0.1); // [cm] clerance between modules (default: 0.1)
   mapper->SetCellClearance(0.05);  // [cm] clerance between cells in a module (default: 0.05)
}







