// 
//  Created 29/09/17  by A. Ivanov 
//
//  Redesign:
//          18/01/17 (A. Tkachenko)
//

#include <TMath.h>
#include <TVector3.h>

#include "SpdFieldMapData.h"

#include <fstream>
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

using TMath::DegToRad;

ClassImp(SpdFieldMapData)

//_____________________________________________________________________________________
SpdFieldMapData::SpdFieldMapData():
fMapName("SpdFieldMapData"),
fPathToData(""),fFileName(""),
fScaleUnit(1.),fFieldUnit(1.),
fInitLevel(0),
fMapRegion(0),
fNx(0),fNy(0),fNz(0), 
fBx(0),fBy(0),fBz(0)      
{  
   SetMapType();
}

//_____________________________________________________________________________________
SpdFieldMapData::SpdFieldMapData(const Char_t* name, const Char_t* type):
fMapName(name),
fPathToData(""),fFileName(""),
fScaleUnit(1.),fFieldUnit(1.),
fInitLevel(0),
fMapRegion(0),
fNx(0),fNy(0),fNz(0), 
fBx(0),fBy(0),fBz(0)      
{  
   SetMapType(type);
}

//_____________________________________________________________________________________
SpdFieldMapData::~SpdFieldMapData() 
{
   if (fBx) delete fBx;
   if (fBy) delete fBy;
   if (fBz) delete fBz;
   
   if (fMapRegion) delete fMapRegion;
}

//_____________________________________________________________________________________
void SpdFieldMapData::Clear()
{  
   fPathToData = "";
   fFileName = "";
 
   ClearData();
}

