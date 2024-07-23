// $Id$
// Author: artur   2020/07/04


//_____________________________________________________________________________
//
// SpdAxialFieldMapData
//_____________________________________________________________________________

#include "SpdAxialFieldMapData.h"

#include <TMath.h>
#include <TVector3.h>

#include "SpdAxialFieldMapData.h"

#include <fstream>
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

using TMath::DegToRad;

ClassImp(SpdAxialFieldMapData)

//_____________________________________________________________________________
SpdAxialFieldMapData::SpdAxialFieldMapData():
fMapName("SpdAxialFieldMapData"),
fPathToData(""),fFileName(""),
fScaleUnit(1.),fFieldUnit(1.),
fInitLevel(0),
fMapRegion(0),
fNr(0),fNz(0), 
fBr(0),fBz(0)
{
 
}

//_____________________________________________________________________________________
SpdAxialFieldMapData::SpdAxialFieldMapData(const Char_t* name, const Char_t* type):
fMapName(name),
fPathToData(""),fFileName(""),
fScaleUnit(1.),fFieldUnit(1.),
fInitLevel(0),
fMapRegion(0),
fNr(0),fNz(0), 
fBr(0),fBz(0)     
{  
   SetMapType(type);
}


//_____________________________________________________________________________
SpdAxialFieldMapData::~SpdAxialFieldMapData() 
{
   if (fBr) delete fBr;
   if (fBz) delete fBz;
   
   if (fMapRegion) delete fMapRegion;
}

//_____________________________________________________________________________________
void SpdAxialFieldMapData::Clear()
{  
   fPathToData = "";
   fFileName = "";
 
   ClearData();
}

//_____________________________________________________________________________________
void SpdAxialFieldMapData::ClearData() 
{
   fMapType = "undefined";
   
   fFieldUnit = 1.;
   fScaleUnit = 1.;
   
   if (fMapRegion) { delete fMapRegion; fMapRegion = 0; }
     
   fNr = 0; 
   fNz = 0;
   
   if (fBr) { delete fBr; fBr = 0; }
   if (fBz) { delete fBz; fBz = 0; }
   
   fInitLevel = 0;
}

