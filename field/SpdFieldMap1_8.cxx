//_____________________________________________________________________________
//
// SpdFieldMap1_8
//_____________________________________________________________________________

#include "SpdFieldMap1_8.h"
#include "SpdFieldPar.h"
#include <TSystem.h>
#include <TMath.h>

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

using TMath::Abs;

ClassImp(SpdFieldMap1_8)

//_____________________________________________________________________________
SpdFieldMap1_8::SpdFieldMap1_8():
SpdField("SpdFieldMap1_8"),SpdFieldMapData1_8("SpdFieldMapData1_8"),
fApproxMethod(0)
{
    fType = 4;
    SetTitle("Map1_8");
    SetNFieldParameters(6);
}

//_____________________________________________________________________________
SpdFieldMap1_8::SpdFieldMap1_8(const Char_t* name, const Char_t* type):
SpdField(name),SpdFieldMapData1_8(name,type),
fApproxMethod(0)
{
    fType = 4;
    SetTitle("Map1_8");
    SetNFieldParameters(6);
}

//_____________________________________________________________________________
SpdFieldMap1_8::SpdFieldMap1_8(SpdFieldPar* fieldPar, const Char_t* name):
SpdField(name),SpdFieldMapData1_8(name),
fApproxMethod(0)
{
   fType = 4;
   SetTitle("Map1_8");

   if (!fieldPar) {
       cout << "-E- <SpdField::SpdFieldMap1_8> Empty parameter container " << endl;
       SetNFieldParameters(6);
       return;
   }

   GetFieldParameters(fieldPar);
   GetRegionParameters(fieldPar);

   InitData(fFileName);

   if (Abs(fParams[0]-1.) > 1e-9 ||
       Abs(fParams[1]-1.) > 1e-9 ||
       Abs(fParams[2]-1.) > 1e-9)
   {
      SpdFieldMapData1_8::MultiplyField(fParams[0],fParams[1],fParams[2]);
   }

   if (Abs(fParams[3]) > 1e-9 ||
       Abs(fParams[4]) > 1e-9 ||
       Abs(fParams[5]) > 1e-9)
   {
      SpdFieldMapData1_8::ShiftField(fParams[3],fParams[4],fParams[5]);
   }
}

//_____________________________________________________________________________
void SpdFieldMap1_8::Clear()
{
   SpdFieldMapData1_8::Clear();
}

//_____________________________________________________________________________
Bool_t SpdFieldMap1_8::InitData(TString fileName, TString path)
{
   TString pathToData;
   Bool_t found = false;

   while (true)
   {
      // if absolute path to magnetic field map
      if (path.BeginsWith("/") ) {
         if (!gSystem->AccessPathName(path.Data())) {
             pathToData = path;
             found = true;
             cout << "-I- <SpdFieldMap1_8::InitData> Path to the field map: " << pathToData << endl;
             break;
         }
      }

      // if MAGFPATH (path to magnetic field map) is set
      path = getenv("MAGFPATH");
      path.ReplaceAll("//","/");
      if (!path.IsNull() ) {
          if (!gSystem->AccessPathName(path.Data())) {
              pathToData = path;
              found = true;
              cout << "-I- <SpdFieldMap1_8::InitData> Path to the field map ($MAGFPATH): " << pathToData << endl;
              break;
          }
      }

      // if file was not found look it in the standard path
      path = TString(getenv("VMCWORKDIR"));
      path.ReplaceAll("//","/");
      if (!path.IsNull() ) {
          (path.EndsWith("/")) ? path += "input/" : path += "/input/";
          if (!gSystem->AccessPathName(path.Data())) {
              pathToData = path;
              cout << "-I- <SpdFieldMap1_8::InitData> Path to the field map ($VMCWORKDIR/input): " << pathToData << endl;
              found = true;
              break;
          }
      }

      break;
   }

   if (!found) cout << "-I- <SpdFieldMap1_8::InitData> Search for the field map in a current directory: " << endl;

   return SpdFieldMapData1_8::InitData(fileName,pathToData);
}

//_____________________________________________________________________________
Bool_t SpdFieldMap1_8::InitData() // protected; to be called form SpdFieldMapData
{
   cout <<"-I- <SpdFieldMap1_8::InitData> (WRONG ONE) " << endl;

   // NAHERA ETO ZDES'?

   fXmin = GetXmin();
   fYmin = GetYmin();
   fZmin = GetZmin();

   fXstep = GetXstep();
   fYstep = GetYstep();
   fZstep = GetZstep();

   fRevCellSize = fXstep*fYstep*fZstep;
   fRevCellSize = 1./fRevCellSize;

   fFx = fBx->GetArray();
   fFy = fBy->GetArray();
   fFz = fBz->GetArray();

   fInitLevel = 2;

   return kTRUE;
}

