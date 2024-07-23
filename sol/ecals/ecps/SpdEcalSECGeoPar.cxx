// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalSECGeoPar
//_____________________________________________________________________________

#include "SpdEcalSECGeoPar.h"
#include "FairParamList.h"
#include "TObjArray.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalSECGeoPar)

//_____________________________________________________________________________________
SpdEcalSECGeoPar::SpdEcalSECGeoPar(const char* name, const char* title, const char* context):
FairParGenericSet(name,title,context),fGeoType(0),fParams(0)
{
  //std::cout << "<SpdEcalSECGeoPar::SpdEcalSECGeoPar> CREATE CONTAINER" << std::endl;  
  fParams = new TObjArray();
}
 
//_____________________________________________________________________________________
SpdEcalSECGeoPar::~SpdEcalSECGeoPar()
{
  FairParGenericSet::clear();
  
  if (fParams) {
      fParams->Delete();
      delete fParams;
      fParams = 0;
  }
}

//_____________________________________________________________________________________
void SpdEcalSECGeoPar::clear()
{
  if (fParams) fParams->Delete();
}

//_____________________________________________________________________________________
Bool_t SpdEcalSECGeoPar::GetParameter(const Text_t* par_name, Int_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdEcalSECGeoPar::GetParameter(const Text_t* par_name, Double_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdEcalSECGeoPar::GetParameter(const Text_t* par_name, TString& value)
{
   value = SpdParameter::GetParameter(fParams,par_name); 
   return !value.IsWhitespace();
}
    
//_____________________________________________________________________________________
void SpdEcalSECGeoPar::SetParameter(const Text_t* par_name, Int_t value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) { par->SetValue(value); par->SetPriority(p); }
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdEcalSECGeoPar::SetParameter(const Text_t* par_name, Double_t value, Int_t p) 
{
   SpdParameter* par = GetParameter(par_name);
   if (par) { par->SetValue(value); par->SetPriority(p); }
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdEcalSECGeoPar::SetParameter(const Text_t* par_name, const Char_t* value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) { *par = value;  par->SetPriority(p); }
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdEcalSECGeoPar::SetParameters(TObjArray* pars) 
{
   if (!pars) return;
   
   SpdParameter *par, *p;
   TIter next(pars);
   while ((par = (SpdParameter*)next())) {
      if (par) {
          p = GetParameter(par->Name());
          if (p) *p = *par; 
          else fParams->Add(new SpdParameter(*par));
      }
   }
}

//_____________________________________________________________________________________
void SpdEcalSECGeoPar::FillParameters(TObjArray* pars) 
{
   if (!pars) return;
   
   SpdParameter *par, *p;
   TIter next(fParams);
   while ((par = (SpdParameter*)next())) {
      if (par) {
          p = (SpdParameter*)pars->FindObject(par->Name());
          if (p) *p = *par; 
          else pars->Add(new SpdParameter(*par));
      }
   }
}

//_____________________________________________________________________________________
Bool_t SpdEcalSECGeoPar::FindParameter(const Text_t* par_name) 
{
   if (!fParams) return kFALSE;
   return fParams->FindObject(par_name);
}

//_____________________________________________________________________________________
SpdParameter* SpdEcalSECGeoPar::GetParameter(const Text_t* par_name) 
{
   if (!fParams) return 0;
   return (SpdParameter*)fParams->FindObject(par_name);
}

//_____________________________________________________________________________________
Int_t SpdEcalSECGeoPar::GetNParameters() 
{ 
   return fParams ? fParams->GetEntries() : 0; 
}
       
//_____________________________________________________________________________________
void SpdEcalSECGeoPar::putParams(FairParamList* l)
{
   if (!l) return;
   l->addObject("Passive geometry parameters",fParams);
}

//_____________________________________________________________________________________
Bool_t SpdEcalSECGeoPar::getParams(FairParamList* l)
{ 
   if (!l) return kFALSE;
   return (l->fillObject("Passive geometry parameters",fParams)) ? kTRUE : kFALSE;
}

//_____________________________________________________________________________________
void SpdEcalSECGeoPar::print(Int_t opt)
{
   cout << "<SpdEcalSECGeoPar::print> Geometry type: " << fGeoType << endl;
   cout << "Number of parameters = " << GetNParameters() << " " << endl; 
   cout << endl;
   
   if (!fParams || opt < 1) return;
    
   SpdParameter* par;
   TIter next(fParams);
   while ((par = (SpdParameter*)next())) {
      if (par) par->PrintParameter();
   }
   cout << endl;
}

//_____________________________________________________________________________________
void SpdEcalSECGeoPar::printParams()
{
   cout << "-------------------------------------------------------------\n";
   cout << "<SpdEcalSECGeoPar::printParams> Name: "<< GetName() << "\n";
   if (!paramContext.IsNull()) { cout << "Context/Purpose:  " << paramContext << endl; }
   if (!author.IsNull())       { cout << "Author:           " << author << endl;      }
   if (!description.IsNull())  { cout << "Description:      " << description << endl;  }
   cout << "-------------------------------------------------------------\n\n";
   
   print(1);
}

