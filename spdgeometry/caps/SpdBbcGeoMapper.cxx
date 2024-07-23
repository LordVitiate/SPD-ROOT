// $Id$
// Author: artur   2021/11/26

//_____________________________________________________________________________
//
// SpdBbcGeoMapper
//_____________________________________________________________________________

#include "SpdBbcGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdBbcGeoMapper)

SpdBbcGeoMapper* SpdBbcGeoMapper::fInstance = 0;

SpdBbcGeoMapper* SpdBbcGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdBbcGeoMapper(); }

SpdBbcGeoMapper* SpdBbcGeoMapper::GetMapper() 
{ return fInstance; } 
    
//_____________________________________________________________________________
SpdBbcGeoMapper::SpdBbcGeoMapper():SpdGeoMapper("Bbc")
{
  if (fInstance) {
      Fatal("SpdBbcGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetBbcDefGeoType();
}

//_____________________________________________________________________________
SpdBbcGeoMapper::SpdBbcGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdBbcGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetBbcDefGeoType();
}

//_____________________________________________________________________________
SpdBbcGeoMapper::~SpdBbcGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdBbcGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdBbcGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
      
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "air")       { fUnsetMedia = "airX";    } 
   else                            { fUnsetMedia = option;    }
   
   //cout << "-I- <SpdBbcGeoMapper::UnsetMaterials> Material: " << fUnsetMedia << endl; 
}

//_____________________________________________________________________________
Bool_t SpdBbcGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 2); 
}

//_____________________________________________________________________________
Bool_t SpdBbcGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdBbcGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
   
   if (!CheckGeoType(gtype,"BbcGeoType")) return kFALSE;
     
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
  
   par = GetParameter("BbcBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetBbcBaseMaterial(); }
   else { fParams->Add(new SpdParameter("BbcBaseMaterial",mapper->GetBbcBaseMaterial())); } 
   
   //cout << "-I- <SpdBbcGeoMapper::InitGeometry> reinit: " << reinit << endl;
     
   if (fGeoType == 1) SetParameters_1(reinit);
   
   return kTRUE;
}

//_____________________________________________________________________________
TString SpdBbcGeoMapper::GetBaseMaterial() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;    
   const SpdParameter* par = GetParameter("BbcBaseMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Double_t SpdBbcGeoMapper::GetMinDistance() const
{
   Double_t dist;
   const SpdParameter* par = GetParameter("BbcMinDist");
   if (!par) return 0;
   par->Value(dist);
   return dist;
}

//_____________________________________________________________________________
Double_t SpdBbcGeoMapper::GetModuleSize() const
{
   Double_t size;
   const SpdParameter* par = GetParameter("BbcSize");
   if (!par) return 0;
   par->Value(size);
   return size;
}

//_____________________________________________________________________________
Double_t SpdBbcGeoMapper::GetModuleWidth() const
{
   Double_t width;
   const SpdParameter* par = GetParameter("BbcWidth");
   if (!par) return 0;
   par->Value(width);
   return width;
}

//_____________________________________________________________________________
Double_t SpdBbcGeoMapper::GetModuleThickness() const
{
   Double_t thickness;
   const SpdParameter* par = GetParameter("BbcThickness");
   if (!par) return 0;
   par->Value(thickness);
   return thickness;
}

//_____________________________________________________________________________
void SpdBbcGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("BbcMinDist");  
   if (par) { if (reinit) *par = mapper->GetBbcMinDist(); }
   else { fParams->Add(new SpdParameter("BbcMinDist",mapper->GetBbcMinDist())); } 
   
   Double_t size = mapper->GetBbcSize();
   par = GetParameter("BbcSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("BbcSize",size)); } 
   
   Double_t width = mapper->GetBbcWidth();
   par = GetParameter("BbcWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("BbcWidth",width)); } 
   
   Double_t thickness = mapper->GetBbcThickness();
   par = GetParameter("BbcThickness");  
   if (par) { if (reinit) *par = thickness; }
   else { fParams->Add(new SpdParameter("BbcThickness",thickness)); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("BbcHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("BbcHmin",hmin,1)); } 
}

//_____________________________________________________________________________
void SpdBbcGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdBbcGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




