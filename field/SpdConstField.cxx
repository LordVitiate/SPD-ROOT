// $Id$
// Author: artur   2017/10/10

#include "SpdConstField.h"
#include "SpdFieldPar.h"

#include <iomanip>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdConstField)

//_____________________________________________________________________________
SpdConstField::SpdConstField():SpdField("SpdConstField")
{
   fType = 0;
   SetTitle("Const");
   SpdField::SetNFieldParameters(3);
}

//_____________________________________________________________________________
SpdConstField::SpdConstField(const char* name):SpdField(name)
{
   fType = 0;
   SetTitle("Const");
   SpdField::SetNFieldParameters(3);
}

//_____________________________________________________________________________
SpdConstField::SpdConstField(SpdFieldPar* fieldPar, const Char_t* name):SpdField(name)
{
   fType = 0;
   SetTitle("Const");
   
   if (!fieldPar) {
       cout << "-E- <SpdField::SpdConstField> Empty parameter container " << endl;
       SpdField::SetNFieldParameters(3);
       return;
   }
   
   GetFieldParameters(fieldPar);
   GetRegionParameters(fieldPar); 
}
//_____________________________________________________________________________
void SpdConstField::SetNFieldParameters(Int_t) 
{ 
   SpdField::SetNFieldParameters(3); 
}

//_____________________________________________________________________________
Double_t SpdConstField::GetBx(Double_t x, Double_t y, Double_t z) 
{
   return (IsInsideRegion(x,y,z)) ? fParams[0] : 0;
}

//_____________________________________________________________________________
Double_t SpdConstField::GetBy(Double_t x, Double_t y, Double_t z) 
{
   return (IsInsideRegion(x,y,z)) ? fParams[1] : 0; 
}

//_____________________________________________________________________________
Double_t SpdConstField::GetBz(Double_t x, Double_t y, Double_t z) 
{
   return (IsInsideRegion(x,y,z)) ? fParams[2] : 0; 
}

//_____________________________________________________________________________
Bool_t SpdConstField::GetBx(Double_t& f, Double_t x, Double_t y, Double_t z) 
{
   if (!IsInsideRegion(x,y,z)) return kFALSE;
   f = fParams[0];
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdConstField::GetBy(Double_t& f, Double_t x, Double_t y, Double_t z) 
{
   if (!IsInsideRegion(x,y,z)) return kFALSE;
   f = fParams[1];
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdConstField::GetBz(Double_t& f, Double_t x, Double_t y, Double_t z) 
{
   if (!IsInsideRegion(x,y,z)) return kFALSE;
   f = fParams[2];
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdConstField::GetField(const Double_t point[3], Double_t* bField)
{
   if (!IsInsideRegion(point[0],point[1],point[2])) return kFALSE;
  
   bField[0] = fParams[0];
   bField[1] = fParams[1];
   bField[2] = fParams[2];  
  
   return kTRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________ 
void SpdConstField::PrintFieldParameters() const
{
   cout << "\t<SpdConstField::PrintFieldParameters>" << endl;
   
   cout << endl;
   
   printf("\t Bx = %12.6e\n",fParams[0]);
   printf("\t By = %12.6e\n",fParams[1]);
   printf("\t Bz = %12.6e\n",fParams[2]);
  
   cout << endl;
}


