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

    TGeoMaterial* mat = new TGeoMaterial("Vacuum",0,0,0);
    TGeoMedium*   pvacuum = new TGeoMedium("Vacuum",1,mat);

    TGeoMaterial* mat_pvc = new TGeoMaterial("PVC",0,0,0);
    TGeoMedium*   pPVC = new TGeoMedium("PVC",1,mat_pvc);

    TGeoMaterial* mat_iron = new TGeoMaterial("iron",0,0,0);
    TGeoMedium*   piron = new TGeoMedium("iron",1,mat_iron);

    TGeoMaterial* mat_g10 = new TGeoMaterial("G10",0,0,0);
    TGeoMedium*   pg10 = new TGeoMedium("G10",1,mat_g10);

    TGeoMaterial* mat_copper = new TGeoMaterial("copper",0,0,0);
    TGeoMedium*   pcopper = new TGeoMedium("copper",1,mat_copper);

    // Shapes and volumes ---------------------------------------------------------------
    // General dimensions
    Double_t widthEnv = 8.5; //cm xAxis
    Double_t thicknessEnv = 1.64; //cm zAxis
    Double_t xSize = 1.0;
    Double_t shiftZ = 3.5/2; //distance between layers is 35mm

    TString transName;
    Int_t Nenvelopes;
    Int_t IncEnv = 0;

    //Matrices
    TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
    rotNoRot->RegisterYourself();
    TGeoRotation* rotZ180 = new TGeoRotation("rotZ180", 0., 0., 180.);
    rotZ180->RegisterYourself();

    //check!
    TGeoCombiTrans* M0 = new TGeoCombiTrans("M0", 0., 0, 0., rotNoRot);
    TGeoCombiTrans* M1 = new TGeoCombiTrans("M1", 0., -170.5, 0., rotNoRot);
    TGeoCombiTrans* M2 = new TGeoCombiTrans("M2", 0., -45.0, 0., rotNoRot);
    TGeoCombiTrans* M3 = new TGeoCombiTrans("M3", 0., -184.5, 0., rotNoRot);
    M0->RegisterYourself();
    M1->RegisterYourself();
    M2->RegisterYourself();
    M3->RegisterYourself();

    Double_t empty_value = (2.0+2.0+8.5)+6.5*2;
    Double_t L_MDT_Envelope = length; //493.0;
    Double_t L_MDT_Envelope_sensitive = L_MDT_Envelope-empty_value;


