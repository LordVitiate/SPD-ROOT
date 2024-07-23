//_____________________________________________________________________________
//
// SpdAxialFieldMap
//_____________________________________________________________________________

#include "SpdAxialFieldMap.h"
#include "SpdFieldPar.h"
#include <TSystem.h>
#include <TMath.h>

#include <exception>
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

using TMath::Abs;

#define RTOLERANCE 1.e-12

ClassImp(SpdAxialFieldMap)

//_____________________________________________________________________________
SpdAxialFieldMap::SpdAxialFieldMap():
SpdField("SpdAxialFieldMap"),SpdAxialFieldMapData("SpdAxialFieldMapData"),
fApproxMethod(0)
{
    fType = 3;
    SetTitle("Map");
    SetNFieldParameters(2);
}

//_____________________________________________________________________________
SpdAxialFieldMap::SpdAxialFieldMap(const Char_t* name, const Char_t* type):
SpdField(name),SpdAxialFieldMapData(name,type),
fApproxMethod(0)
{
    fType = 3;
    SetTitle("AxialMap");
    SetNFieldParameters(2);
}

//_____________________________________________________________________________
SpdAxialFieldMap::SpdAxialFieldMap(SpdFieldPar* fieldPar, const Char_t* name):
SpdField(name),SpdAxialFieldMapData(name),
fApproxMethod(0) 
{
   fType = 3;
   SetTitle("AxialMap");
   
   if (!fieldPar) {
       cout << "-E- <SpdField::SpdAxialFieldMap> Empty parameter container " << endl;
       SetNFieldParameters(2);
       return;
   }
 
   GetFieldParameters(fieldPar);
   GetRegionParameters(fieldPar); 

   InitData(fFileName);
   
   if (Abs(fParams[0]-1.) > 1e-9 || 
       Abs(fParams[1]-1.) > 1e-9) 
   {
      SpdAxialFieldMapData::MultiplyField(fParams[0],fParams[1]);
   }
}

//_____________________________________________________________________________
void SpdAxialFieldMap::Clear()
{
   SpdAxialFieldMapData::Clear();
}

//_____________________________________________________________________________
Bool_t SpdAxialFieldMap::InitData(TString fileName, TString path)
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
             cout << "-I- <SpdAxialFieldMap::InitData> Path to the field map: " << pathToData << endl;
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
              cout << "-I- <SpdAxialFieldMap::InitData> Path to the field map ($MAGFPATH): " << pathToData << endl;
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
              cout << "-I- <SpdAxialFieldMap::InitData> Path to the field map ($VMCWORKDIR/input): " << pathToData << endl;
              found = true; 
              break; 
          }
      }
      
      break;
   }
   
   if (!found) cout << "-I- <SpdAxialFieldMap::InitData> Search for the field map in a current directory: " << endl;
   
   return SpdAxialFieldMapData::InitData(fileName,pathToData);
}

