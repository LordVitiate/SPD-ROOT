// $Id$
// Author: artur   2017/11/29

#include "FairParamList.h"

#include <TObjArray.h>

#include "SpdParameter.h"
#include "SpdFieldPar.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

ClassImp(SpdFieldPar)

//_____________________________________________________________________________________
SpdFieldPar::SpdFieldPar(const char* name, const char* title, const char* context) :
FairParGenericSet(name,title,context),
fParams(0)
{
  //std::cout << "<SpdFieldPar::SpdFieldPar> CREATE CONTAINER" << std::endl;  
  fParams = new TObjArray();
}
 
//_____________________________________________________________________________________
SpdFieldPar::~SpdFieldPar()
{
  FairParGenericSet::clear();
  
  if (fParams) {
      fParams->Delete();
      delete fParams;
      fParams = 0;
  }
}

//_____________________________________________________________________________________
void SpdFieldPar::clear()
{
  if (fParams) fParams->Delete();
}

//_____________________________________________________________________________________
Int_t SpdFieldPar::GetNFields() 
{    
   if (fFields.size() != 0) return fFields.size(); 
   
   if (!fParams || fParams->GetEntriesFast() == 0) return 0;
   
   TString name, ntype;
   Int_t type;
   
   SpdParameter* par;
   TIter next(fParams);
   while ((par = (SpdParameter*)next())) {
      if (par) {
          if (par->Name().BeginsWith("FieldName/")) {
              par->PrintParameter();
              name = par->Name();
              name.Remove(0,10);
              ntype = name + "/type";
              par = (SpdParameter*)fParams->FindObject(ntype);
              if (par) {
                  par->Value(type);
                  AddField(name,type);
              }
          }
      }
   }
   
   return fFields.size(); 
   
   return 0;
}

//_____________________________________________________________________________________
Bool_t SpdFieldPar::AddField(TString name, Int_t type)
{
   if (fFields.empty()) {
       fFields.insert(std::pair<string,Int_t>(name.Data(),type));
   }
   else {
       std::map<string,Int_t>::const_iterator it = fFields.find(name.Data());
       if (it != fFields.end()) {
           cout << "-W- <SpdFieldPar::AddField> Field \"" 
                << name << "\" already have added  " << endl;
           return kFALSE;
       }
       fFields.insert(std::pair<string,Int_t>(name.Data(),type));
   }
   
   TString parname = "FieldName/" + name;
   SetParameter(parname,name.Data());
   
   parname = name + "/type";  
   SetParameter(parname,type);
   
   return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdFieldPar::FindField(TString name, Int_t& type) const
{
   std::map<string,Int_t>::const_iterator it = fFields.find(name.Data());
   if (it != fFields.end()) {
       type = it->second;
       return kTRUE;
   }
   return kFALSE;
}


//_____________________________________________________________________________________
Bool_t SpdFieldPar::GetParameter(const Text_t* par_name, Int_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdFieldPar::GetParameter(const Text_t* par_name, Double_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdFieldPar::GetParameter(const Text_t* par_name, TString& value)
{
   value = SpdParameter::GetParameter(fParams,par_name); 
   return !value.IsWhitespace();
}
    
//_____________________________________________________________________________________
void SpdFieldPar::SetParameter(const Text_t* par_name, Int_t value)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdFieldPar::SetParameter(const Text_t* par_name, Double_t value) 
{
   SpdParameter* par = GetParameter(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdFieldPar::SetParameter(const Text_t* par_name, const Char_t* value)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) *par = value;  
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdFieldPar::SetParameters(TObjArray* pars) 
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
void SpdFieldPar::FillParameters(TObjArray* pars) 
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
Bool_t SpdFieldPar::FindParameter(const Text_t* par_name) 
{
   if (!fParams) return kFALSE;
   return fParams->FindObject(par_name);
}

//_____________________________________________________________________________________
SpdParameter* SpdFieldPar::GetParameter(const Text_t* par_name) 
{
   if (!fParams) return 0;
   return (SpdParameter*)fParams->FindObject(par_name);
}

//_____________________________________________________________________________________
Int_t SpdFieldPar::GetNParameters() 
{ 
   return fParams ? fParams->GetEntries() : 0; 
}
       
//_____________________________________________________________________________________
void SpdFieldPar::putParams(FairParamList* l)
{
   if (!l) return;
   l->addObject("Field parameters",fParams);
}

//_____________________________________________________________________________________
Bool_t SpdFieldPar::getParams(FairParamList* l)
{ 
   if (!l) return kFALSE;
   return (l->fillObject("Field parameters",fParams)) ? kTRUE : kFALSE;
}

//_____________________________________________________________________________________
void SpdFieldPar::print(Int_t opt)
{
   cout << "<SpdFieldPar::print> " << endl;
   cout << "Number of fields :      " << GetNFields() << " " << endl; 
   cout << "Number of parameters :  " << GetNParameters() << " " << endl; 
   cout << endl;
   
   if (!fFields.empty()) {
      int n(0);
      cout << "List of fields: \n\n";
       printf("%4s %4s    %s\n\n","N","Type","Name");
      std::map<string,Int_t>::const_iterator it = fFields.begin(); 
      for ( ; it != fFields.end(); it++) {
           printf("%4d %4d    \"%s\"\n",++n,it->second,it->first.c_str()); 
      }
      cout << endl;
   }
  
   if (!fParams || opt < 1) return;
    
   cout << "List of parameters: \n\n";
   
   SpdParameter* par;
   TIter next(fParams);
   while ((par = (SpdParameter*)next())) {
      if (par) par->PrintParameter();
   }
   cout << endl;
}

//_____________________________________________________________________________________
Bool_t SpdFieldPar::GetFieldNameType(TString& name, Int_t& type) const
{
   if (fFields.empty()) return kFALSE;
   name = fFields.begin()->first.c_str();
   type = fFields.begin()->second;
   return kTRUE;
}

//_____________________________________________________________________________________
TString SpdFieldPar::GetFieldName() const
{
   if (fFields.empty()) return "";
   return fFields.begin()->first.c_str();
}

//_____________________________________________________________________________________
void SpdFieldPar::printParams()
{
   cout << "------------------------------------------------";
   cout << "------------------------------------------------\n";
   cout << "<SpdFieldPar::printParams> Name: "<< GetName() << "\n";
   print(1);
   cout << "------------------------------------------------";
   cout << "------------------------------------------------\n\n";
}



