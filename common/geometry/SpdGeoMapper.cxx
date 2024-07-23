// $Id$
// Author: artur   2018/01/31


//_____________________________________________________________________________
//
// SpdGeoMapper
//_____________________________________________________________________________

#include "SpdGeoMapper.h"

#include <TObjArray.h>
#include <TObjString.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdGeoMapper)

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGeoMapper                                                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdGeoMapper::SpdGeoMapper():
fGeoType(0),fGeoPrefix(""),fMasterVolName(""),fUnsetMedia(""),
fParams(0),fGeoVolPars(0),fGeoTable(0),
fLockGeometry(kFALSE)
{
 
}

//_____________________________________________________________________________
SpdGeoMapper::SpdGeoMapper(TString prefix):
fGeoType(0),fGeoPrefix(prefix),fMasterVolName(""),fUnsetMedia(""),
fParams(0),fGeoVolPars(0),fGeoTable(0),
fLockGeometry(kFALSE)
{
 
}

//_____________________________________________________________________________
SpdGeoMapper::~SpdGeoMapper() 
{
   if (fParams) { 
       fParams->Delete();
       delete fParams; 
       fParams = 0;
   }
   
   if (fGeoVolPars) { 
       fGeoVolPars->Delete();
       delete fGeoVolPars; 
       fGeoVolPars = 0;
   }
   
   if (fGeoTable) {
       delete fGeoTable;
       fGeoTable = 0;
   }
}

//_____________________________________________________________________________
void SpdGeoMapper::DeleteParameters() 
{
   if (fParams) fParams->Delete();
}

//_____________________________________________________________________________
void SpdGeoMapper::DeleteVolPars()
{
   if (fGeoVolPars) fGeoVolPars->Delete();
}

//_____________________________________________________________________________
void SpdGeoMapper::DeleteGeoTable() 
{
   if (fGeoTable) fGeoTable->Delete();
} 

//_____________________________________________________________________________
void SpdGeoMapper::SetGeoType(Int_t gtype) 
{ 
   if (fLockGeometry) {
       cout << "-I- <" << this->ClassName() << "::SetGeoType> "
            << " Geometry is locked " << endl;
       return;
   }     
  
   fGeoType = gtype; 
}

//_____________________________________________________________________________ 
SpdParameter* SpdGeoMapper::AddParameter(TString par_name) 
{
   if (!fParams) return 0;
   SpdParameter* par = GetParameter(par_name);
   if (par) return 0;
   fParams->Add(new SpdParameter(par_name));
   return GetParameter(par_name);
}

//_____________________________________________________________________________ 
SpdParameter* SpdGeoMapper::GetParameter(TString par_name)
{
   if (!fParams) return 0;
   return (SpdParameter*)fParams->FindObject(par_name);
}

//_____________________________________________________________________________ 
const SpdParameter* SpdGeoMapper::GetParameter(TString par_name) const
{
   if (!fParams) return 0;
   return (SpdParameter*)fParams->FindObject(par_name);
}

//_____________________________________________________________________________ 
const SpdGeoVolPars* SpdGeoMapper::GetVolPars(TString vol_name) const
{
   if (!fGeoVolPars) return 0;
   return (SpdGeoVolPars*)fGeoVolPars->FindObject(vol_name); 
}

//_____________________________________________________________________________ 
Int_t SpdGeoMapper::GetVolActivity(TString vol_name) const
{
   const SpdGeoVolPars* pars = GetVolPars(vol_name);
   return (pars) ? pars->GetActivity() : 0;
}

//_____________________________________________________________________________ 
TObjArray* SpdGeoMapper::GetMediums() const
{
   if (!fGeoVolPars || fGeoVolPars->GetEntries() == 0) return 0;
   
   TObjArray* meds = new TObjArray();
   meds->SetOwner(kTRUE);
   
   SpdGeoVolPars* vpars;
   Int_t n = fGeoVolPars->GetEntriesFast();
   for (int i(0); i<n; i++) {
        vpars = (SpdGeoVolPars*)fGeoVolPars->At(i);
        if (vpars) {
            if (!meds->FindObject(vpars->GetMedia()))
            {
                meds->Add(new TObjString(vpars->GetMedia()));
            }
        }
   }
   
   return meds;
}

