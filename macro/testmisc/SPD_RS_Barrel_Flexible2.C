// $Id$
// Author: georgy   2020/06/24

//_____________________________________________________________________________
//
// Creates geometry of SPD RS Barrel segment and fills it with MDT detectors
//_____________________________________________________________________________


TGeoVolume* createMDT(Double_t length = 493.0) {
//_____________________________________________________________________________
//
// Creates MDT of given length;
// Envelope Dimensions(L x W x H): length x 8.5cm x 1.64cm.
// Note: Adopted from Panda Muon System
//_____________________________________________________________________________

    // Medium description
    TGeoMaterial* mat_air = new TGeoMaterial("air",0,0,0);
    TGeoMedium*   pair = new TGeoMedium("air",1,mat_air);

    TGeoMaterial* mat_mix = new TGeoMaterial("MDTMixture",0,0,0);
    TGeoMedium*   pMDTMixture = new TGeoMedium("MDTMixture",1,mat_mix);

    TGeoMaterial* mat_Al = new TGeoMaterial("Aluminium",0,0,0);
    TGeoMedium*   paluminium = new TGeoMedium("Aluminium",1,mat_Al);

//     TGeoMaterial* mat = new TGeoMaterial("Vacuum",0,0,0);
//     TGeoMedium*   pvacuum = new TGeoMedium("Vacuum",1,mat);
//    
//     TGeoMaterial* mat_pvc = new TGeoMaterial("PVC",0,0,0);
//     TGeoMedium*   pPVC = new TGeoMedium("PVC",1,mat_pvc);
//    
//     TGeoMaterial* mat_iron = new TGeoMaterial("iron",0,0,0);
//     TGeoMedium*   piron = new TGeoMedium("iron",1,mat_iron);
// 
//     TGeoMaterial* mat_g10 = new TGeoMaterial("G10",0,0,0);
//     TGeoMedium*   pg10 = new TGeoMedium("G10",1,mat_g10);
// 
//     TGeoMaterial* mat_copper = new TGeoMaterial("copper",0,0,0);
//     TGeoMedium*   pcopper = new TGeoMedium("copper",1,mat_copper);

    // Shapes and volumes ---------------------------------------------------------------
    // General dimensions
    Double_t widthEnv = 8.5; //cm xAxis
    Double_t thicknessEnv = 1.64; //cm zAxis
    Double_t xSize = 1.0;

    TString transName;
    Int_t Nenvelopes;
    Int_t IncEnv = 0;

    //Matrices
    TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
    rotNoRot->RegisterYourself();
    
    Double_t empty_value = (2.0+2.0+8.5)+6.5*2;
    Double_t L_MDT_Envelope = length; //493.0;
    Double_t L_MDT_Envelope_sensitive = L_MDT_Envelope-empty_value;


    Int_t ind = int(length*10);
    //Shapes
    //<box name="Indent20_PartBody" x="(42.500000)*2" y="(10.000000)*2" z="(8.200000)*2" lunit="mm"/>
    TGeoBBox* Indent20_Shape = new TGeoBBox("Indent20_Shape", widthEnv/2,
                                                            thicknessEnv/2,
                                                            1.0);
    //<box name="FEB_box_PartBody" x="(42.500000)*2" y="(42.500000)*2" z="(8.200000)*2" lunit="mm"/>
    TGeoBBox* FEB_box_Shape = new TGeoBBox("FEB_box_Shape", widthEnv/2,
                                                            thicknessEnv/2,
                                                            widthEnv/2);
    //<box name="DeadMat_PartBody" x="(40.700000)*2" y="(32.500000)*2" z="(5.150000)*2" lunit="mm"/>
    TGeoBBox* DeadMat_Shape = new TGeoBBox("DeadMat_Shape", 4.07,
                                                            0.515,
                                                            3.25);
    TGeoVolume* Indent20_Vol = new TGeoVolume("Indent20_Vol", Indent20_Shape, pair);
    TGeoVolume* FEB_box_Vol = new TGeoVolume("FEB_box_Vol", FEB_box_Shape, pair);
    TGeoVolume* DeadMat_Vol = new TGeoVolume("DeadMat_Vol", DeadMat_Shape, pMDTMixture);

    //----------------------4930------------------------------------//
    TGeoBBox* L_Al_A_Shape = new TGeoBBox(Form("L%d_Al_A_Shape",ind), 4.03,
                                                                0.03,
                                                                L_MDT_Envelope_sensitive/2);
    TGeoBBox* L_Al_B_Shape = new TGeoBBox(Form("L%d_Al_B_Shape",ind), 0.03,
                                                                0.42,
                                                                L_MDT_Envelope_sensitive/2);
    TGeoBBox* L_GasCell_Shape = new TGeoBBox(Form("L%d_GasCell_Shape",ind), 0.47,
                                                                        0.46,
                                                                        L_MDT_Envelope_sensitive/2);
    TGeoBBox* L_Cell_Shape = new TGeoBBox(Form("L%d_Cell_Shape",ind), 0.5,
                                                                0.46,
                                                                L_MDT_Envelope_sensitive/2);
    TGeoBBox* L_MDT_S_Shape = new TGeoBBox(Form("L%d_MDT_S_Shape",ind), 4.0,
                                                                    0.46,
                                                                    L_MDT_Envelope_sensitive/2);
    TGeoBBox* L_Envelope_Shape = new TGeoBBox(Form("L%d_Envelope_Shape",ind), widthEnv/2,
                                                                        thicknessEnv/2,
                                                                        L_MDT_Envelope/2);
                                                                                 
    //Volumes
    TGeoVolume* L_Al_A_Vol = new TGeoVolume(Form("L%d_Al_A_Vol",ind), L_Al_A_Shape, paluminium);
    TGeoVolume* L_Al_B_Vol = new TGeoVolume(Form("L%d_Al_B_Vol",ind), L_Al_B_Shape, paluminium);
    TGeoVolume* L_GasCell_Vol = new TGeoVolume(Form("L%d_GasCell_Vol",ind), L_GasCell_Shape, pMDTMixture);
    TGeoVolume* L_Cell_Vol = new TGeoVolume(Form("L%d_Cell_Vol",ind), L_Cell_Shape, pMDTMixture);
    TGeoVolume* L_MDT_S_Vol = new TGeoVolume(Form("L%d_MDT_S_Vol",ind), L_MDT_S_Shape, pMDTMixture);
    TGeoVolume* L_Envelope_Vol = new TGeoVolume(Form("L%d_Envelope_Vol",ind), L_Envelope_Shape, pair);
   
    // Volume hierarchy -----------------------------------------------------------------
    //Structure
    //GasCell and Al_B in Cell
    L_Cell_Vol->AddNode(L_GasCell_Vol, 1,
                            new TGeoCombiTrans("", -0.03, 0, 0, rotNoRot));
    L_Cell_Vol->AddNode(L_Al_B_Vol, 1,
                            new TGeoCombiTrans("", 0.47, -0.04, 0, rotNoRot));

    //8 Cells in MDT_S
    for (UInt_t i=0; i<8; i++) {
        transName.Form("trans_cell_%d", i);
        TGeoCombiTrans* trans1 = new TGeoCombiTrans(transName.Data(), -4.0 + xSize/2. + (Double_t)i*xSize, 0, 0., rotNoRot);
        trans1->RegisterYourself();
        L_MDT_S_Vol->AddNode(L_Cell_Vol, i+1, trans1);
    }

    L_Envelope_Vol->AddNode(Indent20_Vol, 1,
                                new TGeoCombiTrans("", 0, 0, L_MDT_Envelope/2 - 1.0, rotNoRot));
    L_Envelope_Vol->AddNode(Indent20_Vol, 2,
                                new TGeoCombiTrans("", 0, 0, 1.0 - L_MDT_Envelope/2, rotNoRot));
    L_Envelope_Vol->AddNode(FEB_box_Vol, 1,
                                new TGeoCombiTrans("", 0, 0, L_MDT_Envelope/2-2.0-8.5*0.5, rotNoRot));
    L_Envelope_Vol->AddNode(DeadMat_Vol, 1,
                                new TGeoCombiTrans("", 0, 0, ((L_MDT_Envelope-(2.0+2.0+8.5))-6.5)/2-widthEnv/2, rotNoRot));
    L_Envelope_Vol->AddNode(DeadMat_Vol, 2,
                                new TGeoCombiTrans("", 0, 0, -((L_MDT_Envelope-(2.0+2.0+8.5))-6.5)/2-widthEnv/2, rotNoRot));
    L_Envelope_Vol->AddNode(L_Al_A_Vol, 1,
                                new TGeoCombiTrans("", 0, -0.36, -widthEnv/2, rotNoRot));
    L_Envelope_Vol->AddNode(L_Al_B_Vol, 1,
                                new TGeoCombiTrans("", -4.0, 0.09, -widthEnv/2, rotNoRot));
    L_Envelope_Vol->AddNode(L_MDT_S_Vol, 1,
                                new TGeoCombiTrans("", 0.03, 0.13, -widthEnv/2, rotNoRot));                                                          
    //----------------------4930------------------------------------//

    return L_Envelope_Vol;
}