//_____________________________________________________________________________________
void SpdFieldMapData::ClearData() 
{
   fMapType = "undefined";
   
   fFieldUnit = 1.;
   fScaleUnit = 1.;
   
   if (fMapRegion) { delete fMapRegion; fMapRegion = 0; }
     
   fNx = 0; 
   fNy = 0;
   fNz = 0;
   
   if (fBx) { delete fBx; fBx = 0; }
   if (fBy) { delete fBy; fBy = 0; }
   if (fBz) { delete fBz; fBz = 0; }
   
   fInitLevel = 0;
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData::IsGridInit() const
{ 
   if (fNx < 2 || fNy < 2 || fNz < 2) return kFALSE; 
   if (!fMapRegion || !fMapRegion->IsRegionInit()) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData::IsDataInit() const
{
   return (fBx && fBy && fBz); 
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData::IsInsideMapRegion(Double_t x, Double_t y, Double_t z) const
{
   return (fMapRegion) ? fMapRegion->IsInside(x,y,z) : kFALSE;
}
//_____________________________________________________________________________________
Bool_t SpdFieldMapData::IsInsideMapRegion(Double_t r, Double_t z) const
{
   return (fMapRegion) ? fMapRegion->IsInside(r,z) : kFALSE;
}

//_____________________________________________________________________________________
Double_t SpdFieldMapData::GetXstep() const 
{ 
   if (!IsGridInit()) return 0;
   return fMapRegion->GetLx()/(fNx - 1); 
}

//_____________________________________________________________________________________
Double_t SpdFieldMapData::GetYstep() const 
{ 
   if (!IsGridInit()) return 0;
   return fMapRegion->GetLy()/(fNy - 1);
}

//_____________________________________________________________________________________
Double_t SpdFieldMapData::GetZstep() const 
{ 
   if (!IsGridInit()) return 0;
   return fMapRegion->GetLz()/(fNz - 1);
}

//_____________________________________________________________________________________
void SpdFieldMapData::SetPathToData(TString path)
{
   if (path.IsWhitespace()) {
       fPathToData = "";
       return;
   }
   
   fPathToData = path;
   
   if (!path.EndsWith("/")) fPathToData += "/";
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
void SpdFieldMapData::SetMapType(TString type) // protected
{ 
   if (!type.IsWhitespace()) fMapType = type;
   else fMapType = "undefined";
}  

//_____________________________________________________________________________________
Bool_t SpdFieldMapData::SetScaleUnit(TString unit) // protected
{
        if (unit == "mm") fScaleUnit = 0.1;
   else if (unit == "m")  fScaleUnit = 100.0;
   else if (unit == "cm") fScaleUnit = 1.0;
   else {
       cout << "-E- <SpdFieldMapData::SetScaleUnit> Unknown unit: " << unit << endl;
       fScaleUnit = 1;
       return kFALSE;
   }
   return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData::SetFieldUnit(TString unit) // protected
{
        if (unit == "G")  fFieldUnit = 0.001;
   else if (unit == "T")  fFieldUnit = 10.0;
   else if (unit == "kG") fFieldUnit = 1.0;
   else {
       cout << "-E- <SpdFieldMapData::SetFieldUnit> Unknown unit: " << unit << endl;
       fFieldUnit = 1;
       return kFALSE;
   }
   return kTRUE;
}

//_____________________________________________________________________________________
TString SpdFieldMapData::GetScaleUnitAsString() const 
{
   if (fScaleUnit == 0.1)   return "mm";
   if (fScaleUnit == 100.0) return "m";
   if (fScaleUnit == 1.0)   return "cm";
  
   cout << "-E- <SpdFieldMapData::GetScaleUnitString> Unknown unit: " << fScaleUnit << endl;
   return "unknown";
}

//_____________________________________________________________________________________
TString SpdFieldMapData::GetFieldUnitAsString() const 
{
   if (fFieldUnit == 0.001) return "G";
   if (fFieldUnit == 10.0)  return "T";
   if (fFieldUnit == 1.0)   return "kG";
  
   cout << "-E- <SpdFieldMapData::GetFieldUnitString> Unknown unit: " << fFieldUnit << endl;
   return "unknown";
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Bool_t SpdFieldMapData::InitData(TString fileName, TString path)
{
   Clear();
  
   if (fileName.IsWhitespace()) {
       cout << "-E- <SpdFieldMapData::Init> No data file " << endl;
       return kFALSE;
   }
   
   SetPathToData(path);
   fFileName = fileName;
       
   if (fileName.EndsWith(".dat")) {
       if (!ReadAsciiFile()) {
           cout << "-F- <SpdFieldMapData::Init> Bad data (*.dat) file " << endl;
           exit(1);
       }
   }
   else if (fileName.EndsWith(".bin")) { 
       if (!ReadBinaryFile()) {
           cout << "-F- <SpdFieldMapData::Init> Bad data (*.bin) file " << endl;
           exit(1);
       }
   }
   else {
       cout << "-E- <SpdFieldMapData::Init> No proper file name defined: "
            << fileName << endl;         
       Clear();
       return kFALSE;
   }
   
   fInitLevel = 1;
   return InitData();
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData::InitData(const SpdFieldMapData* data)
{
   Clear();
   
   if (!data) {
       cout << "-E- <SpdFieldMapData::Init> No data " << endl;
       return kFALSE;
   }
   
   fPathToData = data->GetPathToData();
   fFileName = data->GetFileName();
   
   fMapName = data->GetMapName();
   fMapType = data->GetMapType();
   
   fScaleUnit = data->GetScaleUnit();
   fFieldUnit = data->GetFieldUnit();
   
   // init grid
   
   if (fMapRegion) delete fMapRegion;
   fMapRegion = new SpdBoxRegion();
   
   fMapRegion->SetBoxRegion(data->GetXmin(),data->GetXmax(),
                            data->GetYmin(),data->GetYmax(),
                            data->GetZmin(),data->GetZmax());
  
   fNx = data->GetNx();
   fNy = data->GetNy();
   fNz = data->GetNz();
   
   if (!IsGridInit()) {
       cout << "-E- <SpdFieldMapData::Init> Bad grid parameters " << endl;
       Clear();
       return kFALSE;
   }

   // init data arrays

   if (data->IsDataInit()) {
       fBx = new TArrayF(*(data->GetBx()));
       fBy = new TArrayF(*(data->GetBy()));
       fBz = new TArrayF(*(data->GetBz()));
   }   
   else {
       Int_t nt = GetNTotal();
       fBx = new TArrayF(nt);
       fBy = new TArrayF(nt);
       fBz = new TArrayF(nt);
   }
   
   fInitLevel = 1;
   return InitData();
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData::InitData(Double_t xmin, Double_t xmax, Int_t nx,
                                 Double_t ymin, Double_t ymax, Int_t ny,
                                 Double_t zmin, Double_t zmax, Int_t nz)
{
   ClearData();
   
   if (fMapRegion) delete fMapRegion;
   fMapRegion = new SpdBoxRegion();
   
   fMapRegion->SetBoxRegion(xmin,xmax,ymin,ymax,zmin,zmax);  

   fNx = nx; 
   fNy = ny;
   fNz = nz;
   
   if (!IsGridInit()) {
       cout << "-E- <SpdFieldMapData::Init> Bad grid parameters " << endl;
       ClearData();
       return kFALSE;
   }
   
   Int_t nt = GetNTotal();
   
   fBx = new TArrayF(nt);
   fBy = new TArrayF(nt);
   fBz = new TArrayF(nt);
   
   fInitLevel = 1;
   return InitData();
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData::InitData()
{
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
void SpdFieldMapData::MultiplyField(Double_t v)
{
   MultiplyField(v,v,v);
}

//_____________________________________________________________________________________
void SpdFieldMapData::MultiplyField(Double_t vx, Double_t vy, Double_t vz)
{
   if (GetInitLevel() < 1) return;

   for (Int_t i(0); i < GetNTotal(); i++) {    
        fBx->AddAt(fBx->At(i)*vx,i);
        fBy->AddAt(fBy->At(i)*vy,i);
        fBz->AddAt(fBz->At(i)*vz,i);   
   }
}

//_____________________________________________________________________________________
void SpdFieldMapData::ShiftField(Double_t dx, Double_t dy, Double_t dz)
{
   if (GetInitLevel() < 1) return;

   if (!fMapRegion) return;
   
   fMapRegion->SetBoxRegion(GetXmin()+dx, GetXmax()+dx,
                            GetYmin()+dy, GetYmax()+dy,
                            GetZmin()+dz, GetZmax()+dz);  
}

//_____________________________________________________________________________________
void SpdFieldMapData::RotateField(Double_t angle, Double_t theta, Double_t phi)
{
    if (GetInitLevel() < 1) return;
    
    theta *= DegToRad();
      phi *= DegToRad();
      
    TVector3 r;
    r.SetMagThetaPhi(1,theta,phi);
    
    TVector3 v;
   
    for (Int_t i(0); i < GetNTotal(); i++) { 
         v.SetXYZ(fBx->At(i),fBy->At(i),fBz->At(i));
         v.Rotate(angle,r);
         fBx->AddAt(v.X(),i);
         fBy->AddAt(v.Y(),i);
         fBz->AddAt(v.Z(),i);   
    }
}

//_____________________________________________________________________________
Bool_t SpdFieldMapData::GetMinMaxStat(Double_t* B) const
{
   // fill (B,BT,Bz,By,Bx)[min,max]
  
   if (!B) return kFALSE;
     
   memset(B,0,10*sizeof(Double_t));
   
   if (!IsDataInit()) return kFALSE;
   
   Int_t nTot = GetNTotal();
   
   for (Int_t i(0); i<5; i++) { B[2*i] = 1e20; B[2*i+1] = -1e20; }
   
   Double_t bx, by, bz, b, bt;
   
   for (Int_t i(0); i<nTot; i++) 
   {
        bx = fBx->At(i);
        by = fBy->At(i);
        bz = fBz->At(i);
        
        bt = bx*bx+by*by;
        b  = TMath::Sqrt(bt+bz*bz);
        bt = TMath::Sqrt(bt);
        
        if (B[0] > b)  B[0] = b; // min  
        if (B[1] < b)  B[1] = b; // max
        
        if (B[2] > bt) B[2] = bt;
        if (B[3] < bt) B[3] = bt;
        
        if (B[4] > bz) B[4] = bz;
        if (B[5] < bz) B[5] = bz;
        
        if (B[6] > by) B[6] = by;
        if (B[7] < by) B[7] = by;
        
        if (B[8] > bx) B[8] = bx;
        if (B[9] < bx) B[9] = bx;
   }
   
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Bool_t SpdFieldMapData::ReadAsciiFile()  // protected
{
  TString fileName = fFileName; 
  
  if (!fPathToData.IsWhitespace()) fileName = fPathToData + fileName;
 
  cout << "-I- <SpdFieldMapData::ReadAsciiFile> Reading file: " 
       << fileName << endl;
      
  std::ifstream mapFile(fileName);

  if (!mapFile.is_open()) {
      cout << "-E- <SpdFieldMapData:ReadAsciiFile> Could not open file: " << fileName << endl;
      return kFALSE;
  }
  
  // Read map type
  mapFile >> fMapType;
  
  // Read Units
  TString unit;
  
  mapFile >> unit; 
  if (!SetScaleUnit(unit)) return kFALSE;
  
  mapFile >> unit; 
  if (!SetFieldUnit(unit)) return kFALSE;
   
  // Read grid parameters
  
  Double_t xmin, xmax, ymin, ymax, zmin, zmax;
  
  mapFile >> xmin >> xmax >> fNx;
  mapFile >> ymin >> ymax >> fNy;
  mapFile >> zmin >> zmax >> fNz;
  
  if (fMapRegion) delete fMapRegion;
  fMapRegion = new SpdBoxRegion();
  
  fMapRegion->SetBoxRegion(xmin*fScaleUnit, xmax*fScaleUnit,
                           ymin*fScaleUnit, ymax*fScaleUnit,
                           zmin*fScaleUnit, zmax*fScaleUnit);  
  
  if (!IsGridInit()) {
      cout << "-E- <SpdFieldMapData:ReadAsciiFile> Bad grid parameters: ";
      cout << "N(x,y,z) = " << fNx << ", " << fNy << ", " << fNz << endl;
      cout << "[min,max] = X[" << xmin << ", " << xmax << "], " 
           << "  Y[" << ymin << ", " << ymax << "]," 
           << "  Z[" << zmin << ", " << zmax << "]" << endl;
      return kFALSE;
  }
 
  // Create field arrays
  
  Int_t nTot = GetNTotal();
     
  fBx = new TArrayF(nTot);
  fBy = new TArrayF(nTot);
  fBz = new TArrayF(nTot);
  
  // Read the field values
 
  Double_t xx, yy, zz;
  Double_t bx, by, bz;
 
  Int_t n = 0;    
  for (; n < nTot; n++) {    

       if (!mapFile.good()) {
           cout << "-E- <SpdFieldMapData::ReadAsciiFile> I/O Error at position " << n << endl;
           break;
       }
      
       mapFile >> xx >> yy >> zz;
       mapFile >> bx >> by >> bz;
      
       //cout << n << "/" << nTot << " "
       //     << " point: " << xx << " " << yy << " " << zz 
       //     << " field: " << bx << " " << by << " " << bz 
       //     << endl;
               
       fBx->AddAt(bx*fFieldUnit, n);
       fBy->AddAt(by*fFieldUnit, n);
       fBz->AddAt(bz*fFieldUnit, n);
    
       if (mapFile.eof()) {
           cout << "-E- <SpdFieldMapData::ReadAsciiFile> End of file reached at position " << n << endl;
           break;
       }     
  }
  
  mapFile.close();
  
  //cout << fBx->At(0) << " " << fBy->At(0) << " " <<  fBz->At(0) << endl; 
  //cout << fBx->At(n-1) << " " << fBy->At(n-1) << " " <<  fBz->At(n-1) << endl; 
  
  cout << "-I- <SpdFieldMapData::ReadAsciiFile> Read field values: "
       << n  << "/" << nTot << endl;  
  
  return kTRUE;
}

//_____________________________________________________________________________________ 
Bool_t SpdFieldMapData::ReadBinaryFile()
{
  TString fileName = fFileName; 
  
  if (!fPathToData.IsWhitespace()) fileName = fPathToData + fileName;
 
  cout << "-I- <SpdFieldMapData::ReadBinaryFile> Reading file: " 
       << fileName << endl;
      
  FILE* fl = fopen(fileName.Data(),"rb");
  
  if (!fl) {
      cout << "-E- <SpdFieldMapData::ReadBinaryFile> File cannot be opened: " 
           << fileName << endl;
      return kFALSE;     
  }
  
  Int_t len;
  char  sdata[256];
  Int_t si = sizeof(Int_t);
  Int_t sd = sizeof(Double_t);
  Int_t sf = sizeof(Float_t);
 
   // Read map type
  fread(&len,si,1,fl);
  fgets(sdata,len,fl); 
  
  fMapType = sdata;
  
  // Read Units
  
  fread(&len,si,1,fl);
  fgets(sdata,len,fl); 
  
  if (!SetScaleUnit(TString(sdata,len-1))) return kFALSE;
 
  fread(&len,si,1,fl);
  fgets(sdata,len,fl); 
  
  if (!SetFieldUnit(TString(sdata,len-1))) return kFALSE;
  
  // Read grid parameters
  
  Double_t xmin, xmax, ymin, ymax, zmin, zmax;
  
  fread(&xmin,sd,1,fl); 
  fread(&xmax,sd,1,fl);
  fread(&fNx,si,1,fl);
  
  fread(&ymin,sd,1,fl); 
  fread(&ymax,sd,1,fl);
  fread(&fNy,si,1,fl);
  
  fread(&zmin,sd,1,fl); 
  fread(&zmax,sd,1,fl);
  fread(&fNz,si,1,fl);
  
  if (fMapRegion) delete fMapRegion;
  fMapRegion = new SpdBoxRegion();
  
  fMapRegion->SetBoxRegion(xmin*fScaleUnit, xmax*fScaleUnit,
                           ymin*fScaleUnit, ymax*fScaleUnit,
                           zmin*fScaleUnit, zmax*fScaleUnit);  
  
  if (!IsGridInit()) {
      cout << "-E- <SpdFieldMapData:ReadBinaryFile> Bad grid parameters: ";
      cout << "N(x,y,z) = " << fNx << ", " << fNy << ", " << fNz << endl;
      cout << "[min,max] = X[" << xmin << ", " << xmax << "], " 
           << "  Y[" << ymin << ", " << ymax << "]," 
           << "  Z[" << zmin << ", " << zmax << "]" << endl;
           
      fclose(fl);
     
      return kFALSE;
  }

  // Create field arrays
  
  Int_t nTot = GetNTotal();
  
  fBx = new TArrayF(nTot);
  fBy = new TArrayF(nTot);
  fBz = new TArrayF(nTot);
  
  // Read the field values
  
  Float_t vf;
  
  Int_t n = 0;    
  for (; n < nTot; n++) {               
       fread(&vf,sf,1,fl); fBx->AddAt(vf*fFieldUnit, n);
       fread(&vf,sf,1,fl); fBy->AddAt(vf*fFieldUnit, n);
       fread(&vf,sf,1,fl); fBz->AddAt(vf*fFieldUnit, n);
  }
 
  fclose(fl);     
  
  //cout << fBx->At(0) << " " << fBy->At(0) << " " <<  fBz->At(0) << endl; 
  //cout << fBx->At(n-1) << " " << fBy->At(n-1) << " " <<  fBz->At(n-1) << endl; 
   
  cout << "-I- <SpdFieldMapData::ReadBinaryFile> Read field values: "
       << n  << "/" << nTot << endl;  
  
  return kTRUE;
}

//_____________________________________________________________________________________ 
Bool_t SpdFieldMapData::WriteAsciiFile(TString fileName, TString dirName) const
{
  // Write the field map to an ASCII file    
    
  if (fileName.IsWhitespace()) {
      cout << "-E- <SpdFieldMapData::WriteAsciiFile> File is not defined " << endl;
      return kFALSE;
  }
  
  if (!fileName.EndsWith(".dat")) fileName += ".dat"; 
  
  if (!dirName.IsWhitespace()) {
      if (!dirName.EndsWith("/")) dirName += "/"; 
      fileName = dirName + fileName;
  }
  
  cout << "-I- <SpdFieldMapData::WriteAsciiFile> Write field map data to an ASCII file: "
       << fileName << endl;
       
  std::ofstream mapFile(fileName);
  if (!mapFile.is_open()) {
      cout << "-E- <SpdFieldMapData::WriteAsciiFile> Could not open file: " 
           << fileName << endl;
      return kFALSE;
  }
     
  /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */
//   Double_t sunit_tmp = fScaleUnit;
//   Double_t funit_tmp = fFieldUnit;
//   
//   Double_t& su_tmp = const_cast<Double_t&>(fScaleUnit);
//   Double_t& fu_tmp = const_cast<Double_t&>(fFieldUnit);
//   
//   su_tmp = 1.;
//   fu_tmp = 1.;
  /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */
 
  mapFile << fMapType << endl;
  
  mapFile << GetScaleUnitAsString() << " " << GetFieldUnitAsString() << endl;
   
  if (!IsDataInit()) {
    
      mapFile << GetXmin()/fScaleUnit << " " << GetXmax()/fScaleUnit << " " << 0 << endl;
      mapFile << GetYmin()/fScaleUnit << " " << GetYmax()/fScaleUnit << " " << 0 << endl;
      mapFile << GetZmin()/fScaleUnit << " " << GetZmax()/fScaleUnit << " " << 0 << endl;
      
      mapFile.close();
      
      /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */
//       su_tmp = sunit_tmp;
//       fu_tmp = funit_tmp;
      /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */ 
              
      cout << "-W- <SpdFieldMapData::WriteAsciiFile> Empty data arrays " << endl;
      
      return kFALSE;
  }
    
  mapFile << GetXmin()/fScaleUnit << " " << GetXmax()/fScaleUnit << " " << fNx << endl;
  mapFile << GetYmin()/fScaleUnit << " " << GetYmax()/fScaleUnit << " " << fNy << endl;
  mapFile << GetZmin()/fScaleUnit << " " << GetZmax()/fScaleUnit << " " << fNz << endl;
  
  Double_t dx = GetXstep();
  Double_t dy = GetYstep();
  Double_t dz = GetZstep();
  
  Int_t index;
  
  for (Int_t ix(0); ix < fNx; ix++) {
  for (Int_t iy(0); iy < fNy; iy++) {
  for (Int_t iz(0); iz < fNz; iz++) {

        index = ix*fNy*fNz + iy*fNz + iz;
     
        mapFile << Form("%16.8f %16.8f %16.8f %16.8f %16.8f %16.8f \n",
                        (GetXmin() + ix*dx)/fScaleUnit,                       
                        (GetYmin() + iy*dy)/fScaleUnit,
                        (GetZmin() + iz*dz)/fScaleUnit,
                        fBx->At(index)/fFieldUnit,
                        fBy->At(index)/fFieldUnit,
                        fBz->At(index)/fFieldUnit);        
    
  }}}
  
  /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */
//   su_tmp = sunit_tmp;
//   fu_tmp = funit_tmp;
  /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */
  
  mapFile.close(); 
  
  return kTRUE;
}

//_____________________________________________________________________________________ 
Bool_t SpdFieldMapData::WriteBinaryFile(TString fileName, TString dirName) const
{
  // Write the field map to an ASCII file    
    
  if (fileName.IsWhitespace()) {
      cout << "-E- <SpdFieldMapData::WriteBinaryFile> File is not defined " << endl;
      return kFALSE;
  }
  
  if (!fileName.EndsWith(".bin")) fileName += ".bin"; 
  
  if (!dirName.IsWhitespace()) {
      if (!dirName.EndsWith("/")) dirName += "/"; 
      fileName = dirName + fileName;
  }
  
  cout << "-I- <SpdFieldMapData::WriteBinaryFile> Write field map data to an BINARY file: "
       << fileName << endl;

  FILE* fl = fopen(fileName.Data(),"wb");
  
  if (!fl) {
      cout << "-E- <SpdFieldMapData::WriteBinaryFile> File cannot be written: " 
           << fileName << endl;
      return kFALSE;     
  }
   
  Int_t len , vi;
  TString sdata;
  Double_t vd;
  Float_t  vf;
  Int_t si = sizeof(Int_t);
  Int_t sd = sizeof(Double_t);
  Int_t sf = sizeof(Float_t);
  
  len = fMapType.Length()+1;
  fwrite(&len,si,1,fl);
  fputs(fMapType.Data(),fl);
  
  sdata = GetScaleUnitAsString();
 
  len = sdata.Length()+1;
  fwrite(&len,si,1,fl);
  fputs(sdata.Data(),fl);
  
  sdata = GetFieldUnitAsString();
 
  len = sdata.Length()+1;
  fwrite(&len,si,1,fl);
  fputs(sdata.Data(),fl);
  
  Double_t value;
  
  // Save grid

  if (!IsDataInit()) {
     
      vi = 0;
     
      vd = GetXmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
      vd = GetXmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
      fwrite(&vi,si,1,fl);
      
      vd = GetYmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
      vd = GetYmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
      fwrite(&vi,si,1,fl);
      
      vd = GetZmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
      vd = GetZmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
      fwrite(&vi,si,1,fl);
     
      fclose(fl);
   
      cout << "-W- <SpdFieldMapData::WriteBinaryFile> Empty data arrays " << endl;
      
      return kFALSE;
  }
  
  vd = GetXmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
  vd = GetXmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
  fwrite(&fNx,si,1,fl);
  
  vd = GetYmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
  vd = GetYmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
  fwrite(&fNy,si,1,fl);
  
  vd = GetZmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
  vd = GetZmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
  fwrite(&fNz,si,1,fl);
    
  // Save field values
   
  Int_t nTot = GetNTotal();
  
  for (Int_t n(0); n < nTot; n++) {    
    
       vf = fBx->At(n)/fFieldUnit; fwrite(&vf,sf,1,fl);
       vf = fBy->At(n)/fFieldUnit; fwrite(&vf,sf,1,fl);
       vf = fBz->At(n)/fFieldUnit; fwrite(&vf,sf,1,fl);   
  }
 
  fclose(fl);
  
  return kTRUE;
}
 
//_____________________________________________________________________________________ 
void SpdFieldMapData::PrintData() const
{
  cout << "<SpdFieldMapData::Print> " << endl;
  
  cout << "\t Data map name: " << fMapName << endl; 
  cout << "\t Path to data:  " << fPathToData << endl;
  cout << "\t File name:     " << fFileName << endl;
  cout << "\t Map type:      " << fMapType << endl; 
  cout << endl;
  cout << "\t Original Scale unit: " << fScaleUnit 
       << " [" << GetScaleUnitAsString() << "] " << endl;
  cout << "\t Original Field unit: " << fFieldUnit << " [" 
       << GetFieldUnitAsString() << "] " << endl;
  cout << endl;
  cout << "\t [Grid]        " << endl;
  cout << "\t N(x,y,z):     " << fNx << ", " << fNy << ", " << fNz 
       << "   [TOTAL = " << GetNTotal() << "] "<< endl;
       
  if (fMapRegion) {     
      cout << "\t X[min,max]:   " << fMapRegion->GetXmin() << ", " 
           << fMapRegion->GetXmax() << " [cm]" << "  (dx = " << GetXstep() << " cm) " << endl;
      cout << "\t Y[min,max]:   " << fMapRegion->GetYmin() << ", "  
           << fMapRegion->GetYmax() << " [cm]" << "  (dy = " << GetYstep() << " cm) " << endl;
      cout << "\t Z[min,max]:   " << fMapRegion->GetZmin() << ", " 
           << fMapRegion->GetZmax() << " [cm]" << "  (dz = " << GetZstep() << " cm) " << endl;
  }
  else {
      cout << "\t MAP REGION is not defined " << endl; 
  }
  
  cout << endl;
}



