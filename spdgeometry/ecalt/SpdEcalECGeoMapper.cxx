// $Id$
// Author: andre/artur   2020/07/31

//_____________________________________________________________________________
//
// SpdEcalECGeoMapper
//_____________________________________________________________________________

#include "SpdEcalECGeoMapper.h"
#include "SpdCommonGeoMapper.h"
#include <TString.h>
#include <TMath.h>
#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdEcalECGeoMapper)

SpdEcalECGeoMapper* SpdEcalECGeoMapper::fInstance = 0;

SpdEcalECGeoMapper* SpdEcalECGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdEcalECGeoMapper(); }

SpdEcalECGeoMapper* SpdEcalECGeoMapper::GetMapper() 
{ return fInstance; }

//_____________________________________________________________________________
SpdEcalECGeoMapper::SpdEcalECGeoMapper():SpdGeoMapper("EcalEC")
{
  if (fInstance) {
      Fatal("SpdEcalECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  fGeoVolPars = new TObjArray();  
  fGeoTable = new SpdGeoTable(); 
   
  fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalTECDefGeoType();
}

//_____________________________________________________________________________
SpdEcalECGeoMapper::SpdEcalECGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdEcalECGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  fGeoVolPars = new TObjArray();  
  fGeoTable = new SpdGeoTable(); 
   
  fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
   
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalTECDefGeoType();
}

//_____________________________________________________________________________
SpdEcalECGeoMapper::~SpdEcalECGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdEcalECGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdEcalECGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "air")       { fUnsetMedia = "airX";    } 
   else                            { fUnsetMedia = option;    }
}

