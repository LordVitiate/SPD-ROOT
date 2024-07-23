
void CheckAxialField() 
{
    
    SpdAxialFieldMap* MagField = new SpdAxialFieldMap("QSolenoidal field");
    MagField->InitData("map_qsolRZ_6cls2cm.bin");
    
    SpdRegion* reg = 0;
  
    double rmin = 0., rmax = 174.*1;  // cm
    double zmin = 0., zmax = 246.*1;  // cm
    
    reg = MagField->CreateFieldRegion("tube");
    reg->SetTubeRegion(rmin,rmax,-zmax,zmax); // (R,Z)_(min,max), cm 
    
    double point[3], field[3] = {0,0,0};
  
    Bool_t ok;
    
    gRandom->SetSeed(0);
    //gRandom->SetSeed(21340389);
    double_t r, phi, z;
    int i(0), n(0);
    for (; i<100; i++) {
         r   = gRandom->Uniform(rmin,rmax);
         phi = gRandom->Uniform(0,TMath::TwoPi());
         z   = gRandom->Uniform(0,1);  
         z   = (z < 0.5) ? gRandom->Uniform(zmin,zmax) : -gRandom->Uniform(zmin,zmax); 
         point[0] = r*TMath::Cos(phi);
         point[1] = r*TMath::Sin(phi);
         point[2] = z;       
         ok = MagField->GetField(point,field);
         if (ok) n++;
         printf("%2d %4d: [r, z] = [%14.8f, %14.8f]  point = [%14.8f, %14.8f, %14.8f] field = [%14.8f, %14.8f, %14.8f]\n",
                ok,i,r,z,point[0],point[1],point[2],field[0],field[1],field[2]); 
    }
    cout << n << "/" << i << endl;
}
