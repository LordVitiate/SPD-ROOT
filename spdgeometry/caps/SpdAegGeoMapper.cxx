// $Id$
// Author: artur   2021/11/26

//_____________________________________________________________________________
//
// SpdAegGeoMapper
//_____________________________________________________________________________

#include "SpdAegGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdAegGeoMapper)

SpdAegGeoMapper* SpdAegGeoMapper::fInstance = 0;

SpdAegGeoMapper* SpdAegGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdAegGeoMapper(); }

SpdAegGeoMapper* SpdAegGeoMapper::GetMapper() 
{ return fInstance; } 
    
//_____________________________________________________________________________
SpdAegGeoMapper::SpdAegGeoMapper():SpdGeoMapper("Aeg")
{
  if (fInstance) {
      Fatal("SpdAegGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetAegDefGeoType();
}

//_____________________________________________________________________________
SpdAegGeoMapper::SpdAegGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdAegGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetAegDefGeoType();
}

//_____________________________________________________________________________
SpdAegGeoMapper::~SpdAegGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdAegGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdAegGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
      
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "air")       { fUnsetMedia = "airX";    } 
   else                            { fUnsetMedia = option;    }
   
   //cout << "-I- <SpdAegGeoMapper::UnsetMaterials> Material: " << fUnsetMedia << endl; 
}

//_____________________________________________________________________________
Bool_t SpdAegGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 2); 
}

//_____________________________________________________________________________
Bool_t SpdAegGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdAegGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
   
   if (!CheckGeoType(gtype,"AegGeoType")) return kFALSE;
     
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
  
   par = GetParameter("AegBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetAegBaseMaterial(); }
   else { fParams->Add(new SpdParameter("AegBaseMaterial",mapper->GetAegBaseMaterial())); } 
   
   //cout << "-I- <SpdAegGeoMapper::InitGeometry> reinit: " << reinit << endl;
     
   if (fGeoType == 1) SetParameters_1(reinit);
   
   return kTRUE;
}

//_____________________________________________________________________________
TString SpdAegGeoMapper::GetBaseMaterial() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;    
   const SpdParameter* par = GetParameter("AegBaseMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Double_t SpdAegGeoMapper::GetMinDistance() const
{
   Double_t dist;
   const SpdParameter* par = GetParameter("AegMinDist");
   if (!par) return 0;
   par->Value(dist);
   return dist;
}

//_____________________________________________________________________________
Double_t SpdAegGeoMapper::GetModuleSize() const
{
   Double_t size;
   const SpdParameter* par = GetParameter("AegSize");
   if (!par) return 0;
   par->Value(size);
   return size;
}

//_____________________________________________________________________________
Double_t SpdAegGeoMapper::GetModuleWidth() const
{
   Double_t width;
   const SpdParameter* par = GetParameter("AegWidth");
   if (!par) return 0;
   par->Value(width);
   return width;
}

//_____________________________________________________________________________
Double_t SpdAegGeoMapper::GetModuleThickness() const
{
   Double_t thickness;
   const SpdParameter* par = GetParameter("AegThickness");
   if (!par) return 0;
   par->Value(thickness);
   return thickness;
}

//_____________________________________________________________________________
void SpdAegGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("AegMinDist");  
   if (par) { if (reinit) *par = mapper->GetAegMinDist(); }
   else { fParams->Add(new SpdParameter("AegMinDist",mapper->GetAegMinDist())); } 
   
   Double_t size = mapper->GetAegSize();
   par = GetParameter("AegSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("AegSize",size)); } 
   
   Double_t width = mapper->GetAegWidth();
   par = GetParameter("AegWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("AegWidth",width)); } 
   
   Double_t thickness = mapper->GetAegThickness();
   par = GetParameter("AegThickness");  
   if (par) { if (reinit) *par = thickness; }
   else { fParams->Add(new SpdParameter("AegThickness",thickness)); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("AegHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("AegHmin",hmin,1)); } 
}

//_____________________________________________________________________________
void SpdAegGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdAegGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