//_____________________________________________________________________________
//
TGeoVolume* createBottomLayer(Double_t rsRadius, Double_t rsThickness, Double_t rsLength) {
//_____________________________________________________________________________
//
// Creates trapezoid representing bottom layer of the RS Barrel part
// (6.0cm thick iron absorber and a layer of MDT detectors)
// Parameters: 1. Outer radius of RS from collision point (rsRadius);
//             2. Thickness of RS rsThickness.
// Returns: TGeoVolume object of the layer with absorber and MDTs.
//_____________________________________________________________________________

    Double_t layerThickness = 6.0+3.5; //cm

    Double_t widthEnv = 8.5; //cm xAxis
    Double_t thicknessEnv = 1.64; //cm zAxis

    TGeoMaterial *mat_iron = new TGeoMaterial("iron",0,0,0);
    TGeoMedium   *piron = new TGeoMedium("iron",1,mat_iron);

    TGeoMaterial *mat_air = new TGeoMaterial("air",0,0,0);
    TGeoMedium   *pair = new TGeoMedium("air",1,mat_air);

     //Matrices
    TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
    rotNoRot->RegisterYourself();
    
    Double_t v_bottom[16];
    Double_t lmin = (rsRadius - rsThickness)*(sqrt(2.0)-1.0); // dimensions (bottom base)
    Double_t lmax = (rsRadius - rsThickness + layerThickness)*(sqrt(2.0)-1.0); // dimensions (top base)
    Double_t hmin = (rsRadius - rsThickness); 
    Double_t hmax = (rsRadius - rsThickness + layerThickness);

    cout<<"lmin: "<<lmin<<endl;
    cout<<"lmax: "<<lmax<<endl;
    cout<<"hmin: "<<hmin<<endl;
    cout<<"hmax: "<<hmax<<endl;
  
    v_bottom[0] =  lmin; v_bottom[1] = hmin;
    v_bottom[2] = -lmin; v_bottom[3] = hmin;
    v_bottom[4] = -lmax; v_bottom[5] = hmax;
    v_bottom[6] =  lmax; v_bottom[7] = hmax;
  
    v_bottom[8]  =  lmin; v_bottom[9]  = hmin;
    v_bottom[10] = -lmin; v_bottom[11] = hmin;
    v_bottom[12] = -lmax; v_bottom[13] = hmax;
    v_bottom[14] =  lmax; v_bottom[15] = hmax;

    TGeoArb8* Layer_SideS_Shape = new TGeoArb8("Layer_SideS_Shape", rsLength/2, v_bottom);

    Double_t Layer_Sector_Abs_dx = lmin;
    Double_t Layer_Sector_Abs_dy = 6./2;
    Double_t Layer_Sector_Abs_dz = rsLength/2;


    cout<<"Layer_SideS_Abs_Base_Shape: "<<2*Layer_Sector_Abs_dx<<"   "<<2*Layer_Sector_Abs_dy<<"  "<<2*Layer_Sector_Abs_dz<<endl;

    
    TGeoBBox* Layer_SideS_Abs_Base_Shape = new TGeoBBox("Layer_SideS_Abs_Base_Shape", 
                                                        Layer_Sector_Abs_dx, 
                                                        Layer_Sector_Abs_dy, 
                                                        Layer_Sector_Abs_dz);
                                                            
    TGeoVolume* Layer_SideS_Vol = new TGeoVolume("Layer_SideS_Vol", Layer_SideS_Shape, pair);
    TGeoVolume* Layer_SideS_Abs_Base_Vol = new TGeoVolume("Layer_SideS_Abs_Base_Vol", Layer_SideS_Abs_Base_Shape, piron);

    Layer_SideS_Vol->AddNode(Layer_SideS_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, hmin+Layer_Sector_Abs_dy, 0, rotNoRot));


    Double_t Layer_Envelope_startX = -1.0*lmin;
    Double_t Layer_Envelope_shiftX = Layer_Envelope_startX + widthEnv/2;

    Double_t Layer_length = 2*abs(Layer_Envelope_startX);
    cout<<"Layer #1 length: " << Layer_length << endl;

    int Nenvelopes = int(Layer_length/widthEnv);
    cout << "Number of MDT detector in Layer #1: " << Nenvelopes << endl;

    
    for (UInt_t i=0; i<Nenvelopes; i++) {
    //for (UInt_t i=0; i<1; i++) {

        Layer_SideS_Vol->AddNode(createMDT(493.0), i+1, new TGeoCombiTrans("", Layer_Envelope_shiftX, hmin+6+3.5/2, 0, rotNoRot)); 
        Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
    }

    return Layer_SideS_Vol;
}

