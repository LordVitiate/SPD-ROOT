// $Id$
// Author: artur   2021/08/18

//_____________________________________________________________________________
//
// SpdTofECGeoMapper
//_____________________________________________________________________________

#include "SpdTofECGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdTofECGeoMapper)

SpdTofECGeoMapper* SpdTofECGeoMapper::fInstance = 0;

SpdTofECGeoMapper* SpdTofECGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdTofECGeoMapper(); }

SpdTofECGeoMapper* SpdTofECGeoMapper::GetMapper() 
{ return fInstance; } 
    
//_____________________________________________________________________________
SpdTofECGeoMapper::SpdTofECGeoMapper():SpdGeoMapper("TofEC")
{
  if (fInstance) {
      Fatal("SpdTofECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetTofECDefGeoType();
}

//_____________________________________________________________________________
SpdTofECGeoMapper::SpdTofECGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdTofECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetTofECDefGeoType();
}

//_____________________________________________________________________________
SpdTofECGeoMapper::~SpdTofECGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdTofECGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdTofECGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
      
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "air")       { fUnsetMedia = "airX";    } 
   else                            { fUnsetMedia = option;    }
   
   //cout << "-I- <SpdTofECGeoMapper::UnsetMaterials> Material: " << fUnsetMedia << endl; 
}

//_____________________________________________________________________________
Bool_t SpdTofECGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 3); 
}

//_____________________________________________________________________________
Bool_t SpdTofECGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdTofECGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
   
   if (!CheckGeoType(gtype,"TofECGeoType")) return kFALSE;
     
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
  
   par = GetParameter("TofECBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetTofECBaseMaterial(); }
   else { fParams->Add(new SpdParameter("TofECBaseMaterial",mapper->GetTofECBaseMaterial())); } 
   
   //cout << "-I- <SpdTofECGeoMapper::InitGeometry> reinit: " << reinit << endl;
     
        if (fGeoType == 1) SetParameters_1(reinit);
   else if (fGeoType == 2) SetParameters_2(reinit);

   return kTRUE;
}

