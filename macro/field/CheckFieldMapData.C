
void check_0();
void check_1();
void check_2();

void CheckFieldMapData() 
{
    check_0();
    //check_1();
    //check_2();
}

//_______________________________________________________________________
void check_0()
{
    Int_t n;
    Double_t bx, by, bz;
    
    Int_t N = 10;
    const Int_t rnum = 4534343;
  
    // default path to a field maps
    TString path = getenv("VMCWORKDIR");
    path += "/input/";
    
    cout << path << endl;
    
    SpdFieldMapData* mdata = new SpdFieldMapData();

//---------------------------------------------- INIT 1
    //mdata->InitData("map_sol_6cls5cm2.bin",path);
    
    mdata->InitData("map_hyb_1T5cm.bin",path);  
 
    mdata->PrintData();
         
    //return;
    
    gRandom->SetSeed(rnum);
    
    N = mdata->GetNTotal();
    
    for (Int_t i(0); i<N; i++) {
      
         //n = gRandom->Integer(mdata->GetNTotal()); 
         n = i;

//          cout << i+1 << "/" << N << ": " << mdata->GetBx(n) 
//                      << " " << mdata->GetBy(n) 
//                      << " " << mdata->GetBz(n)           
//                      << endl;
                     
         bx = mdata->GetBx(n);
         by = mdata->GetBy(n);
         bz = mdata->GetBz(n);
                     
         printf("%4d:  B = (%9.2e, %9.2e, %9.2e); Br = %9.2e; \n",
                i+1,bx,by,bz,TMath::Sqrt(bx*bx+by*by));   
        
         if (i == 20) break;          
    }
//---------------------------------------------- 
    
    //mdata->WriteAsciiFile("map_hyb_1T5cm_1.dat"); 
    //mdata->WriteBinaryFile("map_hyb_1T5cm_1.bin"); 
    //mdata->WriteBinaryFile("map_sol_6cls5cm2.bin");  
    //return;
    
  //----------------------------------------------  INIT 2  
   
//     mdata->InitData("map_hyb_1T5cm_1.dat","../macro");  
// 
//     mdata->PrintData();
//     
//     gRandom->SetSeed(rnum);
//     
//     for (Int_t i(0); i<N; i++) {
//       
//          n = gRandom->Integer(mdata->GetNTotal()); 
//          
//          cout << i << " " << mdata->GetBx(n) 
//                    << " " << mdata->GetBy(n) 
//                    << " " << mdata->GetBz(n) 
//                    << endl;
//     }
    
  //----------------------------------------------  INIT 3 
 
//     mdata->InitData("map_hyb_1T5cm_1.bin","../macro");
//     
//     mdata->PrintData();
//     
//     gRandom->SetSeed(rnum);
//     
//     for (Int_t i(0); i<N; i++) {
//       
//          n = gRandom->Integer(mdata->GetNTotal()); 
//          
//          cout << i << " " << mdata->GetBx(n) 
//                    << " " << mdata->GetBy(n) 
//                    << " " << mdata->GetBz(n) 
//                    << endl;
//     }   

}

//_______________________________________________________________________
void check_1()
{
  
    SpdFieldMapData* mdata_1 = new SpdFieldMapData();
    mdata_1->InitData("karta_200_30.dat");  
    mdata_1->PrintData();
    
    mdata_1->WriteAsciiFile("afield");
    
    
    SpdFieldMapData* mdata_2 = new SpdFieldMapData("map data 2");
    mdata_2->InitData(mdata_1);
    mdata_2->PrintData();
    

    SpdFieldMapData* mdata_3 = new SpdFieldMapData();
    mdata_3->SetPathToData("");
    mdata_3->InitData("afield.dat","");
    mdata_3->PrintData();
}

//_______________________________________________________________________
void check_2()
{
   SpdRegion* reg;
   SpdField* field;
   SpdFieldMapData* mdata;
  
   /*---- define field ---- */
   
   //field = new SpdConstField();
   //field->SetFieldParameter(0,1);
   //field->SetFieldParameter(1,2);
   //field->SetFieldParameter(2,3);
   
   field = new SpdRadialField();
   field->SetFieldParameter(0,1);
   
   /*---- define region ---- */
   
   //reg = field->CreateFieldRegion("box");
   //reg->SetBoxRegion(-1000, 1000, -1000, 1000, -1000, 1000);
   
   //reg = field->CreateFieldRegion("tube");
   //reg->SetTubeRegion(0, 1000, -1000, 1000);
   
   /*---- create data map ---- */
  
   //mdata = field->CreateFieldMapData();
    
   //mdata = field->CreateFieldMapData(0,11,11,11);
  
   SpdFieldMapData* md = new SpdFieldMapData();
   md->InitData(-100,100,21, -100,100,21, -100,100,21);
   mdata = field->CreateFieldMapData(md);
   
   mdata->WriteAsciiFile("bfield.dat");
   
   
   field->Print();
   mdata->PrintData();
   
   
   SpdFieldMapData* mdata_1 = new SpdFieldMapData();
   mdata_1->SetPathToData("");
   mdata_1->InitData("bfield.dat","");
   mdata_1->PrintData();
  
}


