// $Id$
// Author: artur   2016/02/10


//_____________________________________________________________________________
//
// SpdParameter
//_____________________________________________________________________________

#include "SpdParameter.h"
#include <TMath.h>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdParameter)

//_____________________________________________________________________________
SpdParameter::SpdParameter(const Text_t* name): TNamed(name,"Undefined"),
Priority_(0)
{ 
  Value_.Set(0);  
}

//_____________________________________________________________________________
SpdParameter::SpdParameter(const Text_t* name, Bool_t v, Int_t p): 
TNamed(name,"Bool_t"),Priority_(p) 
{ 
  Value_.Set(sizeof(Bool_t));  
  memcpy(Value_.fArray,&v,sizeof(Bool_t)); 
}

//_____________________________________________________________________________
SpdParameter::SpdParameter(const Text_t* name, Int_t v, Int_t p): 
TNamed(name,"Int_t"),Priority_(p) 
{ 
  Value_.Set(sizeof(Int_t));  
  memcpy(Value_.fArray,&v,sizeof(Int_t)); 
}

//_____________________________________________________________________________
SpdParameter::SpdParameter(const Text_t* name, Long_t v, Int_t p): 
TNamed(name,"Long_t"),Priority_(p) 
{ 
  Value_.Set(sizeof(Int_t));  
  memcpy(Value_.fArray,&v,sizeof(Int_t)); 
}

//_____________________________________________________________________________
SpdParameter::SpdParameter(const Text_t* name, Char_t v, Int_t p): 
TNamed(name,"Char_t"),Priority_(p) 
{ 
  Value_.Set(sizeof(Char_t));  
  memcpy(Value_.fArray,&v,sizeof(Char_t)); 
}

//_____________________________________________________________________________         
SpdParameter::SpdParameter(const Text_t* name, Float_t v, Int_t p): 
TNamed(name,"Float_t"),Priority_(p) 
{ 
  Value_.Set(sizeof(Float_t));  
  memcpy(Value_.fArray,&v,sizeof(Float_t)); 
}

//_____________________________________________________________________________         
SpdParameter::SpdParameter(const Text_t* name, Double_t v, Int_t p): 
TNamed(name,"Double_t"),Priority_(p) 
{ 
  Value_.Set(sizeof(Double_t));  
  memcpy(Value_.fArray,&v,sizeof(Double_t)); 
}

//_____________________________________________________________________________         
SpdParameter::SpdParameter(const Text_t* name, const Char_t* v, Int_t p): 
TNamed(name,"String"),Priority_(p) 
{ 
  Int_t n = strlen(v);
  Value_.Set(n+1,v);  
  Value_[n] = '\0';
}