Int_t ind = int(length*10);
    //Shapes
    //<box name="Indent20_PartBody" x="(42.500000)*2" y="(10.000000)*2" z="(8.200000)*2" lunit="mm"/>
    TGeoBBox* Indent20_Shape = new TGeoBBox("Indent20_Shape", widthEnv/2,
                                                            1.0,
                                                            thicknessEnv/2);
    //<box name="FEB_box_PartBody" x="(42.500000)*2" y="(42.500000)*2" z="(8.200000)*2" lunit="mm"/>
    TGeoBBox* FEB_box_Shape = new TGeoBBox("FEB_box_Shape", widthEnv/2,
                                                            widthEnv/2,
                                                            thicknessEnv/2);
    //<box name="DeadMat_PartBody" x="(40.700000)*2" y="(32.500000)*2" z="(5.150000)*2" lunit="mm"/>
    TGeoBBox* DeadMat_Shape = new TGeoBBox("DeadMat_Shape", 4.07,
                                                            3.25,
                                                            0.515);
    TGeoVolume* Indent20_Vol = new TGeoVolume("Indent20_Vol", Indent20_Shape, pair);
    TGeoVolume* FEB_box_Vol = new TGeoVolume("FEB_box_Vol", FEB_box_Shape, pair);
    TGeoVolume* DeadMat_Vol = new TGeoVolume("DeadMat_Vol", DeadMat_Shape, pMDTMixture);

    //----------------------4930------------------------------------//
    TGeoBBox* L_Al_A_Shape = new TGeoBBox(Form("L%d_Al_A_Shape",ind), 4.03,
                                                                L_MDT_Envelope_sensitive/2,
                                                                0.03);
    TGeoBBox* L_Al_B_Shape = new TGeoBBox(Form("L%d_Al_B_Shape",ind), 0.03,
                                                                L_MDT_Envelope_sensitive/2,
                                                                0.42);
    TGeoBBox* L_GasCell_Shape = new TGeoBBox(Form("L%d_GasCell_Shape",ind), 0.47,
                                                                        L_MDT_Envelope_sensitive/2,
                                                                        0.46);
    TGeoBBox* L_Cell_Shape = new TGeoBBox(Form("L%d_Cell_Shape",ind), 0.5,
                                                                L_MDT_Envelope_sensitive/2,
                                                                0.46);
    TGeoBBox* L_MDT_S_Shape = new TGeoBBox(Form("L%d_MDT_S_Shape",ind), 4.0,
                                                                    L_MDT_Envelope_sensitive/2,
                                                                    0.46);
    TGeoBBox* L_Envelope_Shape = new TGeoBBox(Form("L%d_Envelope_Shape",ind), widthEnv/2,
                                                                        L_MDT_Envelope/2,
                                                                        thicknessEnv/2);
                                                                                 
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
                            new TGeoCombiTrans("", 0.47, 0, -0.04, rotNoRot));

    //8 Cells in MDT_S
    for (UInt_t i=0; i<8; i++) {
        transName.Form("trans_cell_%d", i);
        TGeoCombiTrans* trans1 = new TGeoCombiTrans(transName.Data(), -4.0 + xSize/2. + (Double_t)i*xSize, 0, 0., rotNoRot);
        trans1->RegisterYourself();
        L_MDT_S_Vol->AddNode(L_Cell_Vol, i+1, trans1);
    }

    L_Envelope_Vol->AddNode(Indent20_Vol, 1,
                                new TGeoCombiTrans("", 0, L_MDT_Envelope/2 - 1.0, 0, rotNoRot));
    L_Envelope_Vol->AddNode(Indent20_Vol, 2,
                                new TGeoCombiTrans("", 0, 1.0 - L_MDT_Envelope/2, 0, rotNoRot));
    L_Envelope_Vol->AddNode(FEB_box_Vol, 1,
                                new TGeoCombiTrans("", 0, L_MDT_Envelope/2-2.0-8.5*0.5, 0, rotNoRot));
    L_Envelope_Vol->AddNode(DeadMat_Vol, 1,
                                new TGeoCombiTrans("", 0, ((L_MDT_Envelope-(2.0+2.0+8.5))-6.5)/2-widthEnv/2, 0, rotNoRot));
    L_Envelope_Vol->AddNode(DeadMat_Vol, 2,
                                new TGeoCombiTrans("", 0, -((L_MDT_Envelope-(2.0+2.0+8.5))-6.5)/2-widthEnv/2, 0, rotNoRot));
    L_Envelope_Vol->AddNode(L_Al_A_Vol, 1,
                                new TGeoCombiTrans("", 0, -widthEnv/2, -0.36, rotNoRot));
    L_Envelope_Vol->AddNode(L_Al_B_Vol, 1,
                                new TGeoCombiTrans("", -4.0, -widthEnv/2, 0.09, rotNoRot));
    L_Envelope_Vol->AddNode(L_MDT_S_Vol, 1,
                                new TGeoCombiTrans("", 0.03, -widthEnv/2, 0.13, rotNoRot));                                                          
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

    Double_t Sector_dy = rsLength/2.0;

    Double_t widthEnv = 8.5; //cm xAxis
    Double_t thicknessEnv = 1.64; //cm zAxis
    Double_t shiftZ = 3.5/2; //distance between layers is 35mm

    TGeoMaterial *mat_iron = new TGeoMaterial("iron",0,0,0);
    TGeoMedium   *piron = new TGeoMedium("iron",1,mat_iron);

    TGeoMaterial *mat_air = new TGeoMaterial("air",0,0,0);
    TGeoMedium   *pair = new TGeoMedium("air",1,mat_air);

     //Matrices
    TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
    rotNoRot->RegisterYourself();
    TGeoRotation* rotZ180 = new TGeoRotation("rotZ180", 0., 0., 180.);
    rotZ180->RegisterYourself();

    Double_t Layer1_Sector_dz = layerThickness/2; // z-coord of the layer center 
    Double_t Layer1_Sector_dx1 = 1.0*(rsRadius-rsThickness)*(sqrt(2.0)-1.0); // X-coord of the bottom base
    Double_t Layer1_Sector_dx2 = 1.0*(rsRadius-rsThickness+layerThickness)*(sqrt(2.0)-1.0); // X-coord of the top base
    Double_t Layer1_Sector_dy = Sector_dy; // y-coord of the layer center
    cout<<"Layer #1 dimensions (bottom base): ["
        << Layer1_Sector_dx1*2<<" x " 
        << Layer1_Sector_dz*2 << " x " 
        << Layer1_Sector_dy*2 << "] cm" << endl;
    cout<<"Layer #1 dimensions (top base): ["
        <<Layer1_Sector_dx2*2<<" x " 
        << Layer1_Sector_dz*2 << " x " 
        << Layer1_Sector_dy*2 << "] cm" << endl;
    


    TGeoTrd2* Layer1_SideS_Shape = new TGeoTrd2("Layer_SideS_Shape", 
                                                1.0*Layer1_Sector_dx1, 
                                                1.0*Layer1_Sector_dx2, 
                                                Layer1_Sector_dy, 											 
                                                Layer1_Sector_dy,
                                                Layer1_Sector_dz);

    TGeoBBox* Layer1_SideS_Abs_Base_Shape = new TGeoBBox("Layer_SideS_Abs_Base_Shape", 
                                                        1.0*Layer1_Sector_dx1, 
                                                        Layer1_Sector_dy, 
                                                        Layer1_Sector_dz-3.5/2);
                                                            
    TGeoVolume* Layer1_SideS_Vol = new TGeoVolume("Layer_SideS_Vol", Layer1_SideS_Shape, pair);
    TGeoVolume* Layer1_SideS_Abs_Base_Vol = new TGeoVolume("Layer_SideS_Abs_Base_Vol", Layer1_SideS_Abs_Base_Shape, piron);

    Layer1_SideS_Vol->AddNode(Layer1_SideS_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, 0, -1.75, rotNoRot));


    Double_t Layer1_Envelope_startX = -1.0*Layer1_Sector_dx1;
    Double_t Layer1_Envelope_shiftX = Layer1_Envelope_startX + widthEnv/2;

    Double_t Layer1_length = 2*abs(Layer1_Envelope_startX);
    cout<<"Layer #1 length: " << Layer1_length << endl;

    int Nenvelopes = int(Layer1_length/widthEnv);
    cout << "Number of MDT detector in Layer #1: " << Nenvelopes << endl;
    for (UInt_t i=0; i<Nenvelopes; i++) {
    //for (UInt_t i=0; i<1; i++) {

        Layer1_SideS_Vol->AddNode(createMDT(493.0), i+1, new TGeoCombiTrans("", Layer1_Envelope_shiftX, 0, 1.75+thicknessEnv/2, rotNoRot)); 
        Layer1_Envelope_shiftX = Layer1_Envelope_shiftX + widthEnv;
    }

    // Zero Bi-Layer Geometry (currently disabled) 
    // Layer1_Envelope_shiftX = -60.0-0.5;
    // Layer1_Envelope_shiftX = Layer1_Envelope_startX + widthEnv/2;

    // Nenvelopes = 14;
    // for (UInt_t i=0; i<Nenvelopes; i++) 
    // {
    // Layer1_SideS_Vol->AddNode(createMDT(493.0), i+15, new TGeoCombiTrans("", Layer1_Envelope_shiftX, 0, 1.2+thicknessEnv/2+thicknessEnv, rotNoRot)); 
    // Layer1_Envelope_shiftX = Layer1_Envelope_shiftX + widthEnv;
    // }


    return Layer1_SideS_Vol;
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
    Double_t shiftZ = 3.5/2; //distance between layers is 35mm

    TGeoMaterial *mat_iron = new TGeoMaterial("iron",0,0,0);
    TGeoMedium   *piron = new TGeoMedium("iron",1,mat_iron);

    TGeoMaterial *mat_air = new TGeoMaterial("air",0,0,0);
    TGeoMedium   *pair = new TGeoMedium("air",1,mat_air);

     //Matrices
    TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
    rotNoRot->RegisterYourself();
    TGeoRotation* rotZ180 = new TGeoRotation("rotZ180", 0., 0., 180.);
    rotZ180->RegisterYourself();

    Double_t Layer1_Sector_dz = layerThickness/2; // z-coord of the layer center 
    Double_t Layer1_Sector_dx1 = 1.0*(rsRadius - rsThickness + 9.5 + (layerN-2)*layerThickness)*(sqrt(2.0)-1.0); // X-coord of the bottom base
    Double_t Layer1_Sector_dx2 = 1.0*(rsRadius - rsThickness + 9.5 + (layerN-1)*layerThickness)*(sqrt(2.0)-1.0); // X-coord of the top base
    Double_t Layer1_Sector_dy = Sector_dy; // y-coord of the layer center
    cout<<"Layer #"<< layerN << " dimensions (bottom base): ["
        <<Layer1_Sector_dx1*2<<" x " 
        << Layer1_Sector_dz*2 << " x " 
        << Layer1_Sector_dy*2 << "] cm" << endl;
    cout<<"Layer #"<< layerN << " dimensions (top base): ["
        <<Layer1_Sector_dx2*2<<" x " 
        << Layer1_Sector_dz*2 << " x " 
        << Layer1_Sector_dy*2 << "] cm" << endl;
    


    TGeoTrd2* Layer1_SideS_Shape = new TGeoTrd2("Layer_SideS_Shape", 
                                                1.0*Layer1_Sector_dx1, 
                                                1.0*Layer1_Sector_dx2, 
                                                Layer1_Sector_dy, 											 
                                                Layer1_Sector_dy,
                                                Layer1_Sector_dz);

    TGeoBBox* Layer1_SideS_Abs_Base_Shape = new TGeoBBox("Layer_SideS_Abs_Base_Shape", 
                                                        1.0*Layer1_Sector_dx1, 
                                                        Layer1_Sector_dy, 
                                                        3.0/2);
                                                            
    TGeoVolume* Layer1_SideS_Vol = new TGeoVolume("Layer_SideS_Vol", Layer1_SideS_Shape, pair);
    TGeoVolume* Layer1_SideS_Abs_Base_Vol = new TGeoVolume("Layer_SideS_Abs_Base_Vol", Layer1_SideS_Abs_Base_Shape, piron);

    Layer1_SideS_Vol->AddNode(Layer1_SideS_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, 0, -1*Layer1_Sector_dz+3.0/2, rotNoRot));

    if (layerN % 2 != 0) {
        Double_t Layer1_Envelope_startX = Layer1_Sector_dx1;
        Double_t Layer1_Envelope_shiftX = Layer1_Envelope_startX - widthEnv/2;

        Double_t Layer1_length = 2*abs(Layer1_Envelope_startX);
        cout<<"Layer #" << layerN << " length: " << Layer1_length << endl;

        int Nenvelopes = int((Layer1_length - widthEnv/2)/widthEnv);
        cout << "Number of MDT detector in Layer" << layerN << ": " << Nenvelopes << endl;
        for (UInt_t i=0; i<Nenvelopes; i++) {

            Layer1_SideS_Vol->AddNode(createMDT(493.0), i+1, new TGeoCombiTrans("", Layer1_Envelope_shiftX, 0, 0.75+thicknessEnv/2, rotNoRot)); 
            Layer1_Envelope_shiftX = Layer1_Envelope_shiftX - widthEnv;
        }
    }
    else {

        Double_t Layer1_Envelope_startX = -1.0*abs(Layer1_Sector_dx1);
        Double_t Layer1_Envelope_shiftX = Layer1_Envelope_startX + widthEnv/2;

        Double_t Layer1_length = 2*abs(Layer1_Envelope_startX);
        cout<<"Layer #" << layerN << " length: " << Layer1_length << endl;

        int Nenvelopes = int((Layer1_length)/widthEnv);
        cout << "Number of MDT detector in Layer" << layerN << ": " << Nenvelopes << endl;
        for (UInt_t i=0; i<Nenvelopes; i++) {

            Layer1_SideS_Vol->AddNode(createMDT(493.0), i+1, new TGeoCombiTrans("", Layer1_Envelope_shiftX, 0, 0.75+thicknessEnv/2, rotNoRot)); 
            Layer1_Envelope_shiftX = Layer1_Envelope_shiftX + widthEnv;
        }
    }

    return Layer1_SideS_Vol;
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
    Double_t shiftZ = 3.5/2; //distance between layers is 35mm

    TGeoMaterial *mat_iron = new TGeoMaterial("iron",0,0,0);
    TGeoMedium   *piron = new TGeoMedium("iron",1,mat_iron);

    TGeoMaterial *mat_air = new TGeoMaterial("air",0,0,0);
    TGeoMedium   *pair = new TGeoMedium("air",1,mat_air);

     //Matrices
    TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
    rotNoRot->RegisterYourself();
    TGeoRotation* rotZ180 = new TGeoRotation("rotZ180", 0., 0., 180.);
    rotZ180->RegisterYourself();

    Double_t Layer1_Sector_dz = layerThickness/2; // z-coord of the layer center 
    Double_t Layer1_Sector_dx1 = 1.0*(rsRadius - rsThickness + 9.5 + (layerN-1)*layerThickness)*(sqrt(2.0)-1.0); // X-coord of the bottom base
    Double_t Layer1_Sector_dx2 = 1.0*(rsRadius - rsThickness + 9.5 + (layerN)*layerThickness)*(sqrt(2.0)-1.0); // X-coord of the top base
    Double_t Layer1_Sector_dy = Sector_dy; // y-coord of the layer center
    cout<<"Layer #"<< layerN << " dimensions (bottom base): ["
        <<Layer1_Sector_dx1*2<<" x " 
        << Layer1_Sector_dz*2 << " x " 
        << Layer1_Sector_dy*2 << "] cm" << endl;
    cout<<"Layer #"<< layerN << " dimensions (top base): ["
        <<Layer1_Sector_dx2*2<<" x " 
        << Layer1_Sector_dz*2 << " x " 
        << Layer1_Sector_dy*2 << "] cm" << endl;
    


    TGeoTrd2* Layer1_SideS_Shape = new TGeoTrd2("Layer_SideS_Shape", 
                                                1.0*Layer1_Sector_dx1, 
                                                1.0*Layer1_Sector_dx2, 
                                                Layer1_Sector_dy, 											 
                                                Layer1_Sector_dy,
                                                Layer1_Sector_dz);

    TGeoBBox* Layer1_SideS_Abs_Base_Shape = new TGeoBBox("Layer_SideS_Abs_Base_Shape", 
                                                        1.0*Layer1_Sector_dx1, 
                                                        Layer1_Sector_dy, 
                                                        Layer1_Sector_dz);
                                                            
    TGeoVolume* Layer1_SideS_Vol = new TGeoVolume("Layer_SideS_Vol", Layer1_SideS_Shape, pair);
    TGeoVolume* Layer1_SideS_Abs_Base_Vol = new TGeoVolume("Layer_SideS_Abs_Base_Vol", Layer1_SideS_Abs_Base_Shape, piron);

    Layer1_SideS_Vol->AddNode(Layer1_SideS_Abs_Base_Vol, 1, new TGeoCombiTrans("", 0, 0, 0, rotNoRot));


    return Layer1_SideS_Vol;
}

