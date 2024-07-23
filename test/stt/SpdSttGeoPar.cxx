
#include "SpdSttGeoPar.h"
#include "FairParamList.h"
#include <TObjArray.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdSttGeoPar)

//_____________________________________________________________________________________
SpdSttGeoPar::SpdSttGeoPar(const char* name, const char* title, const char* context) :
  FairParGenericSet(name,title,context),
  fGeoType(0),fParams(0)
{
   //std::cout << "<SpdSttGeoPar::SpdSttGeoPar> CREATE CONTAINER" << std::endl;  
  fParams = new TObjArray();
}
 
//_____________________________________________________________________________________
SpdSttGeoPar::~SpdSttGeoPar()
{
  FairParGenericSet::clear();
  
  if (fParams) {
      fParams->Delete();
      delete fParams;
      fParams = 0;
  }
}

//_____________________________________________________________________________________
void SpdSttGeoPar::clear()
{
  if (fParams) fParams->Delete();
}

//_____________________________________________________________________________________
Bool_t SpdSttGeoPar::GetParameter(const Text_t* par_name, Int_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}

//_____________________________________________________________________________________
Bool_t SpdSttGeoPar::GetParameter(const Text_t* par_name, Double_t& value)
{
   return SpdParameter::GetParameter(fParams,par_name,value);
}
    
//_____________________________________________________________________________________
void SpdSttGeoPar::SetParameter(const Text_t* par_name, Int_t value)
{
   SpdParameter* par = GetParameter(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdSttGeoPar::SetParameter(const Text_t* par_name, Double_t value) 
{
   SpdParameter* par = GetParameter(par_name);
   if (par) par->SetValue(value);
   else fParams->Add(new SpdParameter(par_name,value));
}

//_____________________________________________________________________________________
void SpdSttGeoPar::SetParameters(TObjArray* pars) 
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
void SpdSttGeoPar::FillParameters(TObjArray* pars) 
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
Bool_t SpdSttGeoPar::FindParameter(const Text_t* par_name) 
{
   if (!fParams) return kFALSE;
   return fParams->FindObject(par_name);
}

//_____________________________________________________________________________________
SpdParameter* SpdSttGeoPar::GetParameter(const Text_t* par_name) 
{
   if (!fParams) return 0;
   return (SpdParameter*)fParams->FindObject(par_name);
}

//_____________________________________________________________________________________
Int_t SpdSttGeoPar::GetNParameters() 
{ 
   return fParams ? fParams->GetEntries() : 0; 
}
       
//_____________________________________________________________________________________
void SpdSttGeoPar::putParams(FairParamList* l)
{
   if (!l) return;
   l->addObject("Stt geometry parameters",fParams);
}

//_____________________________________________________________________________________
Bool_t SpdSttGeoPar::getParams(FairParamList* l)
{ 
   if (!l) return kFALSE;
   return (l->fillObject("Stt geometry parameters",fParams)) ? kTRUE : kFALSE;
}

//_____________________________________________________________________________________
void SpdSttGeoPar::print(Int_t opt)
{
   cout << "<SpdSttGeoPar::print> Geometry type: " << fGeoType << endl;
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
void SpdSttGeoPar::printParams()
{
   cout << "---------------------------------------------\n";
   cout << "<SpdSttGeoPar::printParams> Name: "<< GetName() << "\n";
   if (!paramContext.IsNull()) { cout << "Context/Purpose:  " << paramContext << endl; }
   if (!author.IsNull())       { cout << "Author:           " << author << endl;      }
   if (!description.IsNull())  { cout << "Description:      " << description << endl;  }
   cout << "---------------------------------------------\n\n";
   
   print(1);
}


