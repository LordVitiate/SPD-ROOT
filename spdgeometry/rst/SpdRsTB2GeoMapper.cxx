// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsTB2GeoMapper
//_____________________________________________________________________________

#include "SpdRsTB2GeoMapper.h"
#include "SpdCommonGeoMapper.h"
#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdRsTB2GeoMapper)

SpdRsTB2GeoMapper* SpdRsTB2GeoMapper::fInstance = 0;

SpdRsTB2GeoMapper* SpdRsTB2GeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdRsTB2GeoMapper(); }

SpdRsTB2GeoMapper* SpdRsTB2GeoMapper::GetMapper() 
{ return fInstance; }   
    
//_____________________________________________________________________________
SpdRsTB2GeoMapper::SpdRsTB2GeoMapper():SpdGeoMapper("RsB")
{
  if (fInstance) {
      Fatal("SpdRsTB2GeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsTBDefGeoType();
}

//_____________________________________________________________________________
SpdRsTB2GeoMapper::SpdRsTB2GeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdRsTB2GeoMapper", "Singleton instance already exists.");
      return;
  }
   
  fInstance = this;
  
  fParams = new TObjArray();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetRsTBDefGeoType();
}

//_____________________________________________________________________________
SpdRsTB2GeoMapper::~SpdRsTB2GeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdRsTB2GeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdRsTB2GeoMapper::UnsetMaterials> Geometry is locked " << endl;
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
TString SpdRsTB2GeoMapper::GetBaseMaterial() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;  
   const SpdParameter* par = GetParameter("RsTBBaseMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
TString SpdRsTB2GeoMapper::GetAbsorbMaterial() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("RsTBAbsorbMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
TString SpdRsTB2GeoMapper::GetPassiveMaterialMDT() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("RsTBPassiveMaterialMDT");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
TString SpdRsTB2GeoMapper::GetActiveMaterialMDT() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("RsTBActiveMaterialMDT");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Double_t SpdRsTB2GeoMapper::GetModuleLength() const
{
   const SpdParameter* par = GetParameter("RsTBLen");
   Double_t len = 0;
   if (par) par->Value(len);
   return len;
}

//_____________________________________________________________________________
Double_t SpdRsTB2GeoMapper::GetModuleSize() const
{
   const SpdParameter* par = GetParameter("RsTBSize");
   Double_t size = 0;
   if (par) par->Value(size);
   return size;
}

//_____________________________________________________________________________
Double_t SpdRsTB2GeoMapper::GetModuleWidth() const
{
   const SpdParameter* par = GetParameter("RsTBWidth");
   Double_t width = 0;
   if (par) par->Value(width);
   return width;
}

//_____________________________________________________________________________
Int_t SpdRsTB2GeoMapper::GetNLayers() const
{
   const SpdParameter* par = GetParameter("RsTBNLayers");
   Int_t nlayers = 0;
   if (par) par->Value(nlayers);
   return nlayers;
}

//_____________________________________________________________________________
Double_t SpdRsTB2GeoMapper::GetModuleHmin() const
{
   const SpdParameter* par = GetParameter("RsTBHmin");
   Double_t h = 0;
   if (par) par->Value(h);
   return h;
}

//_____________________________________________________________________________
Double_t SpdRsTB2GeoMapper::GetModuleHmax() const
{
   const SpdParameter* par = GetParameter("RsTBHmax");
   Double_t h = 0;
   if (par) par->Value(h);
   return h;
}

//_____________________________________________________________________________
Double_t SpdRsTB2GeoMapper::GetModuleLmin() const
{
   const SpdParameter* par = GetParameter("RsTBLmin");
   Double_t l = 0;
   if (par) par->Value(l);
   return l;
}

//_____________________________________________________________________________
Double_t SpdRsTB2GeoMapper::GetModuleLmax() const
{
   const SpdParameter* par = GetParameter("RsTBLmax");
   Double_t l = 0;
   if (par) par->Value(l);
   return l;
}

//_____________________________________________________________________________
Int_t SpdRsTB2GeoMapper::GetNSectors() const
{
   Int_t nsec;
   const SpdParameter* par = GetParameter("RsTBNSectors");
   if (!par) return 0;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdRsTB2GeoMapper::GetSecAngle() const
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Bool_t SpdRsTB2GeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
   return (gtype > 0 && gtype < 3); 
}

// ****************************************************************************
// ********************* GETTERS (END) ****************************************
// ****************************************************************************

//_____________________________________________________________________________
Bool_t SpdRsTB2GeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdRsTB2GeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
 
   if (!fParams) return kFALSE;

   if (!CheckGeoType(gtype,"RsTBGeoType")) return kFALSE;

   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
     
   SpdParameter* par;
   
   // ATTENTION Number of sectors -> FIXED!
   const Int_t NUMBER_OF_SECTORS = 8;
   par = GetParameter("RsTBNSectors");
   if (par) { if (reinit) *par = NUMBER_OF_SECTORS; }
   else { fParams->Add(new SpdParameter("RsTBNSectors",NUMBER_OF_SECTORS)); } 
  
   par = GetParameter("RsTBBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetRsTBBaseMaterial(); }
   else { fParams->Add(new SpdParameter("RsTBBaseMaterial",mapper->GetRsTBBaseMaterial())); } 
          
   //cout << "-I- <SpdRsTB2GeoMapper::InitGeometry> reinit: " << reinit << endl;
      
   SetParameters(reinit);
  
   return kTRUE;
}

//_____________________________________________________________________________
void SpdRsTB2GeoMapper::SetParameters(Bool_t reinit) // private
{
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
  
   SpdParameter* par;
   
   /* define module size */

   par = GetParameter("RsTBLen");  
   if (par) { if (reinit) *par = mapper->GetRsTBLen2(); }
   else { fParams->Add(new SpdParameter("RsTBLen",mapper->GetRsTBLen2())); }
    
   Double_t size = mapper->GetRsTBSize2();
   par = GetParameter("RsTBSize");  
   if (par) { if (reinit) *par = size; }
   else { fParams->Add(new SpdParameter("RsTBSize",size)); }
    
   Double_t width = mapper->GetRsTBWidth2();
   par = GetParameter("RsTBWidth");  
   if (par) { if (reinit) *par = width; }
   else { fParams->Add(new SpdParameter("RsTBWidth",width)); } 
   
   /* define materials */
   
   TString material;
   
   material = "iron";
   par = GetParameter("RsTBAbsorbMaterial");
   if (par) { if (reinit) *par = material; }
   else { fParams->Add(new SpdParameter("RsTBAbsorbMaterial",material)); } 
   
   material = "aluminium";
   par = GetParameter("RsTBPassiveMaterialMDT");
   if (par) { if (reinit) *par = material; }
   else { fParams->Add(new SpdParameter("RsTBPassiveMaterialMDT",material)); } 
   
   material = "MDTMixture";
   par = GetParameter("RsTBActiveMaterialMDT");
   if (par) { if (reinit) *par = material; }
   else { fParams->Add(new SpdParameter("RsTBActiveMaterialMDT",material)); } 
      
   /* define secondary parameters */
  
   Double_t angle = 0.5*GetSecAngle()*DegToRad();
  
   Double_t hmin = size - width;
   par = GetParameter("RsTBHmin");  
   if (par) { *par = hmin; }
   else { fParams->Add(new SpdParameter("RsTBHmin",hmin,1)); }
   
   Double_t hmax = size;
   par = GetParameter("RsTBHmax");  
   if (par) { *par = hmax; }
   else { fParams->Add(new SpdParameter("RsTBHmax",hmax,1)); }
   
   Double_t lmin = 2.*hmin*Tan(angle); 
   par = GetParameter("RsTBHLmin");  
   if (par) { *par = lmin; }
   else { fParams->Add(new SpdParameter("RsTBLmin",lmin,2)); }

   Double_t lmax = 2.*hmax*Tan(angle);
   par = GetParameter("RsTBLmax");  
   if (par) { *par = lmax; }
   else { fParams->Add(new SpdParameter("RsTBLmax",lmax,2)); }
   
   //----------------------------------------------------------------
   // Number of layers calcluation within RS thickness:
   // (Thickness - top absorber - bottom absorber/mdt)/LayerThickness + 1(bottom layer)
   
   Int_t nLayers = Int_t((width - 6. - 9.5)/6.5) + 1; 
   par = GetParameter("RsTBNLayers");  
   if (par) { *par = nLayers; }
   else { fParams->Add(new SpdParameter("RsTBNLayers",nLayers,2)); }
}

//_____________________________________________________________________________
void SpdRsTB2GeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdRsTB2GeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