//_____________________________________________________________________________
Bool_t SpdAxialFieldMap::InitData() // protected
{
   cout <<"-I- <SpdAxialFieldMap::InitData> " << endl;
 
   fRmin = GetRmin();
   fZmin = SpdAxialFieldMapData::GetZmin();
    
   fRstep = GetRstep();
   fZstep = GetZstep();
   
   fRevCellSize = fRstep*fZstep;
   fRevCellSize = 1./fRevCellSize; 
   
   fFr = fBr->GetArray();
   fFz = fBz->GetArray();
   
   fInitLevel = 2;
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdAxialFieldMap::MultiplyField(Double_t v)
{
    MultiplyField(v,v);
}

//_____________________________________________________________________________
void SpdAxialFieldMap::MultiplyField(Double_t vr, Double_t vz)
{
   fParams[0] *= vr;
   fParams[1] *= vz;
   
   SpdAxialFieldMapData::MultiplyField(vr,vz);
}

//_____________________________________________________________________________
Bool_t SpdAxialFieldMap::IsInsideRegion(Double_t x, Double_t y, Double_t z) 
{
   z = Abs(z); 
   if (!IsInsideMapRegion(x,y,z)) return kFALSE;
   if (!fFieldRegion) return kTRUE;
   return (fSwitchFieldRegion) ? fFieldRegion->IsInside(x,y,z) : kFALSE; 
}

//_____________________________________________________________________________
Bool_t SpdAxialFieldMap::IsInsideRegion(Double_t r, Double_t z)
{
   z = Abs(z);  
   if (!IsInsideMapRegion(r,z)) return kFALSE;
   if (!fFieldRegion) return kTRUE;
   return (fSwitchFieldRegion) ? fFieldRegion->IsInside(r,z) : kFALSE; 
}

//_____________________________________________________________________________
void SpdAxialFieldMap::ResetParameters()
{
   SpdField::ResetParameters();
    
   fParams[0] = 1.;
   fParams[1] = 1.;
}

//_____________________________________________________________________________
void SpdAxialFieldMap::SetNFieldParameters(Int_t n)
{
   SpdField::SetNFieldParameters(2);
   
   fParams[0] = 1.;
   fParams[1] = 1.;
}

//_____________________________________________________________________________
void SpdAxialFieldMap::SetApproxMethod(Int_t method)
{ 
   if (method < 0 || method > 2) {
       cout << "<SpdAxialFieldMap::SetApproxMethod> "
            << " Unknown method approximation type: " << method 
            << ", set default (0) " << endl;
       fApproxMethod = 0;       
   }
   
   fApproxMethod = method;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t SpdAxialFieldMap::GetBx(Double_t& f, Double_t x, Double_t y, Double_t z)
{
   Int_t sgn = (z < 0) ? -1 : 1;
   z = Abs(z);  
   
   if (!IsInsideRegion(x,y,z)) return kFALSE;
      
   Double_t v = TMath::Sqrt(x*x+y*y);
   
   FindCell(v,z);
   
   if (v < RTOLERANCE) { // very close to z-axis  FIXME
       // x = 0, y = 0 => phi = pi, cos(phi) = -1 
       // (see ROOT TVector2 phi angle definition)
       //v = -1.; 
       f = 0;
       return kTRUE;
   }
   else {
       if (Abs(x) > v*1.e-12) v = x/v;   // cos(phi)
       else { // cos(phi) < 1e-12 => Bx = 0
           f = 0; 
           return kTRUE;
       }
   }
   
   switch (fApproxMethod) 
   {
      case 0 : { f = sgn*v*Approx_0(fFr); return kTRUE; }
      case 1 : { f = sgn*v*Approx_1(fFr); return kTRUE; }
      case 2 : { f = sgn*v*Approx_2(fFr); return kTRUE; }  
   
      default : return kFALSE;
   } 
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdAxialFieldMap::GetBy(Double_t& f, Double_t x, Double_t y, Double_t z)
{
   Int_t sgn = (z < 0) ? -1 : 1;
   z = Abs(z);
   
   if (!IsInsideRegion(x,y,z)) return kFALSE;
   
   Double_t v = TMath::Sqrt(x*x+y*y);
   
   FindCell(v,z);
   
   if (v < RTOLERANCE) { // very close to z-axis  FIXME
       // x = 0, y = 0 => phi = pi, sin(phi) = 0 => By = 0  
       // (see ROOT TVector2 phi angle definition)
       f = 0; 
       return kTRUE;
   }
   else {
       if (Abs(y) > v*1.e-12) v = y/v;   // sin(phi)
       else { // sin(phi) < 1e-12 => By = 0
           f = 0; 
           return kTRUE;
       }
   }
     
   switch (fApproxMethod) 
   {
      case 0 : { f = sgn*v*Approx_0(fFr); return kTRUE; }
      case 1 : { f = sgn*v*Approx_1(fFr); return kTRUE; }
      case 2 : { f = sgn*v*Approx_2(fFr); return kTRUE; }  
   
      default : return kFALSE;
   } 
    
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdAxialFieldMap::GetBz(Double_t& f, Double_t x, Double_t y, Double_t z)
{
   z = Abs(z);
    
   if (!IsInsideRegion(x,y,z)) return kFALSE;
   
   Double_t v = TMath::Sqrt(x*x+y*y);
   
   FindCell(v,z);
   
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
Bool_t SpdAxialFieldMap::GetField(const Double_t point[3], Double_t* bField)
{
   if (!bField) return kFALSE;
   
   //cout << "-I- <SpdAxialFieldMap::GetField> point: " 
   //     << point[0] << " " << point[1] << " " << point[2] << endl;
       
   if (std::isnan(point[0]) || std::isnan(point[1]) || std::isnan(point[2])) 
   {
       cout << "-W- <SpdAxialFieldMap::GetField> point is NAN, return FALSE" << endl; 
      
       bField[0] = 0;
       bField[1] = 0;
       bField[2] = 0;
       
       return kFALSE;
   }
   
   if (!std::isfinite(point[0]) || !std::isfinite(point[1]) || !std::isfinite(point[2])) 
   {
       cout << "-W- <SpdAxialFieldMap::GetField> point is INFINITY, return TRUE" << endl; 
                 
       bField[0] = 0;
       bField[1] = 0;
       bField[2] = 0;
       
       return kTRUE;
   }

   Double_t z = point[2]; 
   Int_t sgn = (z < 0) ? -1 : 1; 
   z = Abs(z);
   
   if (!IsInsideRegion(point[0],point[1],z)) return kFALSE;
   
   Double_t v = TMath::Sqrt(point[0]*point[0]+point[1]*point[1]);
   
   FindCell(v,z);
   
   Double_t cosp(0), sinp(0);// cos(phi), sin(phi)
   
   if (v < RTOLERANCE) {
       // phi = pi, cos(phi) = -1, sin(phi) = 0 
       // (see ROOT TVector2 phi angle definition for x = y = 0)
       //cosp = -1;
       //sinp =  0.; 
       switch (fApproxMethod) 
       {
          case 0 : bField[2] = Approx_0(fFz);
          case 1 : bField[2] = Approx_1(fFz);
          case 2 : bField[2] = Approx_2(fFz);
       }
       
       bField[0] = 0;
       bField[1] = 0;
   
       return kTRUE;
   }
   else {
       if (Abs(point[0]) > v*1.e-12) cosp = sgn*point[0]/v;  
       if (Abs(point[1]) > v*1.e-12) sinp = sgn*point[1]/v; 
   }
   
   switch (fApproxMethod) 
   {
      case 0 : {
                 v = Approx_0(fFr);
                 
                 bField[0] = cosp*v; 
                 bField[1] = sinp*v; 
                 bField[2] = Approx_0(fFz);
                 
                 //cout << " Point: " <<  point[0]  << " " <<  point[1]  << " " << point[2]  << endl;
                 //cout << " Field: " <<  bField[0] << " " <<  bField[1] << " " << bField[2] << endl;
                 
                 return kTRUE;
               }
      case 1 : { 
                 v = Approx_1(fFr);
                 
                 bField[0] = cosp*v; 
                 bField[1] = sinp*v; 
                 bField[2] = Approx_1(fFz);                 
   
                 //cout << " Point: " <<  point[0]  << " " <<  point[1]  << " " << point[2]  << endl;
                 //cout << " Field: " <<  bField[0] << " " <<  bField[1] << " " << bField[2] << endl;                
                
                 return kTRUE;
               }
      case 2 : { 
                 v = Approx_2(fFr);
                 
                 bField[0] = cosp*v; 
                 bField[1] = sinp*v; 
                 bField[2] = Approx_2(fFz);         
                 
                 //cout << " Point: " <<  point[0]  << " " <<  point[1]  << " " << point[2]  << endl;
                 //cout << " Field: " <<  bField[0] << " " <<  bField[1] << " " << bField[2] << endl;
                 
                 return kTRUE;
               }
      default : {
          
              bField[0] = 0;
              bField[1] = 0;
              bField[2] = 0;
              
              return kFALSE;
      }
   }
  
   return kFALSE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t SpdAxialFieldMap::GetParameters(SpdFieldPar* pars) // protected
{ 
   pars->GetParameter(Add_Name("filename"),fFileName);
   if (fFileName == "unknown") fFileName = "";
   
   pars->GetParameter(Add_Name("approx.type"),fApproxMethod); 
   
   return kTRUE; 
}

//_____________________________________________________________________________
Bool_t SpdAxialFieldMap::PutParameters(SpdFieldPar* pars) // protected
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
void SpdAxialFieldMap::PrintFieldParameters() const
{
   cout << "<SpdAxialFieldMap::PrintFieldParameters>" << endl;
   
   cout << "Init level:           " << fInitLevel << endl;
   cout << "Approximation method: " << fApproxMethod << endl;
   
   cout << endl;
   
   printf(" M(Br) = %12.6e\n",fParams[0]);
   printf(" M(Bz) = %12.6e\n",fParams[1]);
  
   cout << endl;
   
   SpdAxialFieldMapData::PrintData();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________ 
void SpdAxialFieldMap::FindCell(const Double_t& r, const Double_t& z) 
{
    Int_t iz = Int_t((z - fZmin)/fZstep);
    Int_t ir = Int_t((r - fRmin)/fRstep);
    
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
Double_t SpdAxialFieldMap::Approx_0(const Double_t* V) const
{
    Double_t value  = fDz1 * (fDr2 * V[fI_10] + fDr1 * V[fI_11])   // F(10) * dz1*dr2 + F(11) * dz1*dr1
                    + fDz2 * (fDr2 * V[fI_00] + fDr1 * V[fI_01]);  // F(00) * dz2*dr2 + F(01) * dz2*dr1

//     cout << "SIZE: " << fBr->GetSize() << endl;
//    
//     cout << "[z0,r0] F(00): " << fI_00 << " " << V[fI_00] << endl;
//     cout << "[z1,r0] F(10): " << fI_10 << " " << V[fI_10] << endl;
//     cout << "[z0,r1] F(01): " << fI_01 << " " << V[fI_01] << endl;
//     cout << "[z1,r1] F(11): " << fI_11 << " " << V[fI_11] << endl;
    
//    cout << "value: " << fRevCellSize * value  << ";   Dz1, Dr1 = " << fDz1 << ", " << fDr1 << "\n\n";
     
    return fRevCellSize * value;
}

//_____________________________________________________________________________ 
Double_t SpdAxialFieldMap::Approx_1(const Double_t* V) const
{
   if (fDr1 < 0.5*fRstep) 
       return (fDz1 < 0.5*fZstep) ? V[fI_00] : V[fI_10]; 
   else 
       return (fDz1 < 0.5*fZstep) ? V[fI_01] : V[fI_11];
}

//_____________________________________________________________________________ 
Double_t SpdAxialFieldMap::Approx_2(const Double_t* V) const
{
   Double_t value = V[fI_00] + V[fI_10] + V[fI_01] + V[fI_11];  
    
   return 0.25 * value;
}



