

// #include "SpdFieldMap.h"
// #include "SpdAxialFieldMap.h"
// #include "SpdMultiField.h"
// #include "SpdConstField.h"
// #include "SpdRadialField.h"
// #include "SpdFieldCreator.h"
// #include "SpdFieldPar.h"
// #include <TFile.h>
// #include <TRandom.h>
// 
//  
void check0();
void check0axial();

void check1();  // set_of_fields ->  set_of_fields
void check2();  // set_of_fields ->  multi_field
void check3();  // multi_field ->  multi_field

void check4();  // check field map 

void readparams(TString filename = "params_tor.root"); 

void CheckField() 
{
   //check0(); 
   check0axial(); 
  
   //check1();  
   //check2();  
   //check3(); 
  
   //check4(); 
   
   //readparams();
}

//_______________________________________________________________________
void check0()
{
    SpdFieldMap* MagField = new SpdFieldMap("Hybrid field");
    //MagField->InitData("map_hyb_2T5cm.bin");
    MagField->InitData("map_sol_6cls5cm2.bin");
    
    MagField->PrintData();
    //MagField->Print();
    
    Double_t x, y, z;
    Double_t bx, by, bz;
    
    const Int_t N = 20;
    
    gRandom->SetSeed(1234);
    
    for (Int_t i(0); i<N; i++) {
      
         x = gRandom->Uniform(MagField->GetXmin(),MagField->GetXmax()); 
         y = gRandom->Uniform(MagField->GetYmin(),MagField->GetYmax()); 
         z = gRandom->Uniform(MagField->GetZmin(),MagField->GetZmax()); 
         
         x = 0; y = 0;
         
         MagField->GetBx(bx,x,y,z);
         MagField->GetBy(by,x,y,z);
         MagField->GetBz(bz,x,y,z);
              
         printf("(%8.3f, %8.3f, %8.3f); B = (%9.2e, %9.2e, %9.2e); Br = %9.2e; ",
                x,y,z,bx,by,bz,TMath::Sqrt(bx*bx+by*by));   
         
         z = -z;     
              
         MagField->GetBx(bx,x,y,z);
         MagField->GetBy(by,x,y,z);
         MagField->GetBz(bz,x,y,z);
         
         cout << " z->-z: ";
     
         printf("(%8.3f, %8.3f, %8.3f); B = (%9.2e, %9.2e, %9.2e); Br = %9.2e; ",
                x,y,z,bx,by,bz,TMath::Sqrt(bx*bx+by*by));   
           
         cout << endl;
                  
     }
}

//_______________________________________________________________________
void check0axial()
{
    SpdAxialFieldMap* MagField = new SpdAxialFieldMap("QSolenoidal field");
    MagField->InitData("map_qsolRZ_6cls2cm.bin");

    MagField->PrintData();
    //MagField->Print();
    
    Double_t x, y, z, r, phi;
    Double_t bx, by, bz;
    
    const Int_t N = 20;
    
    gRandom->SetSeed(0);
    
    Double_t point[3], Field[3];
    
    for (Int_t i(0); i<N; i++) {
      
         r = gRandom->Uniform(MagField->GetRmin(),MagField->GetRmax()); 
         z = gRandom->Uniform(MagField->GetZmin(),MagField->GetZmax()); 
         
         phi = gRandom->Uniform(0,TMath::TwoPi());
         x = r*TMath::Sin(phi);
         y = r*TMath::Cos(phi);
             
         //x = 0; y = 0;
         
//          point[0] = x;
//          point[1] = y;
//          point[2] = z;
//          
//          MagField->GetField(point,Field);
//          
//          printf("(%8.3f, %8.3f, %8.3f); B = (%9.2e, %9.2e, %9.2e); Br = %9.2e; ",
//                point[0],point[1],point[2],Field[0],Field[1],Field[2],TMath::Sqrt(Field[0]*Field[0]+Field[1]*Field[1]));   
// 
//          point[2] = -point[2];     
//         
//          MagField->GetField(point,Field);
//          
//          cout << " z->-z: ";
//          
//          printf("(%8.3f, %8.3f, %8.3f); B = (%9.2e, %9.2e, %9.2e); Br = %9.2e; \n",
//                point[0],point[1],point[2],Field[0],Field[1],Field[2],TMath::Sqrt(Field[0]*Field[0]+Field[1]*Field[1]));   
         
         
         MagField->GetBx(bx,x,y,z);
         MagField->GetBy(by,x,y,z);
         MagField->GetBz(bz,x,y,z);
                       
         printf("(%8.3f, %8.3f, %8.3f); B = (%9.2e, %9.2e, %9.2e); Br = %9.2e; ",
               x,y,z,bx,by,bz,TMath::Sqrt(bx*bx+by*by));   
        
         z = -z;     
              
         MagField->GetBx(bx,x,y,z);
         MagField->GetBy(by,x,y,z);
         MagField->GetBz(bz,x,y,z);
         
         cout << " z->-z: ";
     
         printf("(%8.3f, %8.3f, %8.3f); B = (%9.2e, %9.2e, %9.2e); Br = %9.2e; ",
                x,y,z,bx,by,bz,TMath::Sqrt(bx*bx+by*by));   
           
         cout << endl;
                  
     }
     
//     cout  << " X[min,max]: " << MagField->GetXmin() << " " << MagField->GetXmax() << " Nx = " << MagField->GetNx() << endl;
//     cout  << " Y[min,max]: " << MagField->GetYmin() << " " << MagField->GetYmax() << " Ny = " << MagField->GetNy() << endl;
//     cout  << " Z[min,max]: " << MagField->GetZmin() << " " << MagField->GetZmax() << " Nz = " << MagField->GetNz() << endl;
}