//_____________________________________________________________________________
//
void SPD_RS_Barrel_Flexible() {
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


    TGeoRotation* rotTest = new TGeoRotation("", 180., 90., 0.);
    rotTest->RegisterYourself();
    TGeoRotation* rotNoRot = new TGeoRotation("rotNoRot", 0., 0., 0.);
    rotNoRot->RegisterYourself();
    TGeoRotation* rotZ180 = new TGeoRotation("rotZ180", 0., 0., 180.);
    rotZ180->RegisterYourself();

    TGeoMaterial* mat_air = new TGeoMaterial("air",0,0,0);
    TGeoMedium*   pair = new TGeoMedium("air",1,mat_air);

    //top->AddNode(SideS_Vol, 1, new TGeoCombiTrans("", 0, 0, 45.0, rotTest));
    //top->AddNode(SideS_Vol, 1);

    //top->AddNode(L4930_Envelope_Vol, 1);
    //top->AddNode(createMDT(493.0), 1);

    // Fill top volume with Range System layers
    // Parameters of the Range System size and position
    Double_t rsRadius = 314.4; // cm (Outer radium of the Range System)
    Double_t rsThickness = 139.0; //25.7; //139.0; // cm (Thickness of the Range System)
    Double_t rsLength = 493.0; // cm (Length of the sector along beam axis)

    TGeoTrd2* SideS_Shape = new TGeoTrd2("SideS_Shape", 
									  (rsRadius - rsThickness)*(sqrt(2.0)-1.0), 
									  (rsRadius )*(sqrt(2.0)-1.0), //1234 Fe without bilayer MDTs
									  rsLength/2, 											 
									  rsLength/2,
									  rsThickness/2); //805 only Fe (without bilayer MDTs)
    TGeoVolume* SideS_Vol = new TGeoVolume("SideS_Vol", SideS_Shape, pair);
    cout<<"Layer #"<< (rsRadius - rsThickness)*(sqrt(2.0)-1.0) << " dimensions (bottom base): ["
     << endl;
    cout<<"Layer #"<< (rsRadius )*(sqrt(2.0)-1.0) << " dimensions (top base): ["
    << endl;


    // Number of layers calcluation within RS thickness:
    // (Thickness - top absorber - bottom absorber/mdt)/LayerThickness + 1(bottom layer)
    Int_t nLayers = int((rsThickness - 6. - 9.5)/6.5) + 1; 
    cout << "# of Layers within " << rsThickness << " cm: " << nLayers << endl;

    // Add bottom layer (absrober(6.0cm) + mdt detectors(3.5cm))
    SideS_Vol->AddNode(createBottomLayer(rsRadius, rsThickness, rsLength), 1, 
                 new TGeoCombiTrans("", 0, 0,  -1.*(rsThickness)/2 + 4.75, rotNoRot));
    
    // Add regular layers (3.0cm absorber + 3.5cm mdt detectors)
//     for (Int_t i=2; i < nLayers+1; ++i) {   
//         
//         SideS_Vol->AddNode(createRegularLayer(i, rsRadius, rsThickness, rsLength), i, 
//                     //new TGeoCombiTrans("", 0, 0, rsRadius - rsThickness + (i-1)*6.5, rotNoRot));
//                     new TGeoCombiTrans("", 0, 0, -1.*(rsThickness)/2 + (i-1)*6.5 + 9.5 - 3.25, rotNoRot));
//     }
    // Add top absorber (6.0cm)
    SideS_Vol->AddNode(createTopLayer(nLayers+1, rsRadius, rsThickness, rsLength), nLayers+1, 
                new TGeoCombiTrans("", 0, 0, 1.0*(rsThickness)/2 - 6.0/2, rotNoRot));


    top->AddNode(SideS_Vol,1);

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