//_____________________________________________________________________________
TString SpdTofECGeoMapper::GetBaseMaterial() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;    
   const SpdParameter* par = GetParameter("TofECBaseMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Double_t SpdTofECGeoMapper::GetMinDistance() const
{
   Double_t dist;
   const SpdParameter* par = GetParameter("TofECMinDist");
   if (!par) return 0;
   par->Value(dist);
   return dist;
}

//_____________________________________________________________________________
Double_t SpdTofECGeoMapper::GetModuleSize() const
{
   Double_t size;
   const SpdParameter* par = GetParameter("TofECSize");
   if (!par) return 0;
   par->Value(size);
   return size;
}

//_____________________________________________________________________________
Double_t SpdTofECGeoMapper::GetModuleWidth() const
{
   Double_t width;
   const SpdParameter* par = GetParameter("TofECWidth");
   if (!par) return 0;
   par->Value(width);
   return width;
}

//_____________________________________________________________________________
Double_t SpdTofECGeoMapper::GetModuleThickness() const
{
   Double_t thickness;
   const SpdParameter* par = GetParameter("TofECThickness");
   if (!par) return 0;
   par->Value(thickness);
   return thickness;
}

//_____________________________________________________________________________
Double_t SpdTofECGeoMapper::GetTofECLengthModuleMRPCTsinghua() const
{
   Double_t length;
   const SpdParameter* par = GetParameter("TofECLengthModuleMRPCTsinghua");
   if (!par) return 0;
   par->Value(length);
   return length;
}
    
//_____________________________________________________________________________    
Double_t SpdTofECGeoMapper::GetTofECSizeModuleMRPCTsinghua() const
{
   Double_t size;
   const SpdParameter* par = GetParameter("TofECSizeModuleMRPCTsinghua");
   if (!par) return 0;
   par->Value(size);
   return size;  
}
    
//_____________________________________________________________________________    
Double_t SpdTofECGeoMapper::GetTofECWidth1ModuleMRPCTsinghua() const
{
   Double_t width;
   const SpdParameter* par = GetParameter("TofECWidth1ModuleMRPCTsinghua");
   if (!par) return 0;
   par->Value(width);
   return width;  
}
    
//_____________________________________________________________________________    
Double_t SpdTofECGeoMapper::GetTofECWidth2ModuleMRPCTsinghua() const
{
   Double_t width;
   const SpdParameter* par = GetParameter("TofECWidth2ModuleMRPCTsinghua");
   if (!par) return 0;
   par->Value(width);
   return width; 
}

//_____________________________________________________________________________
void SpdTofECGeoMapper::SetParameters_1(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("TofECMinDist");  
   if (par) { if (reinit) *par = mapper->GetTofECMinDist(); }
   else { fParams->Add(new SpdParameter("TofECMinDist",mapper->GetTofECMinDist())); } 
   
   Double_t size = mapper->GetTofECSize();
   par = GetParameter("TofECSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("TofECSize",size)); } 
   
   Double_t width = mapper->GetTofECWidth();
   par = GetParameter("TofECWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("TofECWidth",width)); } 
   
   Double_t thickness = mapper->GetTofECThickness();
   par = GetParameter("TofECThickness");  
   if (par) { if (reinit) *par = thickness; }
   else { fParams->Add(new SpdParameter("TofECThickness",thickness)); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("TofECHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("TofECHmin",hmin,1)); } 
}

//_____________________________________________________________________________
void SpdTofECGeoMapper::SetParameters_2(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("TofECMinDist");  
   if (par) { if (reinit) *par = mapper->GetTofECMinDist(); }
   else { fParams->Add(new SpdParameter("TofECMinDist",mapper->GetTofECMinDist())); } 
   
   Double_t size = mapper->GetTofECSize();
   par = GetParameter("TofECSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("TofECSize",size)); } 
   
   Double_t width = mapper->GetTofECWidth();
   par = GetParameter("TofECWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("TofECWidth",width)); } 
   
   Double_t thickness = mapper->GetTofECThickness();
   par = GetParameter("TofECThickness");  
   if (par) { if (reinit) *par = thickness; }
   else { fParams->Add(new SpdParameter("TofECThickness",thickness)); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("TofECHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("TofECHmin",hmin,1)); } 
   
   //----------------------------------------------------
   // module MRPC parameters from suggestion Tsinghua 
   //----------------------------------------------------
   
   Double_t LengthModuleMRPCTsinghua = width; // cm
   par = GetParameter("TofECLengthModuleMRPCTsinghua");  
   if (par) { if (reinit) *par = LengthModuleMRPCTsinghua; }
   else { fParams->Add(new SpdParameter("TofECLengthModuleMRPCTsinghua",LengthModuleMRPCTsinghua)); }
   
   Double_t SizeModuleMRPCTsinghua = 2.5;    // cm
   par = GetParameter("TofECSizeModuleMRPCTsinghua");  
   if (par) { if (reinit) *par = SizeModuleMRPCTsinghua; }
   else { fParams->Add(new SpdParameter("TofECSizeModuleMRPCTsinghua",SizeModuleMRPCTsinghua)); }
   
   Double_t Width1ModuleMRPCTsinghua = 35.5; // cm
   par = GetParameter("TofECWidth1ModuleMRPCTsinghua");  
   if (par) { if (reinit) *par = Width1ModuleMRPCTsinghua; }
   else { fParams->Add(new SpdParameter("TofECWidth1ModuleMRPCTsinghua",Width1ModuleMRPCTsinghua)); }
   
   Double_t Width2ModuleMRPCTsinghua = 12.0; // cm
   par = GetParameter("TofECWidth2ModuleMRPCTsinghua");  
   if (par) { if (reinit) *par = Width2ModuleMRPCTsinghua; }
   else { fParams->Add(new SpdParameter("TofECWidth2ModuleMRPCTsinghua",Width2ModuleMRPCTsinghua)); }
}

//_____________________________________________________________________________
void SpdTofECGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdTofECGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}