//_____________________________________________________________________________
//
TGeoVolume* createRegularLayer(Int_t layerN, Double_t rsRadius, Double_t rsThickness, Double_t rsLength) {
//_____________________________________________________________________________
//
// Creates trapezoid representing regular layer of the RS Barrel part
// (3.0cm thick iron absorber and a layer of MDT detectors)
// Parameters: 1. Number of layer in a sector
//             2. Outer radius of RS from collision point (rsRadius);
//             3. Thickness of RS rsThickness.
// Returns: TGeoVolume object of the layer with absorber and MDTs.
//_____________________________________________________________________________

    Double_t layerThickness = 3.0+3.5; //cm

    Double_t Sector_dy = rsLength/2.0;

    Double_t widthEnv = 8.5; //cm xAxis
    Double_t thicknessEnv = 1.64; //cm zAxis

    TGeoMaterial *mat_iron = new TGeoMaterial("iron",0,0,0);
    TGeoMedium   *piron = new TGeoMedium("iron",1,mat_iron);

    TGeoMaterial *mat_air = new TGeoMaterial("air",0,0,0);
    TGeoMedium   *pair = new TGeoMedium("air",1,mat_air);

     //Matrices
    TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
    rotNoRot->RegisterYourself();
    
    Double_t Layer_Sector_dz = layerThickness/2; // z-coord of the layer center 
    Double_t Layer_Sector_dx1 = 1.0*(rsRadius - rsThickness + 9.5 + (layerN-2)*layerThickness)*(sqrt(2.0)-1.0); // X-coord of the bottom base
    Double_t Layer_Sector_dx2 = 1.0*(rsRadius - rsThickness + 9.5 + (layerN-1)*layerThickness)*(sqrt(2.0)-1.0); // X-coord of the top base
    Double_t Layer_Sector_dy = Sector_dy; // y-coord of the layer center
    cout<<"Layer #"<< layerN << " dimensions (bottom base): ["
        <<Layer_Sector_dx1*2<<" x " 
        << Layer_Sector_dz*2 << " x " 
        << Layer_Sector_dy*2 << "] cm" << endl;
    cout<<"Layer #"<< layerN << " dimensions (top base): ["
        <<Layer_Sector_dx2*2<<" x " 
        << Layer_Sector_dz*2 << " x " 
        << Layer_Sector_dy*2 << "] cm" << endl;
    
    Double_t v_regular[16];
    Double_t lmin = (rsRadius - rsThickness + 9.5 + (layerN-2)*layerThickness)*(sqrt(2.0)-1.0); // dimensions (bottom base)
    Double_t lmax = (rsRadius - rsThickness + 9.5 + (layerN-1)*layerThickness)*(sqrt(2.0)-1.0); // dimensions (top base)
    Double_t hmin = (rsRadius - rsThickness + 9.5 + (layerN-2)*layerThickness); 
    Double_t hmax = (rsRadius - rsThickness + 9.5 + (layerN-1)*layerThickness);

    cout<<"Layer #"<< layerN << " dimensions (bottom base, lmin): [" << -lmin << "," << lmin << "]" << endl;
    cout<<"Layer #"<< layerN << " dimensions (bottom base, lmax): [" << -lmax << "," << lmax << "]" << endl;
    cout<<"hmin: "<<hmin<<endl;
    cout<<"hmax: "<<hmax<<endl;
  
    v_regular[0] =  lmin; v_regular[1] = hmin;
    v_regular[2] = -lmin; v_regular[3] = hmin;
    v_regular[4] = -lmax; v_regular[5] = hmax;
    v_regular[6] =  lmax; v_regular[7] = hmax;
  
    v_regular[8]  =  lmin; v_regular[9]  = hmin;
    v_regular[10] = -lmin; v_regular[11] = hmin;
    v_regular[12] = -lmax; v_regular[13] = hmax;
    v_regular[14] =  lmax; v_regular[15] = hmax;

    TGeoArb8* Layer_SideS_Shape = new TGeoArb8("Layer_SideS_Shape", rsLength/2, v_regular);

    Double_t Layer_Sector_Abs_dx = lmin;
    Double_t Layer_Sector_Abs_dy = 3./2;
    Double_t Layer_Sector_Abs_dz = rsLength/2;


    TGeoBBox* Layer_SideS_Abs_Base_Shape = new TGeoBBox("Layer_SideS_Abs_Base_Shape", 
                                                        Layer_Sector_Abs_dx, 
                                                        Layer_Sector_Abs_dy, 
                                                        Layer_Sector_Abs_dz);
   
                                                            
    TGeoVolume* Layer_SideS_Vol = new TGeoVolume("Layer_SideS_Vol", Layer_SideS_Shape, pair);
    TGeoVolume* Layer_SideS_Abs_Base_Vol = new TGeoVolume("Layer_SideS_Abs_Base_Vol", Layer_SideS_Abs_Base_Shape, piron);

    Layer_SideS_Vol->AddNode(Layer_SideS_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, hmin+Layer_Sector_Abs_dy, 0, rotNoRot));

    if (layerN % 2 == 0) {
        Double_t Layer_Envelope_startX = lmin;
        Double_t Layer_Envelope_shiftX = Layer_Envelope_startX - widthEnv/2;

        Double_t Layer_length = 2*abs(Layer_Envelope_startX);
        cout<<"Layer #" << layerN << " length: " << Layer_length << endl;

        int Nenvelopes = int((Layer_length)/widthEnv);
        cout << "Number of MDT detector in Layer" << layerN << ": " << Nenvelopes << endl;
        for (UInt_t i=0; i<Nenvelopes; i++) {

            Layer_SideS_Vol->AddNode(createMDT(493.0), i+1, new TGeoCombiTrans("", Layer_Envelope_shiftX, hmin + 3 + 3.5/2, 0, rotNoRot)); 
            Layer_Envelope_shiftX = Layer_Envelope_shiftX - widthEnv;
        }
    }
    else {

        Double_t Layer_Envelope_startX = -1.0*abs(lmin);
        Double_t Layer_Envelope_shiftX = Layer_Envelope_startX + widthEnv/2;

        Double_t Layer_length = 2*abs(Layer_Envelope_startX);
        cout<<"Layer #" << layerN << " length: " << Layer_length << endl;

        int Nenvelopes = int((Layer_length)/widthEnv);
        cout << "Number of MDT detector in Layer" << layerN << ": " << Nenvelopes << endl;
        for (UInt_t i=0; i<Nenvelopes; i++) {

            Layer_SideS_Vol->AddNode(createMDT(493.0), i+1, new TGeoCombiTrans("", Layer_Envelope_shiftX, hmin + 3 + 3.5/2, 0, rotNoRot)); 
            Layer_Envelope_shiftX = Layer_Envelope_shiftX + widthEnv;
        }
    }

    return Layer_SideS_Vol;
}


