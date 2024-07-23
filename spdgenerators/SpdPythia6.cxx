// $Id$
// Author: artur   2018/12/14


//_____________________________________________________________________________________
//
// SpdPythia6
//_____________________________________________________________________________________

#include "SpdPythia6.h"
#include "SpdPrimGenParSet.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdPythia6)

//_____________________________________________________________________________________
SpdPythia6::SpdPythia6(Int_t storage_index):TPythia6(),fStorageIndex(storage_index),
fParSetNumber(0),fCheckParName(kTRUE)
{
   fParams = new TObjArray();
}

//_____________________________________________________________________________________
SpdPythia6::~SpdPythia6() 
{
   if (fParams) {
       fParams->Delete();
       delete fParams;
   }
}

//_____________________________________________________________________________________
void SpdPythia6::SelectParSet(Int_t n)
{
   if (n == 1) SetParameters_1();
}

//_____________________________________________________________________________________
Bool_t SpdPythia6::FillParsIn(SpdPrimGenParSet* params)
{
   if (!params) return kFALSE;
     
   fCheckParName = kFALSE;
     
   params->SetParameter("SpdPythia6/PARSET",fParSetNumber,fStorageIndex);
   
   if (fParSetNumber == 1) FillParameters_1(params);
   
   fCheckParName = kTRUE;
   
   /* add local set of parameters in the external set */
   
   TObjArray* pp = params->GetParameters();
   SpdParameter *par;
   
   TIter next(fParams);
   while ((par = (SpdParameter*)next())) {
      par->SetPriority(fStorageIndex);
      if (par) pp->Add(new SpdParameter(*par));
   }
   
   return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdPythia6::LoadParsFrom(SpdPrimGenParSet* params)
{
   if (!params) return kFALSE;

   fCheckParName = kFALSE;
   
   params->GetParameter("SpdPythia6/PARSET",fParSetNumber,fStorageIndex);
   
   if (fParSetNumber == 1) LoadParameters_1(params);
 
   fCheckParName = kTRUE;
   
   /* add external set of parameters in the local set */
   
   fParams->Delete();
   
   TObjArray* pp = params->GetParameters();
   SpdParameter *par;
   TString pname;
   
   TIter next(pp);
   while ((par = (SpdParameter*)next())) {
      if (par) {
         if (par->GetPriority() != fStorageIndex) continue;
         pname = par->GetName();     
         if (CheckParName(pname)) {  
             fParams->Add(new SpdParameter(*par));
         }
      }
   }
   
   return kTRUE;  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
Bool_t SpdPythia6::GetParameter(TString par_name, Bool_t& value)
{
   CheckParName(par_name);
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdPythia6::GetParameter(TString par_name, Char_t& value)
{
   CheckParName(par_name);
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdPythia6::GetParameter(TString par_name, Int_t& value)
{
   CheckParName(par_name);
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdPythia6::GetParameter(TString par_name, Long_t& value)
{
   CheckParName(par_name);
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdPythia6::GetParameter(TString par_name, Float_t& value)
{
   CheckParName(par_name);
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdPythia6::GetParameter(TString par_name, Double_t& value)
{
   CheckParName(par_name);
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdPythia6::GetParameter(TString par_name, TString& value)
{
   CheckParName(par_name);
   value = SpdParameter::GetParameter(fParams,par_name); 
   return !value.IsWhitespace();
}

//_____________________________________________________________________________________
void SpdPythia6::SetParameter(TString par_name, Bool_t value)
{
   CheckParName(par_name);
   SpdParameter* par = GetPar(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdPythia6::SetParameter(TString par_name, Char_t value)
{
   CheckParName(par_name);
   SpdParameter* par = GetPar(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdPythia6::SetParameter(TString par_name, Int_t value)
{
   CheckParName(par_name);
   SpdParameter* par = GetPar(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdPythia6::SetParameter(TString par_name, Long_t value)
{
   CheckParName(par_name);
   SpdParameter* par = GetPar(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdPythia6::SetParameter(TString par_name, Float_t value)
{
   CheckParName(par_name);
   SpdParameter* par = GetPar(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdPythia6::SetParameter(TString par_name, Double_t value)
{
   CheckParName(par_name);
   SpdParameter* par = GetPar(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdPythia6::SetParameter(TString par_name, const Char_t* value)
{
   CheckParName(par_name);
   SpdParameter* par = GetPar(par_name);
   if (par) *par = value; 
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
SpdParameter* SpdPythia6::GetPar(TString par_name) 
{
   if (!fParams) return 0;
   return (SpdParameter*)fParams->FindObject(par_name);
}

//_____________________________________________________________________________________
Bool_t SpdPythia6::CheckParName(TString& parname)
{
   if (!fCheckParName) return kTRUE;
 
   if (!parname.BeginsWith("+SpdPythia6/")) {
       parname = "+SpdPythia6/" + parname;
       return kFALSE;
   }
   return kTRUE;
}

//_____________________________________________________________________________________
void SpdPythia6::Print(Option_t* opt) const
{
  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________________
void SpdPythia6::SetParameters_1()
{
   SetMSEL(1);
    
   fParSetNumber = 1; 
   
   cout << "-I- <SpdPythia6::SetParameters_1> Set number: " << fParSetNumber << endl;
}

//_____________________________________________________________________________________
void SpdPythia6::FillParameters_1(SpdPrimGenParSet* params)
{
   if (!params) return;
   params->SetParameter("SpdPythia6/MSEL",GetMSEL(),fStorageIndex);
}

//_____________________________________________________________________________________
void SpdPythia6::LoadParameters_1(SpdPrimGenParSet* params)
{
   if (!params) return;
    
   Int_t msel;
   if (params->GetParameter("SpdPythia6/MSEL",msel,fStorageIndex)) SetMSEL(msel);
}


