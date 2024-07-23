// $Id$
// Author: alver   2020/07/17

//_____________________________________________________________________________
//
// SpdRsTEC2GeoMapper
//_____________________________________________________________________________

#include "SpdRsTEC2GeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdRsTEC2GeoMapper)

SpdRsTEC2GeoMapper* SpdRsTEC2GeoMapper::fInstance = 0;

SpdRsTEC2GeoMapper* SpdRsTEC2GeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdRsTEC2GeoMapper(); }

SpdRsTEC2GeoMapper* SpdRsTEC2GeoMapper::GetMapper() 
{ return fInstance; } 
    
//_____________________________________________________________________________
SpdRsTEC2GeoMapper::SpdRsTEC2GeoMapper():SpdGeoMapper("RsEC")
{
  if (fInstance) {
      Fatal("SpdRsTEC2GeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsTECDefGeoType();
}

//_____________________________________________________________________________
SpdRsTEC2GeoMapper::SpdRsTEC2GeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdRsTEC2GeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsTECDefGeoType();
}

//_____________________________________________________________________________
SpdRsTEC2GeoMapper::~SpdRsTEC2GeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdRsTEC2GeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdRsTEC2GeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
 
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "air")       { fUnsetMedia = "airX";    } 
   else                            { fUnsetMedia = option;    }
}

// **************************************************************************** 
// ********************* GETTERS (BEGIN) ************************************** 
// **************************************************************************** 

//_____________________________________________________________________________
TString SpdRsTEC2GeoMapper::GetBaseMaterial() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;
   const SpdParameter* par = GetParameter("RsTECBaseMaterial");    
   return (par) ?  par->Value() : ""; 
} 

//_____________________________________________________________________________
TString SpdRsTEC2GeoMapper::GetAbsorbMaterial() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("RsTECAbsorbMaterial");
   return (par) ?  par->Value() : ""; 
}

//_____________________________________________________________________________
TString SpdRsTEC2GeoMapper::GetPassiveMaterialMDT() const
{    
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("RsTECPassiveMaterialMDT");    
   return (par) ?  par->Value() : ""; 
} 

//_____________________________________________________________________________
TString SpdRsTEC2GeoMapper::GetActiveMaterialMDT() const 
{   
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("RsTECActiveMaterialMDT");    
   return (par) ?  par->Value() : ""; 
} 

//_____________________________________________________________________________
Int_t SpdRsTEC2GeoMapper::GetNSectors() const
{
   Int_t nsec;
   const SpdParameter* par = GetParameter("RsTECNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}


//_____________________________________________________________________________
Double_t SpdRsTEC2GeoMapper::GetSecAngleOver2() const
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdRsTEC2GeoMapper::GetModuleInnerRadius() const
{
   const SpdParameter* par = GetParameter("RsTECHmin");
   Double_t l = 0;
   if (par) par->Value(l);
   return l;
}

//_____________________________________________________________________________
Double_t SpdRsTEC2GeoMapper::GetModuleOuterRadius() const
{
   const SpdParameter* par = GetParameter("RsTECSize");
   Double_t l = 0;
   if (par) par->Value(l);
   return l;
}

//_____________________________________________________________________________
Double_t SpdRsTEC2GeoMapper::GetModuleThickness() const
{
   const SpdParameter* par = GetParameter("RsTECThickness");
   Double_t l = 0;
   if (par) par->Value(l);
   return l;
}

//_____________________________________________________________________________
Double_t SpdRsTEC2GeoMapper::GetDist() const
{
   const SpdParameter* par = GetParameter("RsTECMinDist1");
   Double_t l = 0;
   if (par) par->Value(l);
   return l;
}

//_____________________________________________________________________________ 
Int_t SpdRsTEC2GeoMapper::GetNLayers() const 
{    
   const SpdParameter* par = GetParameter("RsTECNLayers");    
   Int_t nlayers = 0;    
   if (par) par->Value(nlayers);    
   return nlayers; 
}

//_____________________________________________________________________________
Bool_t SpdRsTEC2GeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
  return (gtype > 0 && gtype < 3); 
}

// **************************************************************************** 
// ********************* GETTERS (END) **************************************** 
// ****************************************************************************
//
//_____________________________________________________________________________
Bool_t SpdRsTEC2GeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdRsTEC2GeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
   
   if (!CheckGeoType(gtype,"RsTECGeoType")) return kFALSE;
     
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
  
   // ATTENTION Number of sectors -> FIXED!
   const Int_t NUMBER_OF_SECTORS = 8;
   par = GetParameter("RsTECNSectors");
   if (par) { if (reinit) *par = NUMBER_OF_SECTORS; }
   else { fParams->Add(new SpdParameter("RsTECNSectors", NUMBER_OF_SECTORS)); }
   
   par = GetParameter("RsTECBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetRsTECBaseMaterial(); }
   else { fParams->Add(new SpdParameter("RsTECBaseMaterial",mapper->GetRsTECBaseMaterial())); } 
   
   //cout << "-I- <SpdRsTEC2GeoMapper::InitGeometry> reinit: " << reinit << endl;
     
   SetParameters(reinit);
    
   return kTRUE;
}

//_____________________________________________________________________________
void SpdRsTEC2GeoMapper::SetParameters(Bool_t reinit) // private
{ 
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   par = GetParameter("RsTECMinDist");  
   if (par) { if (reinit) *par = mapper->GetRsTECMinDist1(); }
   else { fParams->Add(new SpdParameter("RsTECMinDist1",mapper->GetRsTECMinDist1())); } 
   
   Double_t size = mapper->GetRsTECSize();
   par = GetParameter("RsTECSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("RsTECSize",size)); } 
   
   Double_t width = mapper->GetRsTECWidth();
   par = GetParameter("RsTECWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("RsTECWidth",width)); } 
   
   Double_t thickness = mapper->GetRsTECThickness();
   par = GetParameter("RsTECThickness");  
   if (par) { if (reinit) *par = thickness; }
   else { fParams->Add(new SpdParameter("RsTECThickness",thickness)); }  
   
   /* define secondary parameters */
  
   Double_t hmin = size - width;  
   par = GetParameter("RsTECHmin");
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("RsTECHmin",hmin,1)); } 


   /* define materials */

   TString material;
   material = "iron";    
   par = GetParameter("RsTECAbsorbMaterial");    
   if (par) { if (reinit) *par = material; }   
   else { fParams->Add(new SpdParameter("RsTECAbsorbMaterial",material)); }       
   
   material = "aluminium";    
   par = GetParameter("RsTECPassiveMaterialMDT");    
   if (par) { if (reinit) *par = material; }    
   else { fParams->Add(new SpdParameter("RsTECPassiveMaterialMDT",material)); }       

   material = "MDTMixture";    
   par = GetParameter("RsTECActiveMaterialMDT");    
   if (par) { if (reinit) *par = material; }    
   else { fParams->Add(new SpdParameter("RsTECActiveMaterialMDT",material)); }    

   //----------------------------------------------------------------    
   // Number of layers calcluation within RS thickness:    
   // (Thickness - top absorber - bottom absorber/mdt)/LayerThickness + 1(bottom layer)       

   Int_t nLayers = Int_t((thickness - 6. - 9.5)/6.5) + 1;    
   par = GetParameter("RsTECNLayers");     
   if (par) { *par = nLayers; }    
   else { fParams->Add(new SpdParameter("RsTECNLayers",nLayers,2)); } 

}

//_____________________________________________________________________________
void SpdRsTEC2GeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdRsTEC2GeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




