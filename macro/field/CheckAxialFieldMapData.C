

void CheckAxialFieldMapData() 
{

    Int_t n;
    Double_t bx, by, bz, br, bb;
    
    Int_t N = 10;
    const Int_t rnum = 4534343;
  
    // default path to a field maps
    TString path = getenv("VMCWORKDIR");
    path += "/input/";
    
    cout << path << endl;
    
    SpdAxialFieldMapData* mdata = new SpdAxialFieldMapData();

//---------------------------------------------- INIT 1
    mdata->InitData("map_qsolRZ_6cls2cm.bin",path);
    
    mdata->PrintData();
         
    //return;
    gRandom->SetSeed(rnum);
    
    N = mdata->GetNTotal();

    for (Int_t i(1); i<=N; i++) {
      
         //n = gRandom->Integer(mdata->GetNTotal()); 
         n = i-1;
        
         //cout << i << "/" << N << ": " << mdata->GetBr(n) << " " << mdata->GetBz(n) << endl;
                     
         br = mdata->GetBr(n);
         bz = mdata->GetBz(n);
         bb = TMath::Sqrt(br*br + bz*bz);
         
         printf("%4d: Br = %9.2e; Bz = %9.2e; |B| = %9.2e; \n",i,br,bz,bb);   
         
         if (i%124 == 0) cout << endl;
         if (i == 124) break;          
    }
    
//---------------------------------------------- 
    
//   mdata->WriteAsciiFile("map_qsolRZ_6cls2cm_1.dat"); 
//   mdata->WriteBinaryFile("map_qsolRZ_6cls2cm_1.bin"); 

    //return;
    
  //----------------------------------------------  INIT 2  
   
//     mdata->InitData("map_qsolRZ_6cls2cm_1.dat","../macro");  
// 
//     mdata->PrintData();
//     
//     gRandom->SetSeed(rnum);
//     
//     for (Int_t i(0); i<N; i++) {
//       
//          //n = gRandom->Integer(mdata->GetNTotal()); 
//          n = i;
//          
//          cout << i+1 << " " << mdata->GetBr(n) 
//                    << " " << mdata->GetBz(n) 
//                    << endl;
//          if (i == 20) break;                    
//     }
    
  //----------------------------------------------  INIT 3 
 
//     mdata->InitData("map_qsolRZ_6cls2cm_1.bin","../macro");
//     
//     mdata->PrintData();
//     
//     gRandom->SetSeed(rnum);
//     
//     for (Int_t i(0); i<N; i++) {
//       
//          //n = gRandom->Integer(mdata->GetNTotal()); 
//          n = i;
//          
//          cout << i+1 << " " << mdata->GetBr(n) 
//                    << " " << mdata->GetBz(n) 
//                    << endl;
//          if (i == 20) break;                             
//     }   

}