//________________________________________________________________________
void check1() 
{
   cout << "\n\n=======================================================" << endl;
   cout << "====================  CHECK 1 ========================= " << endl;
   cout << "=======================================================\n\n" << endl;
   
   SpdRegion* reg;
   SpdField* field[7];
  
   field[0] = new SpdField();
   reg = field[0]->CreateFieldRegion("xxx");
   
   
   field[1] = new SpdField();
   
   field[2] = new SpdField();
   reg = field[2]->CreateFieldRegion("box");
   reg->SetBoxRegion(-1000, 1000, -1000, 1000, -1000, 1000);
   
   field[3] = new SpdField();
   
   field[4] = new SpdField("F. Example");
   
   field[5] = new SpdField("F. Example");
   reg = field[5]->CreateFieldRegion("box");
   reg->SetBoxRegion(-500, 500, -100, 100, -10, 50);
   
   field[6] = new SpdField();
   //reg = field[6]->CreateFieldRegion("box");
   reg = field[6]->CreateFieldRegion("tube");
   reg->SetTubeRegion(0, 1000, -1000, 1000);
   field[6]->SwitchFieldRegion(kFALSE);
   
   field[3]->PrintListOfFields();
   
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
   for (Int_t i(0); i<7; i++) field[i]->Print();
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
   
   SpdFieldPar* pars = new SpdFieldPar();
   
   for (int i(0); i<7; i++) field[i]->FillParContainer(pars);
   
   pars->printParams();
   
   for (int i(0); i<7; i++) delete field[i];
   
   
   //-----------------------------------------------------------------
   for (int i(0); i<5; i++) {
        field[i] = new SpdField(pars);
   }
   for (int i(5); i<7; i++) {
        field[i] = new SpdField(pars,"F. Example");
   }
   
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
   for (Int_t i(0); i<7; i++) field[i]->Print();
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
   
}


//________________________________________________________________________
void check2() 
{
  
   cout << "\n\n=======================================================" << endl;
   cout << "====================  CHECK 2 ========================= " << endl;
   cout << "=======================================================\n\n" << endl;
   
  
   SpdRegion* reg;
   SpdField* field[7];
  
   field[0] = new SpdField();
   reg = field[0]->CreateFieldRegion("xxx");
   
   field[1] = new SpdField();
   
   field[2] = new SpdField();
   reg = field[2]->CreateFieldRegion("box");
   reg->SetBoxRegion(-1000, 1000, -1000, 1000, -1000, 1000);
   
   field[3] = new SpdField();
   
   field[4] = new SpdField("F. Example");
   
   field[5] = new SpdField("F. Example");
   reg = field[5]->CreateFieldRegion("box");
   reg->SetBoxRegion(-500, 500, -100, 100, -10, 50);
   
   field[6] = new SpdField();
   //reg = field[6]->CreateFieldRegion("box");
   reg = field[6]->CreateFieldRegion("tube");
   reg->SetTubeRegion(0, 1000, -1000, 1000);
   field[6]->SwitchFieldRegion(kFALSE);
   
   field[3]->PrintListOfFields();
   
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
   for (Int_t i(0); i<7; i++) field[i]->Print();
   cout << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
   
   SpdFieldPar* pars = new SpdFieldPar();
   
   for (int i(0); i<7; i++) field[i]->FillParContainer(pars);
   
   pars->printParams();
   
   for (int i(0); i<7; i++) delete field[i];
   
   
   //-----------------------------------------------------------------
   SpdFieldCreator* creator = new SpdFieldCreator();
   
   FairField* mfield = creator->createFairField(pars);
  
   mfield->Print("");
}

