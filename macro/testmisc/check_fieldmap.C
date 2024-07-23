
#include <string>
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

/* read input file */
bool ReadData(const char* FileName);

/* get field value */
bool GetField(double x /*cm*/, double y /*cm*/, double z /*cm*/, 
              double& Bx /*kG*/, double& By /*kG*/, double& Bz /*kG*/); 

// input data 
double ScaleUnit_ = 1;
double FieldUnit_ = 1;
int Nx_(1), Ny_(1), Nz_(1), NN_(1), Nyz_(1);
double Xmin_(0), Xmax_(0), Ymin_(0), Ymax_(0), Zmin_(0), Zmax_(0);
double Xstep_(0), Ystep_(0), Zstep_(0), InvCellVol_(0);
float *Bx_(0), *By_(0), *Bz_(0);
bool InitFieldMap_ = false;

//_____________________________________________________________________________
void check_fieldmap()
{
  cout << "start ... " << endl;
  
  if (!ReadData("../input/karta_hyb_2.dat")) return;
  
  double Bx, By, Bz;
  
  GetField(0., 0., 0., Bx, By, Bz); // cm, kG
  
  cout << "B = (" << Bx << ", " << By << ", " << Bz << ") [kG]" << endl;
  
  
  GetField(10., 0., -5.5, Bx, By, Bz);   // cm, kG
  
  cout << "B = (" << Bx << ", " << By << ", " << Bz << ") [kG]"<< endl;
  
  
  GetField(-10., 20., 12.5, Bx, By, Bz); // cm, kG
  
  cout << "B = (" << Bx << ", " << By << ", " << Bz << ") [kG]"<< endl;
  
  
  GetField(0.124, 15.467, -8.98, Bx, By, Bz); // cm, kG
  
  cout << "B = (" << Bx << ", " << By << ", " << Bz << ") [kG]"<< endl;
  
  
  GetField(-230.14, -130.02, 190.234, Bx, By, Bz); // cm, kG
  
  cout << "B = (" << Bx << ", " << By << ", " << Bz << ") [kG]"<< endl;
  
  //delete [] Bx_;
  //delete [] By_;
  //delete [] Bz_;
}

//_____________________________________________________________________________
bool GetField(double x, double y, double z, double& Bx, double& By, double& Bz)
{
  Bx = 0.; By = 0.; Bz = 0.;
  
  if (!InitFieldMap_) {
      cout << "-E- <GetField> no data " << endl;
      return false;
  }
  
  if ( x < Xmin_ || x > Xmax_ ||
       y < Ymin_ || y > Ymax_ ||
       z < Zmin_ || z > Zmax_ ) 
  {
      cout << "-W- <GetField> the point (" << x << "," << y << "," << z << ") " 
           << "is out of range " << endl;
      return true;
  }      
  
  int ix = int((x - Xmin_)/Xstep_);
  int iy = int((y - Ymin_)/Ystep_);
  int iz = int((z - Zmin_)/Zstep_);
  
  int I_000 = ix*Nyz_ + iy*Nz_ + iz;
  int I_001 = I_000 + 1;
  int I_010 = I_000 + Nz_;
  int I_011 = I_010 + 1;  
  int I_100 = I_000 + Nyz_; 
  int I_101 = I_100 + 1;
  int I_110 = I_100 + Nz_; 
  int I_111 = I_110 + 1;
  
  double Dx2 = x - ix * Xstep_ - Xmin_;
  double Dy2 = y - iy * Ystep_ - Ymin_;
  double Dz2 = z - iz * Zstep_ - Zmin_;
  
  double Dx1 = Xstep_ - Dx2;
  double Dy1 = Ystep_ - Dy2;
  double Dz1 = Zstep_ - Dz2;
  
  Bx  = Dx1 * (  Dy1 * ( Dz1 * Bx_[I_000] + Dz2 * Bx_[I_001] ) 
               + Dy2 * ( Dz1 * Bx_[I_010] + Dz2 * Bx_[I_011] ) )  
      + Dx2 * (  Dy1 * ( Dz1 * Bx_[I_100] + Dz2 * Bx_[I_101] ) 
               + Dy2 * ( Dz1 * Bx_[I_110] + Dz2 * Bx_[I_111] ) );
  Bx *= InvCellVol_;
  
  By  = Dx1 * (  Dy1 * ( Dz1 * By_[I_000] + Dz2 * By_[I_001] ) 
               + Dy2 * ( Dz1 * By_[I_010] + Dz2 * By_[I_011] ) )
      + Dx2 * (  Dy1 * ( Dz1 * By_[I_100] + Dz2 * By_[I_101] ) 
               + Dy2 * ( Dz1 * By_[I_110] + Dz2 * By_[I_111] ) );
  By *= InvCellVol_;
  
  Bz  = Dx1 * (  Dy1 * ( Dz1 * Bz_[I_000] + Dz2 * Bz_[I_001] ) 
               + Dy2 * ( Dz1 * Bz_[I_010] + Dz2 * Bz_[I_011] ) ) 
      + Dx2 * (  Dy1 * ( Dz1 * Bz_[I_100] + Dz2 * Bz_[I_101] ) 
               + Dy2 * ( Dz1 * Bz_[I_110] + Dz2 * Bz_[I_111] ) );
  Bz *= InvCellVol_;

  return true;
}

