//
//  Created 29/09/17  by A. Ivanov
//
//  Redesign:
//          18/01/17 (A. Tkachenko)
//

#include <TMath.h>
#include <TVector3.h>

#include "SpdFieldMapData1_8.h"

#include <fstream>
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

using TMath::DegToRad;

ClassImp(SpdFieldMapData1_8)

//_____________________________________________________________________________________
SpdFieldMapData1_8::SpdFieldMapData1_8():
fMapName("SpdFieldMapData1_8"),
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
SpdFieldMapData1_8::SpdFieldMapData1_8(const Char_t* name, const Char_t* type):
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
SpdFieldMapData1_8::~SpdFieldMapData1_8()
{
   if (fBx) delete fBx;
   if (fBy) delete fBy;
   if (fBz) delete fBz;

   if (fMapRegion) delete fMapRegion;
}

//_____________________________________________________________________________________
void SpdFieldMapData1_8::Clear()
{
   fPathToData = "";
   fFileName = "";

   ClearData();
}

//_____________________________________________________________________________________
void SpdFieldMapData1_8::ClearData()
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
Bool_t SpdFieldMapData1_8::IsGridInit() const
{
   if (fNx < 2 || fNy < 2 || fNz < 2) return kFALSE;
   if (!fMapRegion || !fMapRegion->IsRegionInit()) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData1_8::IsDataInit() const
{
   return (fBx && fBy && fBz);
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData1_8::IsInsideMapRegion(Double_t x, Double_t y, Double_t z) const
{
   return (fMapRegion) ? fMapRegion->IsInside(x,y,z) : kFALSE;
}
//_____________________________________________________________________________________
Bool_t SpdFieldMapData1_8::IsInsideMapRegion(Double_t r, Double_t z) const
{
   return (fMapRegion) ? fMapRegion->IsInside(r,z) : kFALSE;
}

//_____________________________________________________________________________________
Double_t SpdFieldMapData1_8::GetXstep() const
{
   if (!IsGridInit()) return 0;
   return fMapRegion->GetLx()/2/(fNx - 1);
}

//_____________________________________________________________________________________
Double_t SpdFieldMapData1_8::GetYstep() const
{
   if (!IsGridInit()) return 0;
   return fMapRegion->GetLy()/2/(fNy - 1);
}

//_____________________________________________________________________________________
Double_t SpdFieldMapData1_8::GetZstep() const
{
   if (!IsGridInit()) return 0;
   return fMapRegion->GetLz()/2/(fNz - 1);
}

//_____________________________________________________________________________________
void SpdFieldMapData1_8::SetPathToData(TString path)
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
void SpdFieldMapData1_8::SetMapType(TString type) // protected
{
   if (!type.IsWhitespace()) fMapType = type;
   else fMapType = "undefined";
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData1_8::SetScaleUnit(TString unit) // protected
{
        if (unit == "mm") fScaleUnit = 0.1;
   else if (unit == "m")  fScaleUnit = 100.0;
   else if (unit == "cm") fScaleUnit = 1.0;
   else {
       cout << "-E- <SpdFieldMapData1_8::SetScaleUnit> Unknown unit: " << unit << endl;
       fScaleUnit = 1;
       return kFALSE;
   }
   return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData1_8::SetFieldUnit(TString unit) // protected
{
        if (unit == "G")  fFieldUnit = 0.001;
   else if (unit == "T")  fFieldUnit = 10.0;
   else if (unit == "kG") fFieldUnit = 1.0;
   else {
       cout << "-E- <SpdFieldMapData1_8::SetFieldUnit> Unknown unit: " << unit << endl;
       fFieldUnit = 1;
       return kFALSE;
   }
   return kTRUE;
}

//_____________________________________________________________________________________
TString SpdFieldMapData1_8::GetScaleUnitAsString() const
{
   if (fScaleUnit == 0.1)   return "mm";
   if (fScaleUnit == 100.0) return "m";
   if (fScaleUnit == 1.0)   return "cm";

   cout << "-E- <SpdFieldMapData1_8::GetScaleUnitString> Unknown unit: " << fScaleUnit << endl;
   return "unknown";
}

//_____________________________________________________________________________________
TString SpdFieldMapData1_8::GetFieldUnitAsString() const
{
   if (fFieldUnit == 0.001) return "G";
   if (fFieldUnit == 10.0)  return "T";
   if (fFieldUnit == 1.0)   return "kG";

   cout << "-E- <SpdFieldMapData1_8::GetFieldUnitString> Unknown unit: " << fFieldUnit << endl;
   return "unknown";
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Bool_t SpdFieldMapData1_8::InitData(TString fileName, TString path)
{
   Clear();

   if (fileName.IsWhitespace()) {
       cout << "-E- <SpdFieldMapData1_8::Init> No data file " << endl;
       return kFALSE;
   }

   SetPathToData(path);
   fFileName = fileName;

   if (fileName.EndsWith(".dat")) {
       if (!ReadAsciiFile()) {
           cout << "-F- <SpdFieldMapData1_8::Init> Bad data (*.dat) file " << endl;
           exit(1);
       }
   }
   else if (fileName.EndsWith(".bin")) {
       if (!ReadBinaryFile()) {
           cout << "-F- <SpdFieldMapData1_8::Init> Bad data (*.bin) file " << endl;
           exit(1);
       }
   }
   else {
       cout << "-E- <SpdFieldMapData1_8::Init> No proper file name defined: "
            << fileName << endl;
       Clear();
       return kFALSE;
   }

   fInitLevel = 1;
   return InitData();
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData1_8::InitData(const SpdFieldMapData1_8* data)
{
   Clear();

   if (!data) {
       cout << "-E- <SpdFieldMapData1_8::Init> No data " << endl;
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
       cout << "-E- <SpdFieldMapData1_8::Init> Bad grid parameters " << endl;
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
Bool_t SpdFieldMapData1_8::InitData(Double_t xmin, Double_t xmax, Int_t nx,
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
       cout << "-E- <SpdFieldMapData1_8::Init> Bad grid parameters " << endl;
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
Bool_t SpdFieldMapData1_8::InitData()
{
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
void SpdFieldMapData1_8::MultiplyField(Double_t v)
{
   MultiplyField(v,v,v);
}

//_____________________________________________________________________________________
void SpdFieldMapData1_8::MultiplyField(Double_t vx, Double_t vy, Double_t vz)
{
   if (GetInitLevel() < 1) return;

   for (Int_t i(0); i < GetNTotal(); i++) {
        fBx->AddAt(fBx->At(i)*vx,i);
        fBy->AddAt(fBy->At(i)*vy,i);
        fBz->AddAt(fBz->At(i)*vz,i);
   }
}

//_____________________________________________________________________________________
void SpdFieldMapData1_8::ShiftField(Double_t dx, Double_t dy, Double_t dz)
{
   throw(std::logic_error("This field map does not support shifts!"));
}

//_____________________________________________________________________________________
void SpdFieldMapData1_8::RotateField(Double_t angle, Double_t theta, Double_t phi)
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
Bool_t SpdFieldMapData1_8::GetMinMaxStat(Double_t* B) const
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
Bool_t SpdFieldMapData1_8::ReadAsciiFile()  // protected
{
  TString fileName = fFileName;

  if (!fPathToData.IsWhitespace()) fileName = fPathToData + fileName;

  cout << "-I- <SpdFieldMapData1_8::ReadAsciiFile> Reading file: "
       << fileName << endl;

  std::ifstream mapFile(fileName);

  if (!mapFile.is_open()) {
      cout << "-E- <SpdFieldMapData1_8:ReadAsciiFile> Could not open file: " << fileName << endl;
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

  // here xmin, ymin, and zmin are assumed to be zeroes
  mapFile >> xmin >> xmax >> fNx;
  mapFile >> ymin >> ymax >> fNy;
  mapFile >> zmin >> zmax >> fNz;

  if (fMapRegion) delete fMapRegion;
  fMapRegion = new SpdBoxRegion();

  fMapRegion->SetBoxRegion(-xmax*fScaleUnit, xmax*fScaleUnit,
                           -ymax*fScaleUnit, ymax*fScaleUnit,
                           -zmax*fScaleUnit, zmax*fScaleUnit);

  if (!IsGridInit()) {
      cout << "-E- <SpdFieldMapData1_8:ReadAsciiFile> Bad grid parameters: ";
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

  // Be careful, the file reading assumes the following mapping:
  // n = ix*Nyz + iy*fNz + iz;
  // the actual coorinates of the point are not used
  Int_t n = 0;
  for (; n < nTot; n++) {

       if (!mapFile.good()) {
           cout << "-E- <SpdFieldMapData1_8::ReadAsciiFile> I/O Error at position " << n << endl;
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
           cout << "-E- <SpdFieldMapData1_8::ReadAsciiFile> End of file reached at position " << n << endl;
           break;
       }
  }

  mapFile.close();

  //cout << fBx->At(0) << " " << fBy->At(0) << " " <<  fBz->At(0) << endl;
  //cout << fBx->At(n-1) << " " << fBy->At(n-1) << " " <<  fBz->At(n-1) << endl;

  cout << "-I- <SpdFieldMapData1_8::ReadAsciiFile> Read field values: "
       << n  << "/" << nTot << endl;

  return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData1_8::ReadBinaryFile()
{
  TString fileName = fFileName;

  if (!fPathToData.IsWhitespace()) fileName = fPathToData + fileName;

  cout << "-I- <SpdFieldMapData1_8::ReadBinaryFile> Reading file: "
       << fileName << endl;

  FILE* fl = fopen(fileName.Data(),"rb");

  if (!fl) {
      cout << "-E- <SpdFieldMapData1_8::ReadBinaryFile> File cannot be opened: "
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

  fMapRegion->SetBoxRegion(-xmax*fScaleUnit, xmax*fScaleUnit,
                           -ymax*fScaleUnit, ymax*fScaleUnit,
                           -zmax*fScaleUnit, zmax*fScaleUnit);

  if (!IsGridInit()) {
      cout << "-E- <SpdFieldMapData1_8:ReadBinaryFile> Bad grid parameters: ";
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

  cout << "-I- <SpdFieldMapData1_8::ReadBinaryFile> Read field values: "
       << n  << "/" << nTot << endl;

  return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdFieldMapData1_8::WriteAsciiFile(TString fileName, TString dirName) const
{
  // Write the field map to an ASCII file

  if (fileName.IsWhitespace()) {
      cout << "-E- <SpdFieldMapData1_8::WriteAsciiFile> File is not defined " << endl;
      return kFALSE;
  }

  if (!fileName.EndsWith(".dat")) fileName += ".dat";

  if (!dirName.IsWhitespace()) {
      if (!dirName.EndsWith("/")) dirName += "/";
      fileName = dirName + fileName;
  }

  cout << "-I- <SpdFieldMapData1_8::WriteAsciiFile> Write field map data to an ASCII file: "
       << fileName << endl;

  std::ofstream mapFile(fileName);
  if (!mapFile.is_open()) {
      cout << "-E- <SpdFieldMapData1_8::WriteAsciiFile> Could not open file: "
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

      mapFile << 0./fScaleUnit << " " << GetXmax()/fScaleUnit << " " << 0 << endl;
      mapFile << 0./fScaleUnit << " " << GetYmax()/fScaleUnit << " " << 0 << endl;
      mapFile << 0./fScaleUnit << " " << GetZmax()/fScaleUnit << " " << 0 << endl;

      mapFile.close();

      /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */
//       su_tmp = sunit_tmp;
//       fu_tmp = funit_tmp;
      /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */

      cout << "-W- <SpdFieldMapData1_8::WriteAsciiFile> Empty data arrays " << endl;

      return kFALSE;
  }

  mapFile << 0./fScaleUnit << " " << GetXmax()/fScaleUnit << " " << fNx << endl;
  mapFile << 0./fScaleUnit << " " << GetYmax()/fScaleUnit << " " << fNy << endl;
  mapFile << 0./fScaleUnit << " " << GetZmax()/fScaleUnit << " " << fNz << endl;

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
Bool_t SpdFieldMapData1_8::WriteBinaryFile(TString fileName, TString dirName) const
{
  // Write the field map to an ASCII file

  if (fileName.IsWhitespace()) {
      cout << "-E- <SpdFieldMapData1_8::WriteBinaryFile> File is not defined " << endl;
      return kFALSE;
  }

  if (!fileName.EndsWith(".bin")) fileName += ".bin";

  if (!dirName.IsWhitespace()) {
      if (!dirName.EndsWith("/")) dirName += "/";
      fileName = dirName + fileName;
  }

  cout << "-I- <SpdFieldMapData1_8::WriteBinaryFile> Write field map data to an BINARY file: "
       << fileName << endl;

  FILE* fl = fopen(fileName.Data(),"wb");

  if (!fl) {
      cout << "-E- <SpdFieldMapData1_8::WriteBinaryFile> File cannot be written: "
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

      //vd = GetXmin()/fScaleUnit; fwrite(&vd,sd,1,fl); // FIXME TO ZERRO)
      vd = 0.; fwrite(&vd,sd,1,fl); // FIXME TO ZERRO)
      vd = GetXmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
      fwrite(&vi,si,1,fl);

      //vd = GetYmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
      vd = 0.; fwrite(&vd,sd,1,fl);
      vd = GetYmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
      fwrite(&vi,si,1,fl);

      //vd = GetZmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
      vd = 0.; fwrite(&vd,sd,1,fl);
      vd = GetZmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
      fwrite(&vi,si,1,fl);

      fclose(fl);

      cout << "-W- <SpdFieldMapData1_8::WriteBinaryFile> Empty data arrays " << endl;

      return kFALSE;
  }

  //vd = GetXmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
  vd = 0.; fwrite(&vd,sd,1,fl);
  vd = GetXmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
  fwrite(&fNx,si,1,fl);

  //vd = GetYmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
  vd = 0; fwrite(&vd,sd,1,fl);
  vd = GetYmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
  fwrite(&fNy,si,1,fl);

  //vd = GetZmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
  vd = 0.; fwrite(&vd,sd,1,fl);
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
void SpdFieldMapData1_8::PrintData() const
{
  cout << "<SpdFieldMapData1_8::Print> " << endl;

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