//_____________________________________________________________________________________
Bool_t SpdAxialFieldMapData::IsGridInit() const
{ 
   if (fNr < 2 || fNz < 2) return kFALSE; 
   if (!fMapRegion || !fMapRegion->IsRegionInit()) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdAxialFieldMapData::IsDataInit() const
{
   return (fBr && fBz); 
}

//_____________________________________________________________________________________
Bool_t SpdAxialFieldMapData::IsInsideMapRegion(Double_t x, Double_t y, Double_t z) const
{
   return (fMapRegion) ? fMapRegion->IsInside(x,y,z) : kFALSE;
}
//_____________________________________________________________________________________
Bool_t SpdAxialFieldMapData::IsInsideMapRegion(Double_t r, Double_t z) const
{
   return (fMapRegion) ? fMapRegion->IsInside(r,z) : kFALSE;
}

//_____________________________________________________________________________________
Double_t SpdAxialFieldMapData::GetRstep() const 
{ 
   if (!IsGridInit()) return 0;
   return fMapRegion->GetDR()/(fNr - 1); 
}

//_____________________________________________________________________________________
Double_t SpdAxialFieldMapData::GetZstep() const 
{ 
   if (!IsGridInit()) return 0;
   return fMapRegion->GetLz()/(fNz - 1);
}

//_____________________________________________________________________________________
void SpdAxialFieldMapData::SetPathToData(TString path)
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
void SpdAxialFieldMapData::MultiplyField(Double_t v)
{
   MultiplyField(v,v);
}

//_____________________________________________________________________________________
void SpdAxialFieldMapData::MultiplyField(Double_t vr, Double_t vz)
{
   if (GetInitLevel() < 1) return;

   for (Int_t i(0); i < GetNTotal(); i++) {    
        fBr->AddAt(fBr->At(i)*vr,i);
        fBz->AddAt(fBz->At(i)*vz,i);   
   }
}

//_____________________________________________________________________________
Bool_t SpdAxialFieldMapData::GetMinMaxStat(Double_t* B) const
{
   // fill (B,Bz,Br)[min,max]
  
   if (!B) return kFALSE;
     
   memset(B,0,6*sizeof(Double_t));
   
   if (!IsDataInit()) return kFALSE;
   
   Int_t nTot = GetNTotal();
   
   for (Int_t i(0); i<3; i++) { B[2*i] = 1e20; B[2*i+1] = -1e20; }
   
   Double_t br, bz, b;
   
   for (Int_t i(0); i<nTot; i++) 
   {
        br = fBr->At(i);
        bz = fBz->At(i);
        b  = TMath::Sqrt(br*br+bz*bz);
        
        if (B[0] > b)  B[0] = b; // min  
        if (B[1] < b)  B[1] = b; // max
        
        if (B[2] > bz) B[2] = bz;
        if (B[3] < bz) B[3] = bz;
        
        if (B[4] > br) B[4] = br;
        if (B[5] < br) B[5] = br;
   }
   
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
void SpdAxialFieldMapData::SetMapType(TString type) // protected
{ 
   if (!type.IsWhitespace()) fMapType = type;
   else fMapType = "undefined";
}  

//_____________________________________________________________________________________
Bool_t SpdAxialFieldMapData::SetScaleUnit(TString unit) // protected
{
        if (unit == "mm") fScaleUnit = 0.1;
   else if (unit == "m")  fScaleUnit = 100.0;
   else if (unit == "cm") fScaleUnit = 1.0;
   else {
       cout << "-E- <SpdAxialFieldMapData::SetScaleUnit> Unknown unit: " << unit << endl;
       fScaleUnit = 1;
       return kFALSE;
   }
   return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdAxialFieldMapData::SetFieldUnit(TString unit) // protected
{
        if (unit == "G")  fFieldUnit = 0.001;
   else if (unit == "T")  fFieldUnit = 10.0;
   else if (unit == "kG") fFieldUnit = 1.0;
   else {
       cout << "-E- <SpdAxialFieldMapData::SetFieldUnit> Unknown unit: " << unit << endl;
       fFieldUnit = 1;
       return kFALSE;
   }
   return kTRUE;
}

//_____________________________________________________________________________________
TString SpdAxialFieldMapData::GetScaleUnitAsString() const 
{
   if (fScaleUnit == 0.1)   return "mm";
   if (fScaleUnit == 100.0) return "m";
   if (fScaleUnit == 1.0)   return "cm";
  
   cout << "-E- <SpdAxialFieldMapData::GetScaleUnitString> Unknown unit: " << fScaleUnit << endl;
   return "unknown";
}

//_____________________________________________________________________________________
TString SpdAxialFieldMapData::GetFieldUnitAsString() const 
{
   if (fFieldUnit == 0.001) return "G";
   if (fFieldUnit == 10.0)  return "T";
   if (fFieldUnit == 1.0)   return "kG";
  
   cout << "-E- <SpdAxialFieldMapData::GetFieldUnitString> Unknown unit: " << fFieldUnit << endl;
   return "unknown";
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Bool_t SpdAxialFieldMapData::InitData(TString fileName, TString path)
{
   Clear();
  
   if (fileName.IsWhitespace()) {
       cout << "-E- <SpdAxialFieldMapData::Init> No data file " << endl;
       return kFALSE;
   }
   
   SetPathToData(path);
   fFileName = fileName;
       
   if (fileName.EndsWith(".dat")) {
       if (!ReadAsciiFile()) {
           cout << "-F- <SpdAxialFieldMapData::Init> Bad data (*.dat) file " << endl;
           exit(1);
       }
   }
   else if (fileName.EndsWith(".bin")) { 
       if (!ReadBinaryFile()) {
           cout << "-F- <SpdAxialFieldMapData::Init> Bad data (*.bin) file " << endl;
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
Bool_t SpdAxialFieldMapData::InitData(const SpdAxialFieldMapData* data)
{
   Clear();
   
   if (!data) {
       cout << "-E- <SpdAxialFieldMapData::Init> No data " << endl;
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
   fMapRegion = new SpdTubeRegion();
   
   fMapRegion->SetTubeRegion(data->GetRmin(),data->GetRmax(),
                             data->GetZmin(),data->GetZmax());
  
   fNr = data->GetNr();
   fNz = data->GetNz();
   
   if (!IsGridInit()) {
       cout << "-E- <SpdFieldMapData::Init> Bad grid parameters " << endl;
       Clear();
       return kFALSE;
   }

   // init data arrays

   if (data->IsDataInit()) {
       fBr = new TArrayD(*(data->GetBr()));
       fBz = new TArrayD(*(data->GetBz()));
   }   
   else {
       Int_t nt = GetNTotal();
       fBr = new TArrayD(nt);
       fBz = new TArrayD(nt);
   }
   
   fInitLevel = 1;
   return InitData();
}

//_____________________________________________________________________________________
Bool_t SpdAxialFieldMapData::InitData(Double_t rmin, Double_t rmax, Int_t nr,
                                      Double_t zmin, Double_t zmax, Int_t nz)
{
   ClearData();
   
   if (fMapRegion) delete fMapRegion;
   fMapRegion = new SpdTubeRegion();
   
   fMapRegion->SetTubeRegion(rmin,rmax,zmin,zmax);  

   fNr = nr; 
   fNz = nz;
   
   if (!IsGridInit()) {
       cout << "-E- <SpdAxialFieldMapData::Init> Bad grid parameters " << endl;
       ClearData();
       return kFALSE;
   }
   
   Int_t nt = GetNTotal();
   
   fBr = new TArrayD(nt);
   fBz = new TArrayD(nt);
   
   fInitLevel = 1;
   return InitData();
}

//_____________________________________________________________________________________
Bool_t SpdAxialFieldMapData::InitData()
{
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Bool_t SpdAxialFieldMapData::ReadAsciiFile()  // protected
{
  TString fileName = fFileName; 
  
  if (!fPathToData.IsWhitespace()) fileName = fPathToData + fileName;
 
  cout << "-I- <SpdAxialFieldMapData::ReadAsciiFile> Reading file: " 
       << fileName << endl;
      
  std::ifstream mapFile(fileName);

  if (!mapFile.is_open()) {
      cout << "-E- <SpdAxialFieldMapData:ReadAsciiFile> Could not open file: " << fileName << endl;
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
  
  Double_t rmin, rmax, zmin, zmax;
  
  mapFile >> rmin >> rmax >> fNr;
  mapFile >> zmin >> zmax >> fNz;
  
  if (fMapRegion) delete fMapRegion;
  fMapRegion = new SpdTubeRegion();
  
  fMapRegion->SetTubeRegion(rmin*fScaleUnit, rmax*fScaleUnit,
                            zmin*fScaleUnit, zmax*fScaleUnit);  
  
  if (!IsGridInit()) {
      cout << "-E- <SpdAxialFieldMapData:ReadAsciiFile> Bad grid parameters: ";
      cout << "N(r,z) = " << fNr << ", " << fNz << endl;
      cout << "[min,max] = R[" << rmin << ", " << rmax << "], " 
                     << "  Z[" << zmin << ", " << zmax << "]" << endl;
      return kFALSE;
  }
 
  // Create field arrays
  
  Int_t nTot = GetNTotal();
     
  fBr = new TArrayD(nTot);
  fBz = new TArrayD(nTot);
  
  // Read the field values
 
  Double_t rr, zz;
  Double_t br, bz;
 
  Int_t n = 0;    
  for (; n < nTot; n++) {    

       if (!mapFile.good()) {
           cout << "-E- <SpdAxialFieldMapData::ReadAsciiFile> I/O Error at position " << n << endl;
           break;
       }
      
       mapFile >> rr >> zz;
       mapFile >> br >> bz;
      
       //cout << n << "/" << nTot << " "
       //     << " point: " << rr << " " << zz 
       //     << " field: " << br << " " << bz 
       //     << endl;
               
       fBr->AddAt(br*fFieldUnit, n);
       fBz->AddAt(bz*fFieldUnit, n);
    
       if (mapFile.eof()) {
           cout << "-E- <SpdAxialFieldMapData::ReadAsciiFile> End of file reached at position " << n << endl;
           break;
       }     
  }
  
  mapFile.close();
  
  //cout << fBr->At(0) << " " <<  fBz->At(0) << endl; 
  //cout << fBr->At(n-1) << " " <<  fBz->At(n-1) << endl; 
  
  cout << "-I- <SpdAxialFieldMapData::ReadAsciiFile> Read field values: "
       << n  << "/" << nTot << endl;  
  
  return kTRUE;
}

//_____________________________________________________________________________________ 
Bool_t SpdAxialFieldMapData::ReadBinaryFile()
{
  TString fileName = fFileName; 
  
  if (!fPathToData.IsWhitespace()) fileName = fPathToData + fileName;
 
  cout << "-I- <SpdAxialFieldMapData::ReadBinaryFile> Reading file: " 
       << fileName << endl;
      
  FILE* fl = fopen(fileName.Data(),"rb");
  
  if (!fl) {
      cout << "-E- <SpdAxialFieldMapData::ReadBinaryFile> File cannot be opened: " 
           << fileName << endl;
      return kFALSE;     
  }
  
  Int_t len;
  char  sdata[256];
  Int_t si = sizeof(Int_t);
  Int_t sd = sizeof(Double_t);
 
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
  
  Double_t rmin, rmax, zmin, zmax;
  
  fread(&rmin,sd,1,fl); 
  fread(&rmax,sd,1,fl);
  fread(&fNr,si,1,fl);
  
  fread(&zmin,sd,1,fl); 
  fread(&zmax,sd,1,fl);
  fread(&fNz,si,1,fl);
  
  if (fMapRegion) delete fMapRegion;
  fMapRegion = new SpdTubeRegion();
  
  fMapRegion->SetTubeRegion(rmin*fScaleUnit, rmax*fScaleUnit,
                            zmin*fScaleUnit, zmax*fScaleUnit);  
  
  if (!IsGridInit()) {
      cout << "-E- <SpdAxialFieldMapData:ReadBinaryFile> Bad grid parameters: ";
      cout << "N(r,z) = " << fNr << ", " << fNz << endl;
      cout << "[min,max] = R[" << rmin << ", " << rmax << "], " 
                     << "  Z[" << zmin << ", " << zmax << "]" << endl;
           
      fclose(fl);
     
      return kFALSE;
  }

  // Create field arrays
  
  Int_t nTot = GetNTotal();
  
  fBr = new TArrayD(nTot);
  fBz = new TArrayD(nTot);
  
  // Read the field values
  
  Double_t vd;
  
  Int_t n = 0;    
  for (; n < nTot; n++) {               
       fread(&vd,sd,1,fl); fBr->AddAt(vd*fFieldUnit, n);
       fread(&vd,sd,1,fl); fBz->AddAt(vd*fFieldUnit, n);
  }
 
  fclose(fl);     
  
  //cout << fBr->At(0) << " " <<  fBz->At(0) << endl; 
  //cout << fBr->At(n-1) << " " <<  fBz->At(n-1) << endl; 
   
  cout << "-I- <SpdAxialFieldMapData::ReadBinaryFile> Read field values: "
       << n  << "/" << nTot << endl;  
  
  return kTRUE;
}

//_____________________________________________________________________________________ 
Bool_t SpdAxialFieldMapData::WriteAsciiFile(TString fileName, TString dirName) const
{
  // Write the field map to an ASCII file    
    
  if (fileName.IsWhitespace()) {
      cout << "-E- <SpdAxialFieldMapData::WriteAsciiFile> File is not defined " << endl;
      return kFALSE;
  }
  
  if (!fileName.EndsWith(".dat")) fileName += ".dat"; 
  
  if (!dirName.IsWhitespace()) {
      if (!dirName.EndsWith("/")) dirName += "/"; 
      fileName = dirName + fileName;
  }
  
  cout << "-I- <SpdAxialFieldMapData::WriteAsciiFile> Write field map data to an ASCII file: "
       << fileName << endl;
       
  std::ofstream mapFile(fileName);
  if (!mapFile.is_open()) {
      cout << "-E- <SpdAxialFieldMapData::WriteAsciiFile> Could not open file: " 
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
    
      mapFile << GetRmin()/fScaleUnit << " " << GetRmax()/fScaleUnit << " " << 0 << endl;
      mapFile << GetZmin()/fScaleUnit << " " << GetZmax()/fScaleUnit << " " << 0 << endl;
      
      mapFile.close();
      
      /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */
//       su_tmp = sunit_tmp;
//       fu_tmp = funit_tmp;
      /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */ 
              
      cout << "-W- <SpdAxialFieldMapData::WriteAsciiFile> Empty data arrays " << endl;
      
      return kFALSE;
  }
    
  mapFile << GetRmin()/fScaleUnit << " " << GetRmax()/fScaleUnit << " " << fNr << endl;
  mapFile << GetZmin()/fScaleUnit << " " << GetZmax()/fScaleUnit << " " << fNz << endl;
  
  Double_t dr = GetRstep();
  Double_t dz = GetZstep();
  
  Int_t index;
  
  for (Int_t ir(0); ir < fNr; ir++) {
  for (Int_t iz(0); iz < fNz; iz++) {

        index = ir*fNz + iz;
     
        mapFile << Form("%20.14f %20.14f %20.14f %20.14f \n",
                        (GetRmin() + ir*dr)/fScaleUnit,
                        (GetZmin() + iz*dz)/fScaleUnit,
                        fBr->At(index)/fFieldUnit,
                        fBz->At(index)/fFieldUnit);        
    
  }}
  
  /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */
//   su_tmp = sunit_tmp;
//   fu_tmp = funit_tmp;
  /* FIXME  *  FIXME *  FIXME  *  FIXME   *  FIXME  */
  
  mapFile.close(); 
  
  return kTRUE;
}

//_____________________________________________________________________________________ 
Bool_t SpdAxialFieldMapData::WriteBinaryFile(TString fileName, TString dirName) const
{
  // Write the field map to an ASCII file    
    
  if (fileName.IsWhitespace()) {
      cout << "-E- <SpdAxialFieldMapData::WriteBinaryFile> File is not defined " << endl;
      return kFALSE;
  }
  
  if (!fileName.EndsWith(".bin")) fileName += ".bin"; 
  
  if (!dirName.IsWhitespace()) {
      if (!dirName.EndsWith("/")) dirName += "/"; 
      fileName = dirName + fileName;
  }
  
  cout << "-I- <SpdAxialFieldMapData::WriteBinaryFile> Write field map data to an BINARY file: "
       << fileName << endl;

  FILE* fl = fopen(fileName.Data(),"wb");
  
  if (!fl) {
      cout << "-E- <SpdAxialFieldMapData::WriteBinaryFile> File cannot be written: " 
           << fileName << endl;
      return kFALSE;     
  }
   
  Int_t len , vi;
  TString sdata;
  Double_t vd;
  Int_t si = sizeof(Int_t);
  Int_t sd = sizeof(Double_t);
  
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
     
      vd = GetRmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
      vd = GetRmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
      fwrite(&vi,si,1,fl);
      
      vd = GetZmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
      vd = GetZmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
      fwrite(&vi,si,1,fl);
     
      fclose(fl);
   
      cout << "-W- <SpdAxialFieldMapData::WriteBinaryFile> Empty data arrays " << endl;
      
      return kFALSE;
  }
  
  vd = GetRmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
  vd = GetRmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
  fwrite(&fNr,si,1,fl);
  
  vd = GetZmin()/fScaleUnit; fwrite(&vd,sd,1,fl);
  vd = GetZmax()/fScaleUnit; fwrite(&vd,sd,1,fl);
  fwrite(&fNz,si,1,fl);
    
  // Save field values
   
  Int_t nTot = GetNTotal();
  
  for (Int_t n(0); n < nTot; n++) {    
    
       vd = fBr->At(n)/fFieldUnit; fwrite(&vd,sd,1,fl);
       vd = fBz->At(n)/fFieldUnit; fwrite(&vd,sd,1,fl);   
  }
 
  fclose(fl);
  
  return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________ 
void SpdAxialFieldMapData::PrintData() const
{
  cout << "<SpdAxialFieldMapData::Print> " << endl;
  
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
  cout << "\t N(r,z):     " << fNr << ", " << fNz 
       << "   [TOTAL = " << GetNTotal() << "] "<< endl;
       
  if (fMapRegion) {     
      cout << "\t R[min,max]:   " << fMapRegion->GetRmin() << ", " 
           << fMapRegion->GetRmax() << " [cm]" << "  (dr = " << GetRstep() << " cm) " << endl;
      cout << "\t Z[min,max]:   " << fMapRegion->GetZmin() << ", " 
           << fMapRegion->GetZmax() << " [cm]" << "  (dz = " << GetZstep() << " cm) " << endl;
  }
  else {
      cout << "\t MAP REGION is not defined " << endl; 
  }
  
  cout << endl;
}
