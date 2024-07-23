
using std::cout;
using std::endl;

void ReadAsciiFile(std::string fileName);
bool GetField(const double_t* point, double* bField);

//------------------------------
// units
const double RTOLERANCE = 1.e-12;
const double ScaleUnit  = 100.0; // m -> cm 
const double FieldUnit  = 10.0;  // T -> kG

//------------------------------
// input: field grid & values
double fRmin(0), fRmax(0), fZmin(0), fZmax(0);
int    fNr(0), fNz(0);
double *fBr(0), *fBz(0);

//------------------------------
// auxiliary data members & functions 
double fRstep(0), fZstep(0);
double fRevCellSize;

double fDz1, fDr1, fDz2, fDr2;
int    fI_00, fI_10, fI_01, fI_11;

bool IsInsideRegion(double r, double z) 
{
   if (r < fRmin || r > fRmax) return false;
   if (z < fZmin || z > fZmax) return false;
   return true;
}

void   FindCell(const double& r, const double& z); 
double Approximate(const double* array);

//=====================================================================================
void readAxialFieldMap() 
{
  ReadAsciiFile("map_qsolRZ_6cls2cm.dat");
  
  double point[3], field[3] = {0,0,0};
  
  // >>> test >>>
  
  gRandom->SetSeed(21340389);
  
  double_t r, phi, z;
  
  for (int i(0); i<10; i++) 
  {
       // random point
       r   = gRandom->Uniform(fRmin,fRmax);
       phi = gRandom->Uniform(0,TMath::TwoPi());
       z   = gRandom->Uniform(0,1);  
       z   = (z < 0.5) ? gRandom->Uniform(fZmin, fZmax) : -gRandom->Uniform(fZmin, fZmax); 
       point[0] = r*TMath::Cos(phi);
       point[1] = r*TMath::Sin(phi);
       point[2] = z;  
       
       // get field value
       GetField(point,field); 
       
       printf("%4d: point = [%16.8f, %16.8f, %16.8f]   field = [%16.8f, %16.8f, %16.8f]\n",
                i,point[0],point[1],point[2],field[0],field[1],field[2]); 
  }
}

//_____________________________________________________________________________
bool GetField(const double* point, double* bField)
{
   double z = point[2]; 
   int sgn = (z < 0) ? -1 : 1; 
   
   z = abs(z);
   
   double r = sqrt(point[0]*point[0] + point[1]*point[1]);
   
   // check region
   if (!IsInsideRegion(r,z)) {
       bField[0] = 0.;
       bField[1] = 0.;
       bField[2] = 0.;
       return false;
   }
   
   FindCell(r,z);
   
   double cosp(0), sinp(0); // cos(phi), sin(phi)
   
   if (r < RTOLERANCE) {
       // phi = pi, cos(phi) = -1, sin(phi) = 0 
       // (see ROOT TVector2 phi angle definition for x = y = 0)
       // cosp = -1;
       // sinp =  0.; 
   
       bField[0] = 0;
       bField[1] = 0;
       bField[2] = Approximate(fBz);
       
       return true;
   }

   if (abs(point[0]) > r*RTOLERANCE) cosp = sgn*point[0]/r;  
   if (abs(point[1]) > r*RTOLERANCE) sinp = sgn*point[1]/r; 
   
   r = Approximate(fBr);
   
   bField[0] = cosp*r; 
   bField[1] = sinp*r; 
   bField[2] = Approximate(fBz);
                 
   return true;
}

//_____________________________________________________________________________________
void ReadAsciiFile(std::string fileName) 
{
  cout << "-I- <SpdAxialFieldMapData::ReadAsciiFile> Input file: " 
       << fileName << endl;
      
  std::ifstream mapFile(fileName);

  if (!mapFile.is_open()) {
      cout << "-E- <SpdAxialFieldMapData:ReadAsciiFile> Could not open file: " << fileName << endl;
      return kFALSE;
  }
  
  std::string maptype, unit;

  mapFile >> maptype >> unit >> unit;
     
  // Read grid parameters

  mapFile >> fRmin >> fRmax >> fNr;
  mapFile >> fZmin >> fZmax >> fNz;
  
  fRmin *= ScaleUnit;
  fRmax *= ScaleUnit;
  fZmin *= ScaleUnit;
  fZmax *= ScaleUnit;
  
  if (1) {
      cout << "Grid parameters: ";
      cout << "N(r,z) = " << fNr << ", " << fNz << endl;
      cout << "[min,max] = R[" << fRmin << ", " << fRmax << "], " 
                     << "  Z[" << fZmin << ", " << fZmax << "]" << endl;
  }
  
  fRstep = (fRmax - fRmin)/(fNr - 1); 
  fZstep = (fZmax - fZmin)/(fNz - 1);
 
  fRevCellSize = 1./(fRstep*fZstep);
 
  int nTot = fNr * fNz;
  
  fBr = new double[nTot];
  fBz = new double[nTot];
   
  double rr, zz;
  double br, bz;
 
  int n = 0;    
  for (; n < nTot; n++) {    

       if (!mapFile.good()) {
           cout << "Errorr. Input error at position " << n << endl;
           exit(1);
       }
      
       mapFile >> rr >> zz;
       mapFile >> br >> bz;
               
       fBr[n] = br*FieldUnit; // -> kG
       fBz[n] = bz*FieldUnit; // -> kG
    
       if (mapFile.eof()) {
           cout << "Error. End of file reached at position " << n << endl;
           exit(1);
       }     
  }
  
  mapFile.close();
  
  cout << "Nodes (total): " << n << "/" << nTot << endl;  
}

//_____________________________________________________________________________ 
void FindCell(const double& r, const double& z) 
{
    int iz = int((z - fZmin)/fZstep);
    int ir = int((r - fRmin)/fRstep);
    
    fI_00 = ir*fNz + iz;  // (z0,r0)
    fI_10 = fI_00 + 1;    // (z1,r0)
    fI_01 = fI_00 + fNz;  // (z0,r1)
    fI_11 = fI_01 + 1;    // (z1,r1)

    fDz1 = z - fZstep * iz - fZmin; // dz1 = z-z1
    fDr1 = r - fRstep * ir - fRmin; // dr1 = r-r1
   
    fDz2 = fZstep - fDz1;  // dz2 = z2-z  
    fDr2 = fRstep - fDr1;  // dr2 = r2-r
}

//_____________________________________________________________________________ 
double Approximate(const double* V) 
{
    double value  = fDz1 * (fDr2 * V[fI_10] + fDr1 * V[fI_11])   // F(10) * dz1*dr2 + F(11) * dz1*dr1
                  + fDz2 * (fDr2 * V[fI_00] + fDr1 * V[fI_01]);  // F(00) * dz2*dr2 + F(01) * dz2*dr1
     
    return fRevCellSize * value;
}
