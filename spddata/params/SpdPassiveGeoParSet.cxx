// $Id$
// Author: artur   2018/01/23

//_____________________________________________________________________________
//
// SpdPassiveGeoParSet
//_____________________________________________________________________________

#include "SpdPassiveGeoParSet.h"
#include "FairParamList.h"
#include "TObjArray.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdPassiveGeoParSet)

//_____________________________________________________________________________________
SpdPassiveGeoParSet::SpdPassiveGeoParSet(const char* name, const char* title, const char* context):
FairParGenericSet(name,title,context),fParams(0)
{
  //std::cout << "<SpdPassiveGeoParSet::SpdPassiveGeoParSet> CREATE CONTAINER" << std::endl;  
  fParams = new TObjArray();
}
 
//_____________________________________________________________________________________
SpdPassiveGeoParSet::~SpdPassiveGeoParSet()
{
  FairParGenericSet::clear();
  
  if (fParams) {
      fParams->Delete();
      delete fParams;
      fParams = 0;
  }
}

//_____________________________________________________________________________________
void SpdPassiveGeoParSet::clear()
{
  if (fParams) fParams->Delete();
}

//_____________________________________________________________________________________
Bool_t SpdPassiveGeoParSet::GetParameter(const Text_t* par_name, Int_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdPassiveGeoParSet::GetParameter(const Text_t* par_name, Double_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdPassiveGeoParSet::GetParameter(const Text_t* par_name, TString& value)
{
   value = SpdParameter::GetParameter(fParams,par_name); 
   return !value.IsWhitespace();
}
    
//_____________________________________________________________________________________
void SpdPassiveGeoParSet::SetParameter(const Text_t* par_name, Int_t value)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdPassiveGeoParSet::SetParameter(const Text_t* par_name, Double_t value) 
{
   SpdParameter* par = GetParameter(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdPassiveGeoParSet::SetParameter(const Text_t* par_name, const Char_t* value)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) *par = value;  
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdPassiveGeoParSet::SetParameters(TObjArray* pars) 
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
void SpdPassiveGeoParSet::FillParameters(TObjArray* pars) 
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
Bool_t SpdPassiveGeoParSet::FindParameter(const Text_t* par_name) 
{
   if (!fParams) return kFALSE;
   return fParams->FindObject(par_name);
}

//_____________________________________________________________________________________
SpdParameter* SpdPassiveGeoParSet::GetParameter(const Text_t* par_name) 
{
   if (!fParams) return 0;
   return (SpdParameter*)fParams->FindObject(par_name);
}

//_____________________________________________________________________________________
Int_t SpdPassiveGeoParSet::GetNParameters() 
{ 
   return fParams ? fParams->GetEntries() : 0; 
}
       
//_____________________________________________________________________________________
void SpdPassiveGeoParSet::putParams(FairParamList* l)
{
   if (!l || !fParams) return;
   l->addObject("Passive geometry parameters",fParams);
}

//_____________________________________________________________________________________
Bool_t SpdPassiveGeoParSet::getParams(FairParamList* l)
{ 
   if (!l || !fParams) return kFALSE;
   return (l->fillObject("Passive geometry parameters",fParams)) ? kTRUE : kFALSE;
}

//_____________________________________________________________________________________
void SpdPassiveGeoParSet::print(Int_t opt)
{
   cout << "<SpdPassiveGeoParSet::print>" << endl;
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
void SpdPassiveGeoParSet::printParams()
{
   cout << "---------------------------------------------\n";
   cout << "<SpdPassiveGeoParSet::printParams> Name: "<< GetName() << "\n";
   if (!paramContext.IsNull()) { cout << "Context/Purpose:  " << paramContext << endl; }
   if (!author.IsNull())       { cout << "Author:           " << author << endl;      }
   if (!description.IsNull())  { cout << "Description:      " << description << endl;  }
   cout << "---------------------------------------------\n\n";
   
   print(1);
}