//_____________________________________________________________________________
bool ReadData(const char* FileName)
{
  InitFieldMap_ = false;
  
  ScaleUnit_ = 1.;
  FieldUnit_ = 1.;
  
  Xmin_ = 0; Xmax_ = 0;
  Ymin_ = 0; Ymax_ = 0;
  Zmin_ = 0; Zmax_ = 0;
  
  Xstep_ = 0; Ystep_ = 0; Zstep_ = 0;  
  InvCellVol_ = 0;
  
  Nx_ = 1; Ny_ = 1; Nz_ = 1; NN_ = 1; Nyz_ = 1;
  
  if (Bx_) { delete [] Bx_; Bx_ = 0; }
  if (By_) { delete [] By_; By_ = 0; }
  if (Bz_) { delete [] Bz_; Bz_ = 0; }
   
  std::ifstream mapFile(FileName);

  if (!mapFile.is_open()) {
      cout << "-E- <ReadData> could not open the file " << endl;
      mapFile.close();
      return false;
  }
  
  std::string sdata;
  
  mapFile >> sdata;

  cout << "-I- <ReadData> field type: " << sdata << endl;
  
  mapFile >> sdata;
  
       if (sdata == "m")  ScaleUnit_ = 100.0;
  else if (sdata == "cm") ScaleUnit_ = 1.0;
  else if (sdata == "mm") ScaleUnit_ = 0.1;
  else {
       cout << "-E- <ReadData> unknown scale unit: " << sdata << endl;
       mapFile.close();
       return false;
  }
  
  mapFile >> sdata;
  
       if (sdata == "T")  FieldUnit_ = 10.;
  else if (sdata == "kG") FieldUnit_ = 1.0;
  else if (sdata == "G")  FieldUnit_ = 0.001;
  else {
       cout << "-E- <ReadData> unknown filed unit: " << sdata << endl;
       mapFile.close();
       return kFALSE;
  }
   
  cout << "-I- <ReadData> [Units] scale: " << ScaleUnit_ << "  field: " << FieldUnit_ << endl;
  
  mapFile >> Xmin_ >> Xmax_ >> Nx_;
  mapFile >> Ymin_ >> Ymax_ >> Ny_;
  mapFile >> Zmin_ >> Zmax_ >> Nz_;
  
  Nyz_ = Ny_*Nz_;
  NN_  = Nx_*Nyz_; 
  
  Xmin_ *= ScaleUnit_; Xmax_ *= ScaleUnit_;
  Ymin_ *= ScaleUnit_; Ymax_ *= ScaleUnit_;
  Zmin_ *= ScaleUnit_; Zmax_ *= ScaleUnit_;
  
  Xstep_ = (Xmax_ - Xmin_)/double(Nx_-1);
  Ystep_ = (Ymax_ - Ymin_)/double(Ny_-1);
  Zstep_ = (Zmax_ - Zmin_)/double(Nz_-1);
  
  cout << "-I- <ReadData> [Size] Nx = " << Nx_ << ";  X(min,max): " << Xmin_ << ", " << Xmax_ << " [cm] " << endl;
  cout << "-I- <ReadData> [Size] Ny = " << Ny_ << ";  Y(min,max): " << Ymin_ << ", " << Ymax_ << " [cm] " << endl;
  cout << "-I- <ReadData> [Size] Nz = " << Nz_ << ";  Z(min,max): " << Zmin_ << ", " << Zmax_ << " [cm] " << endl;
  cout << "-I- <ReadData> [Step] (dx,dy,dz) = (" << Xstep_ << ", " << Ystep_ << ", " << Zstep_ << ") [cm] " << endl; 
 
  if (Nx_ < 2 || Ny_ < 2 || Nz_ < 2) {
      cout << "-E- <ReadData> Bad table dimensions " << endl;
      mapFile.close();
      return kFALSE;
  }
  
  InvCellVol_ = Xstep_*Ystep_*Zstep_;
  InvCellVol_ = 1./InvCellVol_;
  
  Bx_ = new float[NN_];
  By_ = new float[NN_];
  Bz_ = new float[NN_];
  
  double xx, yy, zz;
  double bx, by, bz;
 
  for (Int_t n(0); n < NN_; n++) {    

        if (!mapFile.good()) {
            cout << "-E- <ReadData> I/O Error at position " << n << endl;
            mapFile.close();
            return false;
        }
      
        mapFile >> xx >> yy >> zz;
        mapFile >> bx >> by >> bz;
      
        //cout << n << "/" << NN_ << " "
        //     << " point: " << xx << " " << yy << " " << zz 
        //     << " field: " << bx << " " << by << " " << bz 
        //     << endl;
         
        Bx_[n] = bx*FieldUnit_;
        By_[n] = by*FieldUnit_;
        Bz_[n] = bz*FieldUnit_;
    
        if (mapFile.eof()) {
            cout << "-E- <ReadData> End of file reached at position " << n << endl;
            mapFile.close();
            return false;
        }     
  }
  
  cout << "-I- <ReadData> ok. " << endl;
  
  mapFile.close();
  
  InitFieldMap_ = true;
  
  return true;
}
