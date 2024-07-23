// $Id$
// Author: artur   2018/10/29

//_____________________________________________________________________________
//
// SpdParSet
//_____________________________________________________________________________

#include "FairParamList.h"
#include "TObjArray.h"
#include "TObjString.h"

#include <iostream>

using std::cout;
using std::endl;

#include "SpdParSet.h"

ClassImp(SpdParSet)

//_____________________________________________________________________________________
SpdParSet::SpdParSet(const char* name, const char* title, const char* context):
FairParGenericSet(name,title,context),fParams(0),fNodes(0)
{
  //std::cout << "<" << this->ClassName() << "::" << this->ClassName() << ">"
  //          << " CREATE CONTAINER" << std::endl;  
  fParams = new TObjArray();
  fNodes  = new TObjArray();
}
 
//_____________________________________________________________________________________
SpdParSet::~SpdParSet()
{
  FairParGenericSet::clear();
  
  if (fParams) {
      fParams->Delete();
      delete fParams;
      fParams = 0;
  }
  
  if (fNodes) {
      fNodes->Delete();
      delete fNodes;
      fNodes = 0;
  }
}

//_____________________________________________________________________________________
void SpdParSet::clear()
{
  if (fParams) fParams->Delete();
  if (fNodes)  fNodes->Delete();
}

