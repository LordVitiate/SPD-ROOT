// $Id$
// Author: artur   2018/12/05


//_____________________________________________________________________________
//
// SpdPrimGenParSet
//_____________________________________________________________________________

#include "FairParamList.h"
#include "TObjArray.h"

#include <iostream>

using std::cout;
using std::endl;

#include "SpdPrimGenParSet.h"

ClassImp(SpdPrimGenParSet)

//_____________________________________________________________________________________
SpdPrimGenParSet::SpdPrimGenParSet(const char* name, const char* title, const char* context):
FairParGenericSet(name,title,context),fParams(0)
{
  //std::cout << "<" << this->ClassName() << "::" << this->ClassName() << ">"
  //          << " CREATE CONTAINER" << std::endl;  
  fParams = new TObjArray();
}
 
//_____________________________________________________________________________________
SpdPrimGenParSet::~SpdPrimGenParSet()
{
  FairParGenericSet::clear();
  
  if (fParams) {
      fParams->Delete();
      delete fParams;
      fParams = 0;
  }
}

//_____________________________________________________________________________________
void SpdPrimGenParSet::clear()
{
  if (fParams) fParams->Delete();
}

//_____________________________________________________________________________________
Bool_t SpdPrimGenParSet::GetParameter(const Text_t* par_name, Bool_t& value, Int_t p)
{
   return SpdParameter::GetParameter(fParams,par_name,p,value);
}

//_____________________________________________________________________________________
Bool_t SpdPrimGenParSet::GetParameter(const Text_t* par_name, Char_t& value, Int_t p)
{
   return SpdParameter::GetParameter(fParams,par_name,p,value);
}

//_____________________________________________________________________________________
Bool_t SpdPrimGenParSet::GetParameter(const Text_t* par_name, Int_t& value, Int_t p)
{
   return SpdParameter::GetParameter(fParams,par_name,p,value);
}

//_____________________________________________________________________________________
Bool_t SpdPrimGenParSet::GetParameter(const Text_t* par_name, Long_t& value, Int_t p)
{
   return SpdParameter::GetParameter(fParams,par_name,p,value);
}

//_____________________________________________________________________________________
Bool_t SpdPrimGenParSet::GetParameter(const Text_t* par_name, Float_t& value, Int_t p)
{
   return SpdParameter::GetParameter(fParams,par_name,p,value);
}

//_____________________________________________________________________________________
Bool_t SpdPrimGenParSet::GetParameter(const Text_t* par_name, Double_t& value, Int_t p)
{
   return SpdParameter::GetParameter(fParams,par_name,p,value);
}

//_____________________________________________________________________________________
Bool_t SpdPrimGenParSet::GetParameter(const Text_t* par_name, TString& value, Int_t p)
{
   value = SpdParameter::GetParameter(fParams,par_name,p,""); 
   return !value.IsWhitespace();
}

//_____________________________________________________________________________________
void SpdPrimGenParSet::SetParameter(const Text_t* par_name, Bool_t value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name, p);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdPrimGenParSet::SetParameter(const Text_t* par_name, Char_t value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name, p);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value,p));
}
    
//_____________________________________________________________________________________
void SpdPrimGenParSet::SetParameter(const Text_t* par_name, Int_t value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name, p);
   if (par) par->SetValue(value); 
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdPrimGenParSet::SetParameter(const Text_t* par_name, Long_t value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name, p);
   if (par) par->SetValue(value); 
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdPrimGenParSet::SetParameter(const Text_t* par_name, Float_t value, Int_t p) 
{
   SpdParameter* par = GetParameter(par_name, p);
   if (par) par->SetValue(value); 
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdPrimGenParSet::SetParameter(const Text_t* par_name, Double_t value, Int_t p) 
{
   SpdParameter* par = GetParameter(par_name, p);
   if (par) par->SetValue(value); 
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdPrimGenParSet::SetParameter(const Text_t* par_name, const Char_t* value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name, p);
   if (par) *par = value;  
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
SpdParameter* SpdPrimGenParSet::GetParameter(const Text_t* par_name, Int_t p) 
{
   if (!fParams) return 0;
   
   TObject** content = fParams->GetObjectRef();
   SpdParameter* par;
   
   Int_t n = fParams->GetLast()+1;
   for (Int_t i = 0; i < n; ++i) {
        par = (SpdParameter*)content[i];
        if (!par || par->GetPriority() != p) continue;
        if (strcmp(par_name, par->GetName()) == 0) return par;
   }
   return 0;
}

//_____________________________________________________________________________________
Int_t SpdPrimGenParSet::GetNParameters() 
{ 
   return fParams ? fParams->GetEntriesFast() : 0; 
}

//_____________________________________________________________________________________
void SpdPrimGenParSet::putParams(FairParamList* l)
{
   if (!l || !fParams) return;
   l->addObject("Spd Primary Generator Parameters",fParams);
}

//_____________________________________________________________________________________
Bool_t SpdPrimGenParSet::getParams(FairParamList* l)
{ 
   if (!l || !fParams) return kFALSE;
   return (l->fillObject("Spd Primary Generator Parameters",fParams)) ? kTRUE : kFALSE;
}

//_____________________________________________________________________________________
void SpdPrimGenParSet::print(Int_t opt)
{
   cout << "<" << this->ClassName() << "::print>" << endl;
   cout << "Name:                 " << fName << endl;
   cout << "Title:                " << fTitle << endl;
   cout << "Number of parameters: " << GetNParameters() << " " << endl; 
   cout << endl;
   
   if (!fParams || opt < 1) return;
   
   SpdParameter* par;
   TIter next(fParams);
   
   Int_t pr = Int_t(1e9);
   while ((par = (SpdParameter*)next())) {
      if (par && par->GetPriority() < pr)
      pr = par->GetPriority();
   }
   
   next.Reset();
 
   Int_t np(0), nn(0);
 
   while (kTRUE) {   
       nn = 0;
       while ((par = (SpdParameter*)next())) {
          if (par && par->GetPriority() == pr) {
              np++;
              printf("\t%3d.   ",np);
              par->PrintParameter();
              nn++;    
          }
       }
       if (np == fParams->GetEntries()) {
           //cout << "------------" << endl;
           break;
       }
       if (nn > 0) cout << endl;
       if (pr > 10000) break;
       pr++;
       next.Reset();
   }
   cout << endl;
}

//_____________________________________________________________________________________
void SpdPrimGenParSet::printByLabel(Int_t pr)
{
   if (!fParams) return;
   
   SpdParameter* par;
   TIter next(fParams);
   
   cout << "<" << this->ClassName() << "::printByLabel>" << endl;
   cout << "Name:                 " << fName << endl;
   cout << "Title:                " << fTitle << endl;
   cout << endl;
   
   Int_t np(0);
   while ((par = (SpdParameter*)next())) {
      if (par && par->GetPriority() == pr)  {
          np++;
          printf("\t%3d.   ",np);
          par->PrintParameter();
      }
   }
   cout << endl;
}

//_____________________________________________________________________________________
void SpdPrimGenParSet::printParams()
{
   cout << "-------------------------------------------------------------\n";
   cout << "<" << this->ClassName() << "::printParams> Name: "<< GetName() << "\n";
   if (!paramContext.IsNull()) { cout << "Context/Purpose:  " << paramContext << endl; }
   if (!author.IsNull())       { cout << "Author:           " << author << endl;      }
   if (!description.IsNull())  { cout << "Description:      " << description << endl;  }
   cout << "-------------------------------------------------------------\n\n";
   
   print(1);
}

