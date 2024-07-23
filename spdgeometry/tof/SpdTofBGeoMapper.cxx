// $Id$
// Author: artur   2021/08/18

//_____________________________________________________________________________
//
// SpdTofBGeoMapper
//_____________________________________________________________________________

#include "SpdTofBGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdTofBGeoMapper)

SpdTofBGeoMapper* SpdTofBGeoMapper::fInstance = 0;

SpdTofBGeoMapper* SpdTofBGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdTofBGeoMapper(); }

SpdTofBGeoMapper* SpdTofBGeoMapper::GetMapper() 
{ return fInstance; }   
    
//_____________________________________________________________________________
SpdTofBGeoMapper::SpdTofBGeoMapper():SpdGeoMapper("TofB")
{
  if (fInstance) {
      Fatal("SpdTofBGeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetTofBDefGeoType();
}

//_____________________________________________________________________________
SpdTofBGeoMapper::SpdTofBGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdTofBGeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetTofBDefGeoType();
}

//_____________________________________________________________________________
SpdTofBGeoMapper::~SpdTofBGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdTofBGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdTofBGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
  
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "air")       { fUnsetMedia = "airX";    } 
   else                            { fUnsetMedia = option;    }
   
   //cout << "-I- <SpdTofBGeoMapper::UnsetMaterials> Material: " << fUnsetMedia << endl;
}

//_____________________________________________________________________________
Bool_t SpdTofBGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
   return (gtype > 0 && gtype < 3); 
} 

//_____________________________________________________________________________
Bool_t SpdTofBGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdTofBGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
 
   if (!fParams) return kFALSE;
   
   if (!CheckGeoType(gtype,"TofBGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
     
   SpdParameter* par;

   par = GetParameter("TofBBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetTofBBaseMaterial(); }
   else { fParams->Add(new SpdParameter("TofBBaseMaterial",mapper->GetTofBBaseMaterial())); } 
    
   //cout << "-I- <SpdTofBGeoMapper::InitGeometry> reinit: " << reinit << endl;
   
        if (fGeoType == 1) SetParameters_1(reinit);
   else if (fGeoType == 2) SetParameters_2(reinit);
   
   return kTRUE;
}

//_____________________________________________________________________________
TString SpdTofBGeoMapper::GetBaseMaterial() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;    
   const SpdParameter* par = GetParameter("TofBBaseMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Double_t SpdTofBGeoMapper::GetModuleLength() const
{
   Double_t length;
   const SpdParameter* par = GetParameter("TofBLength");
   if (!par) return 0;
   par->Value(length);
   return length;
}

//_____________________________________________________________________________
Double_t SpdTofBGeoMapper::GetModuleSize() const
{
   Double_t size;
   const SpdParameter* par = GetParameter("TofBSize");
   if (!par) return 0;
   par->Value(size);
   return size;
}

//_____________________________________________________________________________
Double_t SpdTofBGeoMapper::GetModuleWidth() const
{
   Double_t width;
   const SpdParameter* par = GetParameter("TofBWidth");
   if (!par) return 0;
   par->Value(width);
   return width;
}

//_____________________________________________________________________________
Double_t SpdTofBGeoMapper::GetTofBLengthModuleMRPCProtvino() const 
{ 
   Double_t length;
   const SpdParameter* par = GetParameter("TofBLengthModuleMRPCProtvino");
   if (!par) return 0;
   par->Value(length);
   return length;
} 

//_____________________________________________________________________________    
Double_t SpdTofBGeoMapper::GetTofBSizeModuleMRPCProtvino() const 
{ 
   Double_t size;
   const SpdParameter* par = GetParameter("TofBSizeModuleMRPCProtvino");
   if (!par) return 0;
   par->Value(size);
   return size;
}

//_____________________________________________________________________________    
Double_t SpdTofBGeoMapper::GetTofBWidthModuleMRPCProtvino() const 
{ 
   Double_t width;
   const SpdParameter* par = GetParameter("TofBWidthModuleMRPCProtvino");
   if (!par) return 0;
   par->Value(width);
   return width;
}      

//_____________________________________________________________________________
void SpdTofBGeoMapper::SetParameters_1(Bool_t reinit) // private
{
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
   
   Double_t length = mapper->GetTofBLength();
   par = GetParameter("TofBLength");  
   if (par) { if (reinit) *par = length; }
   else { fParams->Add(new SpdParameter("TofBLength",length)); }
    
   Double_t size = mapper->GetTofBSize();
   par = GetParameter("TofBSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("TofBSize",size)); }
    
   Double_t width = mapper->GetTofBWidth();
   par = GetParameter("TofBWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("TofBWidth",width)); } 
   
   /* define secondary parameters */
  
   Double_t rmin = size - width;  
   par = GetParameter("TofECRmin");
   if (par) { *par = rmin; }
   else { fParams->Add(new SpdParameter("TofECRmin",rmin,1)); } 
}

//_____________________________________________________________________________
void SpdTofBGeoMapper::SetParameters_2(Bool_t reinit) // private
{
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
   
   Double_t length = mapper->GetTofBLength();
   par = GetParameter("TofBLength");  
   if (par) { if (reinit) *par = length; }
   else { fParams->Add(new SpdParameter("TofBLength",length)); }
    
   Double_t size = mapper->GetTofBSize();
   par = GetParameter("TofBSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("TofBSize",size)); }
    
   Double_t width = mapper->GetTofBWidth();
   par = GetParameter("TofBWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("TofBWidth",width)); }
   
   /* define secondary parameters */
  
   Double_t rmin = size - width;  
   par = GetParameter("TofECRmin");
   if (par) { *par = rmin; }
   else { fParams->Add(new SpdParameter("TofECRmin",rmin,1)); } 
   
   //----------------------------------------------------
   // module MRPC parameters from suggestion Protvino 
   //----------------------------------------------------
   
   Double_t LengthModuleMRPCProtvino = 44.0; // cm // strip length
   par = GetParameter("TofBLengthModuleMRPCProtvino");  
   if (par) { if (reinit) *par = LengthModuleMRPCProtvino; }
   else { fParams->Add(new SpdParameter("TofBLengthModuleMRPCProtvino",LengthModuleMRPCProtvino)); }
   
   Double_t SizeModuleMRPCProtvino = 2.5;   // cm
   par = GetParameter("TofBSizeModuleMRPCProtvino");  
   if (par) { if (reinit) *par = SizeModuleMRPCProtvino; }
   else { fParams->Add(new SpdParameter("TofBSizeModuleMRPCProtvino",SizeModuleMRPCProtvino)); }
   
   Double_t WidthModuleMRPCProtvino = 47.5; // cm
   par = GetParameter("TofBWidthModuleMRPCProtvino");  
   if (par) { if (reinit) *par = WidthModuleMRPCProtvino; }
   else { fParams->Add(new SpdParameter("TofBWidthModuleMRPCProtvino",WidthModuleMRPCProtvino)); }


}

//_____________________________________________________________________________
void SpdTofBGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdTofBGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