//_____________________________________________________________________________
//
TGeoVolume* createTopLayer(Int_t layerN, Double_t rsRadius, Double_t rsThickness, Double_t rsLength) {
//_____________________________________________________________________________
//
// Creates trapezoid representing regular layer of the RS Barrel part
// (3.0cm thick iron absorber and a layer of MDT detectors)
// Parameters: 1. Number of layer in a sector
//             2. Outer radius of RS from collision point (rsRadius);
//             3. Thickness of RS rsThickness.
// Returns: TGeoVolume object of the layer with absorber and MDTs.
//_____________________________________________________________________________

    Double_t layerThickness = 3.0+3.0; //cm

    Double_t Sector_dy = rsLength/2.0;

    Double_t widthEnv = 8.5; //cm xAxis
    Double_t thicknessEnv = 1.64; //cm zAxis

    TGeoMaterial *mat_iron = new TGeoMaterial("iron",0,0,0);
    TGeoMedium   *piron = new TGeoMedium("iron",1,mat_iron);

    TGeoMaterial *mat_air = new TGeoMaterial("air",0,0,0);
    TGeoMedium   *pair = new TGeoMedium("air",1,mat_air);

     //Matrices
    TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
    rotNoRot->RegisterYourself();
    
    Double_t v_top[16];
    Double_t lmin = (rsRadius - rsThickness + 9.5 + (layerN-1)*layerThickness)*(sqrt(2.0)-1.0); // dimensions (bottom base)
    Double_t lmax = (rsRadius - rsThickness + 9.5 + (layerN)*layerThickness)*(sqrt(2.0)-1.0); // dimensions (top base)
    Double_t hmin = (rsRadius - rsThickness + 9.5 + (layerN-2)*6.5); 
    Double_t hmax = (rsRadius - rsThickness + 9.5 + (layerN-2)*6.5 + layerThickness);

    cout<<"Layer #"<< layerN << " dimensions (bottom base, lmin): [" << -lmin << "," << lmin << "]" << endl;
    cout<<"Layer #"<< layerN << " dimensions (bottom base, lmax): [" << -lmax << "," << lmax << "]" << endl;
    cout<<"hmin: "<<hmin<<endl;
    cout<<"hmax: "<<hmax<<endl;
  
    v_top[0] =  lmin; v_top[1] = hmin;
    v_top[2] = -lmin; v_top[3] = hmin;
    v_top[4] = -lmax; v_top[5] = hmax;
    v_top[6] =  lmax; v_top[7] = hmax;
  
    v_top[8]  =  lmin; v_top[9]  = hmin;
    v_top[10] = -lmin; v_top[11] = hmin;
    v_top[12] = -lmax; v_top[13] = hmax;
    v_top[14] =  lmax; v_top[15] = hmax;

    TGeoArb8* Layer_SideS_Shape = new TGeoArb8("Layer_SideS_Shape", rsLength/2, v_top);

    Double_t Layer_Sector_Abs_dx = lmin;
    Double_t Layer_Sector_Abs_dy = layerThickness/2;
    Double_t Layer_Sector_Abs_dz = rsLength/2;


    TGeoBBox* Layer_SideS_Abs_Base_Shape = new TGeoBBox("Layer_SideS_Abs_Base_Shape", 
                                                        Layer_Sector_Abs_dx, 
                                                        Layer_Sector_Abs_dy, 
                                                        Layer_Sector_Abs_dz);
   
                                                            
    TGeoVolume* Layer_SideS_Vol = new TGeoVolume("Layer_SideS_Vol", Layer_SideS_Shape, pair);
    TGeoVolume* Layer_SideS_Abs_Base_Vol = new TGeoVolume("Layer_SideS_Abs_Base_Vol", Layer_SideS_Abs_Base_Shape, piron);

    Layer_SideS_Vol->AddNode(Layer_SideS_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, hmin+Layer_Sector_Abs_dy, 0, rotNoRot));

    return Layer_SideS_Vol;
}