//_____________________________________________________________________________ 
Long_t SpdGeoMapper::GetNodeId(const TString& nodepath)
{
   return (fGeoTable) ? fGeoTable->GetNodeTableNum(nodepath) : -1;
}

//_____________________________________________________________________________ 
TString SpdGeoMapper::GetNodeFullName(Long_t id)
{
   return (fGeoTable) ? fGeoTable->GetNodeGeoPath(id) : "";
}

//_____________________________________________________________________________ 
Bool_t SpdGeoMapper::IsDigit(const Text_t* par_name)
{
   SpdParameter* par = GetParameter(par_name);
   
   if (!par) return kFALSE;
   
   if (par->CheckType(0))  return kTRUE;
   if (par->CheckType(0.)) return kTRUE;
   
   return kFALSE;   
}

//_____________________________________________________________________________
TString SpdGeoMapper::AddPrefix(TString name, Bool_t add_uscore) const
{
   return (add_uscore) ? Form("%s_%s",fGeoPrefix.Data(),name.Data()) : fGeoPrefix+name;
}

//_____________________________________________________________________________  
Int_t SpdGeoMapper::AddVolPars(SpdGeoVolPars* pars)
{
   if (!fGeoVolPars) return kFALSE;

   if (!fGeoVolPars->IsEmpty()) {
       SpdGeoVolPars* p = (SpdGeoVolPars*)fParams->FindObject(pars->GetName());
       if (p) {
           cout << "-W- <" << this->ClassName() << "::AddVolPars> Volume \"" << pars->GetName() 
                << "\" already added " << endl;
           return -1;     
       }
   }
   fGeoVolPars->Add(pars);
   return fGeoVolPars->GetSize()-1;
}

//_____________________________________________________________________________  
Bool_t SpdGeoMapper::CheckGeoType(Int_t geotype, TString parname)
{  
   SpdParameter* par = 0;
   
   Bool_t build_geom = kTRUE;
   
   const Bool_t verb = kTRUE;
   
   if (geotype == 0) {
       fGeoType = 0;
       if (verb) cout << "-I- <" << this->ClassName() << "::CheckGeoType> "
                      << "Reset geometry, type: " << fGeoType  << endl;
  
   }
   else if (geotype < 1) { 
      par = GetParameter(parname);
      if (par) {
          par->Value(fGeoType);
          if (verb) cout << "-I- <" << this->ClassName() << "::CheckGeoType> "
                         << "External geometry [1], type: " << fGeoType  << endl;
      }
      else {
          if (verb) cout << "-I- <" << this->ClassName() << "::CheckGeoType> "
                         << "Current geometry [1], type: " << fGeoType  << endl;
      }
   }
   else { 
      par = GetParameter(parname);
      if (par) {
          par->Value(fGeoType);   
          if (fGeoType != geotype) {
              cout << "-W- <" << this->ClassName() << "::CheckGeoType> Geometry type is already defined as "
                   << fGeoType << ", so this parameter cannot be replaced by " 
                   << geotype << endl;
          }         
          if (verb) cout << "-I- <" << this->ClassName() << "::CheckGeoType> "
                         << "External geometry [2], type: " << fGeoType  << endl;
      }
      else {
          fGeoType = geotype;
          if (verb) cout << "-I- <" << this->ClassName() << "::CheckGeoType> "
                         << "Specified geometry [2], type: " << fGeoType << endl;
      }
   }
   
   if (fGeoType < 1) { 
   
       DeleteParameters();
       fGeoType = 0;
       build_geom = kFALSE;
       
       cout << "-W- <" << this->ClassName() << "::CheckGeoType> No geometry will be built" << endl;
   }
   else {
       if (!IsGeoTypeDefined(fGeoType)) {
   
           DeleteParameters();
           fGeoType = 0;
           build_geom = kFALSE;
           cout << "-W- <" << this->ClassName() << "::CheckGeoType> Unknown type of the geometry: " 
                << geotype << endl;
       }
   }
   
   if (verb) cout << "-I- <" << this->ClassName() << "::CheckGeoType> Geometry type: " << fGeoType 
                  << " (is built: " << ((build_geom) ? "yes" : "no") << ") " << endl;
    
   if (!par && fParams) fParams->Add(new SpdParameter(parname,fGeoType));
     
   return build_geom;
}

