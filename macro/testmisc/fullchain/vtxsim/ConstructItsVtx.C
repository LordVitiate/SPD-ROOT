
void CustomItsGeometry(Int_t option);


//______________________________________________________________
void ConstructItsVtx(Int_t geom_option = 4) 
{

   SpdCommonGeoMapper::Instance()->OpenGeometry();
  
   FairModule* Cave = new SpdCave();
   Cave->SetGeometryFileName("cave.geo");
   Cave->ConstructGeometry();
   
   SpdCommonGeoMapper::Instance()->DefineQslGeometrySet();
   
   //SpdPipe* Pipe = new SpdPipe();
   //Pipe->ConstructGeometry();

   SpdTsTB* ts_barrel = new SpdTsTB();   
   ts_barrel->ConstructGeometry();
   
   //SpdTsTEC* ts_ecps     = new SpdTsTEC();  
   //ts_ecps->ConstructGeometry();

   SpdIts* vxdet = new SpdIts();

   CustomItsGeometry(geom_option);  //option = 0 (standard default), 1, 2 ,3
   
   // enable(1)/disable(0) endcaps; default = enable FIXME
   SpdItsGeoMapperX::Instance()->EnableEndcaps(0); 
   
   vxdet->ConstructGeometry();
   
   /* ~~~~~~~~~~~~~ PRINT GEOMETRY INFO ~~~~~~~~~~~~~~~~~ */
   
   //SpdItsGeoMapperX::Instance()->Print("");           // print all parameters
   SpdItsGeoMapperX::Instance()->PrintVolPars(); 
   SpdItsGeoMapperX::Instance()->PrintGeoTable(); 
   
   /* ~~~~~~~~~~~~~ DRAW GEOMETRY  ~~~~~~~~~~~~~~~~~ */
   
   TGeoManager *geoMan = gGeoManager;
   geoMan->SetVisLevel(2);
  
   geoMan->GetMasterVolume()->Draw("ogl");
   
   //geoMan->GetMasterVolume()->Draw();
}

