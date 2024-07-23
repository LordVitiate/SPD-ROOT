// $Id$
// Author: andre/artur   2020/07/31

//_____________________________________________________________________________
//
// SpdEcalTB2GeoMapper
//_____________________________________________________________________________

#include "SpdEcalTB2GeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdEcalTB2GeoMapper)

SpdEcalTB2GeoMapper* SpdEcalTB2GeoMapper::fInstance = 0;

SpdEcalTB2GeoMapper* SpdEcalTB2GeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdEcalTB2GeoMapper(); }

SpdEcalTB2GeoMapper* SpdEcalTB2GeoMapper::GetMapper() 
{ return fInstance; }
    
//_____________________________________________________________________________
SpdEcalTB2GeoMapper::SpdEcalTB2GeoMapper():SpdGeoMapper("EcalB")
{
  if (fInstance) {
      Fatal("SpdEcalTB2GeoMapper", "Singleton instance already exists.");
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
SpdEcalTB2GeoMapper::SpdEcalTB2GeoMapper(TString prefix):SpdGeoMapper(prefix)
{
  if (fInstance) {
      Fatal("SpdEcalTB2GeoMapper", "Singleton instance already exists.");
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
SpdEcalTB2GeoMapper::~SpdEcalTB2GeoMapper() 
{
 
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdEcalTB2GeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
   
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "air")       { fUnsetMedia = "airX";    } 
   else                            { fUnsetMedia = option;    }
}

//_____________________________________________________________________________
Bool_t SpdEcalTB2GeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
   return (gtype > 0 && gtype < 3); 
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::AddParameterFromCommon(TString parname, Int_t accessFunc(SpdCommonGeoMapper*), Bool_t reinit) 
{
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   SpdParameter* par = GetParameter(parname);
   if (par) { if (reinit) *par = accessFunc(mapper); }
   else fParams->Add(new SpdParameter(parname, accessFunc(mapper)));
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::AddParameterFromCommon(TString parname, Double_t accessFunc(SpdCommonGeoMapper*), Bool_t reinit) 
{
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   SpdParameter* par = GetParameter(parname);
   if (par) { if (reinit) *par = accessFunc(mapper); }
   else fParams->Add(new SpdParameter(parname, accessFunc(mapper)));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Values from CommonGeoMapper:
//
// EcalBarrelNSectors: number of sectors in barrel
// EcalBarrelLength [cm]: length of barrel part
// SectorClearance  [cm]: minimum distance between modules of different sectors
// EcalBarrelRadius [cm]: shortest distance from Z axis to ECAL barrel basket
// EcalTBWidth2     [cm]: (meaningful only for EcalBarrelForceCellSize == false) the radial size of barrel basket. 
//                        All modules are contained within the basket. Size and shape of modules is varied accordingly
//
// Some default values:
//
// EcalTBNBasketsZ: number of baskets in barrel in Z direction [default: 2]
//
// EcalBarrelForceCellSize (bool): 
//   - if TRUE, cell sizes are forced to be as defined with 
//           EcalCellPhiInnerSize/EcalCellPhiOuterSize/EcalCellThetaInnerSize/EcalCellThetaOuterSize; 
//   - if FALSE, cell sizes are calculated automatically to 
//        1) minimize empty spaces between modules, and 
//        2) to be as close to the defined variables 
//           EcalCellPhiInnerSize/EcalCellPhiOuterSize/EcalCellThetaInnerSize/EcalCellThetaOuterSize as possible
//
// EcalBarrelTrimModuleLength (bool): 
//   - if TRUE, the module length is calculated so that the module is fitted inside the basket;
//   - if FALSE, the module size is fixed and defined by EcalModuleNLayers
//
// EcalCellPhiInnerSize [cm]: φ-size of inner size of barrel cell 
//     (if EcalBarrelForceCellSize is false, this defines rough dimensions) [default: 5.5]
//
// EcalCellPhiOuterSize [cm]: φ-size of outer size of barrel cell 
//     (if EcalBarrelForceCellSize is false, chosen automatically to minimize empty space) [default: 7.0]
//
// EcalCellThetaInnerSize [cm]: θ(Z)-size of inner size of barrel cell 
//     (if EcalBarrelForceCellSize is false, this defines rough dimensions) [default: 5.5]
//
// EcalCellThetaOuterSize [cm]: θ(Z)-size of outer size of barrel cell 
//     (if EcalBarrelForceCellSize is false, this defines rough dimensions) [default: 5.5]
//
// EcalBasketClearance [cm]: minimum distance between modules of different Z-baskets [default: 0.5]
//
// EcalModuleClearance [cm]: clearance between modules (2x2 cells) [default: 0.1]
//
// EcalCellClearance   [cm]: clerance between cells in a module [default: 0.05]
//
// EcalModuleNLayers: (meaningful only for EcalBarrelForceCellSize == true or EcalBarrelTrimModuleLength == FALSE) 
//     number of layers in a module (same for barrel and endcap parts), [default: 200]. 
//     In case of EcalBarrelForceCellSize == FALSE and EcalBarrelTrimModuleLength == TRUE, for each module, 
//     maximum number of layers is calculated, for which the module stays within the basket.
//
// EcalAbsorberThickness [cm]: thickness of absorber layer (material hardcoded as lead) [default: 0.05]
//
// EcalScintThickness    [cm]: thickness of scintillator layer (material hardcoded as polystyrene(?)) [default: 0.15]
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t SpdEcalTB2GeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   //cout << "-I- <SpdEcalTB2GeoMapper::InitGeometry>  " << endl;
   
   if (fLockGeometry) {
       cout << "-E- <SpdEcalTB2GeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
  
   if (!fParams) return kFALSE;
   if (fGeoTable) fGeoTable->Clear();
   if (fGeoVolPars) fGeoVolPars->Clear();
 
   if (!CheckGeoType(gtype,"EcalTBGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
 
   AddParameterFromCommon("EcalBarrelNSectors", [](SpdCommonGeoMapper* m) {return m->GetNGeoSectors();}, reinit);
//   AddParameterFromCommon("EcalBarrelLength", [](SpdCommonGeoMapper* m) {return m->GetEcalTBLen2();}, reinit);
//   AddParameterFromCommon("EcalBarrelOuterSize", [](SpdCommonGeoMapper* m) {return m->GetEcalTBSize2();}, reinit);
//   AddParameterFromCommon("EcalBarrelWidth", [](SpdCommonGeoMapper* m) {return m->GetEcalTBWidth2();}, reinit);
//   AddParameterFromCommon("EcalTBWidth2", [](SpdCommonGeoMapper* m) {return m->GetEcalTBWidth2();}, reinit);
   AddParameterFromCommon("SectorClearance", [](SpdCommonGeoMapper* m) {return m->GetSectorClearance();}, reinit);

   SpdParameter* par;

   par = GetParameter("EcalBarrelLength");
   if (par) { if (reinit) *par = 353.6; }
   else fParams->Add(new SpdParameter("EcalBarrelLength", 353.6));

   par = GetParameter("EcalBarrelOuterSize");
   if (par) { if (reinit) *par = 158.0; }
   else fParams->Add(new SpdParameter("EcalBarrelOuterSize", 158.0));

   par = GetParameter("EcalBarrelWidth");
   if (par) { if (reinit) *par = 50.0; }
   else fParams->Add(new SpdParameter("EcalBarrelWidth", 50.0));
 
   par = GetParameter("EcalTBWidth2");
   if (par) { if (reinit) *par = 50.0; }
   else fParams->Add(new SpdParameter("EcalTBWidth2", 50.0));
   
   //sanity check of parameters
   //barrelLength/2 should not be larger than distance to endcaps
   
   
   Int_t nsectors(-1);
   par = GetParameter("EcalBarrelNSectors"); 
   if (par) par->Value(nsectors);
   if (nsectors != -1 && nsectors < 3) {
       cout << "-F- <SpdEcalTB2GeoMapper::InitGeometry> " 
            << "\n\tCannot map ECAL geometry: nsectors should be >=3 (recommended >= 6)" << endl;
       exit(1);
   }
   
   Double_t thetaInnerSize(-1.), thetaOuterSize(-1.);
   Bool_t forceSize(false);
   
   par = GetParameter("EcalCellThetaInnerSize");  if (par) par->Value(thetaInnerSize);
   par = GetParameter("EcalCellThetaOuterSize");  if (par) par->Value(thetaOuterSize);
   par = GetParameter("EcalBarrelForceCellSize"); if (par) par->Value(forceSize);
   
   if (thetaInnerSize != -1. && thetaOuterSize != -1. && par && thetaInnerSize != thetaOuterSize && forceSize) {
       cout << "-F- <SpdEcalTB2GeoMapper::InitGeometry> "
            << "\n\tCannot map ECAL geometry: theta(Z) inner size != theta(Z) outer size (while forced size): "
            << "\n\tprojective geometry support is not implemented yet!" << endl;
       exit(1);
   }
   
   if (thetaInnerSize != -1. && thetaOuterSize != -1. && thetaInnerSize != thetaOuterSize) {
       cout << "-W- <SpdEcalTB2GeoMapper::InitGeometry> "
            << "\n\ttheta(Z) inner size != theta(Z) outer size (size not forced): "
            << "\n\tas a guideline for theta (Z) size is chosen as inner size = " << thetaInnerSize << " cm" << endl;
   }
   
   Double_t phiInnerSize(-1.), phiOuterSize(-1.);
   
   par = GetParameter("EcalCellPhiInnerSize");    if (par) par->Value(phiInnerSize);
   par = GetParameter("EcalCellPhiOuterSize");    if (par) par->Value(phiOuterSize);
   par = GetParameter("EcalBarrelForceCellSize"); 
   
   if (phiInnerSize != -1. && phiOuterSize != -1. && par && phiInnerSize > phiOuterSize && forceSize) {
       cout << "-F- <SpdEcalTB2GeoMapper::InitGeometry> "
            << "\n\tCannot map ECAL geometry: phi inner size > phi outer size (while forcing size): "
            << "\n\tthis gives undefined behaviour and is not recommended" << endl;
       exit(1);
   }
   
   SpdParameter* par2;
   
   Int_t nLayers(-1);
   Bool_t trimModule(false);
   
   par  = GetParameter("EcalModuleNLayers");          if (par)  par->Value(nLayers);
   par  = GetParameter("EcalBarrelForceCellSize");  
   par2 = GetParameter("EcalBarrelTrimModuleLength"); if (par2) par2->Value(trimModule);
   
   if (nLayers != -1 && par && par2 && !forceSize && trimModule) {
       cout << "-W- <SpdEcalTB2GeoMapper::InitGeometry> Automatical selection of cell size is chosen "
            << "\n\t(EcalBarrelForceCellSize = false && EcalBarrelTrimModuleLength == true): "
            << "\n\tnumber of layers in each module will be calculated automatically! "
            << "\n\t(setting EcalModuleNLayers is meaningless) " << endl;
   }
   
   if (nLayers != -1 && par && !par2 && !forceSize) {
       cout << "-W- <SpdEcalTB2GeoMapper::InitGeometry> Automatical selection of cell size is chosen "
            << "\n\t(EcalBarrelForceCellSize = false && EcalBarrelTrimModuleLength == true (by default)): "
            << "\n\tnumber of layers in each module will be calculated automatically! "
            << "\n\t(setting EcalModuleNLayers is meaningless) " << endl;
   }
   
   if (nLayers != -1 && !par && !par2) {
       cout << "-W- <SpdEcalTB2GeoMapper::InitGeometry> Automatical selection of cell size is chosen "
            << "\n\t(EcalBarrelForceCellSize = false (by default) && EcalBarrelTrimModuleLength == true (by default)): "
            << "\n\tnumber of layers in each module will be calculated automatically! "
            << "\n\t(setting EcalModuleNLayers is meaningless) " << endl;
   }
   
   if (nLayers != -1 && !par && par2 && trimModule) {
       cout << "-W- <SpdEcalTB2GeoMapper::InitGeometry> Automatical selection of cell size is chosen "
            << "\n\t(EcalBarrelForceCellSize = false (by default) && EcalBarrelTrimModuleLength == true): "
            << "\n\tnumber of layers in each module will be calculated automatically! "
            << "\n\t(setting EcalModuleNLayers is meaningless) " << endl;
   }
   
   Double_t moduleLength(-1.);
   
   par = GetParameter("EcalTBWidth2");               if (par) par->Value(moduleLength);
   par = GetParameter("EcalBarrelTrimModuleLength"); 
   
   if (par && moduleLength != -1. && moduleLength != mapper->GetEcalTBWidth2() && !trimModule) {
       cout << "-W- <SpdEcalTB2GeoMapper::InitGeometry> : manual selection of cell length is chosen "    
            << "\n\t(EcalBarrelTrimModuleLength = false): cell length is calculated using "
            << "\n\tEcalModuleNLayers/EcalAbsorberThickness/EcalScintThickness "
            << "\n\t(changing EcalTBWidth2 WILL NOT change simulation) " << endl;
   }
   if (!par && moduleLength != -1. && moduleLength != mapper->GetEcalTBWidth2()) {
        cout << "-W- <SpdEcalTB2GeoMapper::InitGeometry> : manual selection of cell length is chosen "
             << "\n\t(EcalBarrelTrimModuleLength = false): cell length is calculated using "
             << "\n\tEcalModuleNLayers/EcalAbsorberThickness/EcalScintThickness "
             << "\n\t(changing EcalTBWidth2 WILL NOT change simulation) " << endl;
   }
   
   par = GetParameter("EcalTBNBasketsZ");
   if (par) { if (reinit) *par = 2; }
   else { fParams->Add(new SpdParameter("EcalTBNBasketsZ", 2)); }
   
   par = GetParameter("EcalCellPhiInnerSize");
   if (par) { if (reinit) *par = 5.5; }
   else { fParams->Add(new SpdParameter("EcalCellPhiInnerSize", 5.5)); }
   
   par = GetParameter("EcalCellPhiOuterSize");
   if (par) { if (reinit) *par = 7.0; }
   else { fParams->Add(new SpdParameter("EcalCellPhiOuterSize", 7.0)); }
   
   par = GetParameter("EcalCellThetaInnerSize");
   if (par) { if (reinit) *par = 5.5; }
   else { fParams->Add(new SpdParameter("EcalCellThetaInnerSize", 5.5)); }
   
   par = GetParameter("EcalCellThetaOuterSize");
   if (par) { if (reinit) *par = 5.5; }
   else { fParams->Add(new SpdParameter("EcalCellThetaOuterSize", 5.5)); }
   
   par = GetParameter("EcalBasketClearance");
   if (par) { if (reinit) *par = 0.5; }
   else { fParams->Add(new SpdParameter("EcalBasketClearance", 0.5)); }
   
   par = GetParameter("EcalModuleClearance");
   if (par) { if (reinit) *par = 0.1; }
   else { fParams->Add(new SpdParameter("EcalModuleClearance", 0.1)); }
   
   par = GetParameter("EcalCellClearance");
   if (par) { if (reinit) *par = 0.05; }
   else { fParams->Add(new SpdParameter("EcalCellClearance", 0.05)); }
   
   par = GetParameter("EcalBarrelMaxTheta");
   if (par) { if (reinit) *par = 0.0; }
   else { fParams->Add(new SpdParameter("EcalBarrelMaxTheta", 0.0)); }
   
   par = GetParameter("EcalModuleNLayers");
   if (par) { if (reinit) *par = 200; }
   else { fParams->Add(new SpdParameter("EcalModuleNLayers", 200)); }
   
   par = GetParameter("EcalAbsorberThickness");
   if (par) { if (reinit) *par = 0.05; }
   else { fParams->Add(new SpdParameter("EcalAbsorberThickness", 0.05)); }
   
   par = GetParameter("EcalScintThickness");
   if (par) { if (reinit) *par = 0.15; }
   else { fParams->Add(new SpdParameter("EcalScintThickness", 0.15)); }
   
   par = GetParameter("EcalBarrelForceCellSize");
   if (par) { if (reinit) *par = false; }
   else { fParams->Add(new SpdParameter("EcalBarrelForceCellSize", false)); }
   
   par = GetParameter("EcalBarrelTrimModuleLength");
   if (par) { if (reinit) *par = false; }
   else { fParams->Add(new SpdParameter("EcalBarrelTrimModuleLength", false)); }
   
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

   return kTRUE;
}

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 ++++++++++++++++++++++++++++++++ GETTERS ++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/ 

//_____________________________________________________________________________
TString SpdEcalTB2GeoMapper::GetBaseMedium() const 
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;  
   const SpdParameter* par = GetParameter("EcalBarrelBaseMedium");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalTB2GeoMapper::GetAbsorberMedium() const 
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;  
   const SpdParameter* par = GetParameter("EcalBarrelAbsorberMedium");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalTB2GeoMapper::GetScintMedium() const 
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia; 
   const SpdParameter* par = GetParameter("EcalBarrelScintMedium");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalTB2GeoMapper::GetScintVolName() const 
{
   const SpdParameter* par = GetParameter("EcalBarrelScintVolName");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
TString SpdEcalTB2GeoMapper::GetAbsorberVolName() const 
{
   const SpdParameter* par = GetParameter("EcalBarrelAbsorberVolName");
   const Char_t* value = par->Value();
   return TString(value);
}

//_____________________________________________________________________________
Bool_t SpdEcalTB2GeoMapper::IsForceCellSize() const 
{
   const SpdParameter* par = GetParameter("EcalBarrelForceCellSize");
   Bool_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Bool_t SpdEcalTB2GeoMapper::IsTrimModuleLength() const 
{
   const SpdParameter* par = GetParameter("EcalBarrelTrimModuleLength");
   Bool_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Int_t SpdEcalTB2GeoMapper::GetNLayers() const
{
   const SpdParameter* par = GetParameter("EcalModuleNLayers");
   Int_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetBarrelModuleRadialSize() const
{
   const SpdParameter* par = GetParameter("EcalTBWidth2");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetBarrelWidth() const
{
   const SpdParameter* par = GetParameter("EcalBarrelWidth");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetBarrelOuterSize() const
{
   const SpdParameter* par = GetParameter("EcalBarrelOuterSize");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}


//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetLayerInnerSizeTheta() const
{
   const SpdParameter* par = GetParameter("EcalCellThetaInnerSize");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetLayerInnerSizePhi() const
{
   const SpdParameter* par = GetParameter("EcalCellPhiInnerSize");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetLayerOuterSizeTheta() const
{
   const SpdParameter* par = GetParameter("EcalCellThetaOuterSize");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetLayerOuterSizePhi() const
{
   const SpdParameter* par = GetParameter("EcalCellPhiOuterSize");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetLayer1SizeZ() const
{
   const SpdParameter* par = GetParameter("EcalAbsorberThickness");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetLayer2SizeZ() const
{
   const SpdParameter* par = GetParameter("EcalScintThickness");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetBarrelLength() const
{
   const SpdParameter* par = GetParameter("EcalBarrelLength");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetBarrelMaxTheta() const
{
   const SpdParameter* par = GetParameter("EcalBarrelMaxTheta");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetBasketClearance() const
{
   const SpdParameter* par = GetParameter("EcalBasketClearance");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetCellClearance() const
{
   const SpdParameter* par = GetParameter("EcalCellClearance");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetModuleClearance() const
{
   const SpdParameter* par = GetParameter("EcalModuleClearance");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdEcalTB2GeoMapper::GetSectorClearance() const
{
   const SpdParameter* par = GetParameter("SectorClearance");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Int_t SpdEcalTB2GeoMapper::GetNSectors() const
{
   const SpdParameter* par = GetParameter("EcalBarrelNSectors");
   Int_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 ++++++++++++++++++++++++++++++++ SETTERS ++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/ 

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetForceInsideBasket(Bool_t value)
{
   SpdParameter* par = GetParameter("EcalBarrelTrimModuleLength");
   if (!par) fParams->Add(new SpdParameter("EcalBarrelTrimModuleLength",value)); 
   else {
       *par = value; 
       cout << "-W- <SpdEcalTB2GeoMapper::SetForceInsideBasket> Overwriting parameter \"EcalBarrelTrimModuleLength\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetBasketNModulesZ(Int_t value)
{
   SpdParameter* par = GetParameter("EcalBarrelBasketNModulesZ");
   if (!par) fParams->Add(new SpdParameter("EcalBarrelBasketNModulesZ", value)); 
   else {
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetBasketNModulesPhi(Int_t value)
{
   SpdParameter* par = GetParameter("EcalBarrelBasketNModulesPhi");
   if (!par) fParams->Add(new SpdParameter("EcalBarrelBasketNModulesPhi", value)); 
   else {
       *par = value;
   }
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetForceCellSize(Bool_t value)
{
   SpdParameter* par = GetParameter("EcalBarrelForceCellSize");
   if (!par) fParams->Add(new SpdParameter("EcalBarrelForceCellSize", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalTB2GeoMapper::SetForceCellSize> Overwriting parameter \"EcalBarrelForceCellSize\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetCellZSize(Double_t value)
{
   if (value <= 0) {
       cout << "-F- <SpdEcalTB2GeoMapper::SetCellZSize> Cell z size should be > 0" << endl;
       exit(1);
   }
   
   if (value > SpdCommonGeoMapper::Instance()->GetEcalTBLen2()) {
       cout << "-W- <SpdEcalTB2GeoMapper::SetCellSize> "
            << "\n\tcell z size is more than barrel length, "
            << "\n\trecommended decreasing cell z size " << endl;
   }
   
   SpdParameter* par = GetParameter("EcalCellThetaInnerSize");
   if (!par) fParams->Add(new SpdParameter("EcalCellThetaInnerSize", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalTB2GeoMapper::SetCellZSize> Overwriting parameter \"EcalCellThetaInnerSize\"" << endl; 
   }
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetCellInnerPhiSize(Double_t value)
{
   if (value <= 0) {
        cout << "-F- <SpdEcalTB2GeoMapper::SetCellInnerPhiSize> Cell phi size should be > 0" << endl;
        exit(1);
   }
   
   if (value > SpdCommonGeoMapper::Instance()->GetEcalTBSize2()) {
       cout << "-W- <SpdEcalTB2GeoMapper::SetCellInnerPhiSize> "
            << "\n\tCell phi inner size is more than barrel outer radius, "
            << "\n\trecommended decreasing cell phi inner size " << endl;
   }

   SpdParameter* par = GetParameter("EcalCellPhiInnerSize");
   if (!par) fParams->Add(new SpdParameter("EcalCellPhiInnerSize", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalTB2GeoMapper::SetCellInnerPhiSize> Overwriting parameter \"EcalCellPhiInnerSize\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetCellOuterPhiSize(Double_t value)
{
   if (value <= 0) {
       cout << "-F- <SpdEcalTB2GeoMapper::SetCellOuterPhiSize> Cell phi size should be > 0" << endl;
       exit(1);
   }
   
   if (value > SpdCommonGeoMapper::Instance()->GetEcalTBSize2()) {
       cout << "-W- <SpdEcalTB2GeoMapper::SetCellOuterPhiSize> " 
            << "\n\tCell phi inner size is more than barrel outer radius, "
            << "\n\trecommended decreasing cell phi inner size " << endl;
   }
   
   SpdParameter* par = GetParameter("EcalCellPhiOuterSize");
   if (!par) fParams->Add(new SpdParameter("EcalCellPhiOuterSize",value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalTB2GeoMapper::SetCellOuterPhiSize> Overwriting parameter \"EcalCellPhiOuterSize\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetNLayers(Int_t value)
{
   if (value <= 0) {
       cout << "-F- <SpdEcalTB2GeoMapper::SetNLayers> Number of layers should be > 0" << endl;
       exit(1);
   }
   
   SpdParameter* par = GetParameter("EcalModuleNLayers");
   if (!par) fParams->Add(new SpdParameter("EcalModuleNLayers", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalTB2GeoMapper::SetNLayers> Overwriting parameter \"EcalModuleNLayers\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetAbsorberLayerThickness(Double_t value)
{
   if (value <= 0) {
       cout << "-F- <SpdEcalTB2GeoMapper::SetAbsorberLayerThickness> Absorber layer thickness should be > 0" << endl;
       exit(1);
   }
   
   SpdParameter* par = GetParameter("EcalAbsorberThickness");
   if (!par) fParams->Add(new SpdParameter("EcalAbsorberThickness", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalTB2GeoMapper::SetAbsorberLayerThickness> Overwriting parameter \"EcalAbsorberThickness\"" << endl;
   }
   
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetScintLayerThickness(Double_t value)
{
   if (value <= 0) {
       cout << "-F- <SpdEcalTB2GeoMapper::SetScintLayerThickness> Scintillator layer thickness should be > 0" << endl;
       exit(1);
   }
   
   SpdParameter* par = GetParameter("EcalScintThickness");
   if (!par) fParams->Add(new SpdParameter("EcalScintThickness", value)); 
   else {
       *par = value;
       cout << "-W- <SpdEcalTB2GeoMapper::SetScintLayerThickness> Overwriting parameter \"EcalScintThickness\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetCellClearance(Double_t value) 
{
   if (value < 0) {
       cout << "-F- <SpdEcalTB2GeoMapper::SetCellClearance> Cell clearance should be >= 0" << endl;
       exit(1);
   }

   SpdParameter* par = GetParameter("EcalCellClearance");
   if (!par) fParams->Add(new SpdParameter("EcalCellClearance",value));
   else {
       *par = value;
       cout << "-W- <SpdEcalTB2GeoMapper::SetCellClearance> Overwriting parameter \"EcalCellClearance\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::SetModuleClearance(Double_t value) 
{
   if (value < 0) {
       cout << "-F- <SpdEcalTB2GeoMapper::SetModuleClearance> Module clearance should be >= 0" << endl;
       exit(1);
   }
   
   SpdParameter* par = GetParameter("EcalModuleClearance");
   if (!par) fParams->Add(new SpdParameter("EcalModuleClearance",value));
   else {
       *par = value;
       cout << "-W- <SpdEcalTB2GeoMapper::SetModuleClearance> Overwriting parameter \"EcalModuleClearance\"" << endl;
   }
}

//_____________________________________________________________________________
void SpdEcalTB2GeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdEcalTB2GeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}