//_____________________________________________________________________________________
Bool_t SpdParSet::GetParameter(const Text_t* par_name, Bool_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdParSet::GetParameter(const Text_t* par_name, Char_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdParSet::GetParameter(const Text_t* par_name, Int_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdParSet::GetParameter(const Text_t* par_name, Long_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdParSet::GetParameter(const Text_t* par_name, Float_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdParSet::GetParameter(const Text_t* par_name, Double_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdParSet::GetParameter(const Text_t* par_name, TString& value)
{
   value = SpdParameter::GetParameter(fParams,par_name); 
   return !value.IsWhitespace();
}

//_____________________________________________________________________________________
void SpdParSet::SetParameter(const Text_t* par_name, Bool_t value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) { par->SetValue(value); par->SetPriority(p); }
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdParSet::SetParameter(const Text_t* par_name, Char_t value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) { par->SetValue(value); par->SetPriority(p); }
   else fParams->Add(new SpdParameter(par_name,value,p));
}
    
//_____________________________________________________________________________________
void SpdParSet::SetParameter(const Text_t* par_name, Int_t value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) { par->SetValue(value); par->SetPriority(p); }
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdParSet::SetParameter(const Text_t* par_name, Long_t value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) { par->SetValue(value); par->SetPriority(p); }
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdParSet::SetParameter(const Text_t* par_name, Float_t value, Int_t p) 
{
   SpdParameter* par = GetParameter(par_name);
   if (par) { par->SetValue(value); par->SetPriority(p); }
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdParSet::SetParameter(const Text_t* par_name, Double_t value, Int_t p) 
{
   SpdParameter* par = GetParameter(par_name);
   if (par) { par->SetValue(value); par->SetPriority(p); }
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdParSet::SetParameter(const Text_t* par_name, const Char_t* value, Int_t p)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) { *par = value;  par->SetPriority(p); }
   else fParams->Add(new SpdParameter(par_name,value,p));
}

//_____________________________________________________________________________________
void SpdParSet::SetParameters(TObjArray* pars) 
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
void SpdParSet::SetParameters(TObjArray* pars, Int_t pr) 
{
   if (!pars) return;
   
   SpdParameter *par, *p;
   TIter next(pars);
   while ((par = (SpdParameter*)next())) {
      if (par) {
          if (par->GetPriority() != pr) continue;
          p = GetParameter(par->Name());
          if (p) *p = *par; 
          else fParams->Add(new SpdParameter(*par));
      }
   }
}

//_____________________________________________________________________________________
void SpdParSet::SetMapperParameters(TObjArray* pars) 
{
   if (!pars) return;
   
   SpdParameter *par, *p;
   TIter next(pars);
   while ((par = (SpdParameter*)next())) {
      if (par) {
          if (par->GetPriority() < 0) continue;
          p = GetParameter(par->Name());
          if (p) *p = *par; 
          else fParams->Add(new SpdParameter(*par));
      }
   }
}

//_____________________________________________________________________________________
void SpdParSet::FillParameters(TObjArray* pars) 
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
void SpdParSet::FillParameters(TObjArray* pars, Int_t pr) 
{
   if (!pars) return;
   
   SpdParameter *par, *p;
   TIter next(fParams);
   while ((par = (SpdParameter*)next())) {
      if (par) {
          if (par->GetPriority() != pr) continue;
          p = (SpdParameter*)pars->FindObject(par->Name());
          if (p) *p = *par; 
          else pars->Add(new SpdParameter(*par));
      }
   }
}
//_____________________________________________________________________________________
void SpdParSet::FillMapperParameters(TObjArray* pars) 
{
   if (!pars) return;
   
   SpdParameter *par, *p;
   TIter next(fParams);
   while ((par = (SpdParameter*)next())) {
      if (par) {
          if (par->GetPriority() < 0) continue;
          p = (SpdParameter*)pars->FindObject(par->Name());
          if (p) *p = *par; 
          else pars->Add(new SpdParameter(*par));
      }
   }
}

//_____________________________________________________________________________________
Bool_t SpdParSet::FindParameter(const Text_t* par_name) 
{
   if (!fParams) return kFALSE;
   return fParams->FindObject(par_name);
}

//_____________________________________________________________________________________
SpdParameter* SpdParSet::GetParameter(const Text_t* par_name) 
{
   if (!fParams) return 0;
   return (SpdParameter*)fParams->FindObject(par_name);
}

//_____________________________________________________________________________________
Int_t SpdParSet::GetNParameters() 
{ 
   return fParams ? fParams->GetEntriesFast() : 0; 
}

//_____________________________________________________________________________________
Int_t SpdParSet::GetNNodes() 
{ 
   return fNodes ? fNodes->GetEntriesFast() : 0; 
}

//_____________________________________________________________________________________
TString SpdParSet::GetNodeFullName(Int_t num)
{
   if (!fNodes || num < 0 || num >= fNodes->GetEntriesFast()) return "";
   TObjString* node = (TObjString*)fNodes->At(num);
   return (node) ? node->GetName() : 0;
}

//_____________________________________________________________________________________
Int_t SpdParSet::FindNode(TObjString node)
{
   if (!fNodes || fNodes->GetEntriesFast() == 0) return -1;  
   return fNodes->IndexOf(&node);  
}
       
//_____________________________________________________________________________________
void SpdParSet::putParams(FairParamList* l)
{
   if (!l || !fParams) return;
   l->addObject("Spd Parameters Set",fParams);
}

//_____________________________________________________________________________________
Bool_t SpdParSet::getParams(FairParamList* l)
{ 
   if (!l || !fParams) return kFALSE;
   return (l->fillObject("Spd Parameters Set",fParams)) ? kTRUE : kFALSE;
}

//_____________________________________________________________________________________
void SpdParSet::print(Int_t opt)
{
   cout << "<" << this->ClassName() << "::print>" << endl;
   cout << "Name:                    " << fName << endl;
   cout << "Number of parameters:    " << GetNParameters() << " " << endl; 
   cout << "Number of special nodes: " << GetNNodes() << " " << endl; 
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
void SpdParSet::printByLabel(Int_t pr)
{
   if (!fParams) return;
   
   SpdParameter* par;
   TIter next(fParams);
   
   cout << "<" << this->ClassName() << "::printByLabel>" << endl;
   cout << "Name:                 " << fName << endl;
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
void SpdParSet::printParams()
{
   cout << "-------------------------------------------------------------\n";
   cout << "<" << this->ClassName() << "::printParams> Name: "<< GetName() << "\n";
   if (!paramContext.IsNull()) { cout << "Context/Purpose:  " << paramContext << endl; }
   if (!author.IsNull())       { cout << "Author:           " << author << endl;      }
   if (!description.IsNull())  { cout << "Description:      " << description << endl;  }
   cout << "-------------------------------------------------------------\n\n";
   
   print(1);
}
