// $Id$
// Author: artur   2021/09/21

//_____________________________________________________________________________
//
// SpdZdcGeoMapper
//_____________________________________________________________________________

#include "SpdZdcGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdZdcGeoMapper)

SpdZdcGeoMapper* SpdZdcGeoMapper::fInstance = 0;

SpdZdcGeoMapper* SpdZdcGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdZdcGeoMapper(); }

SpdZdcGeoMapper* SpdZdcGeoMapper::GetMapper() 
{ return fInstance; }   
    
//_____________________________________________________________________________
SpdZdcGeoMapper::SpdZdcGeoMapper():SpdGeoMapper("Zdc")
{
  if (fInstance) {
      Fatal("SpdZdcGeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = 1; //SpdCommonGeoMapper::Instance()->GetZdcDefGeoType();
}

//_____________________________________________________________________________
SpdZdcGeoMapper::SpdZdcGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdZdcGeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = 1; //SpdCommonGeoMapper::Instance()->GetZdcDefGeoType();
}

//_____________________________________________________________________________
SpdZdcGeoMapper::~SpdZdcGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdZdcGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdZdcGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
  
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "air")       { fUnsetMedia = "airX";    } 
   else                            { fUnsetMedia = option;    }
   
   //cout << "-I- <SpdZdcGeoMapper::UnsetMaterials> Material: " << fUnsetMedia << endl;
}

//_____________________________________________________________________________
Bool_t SpdZdcGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
   return (gtype > 0 && gtype < 2); 
}

//_____________________________________________________________________________
Bool_t SpdZdcGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdZdcGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
 
   if (!fParams) return kFALSE;
   
   if (!CheckGeoType(gtype,"ZdcGeoType")) return kFALSE;
    
   //static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
     
   SpdParameter* par;

   TString basematerial = "air"; // mapper->GetZdcBaseMaterial()
   par = GetParameter("ZdcBaseMaterial");
   if (par) { if (reinit) *par = basematerial; }
   else { fParams->Add(new SpdParameter("ZdcBaseMaterial",basematerial)); } 
    
   //cout << "-I- <SpdZdcGeoMapper::InitGeometry> reinit: " << reinit << endl;
      
   if (fGeoType == 1) SetParameters_1(reinit);
   
   return kTRUE;
}

//_____________________________________________________________________________
TString SpdZdcGeoMapper::GetBaseMaterial() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;    
   const SpdParameter* par = GetParameter("ZdcBaseMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Double_t SpdZdcGeoMapper::GetMinDistance() const
{
   Double_t dist;
   const SpdParameter* par = GetParameter("ZdcMinDist");
   if (!par) return 0;
   par->Value(dist);
   return dist;
}

//_____________________________________________________________________________
Double_t SpdZdcGeoMapper::GetModuleLength() const
{
   Double_t length;
   const SpdParameter* par = GetParameter("ZdcLength");
   if (!par) return 0;
   par->Value(length);
   return length;
}

//_____________________________________________________________________________
Double_t SpdZdcGeoMapper::GetModuleWidth() const
{
   Double_t width;
   const SpdParameter* par = GetParameter("ZdcWidth");
   if (!par) return 0;
   par->Value(width);
   return width;
}

//_____________________________________________________________________________
Double_t SpdZdcGeoMapper::GetModuleHmin() const
{
   Double_t hmin;
   const SpdParameter* par = GetParameter("ZdcHmin");
   if (!par) return 0;
   par->Value(hmin);
   return hmin;
}

//_____________________________________________________________________________
Double_t SpdZdcGeoMapper::GetModuleHmax() const
{
   Double_t hmax;
   const SpdParameter* par = GetParameter("ZdcHmax");
   if (!par) return 0;
   par->Value(hmax);
   return hmax;
}

//_____________________________________________________________________________
void SpdZdcGeoMapper::SetParameters_1(Bool_t reinit) // private
{
   //static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
   
   Double_t mindist = 1289.5; // mapper->GetTofECMinDist()
   par = GetParameter("ZdcMinDist");  
   if (par) { if (reinit) *par = mindist; }
   else { fParams->Add(new SpdParameter("ZdcMinDist",mindist)); } 
   
   Double_t length = 63.4;   // mapper->GetZdcLength();
   par = GetParameter("ZdcLength");  
   if (par) { if (reinit) *par = length; }
   else { fParams->Add(new SpdParameter("ZdcLength",length)); }
    
   Double_t width = 20.0;   // mapper->GetZdcWidth();
   par = GetParameter("ZdcWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("ZdcWidth",width)); } 
   
   Double_t hmin = 7.4;     // mapper->GetZdcHmin();
   par = GetParameter("ZdcHmin");  
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("ZdcHmin",hmin)); }
   
   Double_t hmax = 12.6;   // mapper->GetZdcHmax();
   par = GetParameter("ZdcHmax");  
   if (par) { *par = hmax; }
   else { fParams->Add(new SpdParameter("ZdcHmax",hmax)); }
}

//_____________________________________________________________________________
void SpdZdcGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdZdcGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




