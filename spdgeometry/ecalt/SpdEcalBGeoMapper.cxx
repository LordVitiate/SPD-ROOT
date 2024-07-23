// $Id$
// Author: andre/artur   2020/07/31

//_____________________________________________________________________________
//
// SpdEcalBGeoMapper
//_____________________________________________________________________________

#include "SpdEcalBGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdEcalBGeoMapper)

SpdEcalBGeoMapper* SpdEcalBGeoMapper::fInstance = 0;

SpdEcalBGeoMapper* SpdEcalBGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdEcalBGeoMapper(); }

SpdEcalBGeoMapper* SpdEcalBGeoMapper::GetMapper() 
{ return fInstance; }
    
//_____________________________________________________________________________
SpdEcalBGeoMapper::SpdEcalBGeoMapper():SpdGeoMapper("EcalB")
{
  if (fInstance) {
      Fatal("SpdEcalBGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  fGeoVolPars = new TObjArray();  
  fGeoTable = new SpdGeoTable(); 
   
  fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalTBDefGeoType();
}

//_____________________________________________________________________________
SpdEcalBGeoMapper::SpdEcalBGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdEcalBGeoMapper", "Singleton instance already exists.");
      return;
  }
  
  fInstance = this;
  
  fParams = new TObjArray();
  fGeoVolPars = new TObjArray();  
  fGeoTable = new SpdGeoTable(); 
   
  fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
  
  fGeoType = SpdCommonGeoMapper::Instance()->GetEcalTBDefGeoType();
}

//_____________________________________________________________________________
SpdEcalBGeoMapper::~SpdEcalBGeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdEcalBGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "air")       { fUnsetMedia = "airX";    } 
   else                            { fUnsetMedia = option;    }
}

//_____________________________________________________________________________
Bool_t SpdEcalBGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
   return (gtype > 0 && gtype < 3); 
}