//______________________________________________________________
void CustomItsGeometry(Int_t option)
{
   if (option < 1) return;
       
   Double_t mm = 0.1;  
   
   SpdItsGeoMapperX* mapper = SpdItsGeoMapperX::Instance(); 
   
   //------------------------------------------------------------------------
   // List of default layer/ladder parameters (geom = [1,5], layer = [1,5]):
   //----------------------------------------------------------------------------
   // par[0] = Rlayer    : the distance from z axis to ladder center (LAYER radius)
   // par[1] = Lz        : ladder size along z axis
   // par[2] = Lphi      : ladder size along phi axis
   // par[3] = Lr        : radial size of ladder 
   // par[4] = angle     : local rotation angle for ladder
   // par[5] = Phi       : global rotation angle for LAYER
   // par[6] = Nladders  : number of ladders inside the LAYER (will be calculated automatically for geometry = 1,2)
   //-------------------------------------------------------------------------
   //static void SetDefaultLadderPars(Int_t geometry, Int_t layer, Int_t par, Double_t value);
   //static void SetDefaultLadderPars(Int_t geometry, Int_t par, Double_t value); // all layers
 
   //-------------------------------------------------------------------------
   // List of default ladder/chip parameters (geom = [1,5], layer = [1,5]): 
   //-------------------------------------------------------------------------
   // par[0] = lz        : chip size along z-axis
   // par[1] = lphi      : chip phi-size  
   // par[2] = dlz       : gap size between chips along z-axis
   // par[3] = dlphi     : gap size between chips along phi-axis
   // par[4] = ncz       : number of chip cells (channels) along z-axis
   // par[5] = ncphi     : number of chip cells (channels) along phi-axis
   // par[6] = type      : chip type (0: NO INNER STRUCTURE, 1: MAPS, 2: DSSD, >2: USER DEFINED)
   //-------------------------------------------------------------------------
   // static void SetDefaultChipPars(Int_t geometry, Int_t layer, Int_t par, Double_t value);
   // static void SetDefaultChipPars(Int_t geometry, Int_t par, Double_t value); // all layers
   
   //============================================================================
   
   if (option == 1) 
   {
       //----------------------------------------------
       // Number of layers = 5
       // Type of chips (all layers): DSSD (all layers)
       // Chips thickness(all layers): 50 mkm
       //----------------------------------------------
       
       // layers:   
       mapper->SetDefaultLadderPars(3, 3, 0.05*mm);
    
       mapper->SetGeometryPars(3); // set new parameters !
   
       return;
   }

   if (option == 2) 
   {
       //----------------------------------------------
       // Number of layers = 5
       // Type of chips (all layers): MAPS (all layers)
       // Chips thickness (all layers): 50 mkm
       //----------------------------------------------
       
       // layers:
       mapper->SetDefaultLadderPars(2, 2, 15*mm);
       mapper->SetDefaultLadderPars(2, 3, 0.05*mm);
   
       // chips:
       mapper->SetDefaultChipPars(1, 0, 14.336*mm);
       mapper->SetDefaultChipPars(1, 1, 14.336*mm);
       mapper->SetDefaultChipPars(1, 2, 0.664*mm);
       mapper->SetDefaultChipPars(1, 3, 0);
       mapper->SetDefaultChipPars(1, 4, 512);
       mapper->SetDefaultChipPars(1, 5, 512);
       mapper->SetDefaultChipPars(1, 6, 1);
      
       mapper->SetGeometryPars(2,1); // set new parameters !
   
       return;
   }
   
   if (option == 3) 
   {
       // layers:   
       mapper->SetDefaultLadderPars(3, 1, 2, 15*mm);
       mapper->SetDefaultLadderPars(3, 1, 3, 0.05*mm);
       mapper->SetDefaultLadderPars(3, 1, 4, 0);
       mapper->SetDefaultLadderPars(3, 1, 6, 30);
                                    
       mapper->SetDefaultLadderPars(3, 2, 2, 15*mm);
       mapper->SetDefaultLadderPars(3, 2, 3, 0.05*mm);
       mapper->SetDefaultLadderPars(3, 2, 4, 0);
       mapper->SetDefaultLadderPars(3, 2, 6, 57);
       
       mapper->SetDefaultLadderPars(3, 3, 2, 15*mm);
       mapper->SetDefaultLadderPars(3, 3, 3, 0.05*mm);
       mapper->SetDefaultLadderPars(3, 3, 4, 0);
       mapper->SetDefaultLadderPars(3, 3, 6, 77);
       
       mapper->SetGeometryPars(3,1); // set new parameters !
       
       return;
   }
   
   if (option == 4) 
   {
       //----------------------------------------------
       // Number of layers = 4
       // Type of chips (all layers): MAPS (all layers)
       // Chips thickness (all layers): 50 mkm
       //----------------------------------------------
        
       // layers:
       mapper->SetDefaultLadderPars(2, 1, 0, 70*mm);
       mapper->SetDefaultLadderPars(2, 2, 0, 127*mm);
       mapper->SetDefaultLadderPars(2, 3, 0, 184*mm);
       mapper->SetDefaultLadderPars(2, 4, 0, 241*mm);
       //mapper->SetDefaultLadderPars(2, 5, 0, 298*mm);
       
       mapper->SetDefaultLadderPars(2, 2, 15*mm);
       mapper->SetDefaultLadderPars(2, 3, 0.05*mm);
   
       // chips:
       mapper->SetDefaultChipPars(1, 0, 14.336*mm);
       mapper->SetDefaultChipPars(1, 1, 14.336*mm);
       mapper->SetDefaultChipPars(1, 2, 0.664*mm);
       mapper->SetDefaultChipPars(1, 3, 0);
       mapper->SetDefaultChipPars(1, 4, 512);
       mapper->SetDefaultChipPars(1, 5, 512);
       mapper->SetDefaultChipPars(1, 6, 1);
      
       mapper->SetGeometryPars(2,1); // set new parameters !
    
       mapper->SetNLayers(4);
    
       return;
   }
 
}