//_____________________________________________________________________________
void SpdFieldMap1_8::MultiplyField(Double_t v)
{
    MultiplyField(v,v,v);
}

//_____________________________________________________________________________
void SpdFieldMap1_8::MultiplyField(Double_t vx, Double_t vy, Double_t vz)
{
   fParams[0] *= vx;
   fParams[1] *= vy;
   fParams[2] *= vz;

   SpdFieldMapData1_8::MultiplyField(vx,vy,vz);
}

//_____________________________________________________________________________
void SpdFieldMap1_8::ShiftField(Double_t dx, Double_t dy, Double_t dz)
{
   fParams[3] += dx;
   fParams[4] += dy;
   fParams[5] += dz;

   SpdFieldMapData1_8::ShiftField(dx,dy,dz);
}

//_____________________________________________________________________________
Bool_t SpdFieldMap1_8::IsInsideRegion(Double_t x, Double_t y, Double_t z)
{
   if (!IsInsideMapRegion(x,y,z)) return kFALSE;
   if (!fFieldRegion) return kTRUE;
   return (fSwitchFieldRegion) ? fFieldRegion->IsInside(x,y,z) : kFALSE;
}

//_____________________________________________________________________________
Bool_t SpdFieldMap1_8::IsInsideRegion(Double_t r, Double_t z)
{
   if (!IsInsideMapRegion(r,z)) return kFALSE;
   if (!fFieldRegion) return kTRUE;
   return (fSwitchFieldRegion) ? fFieldRegion->IsInside(r,z) : kFALSE;
}

//_____________________________________________________________________________
void SpdFieldMap1_8::ResetParameters()
{
   SpdField::ResetParameters();

   fParams[0] = 1.;
   fParams[1] = 1.;
   fParams[2] = 1.;
}

//_____________________________________________________________________________
void SpdFieldMap1_8::SetNFieldParameters(Int_t n)
{
   SpdField::SetNFieldParameters(6);

   fParams[0] = 1.;
   fParams[1] = 1.;
   fParams[2] = 1.;
}

