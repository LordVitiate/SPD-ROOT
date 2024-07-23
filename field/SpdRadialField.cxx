// $Id$
// Author: artur   2017/11/24

//_____________________________________________________________________________
//
// SpdRadialField
//_____________________________________________________________________________

#include "SpdRadialField.h"
#include "SpdFieldPar.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(SpdRadialField)

//_____________________________________________________________________________
SpdRadialField::SpdRadialField():
SpdField("SpdRadialField"), fRadialFunctionType(0),
fX(0),fY(0),fZ(0),fR(0)
{
   fType = 1;
   SetTitle("Radial");
   SetNFieldParameters(1);
}

//_____________________________________________________________________________
SpdRadialField::SpdRadialField(const Char_t* name):
SpdField(name), fRadialFunctionType(0),
fX(0),fY(0),fZ(0),fR(0)
{
   fType = 1;
   SetTitle("Radial");
   SetNFieldParameters(1);
}

//_____________________________________________________________________________
SpdRadialField::SpdRadialField(SpdFieldPar* fieldPar, const Char_t* name):
SpdField(name), fRadialFunctionType(0),
fX(0),fY(0),fZ(0),fR(0)
{
  fType = 1;
  SetTitle("Radial");

  if (!fieldPar) {
      cout << "-E- <SpdField::SpdRadialField> Empty parameter container " << endl;
      SetNFieldParameters(1);
      return;
  }
  
  GetFieldParameters(fieldPar);
  GetRegionParameters(fieldPar); 
}

//_____________________________________________________________________________
Bool_t SpdRadialField::GetBx(Double_t& f, Double_t x, Double_t y, Double_t z)
{
   if (!IsInsideRegion(x,y,z)) return kFALSE;
   
   fX = x; fY = y; fZ = z;
   fR = TMath::Sqrt(x*x + y*y);
   f = Bx();
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdRadialField::GetBy(Double_t& f, Double_t x, Double_t y, Double_t z)
{
   if (!IsInsideRegion(x,y,z)) return kFALSE;
   
   fX = x; fY = y; fZ = z;
   fR = TMath::Sqrt(x*x + y*y);
   f = By();
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdRadialField::GetBz(Double_t& f, Double_t x, Double_t y, Double_t z)
{
   f = 0;
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdRadialField::GetField(const Double_t point[3], Double_t* bField)
{
   if (!IsInsideRegion(point[0],point[1],point[2])) return kFALSE;
 
   fX = point[0]; fY = point[1]; fZ = point[2];
   fR = TMath::Sqrt(point[0]*point[0]+point[1]*point[1]);
   
   bField[0] = Bx();
   bField[1] = By(); 
   bField[2] = 0; 
   
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t SpdRadialField::GetParameters(SpdFieldPar* pars) // protected
{ 
   pars->GetParameter(Add_Name("rfunc.type"),fRadialFunctionType); 
   return kTRUE; 
}

//_____________________________________________________________________________
Bool_t SpdRadialField::PutParameters(SpdFieldPar* pars) // protected
{ 
   pars->SetParameter(Add_Name("rfunc.type"),fRadialFunctionType); 
   return kTRUE; 
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Double_t SpdRadialField::Bx() const // protected
{
   if (fRadialFunctionType == 0) {
       return (fR > 1e-9) ? fParams[0]*fX/fR : fParams[0];
   }
   if (fRadialFunctionType == 1) {
       return (fR > 1e-9) ? Radial_1(1)*fX/fR : Radial_1(0);
   }
   return 0;
}
 
//_____________________________________________________________________________ 
Double_t SpdRadialField::By() const // protected
{
   if (fRadialFunctionType == 0) {
       return (fR > 1e-9) ? fParams[0]*fY/fR : fParams[0];
   }
   if (fRadialFunctionType == 1) {
       return (fR > 1e-9) ? Radial_1(1)*fY/fR : Radial_1(0);
   }
   return 0;  
}

//_____________________________________________________________________________ 
Double_t SpdRadialField::Radial_1(const Int_t exception) const
{
   if (exception == 0) return fParams[0];
   return fParams[0];
} 

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________ 
void SpdRadialField::PrintFieldParameters() const
{
   cout << "\t<SpdRadialField::PrintFieldParameters>" << endl;
   cout << "\t Radial function type: " << fRadialFunctionType << endl;
   cout << "\t Number of parameters: " << fNpars << endl;
    
   if (fNpars <= 0) return;
    
   for (Int_t i(0); i<fNpars; i++) printf("\t\t\t %4d: %12.6e\n",i,fParams[i]);
    
   cout << endl;
}