//_____________________________________________________________________________
//
void SPD_RS_Barrel_Flexible2() {
//_____________________________________________________________________________
//
// Main:
// Creates a segment of Range System Barrel with a given number of layers
// and fills each layer with MDT detectors.
// Parameters: distance from collision point, barrel size.
//_____________________________________________________________________________

    gSystem->Load("libGeom");

    TGeoManager* geoM = new TGeoManager("World", "Barrel");

    //--- make the top container volume
    TGeoVolume *top = new TGeoVolumeAssembly("RS_Barrel");
    gGeoManager->SetTopVolume(top);


    TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
    rotNoRot->RegisterYourself();
    
    TGeoMaterial* mat_air = new TGeoMaterial("air",0,0,0);
    TGeoMedium*   pair = new TGeoMedium("air",1,mat_air);

    //top->AddNode(SideS_Vol, 1);

    //top->AddNode(L4930_Envelope_Vol, 1);
    
    // Fill top volume with Range System layers
    // Parameters of the Range System size and position
    Double_t rsRadius = 314.4; // cm (Size of the Range System)
    Double_t rsThickness = 139.0; //25.7; //139.0; // cm (Thickness of the Range System)
    Double_t rsLength = 493.0; // cm (Length of the sector along beam axis)

    Double_t v[16];
    Double_t lmin = (rsRadius - rsThickness)*(sqrt(2.0)-1.0); // dimensions (bottom base)
    Double_t lmax = (rsRadius )*(sqrt(2.0)-1.0); // dimensions (top base)
    Double_t hmin = (rsRadius-rsThickness); 
    Double_t hmax = rsRadius;
  
    v[0] =  lmin; v[1] = hmin;
    v[2] = -lmin; v[3] = hmin;
    v[4] = -lmax; v[5] = hmax;
    v[6] =  lmax; v[7] = hmax;
  
    v[8]  =  lmin; v[9]  = hmin;
    v[10] = -lmin; v[11] = hmin;
    v[12] = -lmax; v[13] = hmax;
    v[14] =  lmax; v[15] = hmax;

    TGeoArb8* SideS_Shape = new TGeoArb8("SideS_Shape", rsLength/2, v);
    TGeoVolume* SideS_Vol = new TGeoVolume("SideS_Vol", SideS_Shape, pair);

    // Number of layers calcluation within RS thickness:
    // (Thickness - top absorber - bottom absorber/mdt)/LayerThickness + 1(bottom layer)
    Int_t nLayers = int((rsThickness - 6. - 9.5)/6.5) + 1; 
    cout << "# of Layers within " << rsThickness << " cm: " << nLayers << endl;

    cout<<"Layer #"<< (rsRadius - rsThickness)*(sqrt(2.0)-1.0) << " dimensions (bottom base): ["
     << endl;
    cout<<"Layer #"<< (rsRadius )*(sqrt(2.0)-1.0) << " dimensions (top base): ["
    << endl;

    // Add bottom layer (absrober(6.0cm) + mdt detectors(3.5cm))
    SideS_Vol->AddNode(createBottomLayer(rsRadius, rsThickness, rsLength), 1, 
                 new TGeoCombiTrans("",  0, 0, 0, rotNoRot));
        
    // Add regular layers (3.0cm absorber + 3.5cm mdt detectors)

    for (Int_t i=2; i < nLayers+1; ++i) {   
        SideS_Vol->AddNode(createRegularLayer(i, rsRadius, rsThickness, rsLength), i, 
                    new TGeoCombiTrans("", 0, 0, 0, rotNoRot));
    }

    // Add top absorber (6.0cm)
    SideS_Vol->AddNode(createTopLayer(nLayers+1, rsRadius, rsThickness, rsLength), nLayers+1, 
                new TGeoCombiTrans("", 0, 0, 0, rotNoRot));


    top->AddNode(SideS_Vol,1);
    //top->AddNode(createMDT(493.0), 1);
    gGeoManager->CloseGeometry();
    top->SetLineColor(kMagenta);
    //gGeoManager->SetTopVisible(); // the TOP is invisible
    top->Draw();

    //gGeoManager->CheckGeometry();
    //gGeoManager->CheckGeometryFull();
    //gGeoManager->Test();

    TString outGeoFilenameRoot = "./Barrel_cm.root";
    TFile* outGeoFileRoot = new TFile(outGeoFilenameRoot, "RECREATE");
    geoM->GetTopVolume()->Write();
    outGeoFileRoot->Close();

}