//_____________________________________________________________________________ 
void SpdGeoMapper::FillParametersIn(SpdParSet* params)
{
   if (!params || !fParams) return; 
   
   TString pname = Form("%sUnsetMedia",fGeoPrefix.Data());
   SpdParameter* par = GetParameter(pname);
   if (par) *par = fUnsetMedia;
   else fParams->Add(new SpdParameter(pname,fUnsetMedia));
          
   params->SetMapperParameters(fParams);
}
   
//_____________________________________________________________________________   
void SpdGeoMapper::LoadParametersFrom(SpdParSet* params)
{
   if (!params| !fParams) return;
   params->FillMapperParameters(fParams);
}

//_____________________________________________________________________________   
void SpdGeoMapper::ResetMediaFromParams()
{
   if (!fParams) return; 
   TString pname = Form("%sUnsetMedia",fGeoPrefix.Data());
   SpdParameter* par = GetParameter(pname);
   if (!par) return;
   fUnsetMedia = par->Value();
}

//_____________________________________________________________________________ 
void SpdGeoMapper::Print(Option_t*) const
{
   cout << "\tGeometry type/pefix: " << fGeoType << "/" << fGeoPrefix << endl;
   cout << "\tMaster volume name:  " << fMasterVolName << "\n";
   
   if (!fParams || fParams->GetEntriesFast() == 0) return;
   
   cout << "\tParameters: " << "\n\n"; 
     
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
       if (np == fParams->GetEntries()) break;
       if (nn > 0) cout << endl;
       if (pr > 9) break;
       pr++;
       next.Reset();
   }
   cout << endl;
}

//_____________________________________________________________________________ 
void SpdGeoMapper::PrintVolPars(Int_t nvolumes) const
{ 
   Int_t nvolpars = (fGeoVolPars) ? fGeoVolPars->GetEntriesFast() : 0;
  
   cout << "\n<" << this->ClassName() << "::PrintVolPars> List of unique volumes " 
        << "(" << nvolpars << ") " << endl;
   
   if (nvolpars < 1) return;
  
   TString divider('-',150);
  
   cout << divider << endl;
   
   TString cap = GetPrintVolParsCap();
   
   if (!cap.IsWhitespace()) {
       cout << cap << endl;
       cout << divider << endl;
   }
   
   Int_t nv = nvolpars;
   if (nvolumes > 0 && nvolumes < nvolpars-1) nv = nvolumes;
     
   SpdGeoVolPars* vpars;
   for (Int_t i(0); i < nv; i++) {
        vpars = (SpdGeoVolPars*)fGeoVolPars->At(i);
        if (vpars) vpars->Prn(i,kTRUE);
   }
   
   if (nv < nvolpars-1) {
       cout << "..." << endl;
       vpars = (SpdGeoVolPars*)fGeoVolPars->At(nvolpars-1);
       if (vpars) vpars->Prn(nvolpars-1,kTRUE);
   }
   
   cout << divider << endl;
}

//_____________________________________________________________________________ 
void SpdGeoMapper::PrintGeoTable(Int_t level) const
{
   if (!fGeoTable) return; 
   cout << "\n"; 
   fGeoTable->PrintTable(level);
}