//_____________________________________________________________________________   
Bool_t SpdParameter::SetValue(Bool_t x) 
{
  if (!CheckType(x)) {
      cout << "<SpdParameter::SetValue> Bad type of the Value" << endl;
      return kFALSE;
  }
  memcpy(Value_.fArray,&x,Value_.fN);
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdParameter::SetValue(Char_t x) 
{
  if (!CheckType(x)) {
      cout << "<SpdParameter::SetValue> Bad type of the Value" << endl;
      return kFALSE;
  }
  memcpy(Value_.fArray,&x,Value_.fN);
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdParameter::SetValue(Int_t x) 
{
  if (!CheckType(x)) {
      cout << "<SpdParameter::SetValue> Bad type of the Value" << endl;
      return kFALSE;
  }
  memcpy(Value_.fArray,&x,Value_.fN);
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdParameter::SetValue(Long_t x) 
{
  if (!CheckType(x)) {
      cout << "<SpdParameter::SetValue> Bad type of the Value" << endl;
      return kFALSE;
  }
  memcpy(Value_.fArray,&x,Value_.fN);
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdParameter::SetValue(Float_t x) 
{
  if (!CheckType(x)) {
      cout << "<SpdParameter::SetValue> Bad type of the Value" << endl;
      return kFALSE;
  }
  memcpy(Value_.fArray,&x,Value_.fN); 
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdParameter::SetValue(Double_t x) 
{
  if (!CheckType(x)) {
      cout << "<SpdParameter::SetValue> Bad type of the Value" << endl;
      return kFALSE;
  }
  memcpy(Value_.fArray,&x,Value_.fN); 
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdParameter::Value(Bool_t& x) const
{ 
  if (!CheckType(x)) return kFALSE;
  memcpy(&x,Value_.fArray,Value_.fN);
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdParameter::Value(Char_t& x) const
{ 
  if (!CheckType(x)) return kFALSE;
  memcpy(&x,Value_.fArray,Value_.fN);
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdParameter::Value(Int_t& x) const
{ 
  if (!CheckType(x)) return kFALSE;
  memcpy(&x,Value_.fArray,Value_.fN);
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdParameter::Value(Long_t& x) const
{ 
  if (!CheckType(x)) return kFALSE;
  memcpy(&x,Value_.fArray,Value_.fN);
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdParameter::Value(Float_t& x) const
{
  if (!CheckType(x)) return kFALSE;
  memcpy(&x,Value_.fArray,Value_.fN);
  return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdParameter::Value(Double_t& x) const
{
  if (!CheckType(x)) return kFALSE;
  memcpy(&x,Value_.fArray,Value_.fN);
  return kTRUE;
}

//_____________________________________________________________________________   
const Char_t* SpdParameter::Value() const
{
  if (fTitle != "String") return "";
  return Value_.fArray;
}

//_____________________________________________________________________________  
SpdParameter& SpdParameter::operator=(Bool_t x) 
{ 
  fTitle = "Bool_t";
  Value_.Set(sizeof(Bool_t));  
  memcpy(Value_.fArray,&x,sizeof(Bool_t));
  return *this;
}

//_____________________________________________________________________________  
SpdParameter& SpdParameter::operator=(Char_t x) 
{ 
  fTitle = "Char_t";
  Value_.Set(sizeof(Char_t));  
  memcpy(Value_.fArray,&x,sizeof(Char_t));
  return *this;
}

//_____________________________________________________________________________  
SpdParameter& SpdParameter::operator=(Int_t x) 
{ 
  fTitle = "Int_t";
  Value_.Set(sizeof(Int_t));  
  memcpy(Value_.fArray,&x,sizeof(Int_t));
  return *this;
}

//_____________________________________________________________________________  
SpdParameter& SpdParameter::operator=(Long_t x) 
{ 
  fTitle = "Long_t";
  Value_.Set(sizeof(Long_t));  
  memcpy(Value_.fArray,&x,sizeof(Long_t));
  return *this;
}

//_____________________________________________________________________________  
SpdParameter& SpdParameter::operator=(Float_t x) 
{ 
  fTitle = "Double_t";
  Value_.Set(sizeof(Float_t));  
  memcpy(Value_.fArray,&x,sizeof(Float_t)); 
  return *this;
}

//_____________________________________________________________________________  
SpdParameter& SpdParameter::operator=(Double_t x) 
{ 
  fTitle = "Double_t";
  Value_.Set(sizeof(Double_t));  
  memcpy(Value_.fArray,&x,sizeof(Double_t)); 
  return *this;
}

//_____________________________________________________________________________  
SpdParameter& SpdParameter::operator=(const Char_t* x)
{
  fTitle = "String";
  Int_t n = strlen(x);
  Value_.Set(n+1,x);  
  Value_[n] = '\0';
  return *this;
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Bool_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = (SpdParameter*)pars->FindObject(par_name);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Char_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = (SpdParameter*)pars->FindObject(par_name);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Int_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = (SpdParameter*)pars->FindObject(par_name);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Long_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = (SpdParameter*)pars->FindObject(par_name);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Float_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = (SpdParameter*)pars->FindObject(par_name);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Double_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = (SpdParameter*)pars->FindObject(par_name);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//================================================================================= 
const Char_t* SpdParameter::GetParameter(TObjArray* pars, TString par_name) 
{
  if (!pars) return "";
  SpdParameter* par = (SpdParameter*)pars->FindObject(par_name);
  if (!par) return "";
  return par->Value();
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Int_t p, Bool_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = FindObject(pars,par_name,p);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Int_t p, Char_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = FindObject(pars,par_name,p);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Int_t p, Int_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = FindObject(pars,par_name,p);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Int_t p, Long_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = FindObject(pars,par_name,p);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Int_t p, Float_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = FindObject(pars,par_name,p);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//=================================================================================        
Bool_t SpdParameter::GetParameter(TObjArray* pars, TString par_name, Int_t p, Double_t& Value_) 
{
  if (!pars) return kFALSE;
  SpdParameter* par = FindObject(pars,par_name,p);
  if (!par) return kFALSE;
  return par->Value(Value_);
}

//================================================================================= 
const Char_t* SpdParameter::GetParameter(TObjArray* pars, TString par_name, Int_t p, const Char_t*) 
{
  if (!pars) return "";
  SpdParameter* par = FindObject(pars,par_name,p);
  if (!par) return "";
  return par->Value();
}

//_____________________________________________________________________________ 
SpdParameter* SpdParameter::FindObject(TObjArray* pars, const Char_t* name, const Int_t& p)
{
   TObject** content = pars->GetObjectRef();
   SpdParameter* par;
   
   Int_t n = pars->GetLast()+1;
   
   for (Int_t i = 0; i < n; ++i) {
        par = (SpdParameter*)content[i];
        if (!par || par->GetPriority() != p) continue;
        if (strcmp(name, par->GetName()) == 0) return par;
   }
   return 0;
}

//_____________________________________________________________________________         
void SpdParameter::PrintParameter() 
{
  printf("par:  %-40s type:  %-20s",fName.Data(),fTitle.Data());
  
  if (Priority_ < -1) {
      printf("[%d] ",Priority_);
  }
  else if (Priority_ == -1) {
      printf("%3s  ","[x]");
  }
  else if (Priority_ == 0) {
      printf("%3s  ","[+]");
  }
  else {
      printf("[%d]  ",Priority_);
  }
  
  if (fTitle == "Bool_t") {
      Bool_t x;
      Value(x);
      printf(" Value_:  %d\n",x);
  }
  else if (fTitle == "Char_t") {
      Char_t x;
      Value(x);
      printf(" Value_:  '%c'\n",x);
  }
  else if (fTitle == "Int_t") {
      Int_t x;
      Value(x);
      printf(" Value_:  %d\n",x);
  }  
  else if (fTitle == "Long_t") {
      Long_t x;
      Value(x);
      printf(" Value_:  %li\n",x);
  }  
  else if (fTitle == "Float_t") {
      Float_t x;
      Value(x);
      if (TMath::Abs(x) < 1e-15) printf(" Value_:  %-9.6f\n",0.);
      else if (!(TMath::Abs(x) < 1e-4) ) printf(" Value_:  %-9.6f\n",x);
      else printf(" Value_:  %-12.6e\n",x);
  }
  else if (fTitle == "Double_t") {
      Double_t x;
      Value(x);
      if (TMath::Abs(x) < 1e-15) printf(" Value_:  %-9.6f\n",0.);
      else if (!(TMath::Abs(x) < 1e-4) ) printf(" Value_:  %-9.6f\n",x);
      else printf(" Value_:  %-12.6e\n",x);
  }
  else if (fTitle == "String") {
      const Char_t* x = Value();
      if (x) printf(" Value_: \"%s\"\n",x);
  }
  //printf("\n");
} 