//_____________________________________________________________________________
void SpdFieldMap1_8::SetApproxMethod(Int_t method)
{
   if (method < 0 || method > 2) {
       cout << "<SpdFieldMap1_8::SetApproxMethod> "
            << " Unknown method approximation type: " << method
            << ", set default (0) " << endl;
       fApproxMethod = 0;
   }

   fApproxMethod = method;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct SymmetryHelper {
    bool    inverseBx;
    bool    inverseBy;
    bool    inverseBxBy;

    SymmetryHelper(double& x, double& y, double& z) {
        inverseBx = x < 0;
        inverseBy = y < 0;
        inverseBxBy = z < 0;
        x = std::abs(x);
        y = std::abs(y);
        z = std::abs(z);
        if (x==0) x += 1e-8;
        if (y==0) y += 1e-8;
        if (z==0) z += 1e-8;
    }

    void UpdateBx(double& Bx) {
        if (inverseBx)   Bx = -Bx;
        if (inverseBxBy) Bx = -Bx;
    }

    void UpdateBy(double& By) {
        if (inverseBy)   By = -By;
        if (inverseBxBy) By = -By;
    }

    // Bz is never changed

    void UpdateField(double& Bx, double& By, double& Bz) {
        if (inverseBx) Bx = -Bx;
        if (inverseBy) By = -By;
        if (inverseBxBy) {
            Bx = -Bx;
            By = -By;
        }
    }
};

//_____________________________________________________________________________
Bool_t SpdFieldMap1_8::GetBx(Double_t& f, Double_t x, Double_t y, Double_t z)
{

   if (!IsInsideRegion(x,y,z)) return kFALSE;

   SymmetryHelper sh(x, y, z);

   FindCell(x,y,z);

   switch (fApproxMethod)
   {
      case 0 : { f = Approx_0(fFx); break; }
      case 1 : { f = Approx_1(fFx); break; }
      case 2 : { f = Approx_2(fFx); break; }

      default : return kFALSE;
   }

   sh.UpdateBx(f);

   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdFieldMap1_8::GetBy(Double_t& f, Double_t x, Double_t y, Double_t z)
{
   if (!IsInsideRegion(x,y,z)) return kFALSE;

   SymmetryHelper sh(x, y, z);

   FindCell(x,y,z);

   switch (fApproxMethod)
   {
      case 0 : { f = Approx_0(fFy); break; }
      case 1 : { f = Approx_1(fFy); break; }
      case 2 : { f = Approx_2(fFy); break; }

      default : return kFALSE;
   }

   sh.UpdateBy(f);

   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdFieldMap1_8::GetBz(Double_t& f, Double_t x, Double_t y, Double_t z)
{
   if (!IsInsideRegion(x,y,z)) return kFALSE;

   SymmetryHelper sh(x, y, z);

   FindCell(x,y,z);

   switch (fApproxMethod)
   {
      case 0 : { f = Approx_0(fFz); return kTRUE; }
      case 1 : { f = Approx_1(fFz); return kTRUE; }
      case 2 : { f = Approx_2(fFz); return kTRUE; }

      default : return kFALSE;
   }

   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdFieldMap1_8::GetField(const Double_t point[3], Double_t* bField)
{
   if (!IsInsideRegion(point[0],point[1],point[2])) return kFALSE;

   double x = point[0];
   double y = point[1];
   double z = point[2];

   SymmetryHelper sh(x, y, z);

   FindCell(x, y, z);

   switch (fApproxMethod)
   {
      case 0 : {
                 bField[0] = Approx_0(fFx);
                 bField[1] = Approx_0(fFy);
                 bField[2] = Approx_0(fFz);

                 //cout << " Point: " <<  point[0]  << " " <<  point[1]  << " " << point[2]  << endl;
                 //cout << " Field: " <<  bField[0] << " " <<  bField[1] << " " << bField[2] << endl;

                 sh.UpdateField(bField[0], bField[1], bField[2]);

                 return kTRUE;
               }
      case 1 : {
                 bField[0] = Approx_1(fFx);
                 bField[1] = Approx_1(fFy);
                 bField[2] = Approx_1(fFz);

                 //cout << " Point: " <<  point[0]  << " " <<  point[1]  << " " << point[2]  << endl;
                 //cout << " Field: " <<  bField[0] << " " <<  bField[1] << " " << bField[2] << endl;

                 sh.UpdateField(bField[0], bField[1], bField[2]);

                 return kTRUE;
               }
      case 2 : {
                 bField[0] = Approx_2(fFx);
                 bField[1] = Approx_2(fFy);
                 bField[2] = Approx_2(fFz);

                 //cout << " Point: " <<  point[0]  << " " <<  point[1]  << " " << point[2]  << endl;
                 //cout << " Field: " <<  bField[0] << " " <<  bField[1] << " " << bField[2] << endl;

                 sh.UpdateField(bField[0], bField[1], bField[2]);

                 return kTRUE;
               }
      default : return kFALSE;
   }

   return kFALSE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t SpdFieldMap1_8::GetParameters(SpdFieldPar* pars) // protected
{
   pars->GetParameter(Add_Name("filename"),fFileName);
   if (fFileName == "unknown") fFileName = "";

   pars->GetParameter(Add_Name("approx.type"),fApproxMethod);

   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdFieldMap1_8::PutParameters(SpdFieldPar* pars) // protected
{
   if (fFileName.IsWhitespace()) pars->SetParameter(Add_Name("filename"),"unknown");
   else pars->SetParameter(Add_Name("filename"),fFileName);

   pars->SetParameter(Add_Name("approx.type"),fApproxMethod);

   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdFieldMap1_8::PrintFieldParameters() const
{
   cout << "<SpdFieldMap1_8::PrintFieldParameters>" << endl;

   cout << "Init level:           " << fInitLevel << endl;
   cout << "Approximation method: " << fApproxMethod << endl;

   cout << endl;

   printf(" M(Bx) = %12.6e\n",fParams[0]);
   printf(" M(By) = %12.6e\n",fParams[1]);
   printf(" M(Bz) = %12.6e\n",fParams[2]);

   cout << endl;

   printf(" Shift(X) = %12.6e\n",fParams[3]);
   printf(" Shift(Y) = %12.6e\n",fParams[4]);
   printf(" Shift(Z) = %12.6e\n",fParams[5]);

   cout << endl;

   SpdFieldMapData1_8::PrintData();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdFieldMap1_8::FindCell(const Double_t& x, const Double_t& y, const Double_t& z)
{
  static Int_t Nyz(fNy*fNz), ix, iy, iz;

  ix = Int_t(x/fXstep);
  iy = Int_t(y/fYstep);
  iz = Int_t(z/fZstep);

  fI_000 = ix*Nyz + iy*fNz + iz;
  fI_001 = fI_000 + 1;
  fI_010 = fI_000 + fNz;
  fI_011 = fI_010 + 1;
  fI_100 = fI_000 + Nyz;
  fI_101 = fI_100 + 1;
  fI_110 = fI_100 + fNz;
  fI_111 = fI_110 + 1;

//  cout << "AUXOUT: fXmin = " << fXmin << endl;

  //fDx2 = x - fXstep * ix - fXmin;
  //fDy2 = y - fYstep * iy - fYmin;
  //fDz2 = z - fZstep * iz - fZmin;
  fDx2 = x - fXstep * ix - 0;
  fDy2 = y - fYstep * iy - 0;
  fDz2 = z - fZstep * iz - 0;

  fDx1 = fXstep - fDx2;
  fDy1 = fYstep - fDy2;
  fDz1 = fZstep - fDz2;

//  cout << "AUXOUT: ix, iy, iz = " << ix << ", " << iy << ", " << iz << endl;
//  cout << "AUXOUT: fDx1 = " << fDx1 << endl;
}

//_____________________________________________________________________________
Double_t SpdFieldMap1_8::Approx_0(const Float_t* V) const
{
   static Double_t value;

//    cout << "000: " << V[fI_000] << endl;
//    cout << "001: " << V[fI_001] << endl;
//    cout << "010: " << V[fI_010] << endl;
//    cout << "011: " << V[fI_011] << endl;
//    cout << "100: " << V[fI_100] << endl;
//    cout << "101: " << V[fI_101] << endl;
//    cout << "110: " << V[fI_110] << endl;
//    cout << "111: " << V[fI_111] << endl;

   value  = fDx1 * (  fDy1 * ( fDz1 * V[fI_000] + fDz2 * V[fI_001] )
                    + fDy2 * ( fDz1 * V[fI_010] + fDz2 * V[fI_011] ) );

   value += fDx2 * (  fDy1 * ( fDz1 * V[fI_100] + fDz2 * V[fI_101] )
                    + fDy2 * ( fDz1 * V[fI_110] + fDz2 * V[fI_111] ) );

//   cout << "AUXOUT: fRevCellSize: " << fRevCellSize << endl;
//   cout << "AUXOUT: fDx1 fDy1 fDz1 " << fDx1 << " " << fDy1 << " " << fDz1 << endl;
//   cout << "AUXOUT: V[fI_000]: " << V[fI_000] << endl;
//   cout << "AUXOUT: V[fI_001]: " << V[fI_001] << endl;
//   cout << "AUXOUT: V[fI_010]: " << V[fI_010] << endl;
//   cout << "AUXOUT: V[fI_011]: " << V[fI_011] << endl;
//   cout << "AUXOUT: V[fI_100]: " << V[fI_100] << endl;
//   cout << "AUXOUT: V[fI_101]: " << V[fI_101] << endl;
//   cout << "AUXOUT: V[fI_110]: " << V[fI_110] << endl;
//   cout << "AUXOUT: V[fI_111]: " << V[fI_111] << endl;
//
//   cout << "AUXOUT: value: " << value * fRevCellSize << endl;

   return fRevCellSize * value;
}

//_____________________________________________________________________________
Double_t SpdFieldMap1_8::Approx_1(const Float_t* V) const
{
   if (fDx2 < 0.5*fXstep) {
       if (fDy2 < 0.5*fYstep) return (fDz2 < 0.5*fZstep) ? V[fI_000] : V[fI_001];
       else return (fDz2 < 0.5*fZstep) ? V[fI_010] : V[fI_011];
   }
   else {
       if (fDy2 < 0.5*fYstep) return (fDz2 < 0.5*fZstep) ? V[fI_100] : V[fI_101];
       else return (fDz2 < 0.5*fZstep) ? V[fI_110] : V[fI_111];
   }
   return 0.;
}

//_____________________________________________________________________________
Double_t SpdFieldMap1_8::Approx_2(const Float_t* V) const
{
   static Double_t value;

   value =   V[fI_000] + V[fI_001] + V[fI_010] + V[fI_011]
           + V[fI_100] + V[fI_101] + V[fI_110] + V[fI_111];

   return 0.125 * value;
}