//_____________________________________________________________________________
Bool_t SpdEcalBGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   //cout << "-I- <SpdEcalBGeoMapper::InitGeometry>  " << endl;
   
   if (fLockGeometry) {
       cout << "-E- <SpdEcalBGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
   if (fGeoTable) fGeoTable->Clear();
   if (fGeoVolPars) fGeoVolPars->Clear();
 
   if (!CheckGeoType(gtype,"EcalTBGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();


   SpdParameter* par;

   par = GetParameter("EcalBarrelNSectors");
   if (par) { if (reinit) *par = mapper->GetNGeoSectors(); }
   else fParams->Add(new SpdParameter("EcalBarrelNSectors", mapper->GetNGeoSectors()));

   par = GetParameter("EcalBarrelLength");
   if (par) { if (reinit) *par = mapper->GetEcalTBLen2(); }
   else fParams->Add(new SpdParameter("EcalBarrelLength", mapper->GetEcalTBLen2()));

   par = GetParameter("EcalBarrelOuterSize");
   if (par) { if (reinit) *par = mapper->GetEcalTBSize2(); }
   else fParams->Add(new SpdParameter("EcalBarrelOuterSize", mapper->GetEcalTBSize2()));

   par = GetParameter("EcalBarrelWidth");
   if (par) { if (reinit) *par = mapper->GetEcalTBWidth2(); }
   else fParams->Add(new SpdParameter("EcalBarrelWidth", mapper->GetEcalTBWidth2()));
 
   par = GetParameter("EcalTBWidth2");
   if (par) { if (reinit) *par = mapper->GetEcalTBWidth2(); }
   else fParams->Add(new SpdParameter("EcalTBWidth2", mapper->GetEcalTBWidth2()));

   //sanity check of parameters
   //barrelLength/2 should not be larger than distance to endcaps
   
   
   Int_t nsectors(-1);
   par = GetParameter("EcalBarrelNSectors"); 
   if (par) par->Value(nsectors);
   if (nsectors != -1 && nsectors < 3) {
       cout << "-F- <SpdEcalBGeoMapper::InitGeometry> " 
            << "\n\tCannot map ECAL geometry: nsectors should be >=3 (recommended >= 6)" << endl;
       exit(1);
   }
   
   
   par = GetParameter("EcalTBNBasketsZ");
   if (par) { if (reinit) *par = 2; }
   else { fParams->Add(new SpdParameter("EcalTBNBasketsZ", 2)); }
   
   par = GetParameter("EcalCellPhiInnerSize");
   if (par) { if (reinit) *par = 4.0; }
   else { fParams->Add(new SpdParameter("EcalCellPhiInnerSize", 4.0)); }

   par = GetParameter("EcalCellPhiOuterSize");
   if (par) { if (reinit) *par = 5.2; }
   else { fParams->Add(new SpdParameter("EcalCellPhiOuterSize", 5.2)); }

   par = GetParameter("EcalCellZSize");
   if (par) { if (reinit) *par = 4.0; }
   else { fParams->Add(new SpdParameter("EcalCellZSize", 4.0)); }
   
   par = GetParameter("EcalBasketZClearance");
   if (par) { if (reinit) *par = 0.5; }
   else { fParams->Add(new SpdParameter("EcalBasketZClearance", 0.5)); }

   par = GetParameter("EcalBasketPhiClearance");
   if (par) { if (reinit) *par = 0.5; }
   else { fParams->Add(new SpdParameter("EcalBasketPhiClearance", 0.5)); }

   par = GetParameter("SectorClearance");
   if (par) { if (reinit) *par = 0.0; }
   else { fParams->Add(new SpdParameter("SectorClearance", 0.0)); }
   
   par = GetParameter("EcalModuleClearance");
   if (par) { if (reinit) *par = 0.1; }
   else { fParams->Add(new SpdParameter("EcalModuleClearance", 0.1)); }
   
   par = GetParameter("EcalCellClearance");
   if (par) { if (reinit) *par = 0.05; }
   else { fParams->Add(new SpdParameter("EcalCellClearance", 0.05)); }
   
   par = GetParameter("EcalModuleNLayers");
   if (par) { if (reinit) *par = 190; }
   else { fParams->Add(new SpdParameter("EcalModuleNLayers", 190)); }
   
   par = GetParameter("EcalAbsorberThickness");
   if (par) { if (reinit) *par = 0.05; }
   else { fParams->Add(new SpdParameter("EcalAbsorberThickness", 0.05)); }
   
   par = GetParameter("EcalScintThickness");
   if (par) { if (reinit) *par = 0.15; }
   else { fParams->Add(new SpdParameter("EcalScintThickness", 0.15)); }
   
   par = GetParameter("EcalBarrelBaseMedium");
   if (par) { if (reinit) *par = "air"; }
   else { fParams->Add(new SpdParameter("EcalBarrelBaseMedium", "air")); }

   par = GetParameter("EcalBarrelAbsorberMedium");
   if (par) { if (reinit) *par = "lead"; }
   else { fParams->Add(new SpdParameter("EcalBarrelAbsorberMedium", "lead")); }
   
   par = GetParameter("EcalBarrelScintMedium");
   if (par) { if (reinit) *par = "FscScint"; }
   else { fParams->Add(new SpdParameter("EcalBarrelScintMedium", "FscScint")); }
   
   par = GetParameter("EcalBarrelScintVolName");
   if (par) { if (reinit) *par = "EcalBScintCell"; }
   else { fParams->Add(new SpdParameter("EcalBarrelScintVolName", "EcalBScintCell")); }

   par = GetParameter("EcalBarrelAbsorberVolName");
   if (par) { if (reinit) *par = "EcalBLeadCell"; }
   else { fParams->Add(new SpdParameter("EcalBarrelAbsorberVolName", "EcalBLeadCell")); }

   par = GetParameter("EcalBarrelUseZBasketGaps");
   if (par) { if (reinit) *par = false; }
   else { fParams->Add(new SpdParameter("EcalBarrelUseZBasketGaps", false)); }
    
   par = GetParameter("EcalBarrelZBasketPlugMedium");
   if (par) { if (reinit) *par = "carbon"; }
   else { fParams->Add(new SpdParameter("EcalBarrelZBasketPlugMedium", "carbon")); }
   

   return kTRUE;
}


/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 ++++++++++++++++++++++++++++++++ SETTERS ++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/ 

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetNZBaskets(Int_t value) 
{
   SpdParameter* par = GetParameter("EcalTBNBasketsZ");
   if (!par) fParams->Add(new SpdParameter("EcalTBNBasketsZ", value)); 
   else {
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetNLayers(Int_t value)
{
   if (value <= 0) {
       cout << "-F- <SpdEcalBGeoMapper::SetNLayers> Number of layers should be > 0" << endl;
       exit(1);
   }
   
   SpdParameter* par = GetParameter("EcalModuleNLayers");
   if (!par) fParams->Add(new SpdParameter("EcalModuleNLayers", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalBGeoMapper::SetNLayers> Overwriting parameter \"EcalModuleNLayers\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetCellZSize(Double_t value)
{
   if (value <= 0) {
       cout << "-F- <SpdEcalBGeoMapper::SetCellZSize> Cell z size should be > 0" << endl;
       exit(1);
   }
   
   if (value > SpdCommonGeoMapper::Instance()->GetEcalTBLen2()) {
       cout << "-W- <SpdEcalBGeoMapper::SetCellSize> "
            << "\n\tcell z size is more than barrel length, "
            << "\n\trecommended decreasing cell z size " << endl;
   }
   
   SpdParameter* par = GetParameter("EcalCellZSize");
   if (!par) fParams->Add(new SpdParameter("EcalCellZSize", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalBGeoMapper::SetCellZSize> Overwriting parameter \"EcalCellZSize\"" << endl; 
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetCellInnerPhiSize(Double_t value)
{
   if (value <= 0) {
        cout << "-F- <SpdEcalBGeoMapper::SetCellInnerPhiSize> Cell phi size should be > 0" << endl;
        exit(1);
   }
   
   if (value > SpdCommonGeoMapper::Instance()->GetEcalTBSize2()) {
       cout << "-W- <SpdEcalBGeoMapper::SetCellInnerPhiSize> "
            << "\n\tCell phi inner size is more than barrel outer radius, "
            << "\n\trecommended decreasing cell phi inner size " << endl;
   }

   SpdParameter* par = GetParameter("EcalCellPhiInnerSize");
   if (!par) fParams->Add(new SpdParameter("EcalCellPhiInnerSize", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalBGeoMapper::SetCellInnerPhiSize> Overwriting parameter \"EcalCellPhiInnerSize\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetAbsorberLayerThickness(Double_t value)
{
   if (value <= 0) {
       cout << "-F- <SpdEcalBGeoMapper::SetAbsorberLayerThickness> Absorber layer thickness should be > 0" << endl;
       exit(1);
   }
   
   SpdParameter* par = GetParameter("EcalAbsorberThickness");
   if (!par) fParams->Add(new SpdParameter("EcalAbsorberThickness", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalBGeoMapper::SetAbsorberLayerThickness> Overwriting parameter \"EcalAbsorberThickness\"" << endl;
   }
   
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetScintLayerThickness(Double_t value)
{
   if (value <= 0) {
       cout << "-F- <SpdEcalBGeoMapper::SetScintLayerThickness> Scintillator layer thickness should be > 0" << endl;
       exit(1);
   }
   
   SpdParameter* par = GetParameter("EcalScintThickness");
   if (!par) fParams->Add(new SpdParameter("EcalScintThickness", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalBGeoMapper::SetScintLayerThickness> Overwriting parameter \"EcalScintThickness\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetCellClearance(Double_t value) 
{
   if (value < 0) {
       cout << "-F- <SpdEcalBGeoMapper::SetCellClearance> Cell clearance should be >= 0" << endl;
       exit(1);
   }

   SpdParameter* par = GetParameter("EcalCellClearance");
   if (!par) fParams->Add(new SpdParameter("EcalCellClearance",value));
   else {
       *par = value;
       cout << "-W- <SpdEcalBGeoMapper::SetCellClearance> Overwriting parameter \"EcalCellClearance\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetModuleClearance(Double_t value) 
{
   if (value < 0) {
       cout << "-F- <SpdEcalBGeoMapper::SetModuleClearance> Module clearance should be >= 0" << endl;
       exit(1);
   }
   
   SpdParameter* par = GetParameter("EcalModuleClearance");
   if (!par) fParams->Add(new SpdParameter("EcalModuleClearance",value));
   else {
       *par = value;
       cout << "-W- <SpdEcalBGeoMapper::SetModuleClearance> Overwriting parameter \"EcalModuleClearance\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetBasketZClearance(Double_t value)
{
   SpdParameter* par = GetParameter("EcalBasketZClearance");
   if (!par) fParams->Add(new SpdParameter("EcalBasketZClearance", value)); 
   else {
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetBasketPhiClearance(Double_t value)
{
   SpdParameter* par = GetParameter("EcalBasketPhiClearance");
   if (!par) fParams->Add(new SpdParameter("EcalBasketPhiClearance", value)); 
   else {
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetUseZBasketGaps(Bool_t value)
{
   SpdParameter* par = GetParameter("EcalBarrelUseZBasketGaps");
   if (!par) fParams->Add(new SpdParameter("EcalBarrelUseZBasketGaps", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalBGeoMapper::SetUseZBasketGaps> Overwriting parameter \"EcalBarrelUseZBasketGaps\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetZBasketPlugMedium(TString value)
{
   SpdParameter* par = GetParameter("EcalBarrelZBasketPlugMedium");
   if (!par) fParams->Add(new SpdParameter("EcalBarrelZBasketPlugMedium", value)); 
   else {
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetCalcCellZSize(Double_t value)
{
   SpdParameter* par = GetParameter("EcalCalcCellZSize");
   if (!par) fParams->Add(new SpdParameter("EcalCalcCellZSize", value)); 
   else {
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetCalcCellInnerPhiSize(Double_t value)
{
   SpdParameter* par = GetParameter("EcalCalcCellPhiInnerSize");
   if (!par) fParams->Add(new SpdParameter("EcalCalcCellPhiInnerSize", value)); 
   else {
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetCalcCellOuterPhiSize(Double_t value)
{
   SpdParameter* par = GetParameter("EcalCalcCellPhiOuterSize");
   if (!par) fParams->Add(new SpdParameter("EcalCalcCellPhiOuterSize", value)); 
   else {
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetBasketNModulesZ(Int_t value)
{
   SpdParameter* par = GetParameter("EcalBarrelBasketNModulesZ");
   if (!par) fParams->Add(new SpdParameter("EcalBarrelBasketNModulesZ", value)); 
   else {
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalBGeoMapper::SetBasketNModulesPhi(Int_t value)
{
   SpdParameter* par = GetParameter("EcalBarrelBasketNModulesPhi");
   if (!par) fParams->Add(new SpdParameter("EcalBarrelBasketNModulesPhi", value)); 
   else {
       *par = value;
   }
}


/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 ++++++++++++++++++++++++++++++++ GETTERS ++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/ 

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetBarrelLength() const
{
   const SpdParameter* par = GetParameter("EcalBarrelLength");
   Double_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetBarrelWidth() const
{
   const SpdParameter* par = GetParameter("EcalBarrelWidth");
   Double_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetBarrelOuterSize() const
{
   const SpdParameter* par = GetParameter("EcalBarrelOuterSize");
   Double_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetBarrelRadius() const
{
   return GetBarrelOuterSize() - GetBarrelWidth();
}

//_____________________________________________________________________________
Int_t SpdEcalBGeoMapper::GetNSectors() const
{
   const SpdParameter* par = GetParameter("EcalBarrelNSectors");
   Int_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetBarrelModuleRadialSize() const
{
   const SpdParameter* par = GetParameter("EcalTBWidth2");
   Double_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Int_t SpdEcalBGeoMapper::GetNZBaskets() const 
{
   const SpdParameter* par = GetParameter("EcalTBNBasketsZ");
   Int_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Int_t SpdEcalBGeoMapper::GetNLayers() const
{
   const SpdParameter* par = GetParameter("EcalModuleNLayers");
   Int_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetCellZSize() const
{
   const SpdParameter* par = GetParameter("EcalCalcCellZSize");
   Double_t val = 0;
   if (par) par->Value(val);
   else {
       const SpdParameter* par2 = GetParameter("EcalCellZSize");
       if (par2) par2->Value(val);
   }
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetCellInnerSizePhi() const
{
   const SpdParameter* par = GetParameter("EcalCalcCellPhiInnerSize");
   Double_t val = 0;
   if (par) par->Value(val);
   else {
       const SpdParameter* par2 = GetParameter("EcalCellPhiInnerSize");
       if (par2) par2->Value(val);
   }
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetCellOuterSizePhi() const
{
   const SpdParameter* par = GetParameter("EcalCalcCellPhiOuterSize");
   Double_t val = 0;
   if (par) par->Value(val);
   else {
       const SpdParameter* par2 = GetParameter("EcalCellPhiOuterSize");
       if (par2) par2->Value(val);
   }
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetAbsorberLayerThickness() const
{
   const SpdParameter* par = GetParameter("EcalAbsorberThickness");
   Double_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetScintLayerThickness() const
{
   const SpdParameter* par = GetParameter("EcalScintThickness");
   Double_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetCellClearance() const
{
   const SpdParameter* par = GetParameter("EcalCellClearance");
   Double_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetModuleClearance() const
{
   const SpdParameter* par = GetParameter("EcalModuleClearance");
   Double_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetBasketZClearance() const
{
   const SpdParameter* par = GetParameter("EcalBasketZClearance");
   Double_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetBasketPhiClearance() const
{
   const SpdParameter* par = GetParameter("EcalBasketPhiClearance");
   Double_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Double_t SpdEcalBGeoMapper::GetSectorClearance() const
{
   const SpdParameter* par = GetParameter("SectorClearance");
   Double_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
Bool_t SpdEcalBGeoMapper::IsUsingZBasketGapsPlugs() const 
{
   const SpdParameter* par = GetParameter("EcalBarrelUseZBasketGaps");
   Bool_t val = 0;
   if (par) par->Value(val);
   return val;
}

//_____________________________________________________________________________
TString SpdEcalBGeoMapper::GetBaseMedium() const 
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;  
   const SpdParameter* par = GetParameter("EcalBarrelBaseMedium");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalBGeoMapper::GetAbsorberMedium() const 
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;  
   const SpdParameter* par = GetParameter("EcalBarrelAbsorberMedium");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalBGeoMapper::GetScintMedium() const 
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("EcalBarrelScintMedium");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalBGeoMapper::GetZBasketPlugMedium() const 
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("EcalBarrelZBasketPlugMedium");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalBGeoMapper::GetAbsorberVolName() const 
{
   const SpdParameter* par = GetParameter("EcalBarrelAbsorberVolName");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalBGeoMapper::GetScintVolName() const 
{
   const SpdParameter* par = GetParameter("EcalBarrelScintVolName");
   const Char_t* value = par->Value();
   return TString(value);
}

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 ++++++++++++++++++++++++++++++++ GETTERS (END) ++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/ 

//_____________________________________________________________________________
void SpdEcalBGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdEcalBGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}