//________________________________________________________________________
void check3() 
{
  
   cout << "\n\n=======================================================" << endl;
   cout << "====================  CHECK 3 ========================= " << endl;
   cout << "=======================================================\n\n" << endl;
  
  
   SpdRegion* reg;
   
   SpdField* field[7];
   
   field[0] = new SpdField();
   reg = field[0]->CreateFieldRegion("xxx");
   
   field[1] = new SpdConstField();
   field[1]->SetFieldParameter(0,2.3);
   field[1]->SetFieldParameter(1,0.5);
   field[1]->SetFieldParameter(2,9.55);
   
   reg = field[1]->CreateFieldRegion("physical");
   //reg->SetPhysicalRegion("CAVE","volume");
   reg->SetPhysicalRegion("Aluminium","medium");
   
   cout <<"---------------------"<<endl;
   
   field[2] = new SpdField();
   reg = field[2]->CreateFieldRegion("box");
   reg->SetBoxRegion(-1000, 1000, -1000, 1000, -1000, 1000);
   
   field[3] = new SpdField("F Example");
   
   field[4] = new SpdRadialField();
   field[4]->SetFieldParameter(0,5.98);
   reg = field[4]->CreateFieldRegion("tube");
   reg->SetTubeRegion(0, 1000, -500, 500);
   
   field[5] = new SpdField("F Example");
   reg = field[5]->CreateFieldRegion("box");
   reg->SetBoxRegion(-500, 500, -100, 100, -10, 50);
   
   field[6] = new SpdField();
   //reg = field[6]->CreateFieldRegion("box");
   reg = field[6]->CreateFieldRegion("tube");
   reg->SetTubeRegion(0, 1000, -1000, 1000);
   field[6]->SwitchFieldRegion(kFALSE);
   
   SpdMultiField* mfield = new SpdMultiField();
   for (int i(0); i<7; i++) {
        mfield->AddField(field[i]);
   }
   
   mfield->SetOverlapped(true); 
   mfield->SetOverlapSumMode(1); // 0 (default) or 1 
   
   //mfield->SwitchField("SpdField_1",0); // switch off 
   mfield->SwitchField("F Example_1",0); // switch off 
   mfield->SwitchField("F Example",0); // switch off 
   //mfield->SwitchField("F Example_2",0); // switch off 
   mfield->SwitchField("SpdField_2",0); // switch off 
   //mfield->SwitchField("SpdField_3",0); // switch off 
   
   //return;
   
   mfield->Print("");
   
   //return;
  
   SpdFieldPar* pars = new SpdFieldPar();
   mfield->FillParContainer(pars);
   
   pars->printParams();
   
   delete mfield;
   
   //-----------------------------------------------------------------
   SpdFieldCreator* creator = new SpdFieldCreator();
   
   mfield = (SpdMultiField*)creator->createFairField(pars);
   
   mfield->SwitchField("SpdField_2",1); // switch on 
  
   mfield->Print("");
}

//________________________________________________________________________
void check4() 
{
  
   cout << "\n\n=======================================================" << endl;
   cout << "====================  CHECK 4 ========================= " << endl;
   cout << "=======================================================\n\n" << endl;
  
   SpdRegion* reg;
   SpdFieldMap* field;
   
   field = new SpdFieldMap("Map Example");
   field->InitData("karta3-1.dat");
   field->SetApproxMethod(0);
   field->MultiplyField(5);
   
   //reg = field->CreateFieldRegion("box");
   //reg->SetBoxRegion(-50, 50, -50, 50, -50, 50);
   //reg->SetBoxRegion(-190, 190, -190, 190, -90, 90);
   
   field->Print();
   
   return;
   
   SpdFieldPar* pars = new SpdFieldPar();
   field->FillParContainer(pars);
   
   pars->printParams();
   
   field->Clear();
   
   delete field;
   
   //-----------------------------------------------------------------
   SpdFieldCreator* creator = new SpdFieldCreator();
   
   field = (SpdFieldMap*)creator->createFairField(pars);
   
   field->Print();
   
   //SpdFieldMapData* md = field->CreateFieldMapData(0,77,77,37);
   SpdFieldMapData* md = field->CreateFieldMapData(0,51,51,51);
   
   md->PrintData();
   
   md->WriteAsciiFile("cfield.dat");
}

//________________________________________________________________________
void readparams(TString filename)
{
  TFile* fp = new TFile(filename);
  if (!fp) return;
  
  SpdFieldPar* pars = (SpdFieldPar*)fp->Get("SpdFieldPar");
  
  if (!pars) return;
  
  pars->print(1);
  
  fp->Close();
}