//_____________________________________________________________________________
Bool_t SpdEcalECGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
   return (gtype > 0 && gtype < 3); 
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Values from CommonGeoMapper:
//
// EcalTECMinDist1 [cm]: fabs(Z) of the front face of endcap cell 
//                      (distance bewteen Z = 0 plane and endcap front face plane)
//
// Endcap is made in octagonal shape with a hole. It has two parameters:
//
// EcalTECSize [cm]: distance between center of endcap and the middle of octagon edge
// EcalTECWidth [cm]: distance between centers of edges of middle and outer octagon (for same φ)
// EcalTECThickness [cm]: thickness of endcap cell (first-level geometry)
//
// Some default values
//
// EcalECModuleClearance [cm]: clearance between modules (2x2 cells) (default: 0.1)
// EcalECCellClearance [cm]: clerance between cells in a module (default: 0.05)
// EcalECCellSize [cm]: X/Y size of endcap cell (default: 5.5)
// EcalECAbsorberThickness [cm]: thickness of absorber layer (material hardcoded as lead) (default: 0.05)
// EcalECScintThickness [cm]: thickness of scintillator layer (material hardcoded as polystyrene(?)) (default: 0.15)
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t SpdEcalECGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   //cout << "-I- <SpdEcalECGeoMapper::InitGeometry>  " << endl;
      
   if (fLockGeometry) {
       cout << "-E- <SpdEcalECGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
   
   if (!CheckGeoType(gtype,"EcalTECGeoType")) return kFALSE;
   
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par; 
    
   /* set BASE material */

   par = GetParameter("EcalECBaseMedium");
   if (par) { if (reinit) *par = mapper->GetEcalTECBaseMaterial(); }
   else fParams->Add(new SpdParameter("EcalECBaseMedium",mapper->GetEcalTECBaseMaterial())); 
   
   /* set BASE dimensions */

    par = GetParameter("EcalTECMinDist1");
    if (par) { if (reinit) *par = mapper->GetEcalTECMinDist1();  }
    else { fParams->Add(new SpdParameter("EcalTECMinDist1",mapper->GetEcalTECMinDist1())); }
    
    par = GetParameter("EcalTECSize");
    if (par) { if (reinit) *par = mapper->GetEcalTECSize();  }
    else { fParams->Add(new SpdParameter("EcalTECSize",mapper->GetEcalTECSize())); }
    
    par = GetParameter("EcalTECWidth");
    if (par) { if (reinit) *par = mapper->GetEcalTECWidth();  }
    else { fParams->Add(new SpdParameter("EcalTECWidth",mapper->GetEcalTECWidth())); }
    
    par = GetParameter("EcalTECThickness");
    if (par) { if (reinit) *par = mapper->GetEcalTECThickness();  }
    else { fParams->Add(new SpdParameter("EcalTECThickness",mapper->GetEcalTECThickness())); }
   
   //sanity check of parameters
   //barrelLength/2 should not be larger than distance to endcaps

   Double_t endcapSize(-1), endcapWidth(-2); 
   
   par = GetParameter("EcalTECSize");  
   if (par) par->Value(endcapSize);

   par = GetParameter("EcalTECWidth"); 
   if (par) par->Value(endcapWidth);
  
   if (endcapSize < 0 || endcapWidth < 0 || endcapSize < endcapWidth) {
       cout << "-F- <SpdEcalECGeoMapper::InitGeometry> "
            << "\n Cannot map ECAL geometry! ECAL endcap size < ECAL endcap width: doesn't make sense! " <<endl;
       exit(1);     
   }   
   
   /* define materials */
   TString material;
   
   material = "lead";
   par = GetParameter("EcalECAbsorberMedium");    
   if (par) { if (reinit) *par = material; }   
   else fParams->Add(new SpdParameter("EcalECAbsorberMedium",material)); 
            
   material = "FscScint";
   par = GetParameter("EcalECScintMedium");    
   if (par) { if (reinit) *par = material; }    
   else { fParams->Add(new SpdParameter("EcalECScintMedium",material)); }      
    
   /* define dimensions */

   par = GetParameter("EcalECModuleClearance");
   if (par) { if (reinit) *par = 0.1;}
   else { fParams->Add(new SpdParameter("EcalECModuleClearance", 0.1)); }
   
   par = GetParameter("EcalECCellClearance");
   if (par) { if (reinit) *par = 0.05; }
   else { fParams->Add(new SpdParameter("EcalECCellClearance", 0.05)); }
   
   par = GetParameter("EcalECCellSize");
   if (par) { if (reinit) *par = 4.0; }
   else { fParams->Add(new SpdParameter("EcalECCellSize", 4.0)); }
   
   par = GetParameter("EcalECAbsorberThickness");
   if (par) {  if (reinit) *par = 0.05; }
   else { fParams->Add(new SpdParameter("EcalECAbsorberThickness", 0.05)); }
   
   par = GetParameter("EcalECScintThickness");
   if (par) {  if (reinit) *par = 0.15; }
   else { fParams->Add(new SpdParameter("EcalECScintThickness", 0.15)); }
   
   par = GetParameter("EcalECModuleNLayers");
   if (par) { if (reinit) *par = 190; }
   else { fParams->Add(new SpdParameter("EcalECModuleNLayers", 190)); }
   
   par = GetParameter("EcalECScintVolName");
   if (par) { if (reinit) *par = "EcalECScintCell"; }
   else { fParams->Add(new SpdParameter("EcalECScintVolName", "EcalECScintCell")); }

   par = GetParameter("EcalECAbsorberVolName");
   if (par) { if (reinit) *par = "EcalECLeadCell"; }
   else { fParams->Add(new SpdParameter("EcalECAbsorberVolName", "EcalECLeadCell")); }
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdEcalECGeoMapper::SetCellSize(Double_t value) 
{
   if (value <= 0) {
      cout << "-F- <SpdEcalECGeoMapper::SetCellSize> cell size should be > 0" << endl;
      exit(1);  
   }
   
   if (value > SpdCommonGeoMapper::Instance()->GetEcalTECSize()) {
       cout << "-W- <SpdEcalECGeoMapper::SetCellSize> Cell size of " 
            << value << " cm is more than the endcap size (" 
            << SpdCommonGeoMapper::Instance()->GetEcalTECSize() 
            << " cm), decreasing cell size is recommended " << endl;
   }
   
   SpdParameter* par = GetParameter("EcalECCellSize");
   if (!par) fParams->Add(new SpdParameter("EcalECCellSize", value));
   else {
      cout << "-I- <SpdEcalECGeoMapper::SetCellSize> Overwriting parameter \"EcalECCellSize\" " << endl;  
      *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalECGeoMapper::SetAbsorberLayerThickness(Double_t value) 
{
   if (value <= 0) {
       cout << "-F- <SpdEcalECGeoMapper::SetAbsorberLayerThickness> Absorber layer thickness should be > 0" << endl;
       exit(1);    
   }
   
   if (value > SpdCommonGeoMapper::Instance()->GetEcalTECThickness()/2) {
       cout << "-W- <SpdEcalECGeoMapper::SetAbsorberLayerThickness> Absorber layer thickness of " 
            << value << " cm is more than half of the endcap thickness (" 
            << SpdCommonGeoMapper::Instance()->GetEcalTECThickness()/2 
            << " cm), decreasing layer width is recommended " << endl;
   }
    
   SpdParameter* par = GetParameter("EcalECAbsorberThickness");
   if (!par) fParams->Add(new SpdParameter("EcalECAbsorberThickness", value));
   else {
      cout << "-I- <SpdEcalECGeoMapper::SetAbsorberLayerThickness> Overwriting parameter \"EcalECAbsorberThickness\" " << endl;      
      *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalECGeoMapper::SetScintLayerThickness(Double_t value) 
{
   if (value <= 0) {   
       cout << "-F- <SpdEcalECGeoMapper::SetScintLayerThickness> Scintillator layer thickness should be > 0" << endl;
       exit(1);    
   }
   
   if (value > SpdCommonGeoMapper::Instance()->GetEcalTECThickness()/2) {
       cout << "-W- <SpdEcalECGeoMapper::SetScintLayerThickness> Scintillator layer thickness of " 
            << value << " cm is more than half of the endcap thickness (" 
            << SpdCommonGeoMapper::Instance()->GetEcalTECThickness()/2 
            << " cm), decreasing layer width is recommended " <<endl;
   }
   
   SpdParameter* par = GetParameter("EcalECScintThickness");
   if (!par) fParams->Add(new SpdParameter("EcalECScintThickness", value));
   else {
      cout << "-I- <SpdEcalECGeoMapper::SetModuleClearance> Overwriting parameter \"EcalECScintThickness\" " << endl;    
      *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalECGeoMapper::SetNLayers(Int_t value)
{
   if (value <= 0) {
       cout << "-F- <SpdEcalTEC2GeoMapper::SetNLayers> Number of layers should be > 0" << endl;
       exit(1);
   }
   
   SpdParameter* par = GetParameter("EcalECModuleNLayers");
   if (!par) fParams->Add(new SpdParameter("EcalECModuleNLayers", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalTEC2GeoMapper::SetNLayers> Overwriting parameter \"EcalECModuleNLayers\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalECGeoMapper::SetCellClearance(Double_t value) 
{
   if (value < 0) {
       cout << "-F- <SpdEcalECGeoMapper::SetModuleClearance> Cell clearance should be >= 0" << endl;
       exit(1);    
   }

   SpdParameter* par = GetParameter("EcalECCellClearance");
   if (!par) fParams->Add(new SpdParameter("EcalECCellClearance", value));
   else {
       cout << "-I- <SpdEcalECGeoMapper::SetModuleClearance> Overwriting parameter \"EcalECCellClearance\" " << endl; 
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalECGeoMapper::SetModuleClearance(Double_t value) 
{
   if (value < 0) {
       cout << "-F- <SpdEcalECGeoMapper::SetModuleClearance> Module clearance should be >= 0" << endl;
       exit(1);    
   }
   
   SpdParameter* par = GetParameter("EcalECModuleClearance");
   if (!par) fParams->Add(new SpdParameter("EcalECModuleClearance", value));
   else {
       cout << "-I- <SpdEcalECGeoMapper::SetModuleClearance> Overwriting parameter \"EcalECModuleClearance\" " << endl; 
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalECGeoMapper::SetNModulesX(Int_t value) 
{
   SpdParameter* par = GetParameter("EcalECNModulesX");
   if (!par) fParams->Add(new SpdParameter("EcalECNModulesX", value));
   else {
      *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalECGeoMapper::SetNModulesY(Int_t value) 
{
   SpdParameter* par = GetParameter("EcalECNModulesY");
   if (!par) fParams->Add(new SpdParameter("EcalECNModulesY", value));
   else {
      *par = value;
   }
}

//_____________________________________________________________________________
Double_t SpdEcalECGeoMapper::GetCellSize() const
{
   const SpdParameter* par = GetParameter("EcalECCellSize");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Int_t SpdEcalECGeoMapper::GetNLayers() const
{
   const SpdParameter* par = GetParameter("EcalECModuleNLayers");
   Int_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalECGeoMapper::GetAbsorberLayerThickness() const
{
   const SpdParameter* par = GetParameter("EcalECAbsorberThickness");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalECGeoMapper::GetScintLayerThickness() const
{
   const SpdParameter* par = GetParameter("EcalECScintThickness");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalECGeoMapper::GetCellClearance() const
{
   const SpdParameter* par = GetParameter("EcalECCellClearance");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalECGeoMapper::GetModuleClearance() const
{
   const SpdParameter* par = GetParameter("EcalECModuleClearance");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalECGeoMapper::GetEndcapMinDist() const
{
   const SpdParameter* par = GetParameter("EcalTECMinDist1");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalECGeoMapper::GetEndcapThickness() const
{
   const SpdParameter* par = GetParameter("EcalTECThickness");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalECGeoMapper::GetEndcapSize() const
{
   const SpdParameter* par = GetParameter("EcalTECSize");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalECGeoMapper::GetEndcapWidth() const
{
   const SpdParameter* par = GetParameter("EcalTECWidth");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
TString SpdEcalECGeoMapper::GetBaseMedium() const 
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("EcalECBaseMedium");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalECGeoMapper::GetAbsorberMedium() const 
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("EcalECAbsorberMedium");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalECGeoMapper::GetScintMedium() const 
{
    const SpdParameter* par = GetParameter("EcalECScintMedium");
    const Char_t* value = par->Value();
    return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalECGeoMapper::GetAbsorberVolName() const 
{
   const SpdParameter* par = GetParameter("EcalECAbsorberVolName");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalECGeoMapper::GetScintVolName() const 
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("EcalECScintVolName");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
void SpdEcalECGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdEcalECGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}














