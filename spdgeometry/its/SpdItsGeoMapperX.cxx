// $Id$
// Author: artur   2019/03/07


//_____________________________________________________________________________
//
// SpdItsGeoMapperX
//_____________________________________________________________________________

#include "SpdCommonGeoMapper.h"
#include "SpdItsGeoMapperX.h"
#include "SpdItsVolPars.h"
#include "SpdGeoVVolume.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using std::pair;

using namespace TMath;

SpdItsGeoMapperX* SpdItsGeoMapperX::fInstance = 0;

SpdItsGeoMapperX* SpdItsGeoMapperX::Instance() 
{ return (fInstance) ? fInstance : new SpdItsGeoMapperX(); }

SpdItsGeoMapperX* SpdItsGeoMapperX::GetMapper() 
{ return fInstance; }

#define mm_ 0.1

//----------------------------------------------------------------------
// List of default layer/ladder parameters 
//----------------------------------------------------------------------
// (1) Rlayer    : the distance from z-axis to ladder center (LAYER radius)
// (2) Lz        : ladder size along z axis
// (3) Lphi      : ladder size along phi axis
// (4) Lr        : radial size of ladder 
// (5) angle     : local rotation angle for ladder
// (6) Phi       : global rotation angle for LAYER
// (7) Nladders  : number of ladders inside the LAYER (it will be calculated automatically for g = 1,2)
//----------------------------------------------------------------------

Double_t SpdItsGeoMapperX::thePars[5][5][7] = 
{
  { // g = 1 (standard geometry 1) // MAPS
    {  40.*mm_,  762.*mm_, 15.*mm_, 0.33*mm_,  0.,  0.,   0 },  //  layer 1
    {  96.*mm_,  889.*mm_, 15.*mm_, 0.33*mm_,  0.,  0.,   0 },  //  layer 2
    { 152.*mm_, 1016.*mm_, 15.*mm_, 0.33*mm_,  0.,  0.,   0 },  //  layer 3
    { 210.*mm_, 1270.*mm_, 15.*mm_, 0.33*mm_,  0.,  0.,   0 },  //  layer 4
    { 220.*mm_, 1270.*mm_, 15.*mm_, 0.33*mm_,  0.,  0.,   0 }   //  layer 5 // not used
  }
, 
  { // g = 2 (standard geometry 2) // DSSD auto
    {  40.*mm_, 738.*mm_, 64.*mm_, 0.50*mm_,  15.,  0.,   0 },  //  layer 1
    { 130.*mm_, 738.*mm_, 64.*mm_, 0.50*mm_,  15.,  0.,   0 },  //  layer 2
    { 210.*mm_, 738.*mm_, 64.*mm_, 0.50*mm_,  15.,  0.,   0 },  //  layer 3
    // The rest are not used except for additional studies
    { 212.*mm_, 738.*mm_, 64.*mm_, 0.50*mm_,  0.,  0.,   0 },  //  layer 4
    { 241.*mm_, 738.*mm_, 64.*mm_, 0.50*mm_,  0.,  0.,   0 }   //  layer 5    
  }
, 
  { // g = 3 (standard geometry 3)  // DSSD manual
    {  50.*mm_, 738.*mm_, 64.*mm_, 0.50*mm_,  15.,  0.,   6 },  //  layer 1
    { 130.*mm_, 738.*mm_, 64.*mm_, 0.50*mm_,  15.,  0.,  14 },  //  layer 2
    { 210.*mm_, 738.*mm_, 64.*mm_, 0.50*mm_,  15.,  0.,  23 },  //  layer 3
    // The rest are not used except for additional studies
    { 212.*mm_, 738.*mm_, 64.*mm_, 0.50*mm_,  0.,  0.,   0 },  //  layer 4
    { 241.*mm_, 738.*mm_, 64.*mm_, 0.50*mm_,  0.,  0.,   0 }   //  layer 5    
  }  
//------------------------------------------------------------------
,
// Some trange things here
  { // g = 4
    {  70.*mm_,  624.*mm_, 64.*mm_, 0.30*mm_, 10.,  0.,   8 },  //  layer 1
    { 136.*mm_, 1120.*mm_, 64.*mm_, 0.30*mm_, 10., 18.,  15 },  //  layer 2
    { 182.*mm_, 1369.*mm_, 64.*mm_, 0.30*mm_, 10.,  0.,  20 },  //  layer 3
    { 212.*mm_, 1621.*mm_, 64.*mm_, 0.30*mm_, 10.,  8.,  22 },  //  layer 4
    { 241.*mm_, 1873.*mm_, 64.*mm_, 0.30*mm_, 10.,  7.,  26 }   //  layer 5    
  } 
,  
  { // g = 5
    {  70.*mm_, 1873.*mm_, 64.*mm_, 0.30*mm_, 10.,  0.,   8 },  //  layer 1
    { 136.*mm_, 1873.*mm_, 64.*mm_, 0.30*mm_, 10., 18.,  15 },  //  layer 2
    { 182.*mm_, 1873.*mm_, 64.*mm_, 0.30*mm_, 10.,  0.,  20 },  //  layer 3
    { 212.*mm_, 1873.*mm_, 64.*mm_, 0.30*mm_, 10.,  8.,  22 },  //  layer 4
    { 241.*mm_, 1873.*mm_, 64.*mm_, 0.30*mm_, 10.,  7.,  26 }   //  layer 5    
  }  
};

//----------------------------------------------------------------------
// List of default ladder/chip parameters 
//----------------------------------------------------------------------
// (1) lz        : chip size along z-axis
// (2) lphi      : chip phi-size  
// (3) dlz       : gap size between chips along z-axis
// (4) dlphi     : gap size between chips along phi-axis
// (5) ncz       : number of chip cells (channels) along z-axis
// (6) ncphi     : number of chip cells (channels) along phi-axis
// (7) type      : chip type (1: MAPS, 2: DSSD)
//----------------------------------------------------------------------
//
// 14.336 mm = 512 *  28 mkm,  0.664 mm = 15 mm - 512 *  28 mkm
//   60.8 mm = 640 *  95 mkm,    2.4 mm = 63 mm - 640 *  95 mkm + 0.2 mm
//   60.8 mm = 320 * 190 mkm,    2.4 mm = 63 mm - 320 * 190 mkm + 0.2 mm
//
//----------------------------------------------------------------------

Double_t SpdItsGeoMapperX::theCPars[5][5][7]
{
  { // g = 1  // MAPS, no gaps
    { 14.336*mm_, 14.336*mm_, 0.664*mm_, 0., 512, 512, 1 }, // layer 1
    { 14.336*mm_, 14.336*mm_, 0.664*mm_, 0., 512, 512, 1 }, // layer 2
    { 14.336*mm_, 14.336*mm_, 0.664*mm_, 0., 512, 512, 1 }, // layer 3
    { 14.336*mm_, 14.336*mm_, 0.664*mm_, 0., 512, 512, 1 }, // layer 4
    { 14.336*mm_, 14.336*mm_, 0.664*mm_, 0., 512, 512, 1 }, // layer 5
  }
,
  { // g = 2 // DSSD, no gaps
    {   92.0*mm_,   63.0*mm_,     0.,    0., 320, 640, 2 }, // layer 1  // should be 93, but not enough length according to the TDR dr.
    {   92.0*mm_,   63.0*mm_,     0.,    0., 320, 640, 2 }, // layer 2
    {   92.0*mm_,   63.0*mm_,     0.,    0., 320, 640, 2 }, // layer 3
    {   92.0*mm_,   63.0*mm_,     0.,    0., 320, 640, 2 }, // layer 4
    {   92.0*mm_,   63.0*mm_,     0.,    0., 320, 640, 2 }  // layer 5
  }
,
  { // g = 3 // DSSD, with gaps
    {   60.8*mm_,   60.8*mm_,   2.4*mm_, 0., 320, 640, 2 }, // layer 1
    {   60.8*mm_,   60.8*mm_,   2.4*mm_, 0., 320, 640, 2 }, // layer 2
    {   60.8*mm_,   60.8*mm_,   2.4*mm_, 0., 320, 640, 2 }, // layer 3
    {   60.8*mm_,   60.8*mm_,   2.4*mm_, 0., 320, 640, 2 }, // layer 4
    {   60.8*mm_,   60.8*mm_,   2.4*mm_, 0., 320, 640, 2 }  // layer 5  
  }  
// Nobody knows about the rest...
//------------------------------------------------------------------------------
,  
  { // g = 4
    {   28.8*mm_,   28.8*mm_,   2.4*mm_, 2.4*mm_, 320, 320, 1 }, // layer 1
    {   28.8*mm_,   28.8*mm_,   2.4*mm_, 2.4*mm_, 320, 320, 1 }, // layer 2
    {   60.8*mm_,   60.8*mm_,   2.4*mm_,  0.*mm_, 320, 640, 2 }, // layer 3
    {   60.8*mm_,   60.8*mm_,   2.4*mm_,  0.*mm_, 320, 640, 2 }, // layer 4
    {   60.8*mm_,   60.8*mm_,   2.4*mm_,  0.*mm_, 320, 640, 2 }  // layer 5  
  } 
,  
  { // g = 5
    { 14.336*mm_, 14.336*mm_, 0.664*mm_,  0.,  512, 512, 1 }, // layer 1
    { 14.336*mm_, 14.336*mm_, 0.664*mm_,  0.,  512, 512, 1 }, // layer 2
    { 14.336*mm_, 14.336*mm_, 0.664*mm_,  0.,  512, 512, 1 }, // layer 3
    { 14.336*mm_, 14.336*mm_, 0.664*mm_,  0.,  512, 512, 1 }, // layer 4
    { 14.336*mm_, 14.336*mm_, 0.664*mm_,  0.,  512, 512, 1 }  // layer 5
  }
};
 
/* +++++++++++++++++++ static getters +++++++++++++++++++ */

Double_t SpdItsGeoMapperX::Rl_(Int_t g, Int_t nlayer)    { return thePars[g-1][nlayer-1][0]; }
Double_t SpdItsGeoMapperX::Lz_(Int_t g, Int_t nlayer)    { return thePars[g-1][nlayer-1][1]; }
Double_t SpdItsGeoMapperX::Lphi_(Int_t g, Int_t nlayer)  { return thePars[g-1][nlayer-1][2]; }
Double_t SpdItsGeoMapperX::Lr_(Int_t g, Int_t nlayer)    { return thePars[g-1][nlayer-1][3]; }
Double_t SpdItsGeoMapperX::angle_(Int_t g, Int_t nlayer) { return thePars[g-1][nlayer-1][4]; }
Double_t SpdItsGeoMapperX::Phi_(Int_t g, Int_t nlayer)   { return thePars[g-1][nlayer-1][5]; }
Int_t    SpdItsGeoMapperX::Nl_(Int_t g, Int_t nlayer)    { return thePars[g-1][nlayer-1][6]; }
 
Double_t SpdItsGeoMapperX::lz_(Int_t g, Int_t nlayer)    { return theCPars[g-1][nlayer-1][0]; }
Double_t SpdItsGeoMapperX::lphi_(Int_t g, Int_t nlayer)  { return theCPars[g-1][nlayer-1][1]; }
Double_t SpdItsGeoMapperX::dlz_(Int_t g, Int_t nlayer)   { return theCPars[g-1][nlayer-1][2]; }
Double_t SpdItsGeoMapperX::dlphi_(Int_t g, Int_t nlayer) { return theCPars[g-1][nlayer-1][3]; }
Int_t    SpdItsGeoMapperX::ncz_(Int_t g, Int_t nlayer)   { return theCPars[g-1][nlayer-1][4]; }
Int_t    SpdItsGeoMapperX::ncphi_(Int_t g, Int_t nlayer) { return theCPars[g-1][nlayer-1][5]; }
Int_t    SpdItsGeoMapperX::ctype_(Int_t g, Int_t nlayer) { return theCPars[g-1][nlayer-1][6]; }

/* +++++++++++++++++++ static setters  +++++++++++++++++++ */

void SpdItsGeoMapperX::SetDefaultLadderPars(Int_t g, Int_t l, Int_t p, Double_t value)
{
   if (g < 1 || g > 5) return;
   if (l < 1 || l > 5) return;
   if (p < 0 || p > 6) return;
   
   thePars[g-1][l-1][p] = value;
}

void SpdItsGeoMapperX::SetDefaultLadderPars(Int_t g, Int_t p, Double_t value) 
{
   if (g < 1 || g > 5) return;
   if (p < 0 || p > 6) return;
   
   for (Int_t i(0); i<5; i++) thePars[g-1][i][p] = value;
}

void SpdItsGeoMapperX::SetDefaultChipPars(Int_t g, Int_t l, Int_t p, Double_t value)
{
   if (g < 1 || g > 5) return;
   if (l < 1 || l > 5) return;
   if (p < 0 || p > 6) return;

   theCPars[g-1][l-1][p] = value;
}

void SpdItsGeoMapperX::SetDefaultChipPars(Int_t g, Int_t p, Double_t value) 
{
   if (g < 1 || g > 5) return;
   if (p < 0 || p > 6) return;
   
   for (Int_t i(0); i<5; i++) theCPars[g-1][i][p] = value;
}

ClassImp(SpdItsGeoMapperX)

//_____________________________________________________________________________
SpdItsGeoMapperX::SpdItsGeoMapperX():SpdGeoMapper("Its")
{
   if (fInstance) {
       Fatal("SpdItsGeoMapperX", "Singleton instance already exists.");
       return;
   }
    
   fInstance = this;
   
   fParams = new TObjArray();
   fGeoVolPars = new TObjArray();  
   fGeoTable = new SpdGeoTable(); 
   
   fGeoType = SpdCommonGeoMapper::Instance()->GetItsDefGeoType();
   fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
   
   fVVolumes = new VVolumesTable;
}

//_____________________________________________________________________________
SpdItsGeoMapperX::SpdItsGeoMapperX(TString prefix):SpdGeoMapper(prefix)
{
   if (fInstance) {
       Fatal("SpdItsGeoMapperX", "Singleton instance already exists.");
       return;
   }
    
   fInstance = this;
   
   fParams = new TObjArray();
   fGeoVolPars = new TObjArray();  
   fGeoTable = new SpdGeoTable(); 
   
   fGeoType = SpdCommonGeoMapper::Instance()->GetItsDefGeoType();
   fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
   
   fVVolumes = new VVolumesTable;
}

//_____________________________________________________________________________
SpdItsGeoMapperX::~SpdItsGeoMapperX() 
{
   DeleteVVolumes();
  
   fInstance = 0;
}

//_____________________________________________________________________________
void SpdItsGeoMapperX::DeleteVVolumes() // private
{
   if (!fVVolumes) return;
  
   VVolumesTableIter it = fVVolumes->begin();
   for (; it != fVVolumes->end(); it++) delete it->second;
  
   delete fVVolumes;
   fVVolumes = 0;
}

//_____________________________________________________________________________
SpdGeoVVolume* SpdItsGeoMapperX::GetVVolume(TString chip) // public
{
   if (!fVVolumes) return 0;

   VVolumesTableIter it = fVVolumes->find(chip);
   return (it == fVVolumes->end()) ? 0 : it->second;
}

//_____________________________________________________________________________
SpdGeoVVolume* SpdItsGeoMapperX::GetVVolume(Int_t node_id) // public
{
   if (!fGeoTable) return 0;
   
   TString volname = fGeoTable->GetNodeVolumeName(node_id);
   return (!volname.IsWhitespace()) ? GetVVolume(volname) : 0;
}

//_____________________________________________________________________________
SpdGeoVVolumeBox2D* SpdItsGeoMapperX::AddNewVolume(TString volname, VVolumesTable* table) const // private
{
   if (!table) return 0;
   
   VVolumesTableIter it = table->find(volname);
   if (it != table->end()) return 0;
   
   SpdGeoVVolumeBox2D* vol = new SpdGeoVVolumeBox2D();
   table->insert(std::pair<TString,SpdGeoVVolume*>(volname,vol));
   
   return vol;
}

//_____________________________________________________________________________
SpdGeoVVolumeBox2D* SpdItsGeoMapperX::FindVolume(TString volname, VVolumesTable* table) const // private
{
   if (!table) return 0; 
   VVolumesTableConstIter it = table->find(volname);
   return (it == table->end()) ? 0 : (SpdGeoVVolumeBox2D*)it->second;
}

//_____________________________________________________________________________
void SpdItsGeoMapperX::PrintVVolumes() const // private
{
   if (!fVVolumes) return;
   
   cout << "\t<SpdItsGeoMapperX::PrintVVolumes>" << endl;
   printf("\n%20s  %20s %8s %8s %10s %10s %10s %10s %10s %5s\n\n","Chip",
          "VVolume","N(1)","N(2)","Size(1)","Size(2)","Pitch(1)","Pitch(2)","Max.step","Axes");
   
   VVolumesTableConstIter it = fVVolumes->begin();
   for (; it != fVVolumes->end(); it++) {
       printf("%20s |",it->first.Data());
       ((SpdGeoVVolumeBox2D*)it->second)->PrintVolume();
   }
  
   cout << endl;
}

//_____________________________________________________________________________
void SpdItsGeoMapperX::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdItsGeoMapperX::UnsetMaterials> Geometry is locked " << endl;
       return;
   }

        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "air")       { fUnsetMedia = "airX";    } 
   else                            { fUnsetMedia = option;    }
   
   //cout << "-I- <SpdItsGeoMapperX::UnsetMaterials> Material: " << fUnsetMedia << endl;
}

//_____________________________________________________________________________
void  SpdItsGeoMapperX::SetNLayers(Int_t nlayers)
{
   if (fLockGeometry) {
       cout << "-E- <SpdItsGeoMapperX::SetNLayers> Geometry is locked " << endl;
       return;
   }
   
   if (nlayers < 1) {
       cout << "-E- <SpdItsGeoMapperX::SetNLayers> Bad number of layers: " 
            << nlayers << endl;
       return;
   }
   
   SpdParameter* par = GetParameter("ItsNLayers");
   if (par) { *par = nlayers; }
   else { fParams->Add(new SpdParameter("ItsNLayers",nlayers)); }  
}

//_____________________________________________________________________________
void SpdItsGeoMapperX::SetMaxStepFactor(Double_t f) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdItsGeoMapperX::SetMaxStepFactor> Geometry is locked " << endl;
       return;
   }
      
   if (!(f > 0)) f = 1;
   
   SpdParameter* par = GetParameter("ItsMaxStepFactor");
   if (par) { *par = f; }
   else { fParams->Add(new SpdParameter("ItsMaxStepFactor",f)); }  
}

//_____________________________________________________________________________
Double_t SpdItsGeoMapperX::GetMaxStepFactor() 
{ 
   SpdParameter* par = GetParameter("ItsMaxStepFactor");
   
   Double_t f(0.1);
   
   if (par) par->Value(f);
   else fParams->Add(new SpdParameter("ItsMaxStepFactor",f));
   
   return f;
}

//_____________________________________________________________________________
TString SpdItsGeoMapperX::GetBaseMaterial() const
{
   const SpdParameter* par = GetParameter("ItsBaseMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
TString SpdItsGeoMapperX::GetLadderMaterial() const
{
   const SpdParameter* par = GetParameter("ItsLadderMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
TString SpdItsGeoMapperX::GetSensorMaterial() const
{
   const SpdParameter* par = GetParameter("ItsSensorMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Int_t SpdItsGeoMapperX::GetNLayers() const
{
   const SpdParameter* par = GetParameter("ItsNLayers");
   Int_t nlayers = 0;
   if (par) par->Value(nlayers);
   return nlayers;
}

//_____________________________________________________________________________
Double_t SpdItsGeoMapperX::GetMinRadius() const
{
   const SpdParameter* par = GetParameter("ItsMinRadius");
   Double_t rmin = 0;
   if (par) par->Value(rmin);
   return rmin;
}

//_____________________________________________________________________________
Double_t SpdItsGeoMapperX::GetMaxRadius() const
{
   const SpdParameter* par = GetParameter("ItsMaxRadius");
   Double_t rmax = 0;
   if (par) par->Value(rmax);
   return rmax;
}

//_____________________________________________________________________________
Double_t SpdItsGeoMapperX::GetMaxLength() const
{
   const SpdParameter* par = GetParameter("ItsMaxLength");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
Double_t SpdItsGeoMapperX::GetLayerLength(Int_t nlayer) const
{
   if (nlayer < 1 || nlayer > GetNLayers())  return 0;
   
   const SpdParameter* par = GetParameter(Form("ItsLayer%dLz",nlayer));
   Double_t len = 0;
   if (par) par->Value(len);
   return len;
}

//_____________________________________________________________________________
Double_t SpdItsGeoMapperX::GetLayerWidth(Int_t nlayer) const
{
   if (nlayer < 1 || nlayer > GetNLayers())  return 0;
   
   const SpdParameter* par = GetParameter(Form("ItsLayer%dLr",nlayer));
   Double_t width = 0;
   if (par) par->Value(width);
   return width;
}

//_____________________________________________________________________________
Double_t SpdItsGeoMapperX::GetLayerRadius(Int_t nlayer) const
{
   if (nlayer < 1 || nlayer > GetNLayers())  return 0;
   
   const SpdParameter* par = GetParameter(Form("ItsLayer%dRlayer",nlayer));
   Double_t r = 0;
   if (par) par->Value(r);
   return r;
}

//_____________________________________________________________________________
Double_t SpdItsGeoMapperX::GetLayerRmin(Int_t nlayer) const
{
   if (nlayer < 1 || nlayer > GetNLayers())  return 0;
   
   const SpdParameter* par = GetParameter(Form("ItsLayer%dRmin",nlayer));
   Double_t rmin = 0;
   if (par) par->Value(rmin);
   return rmin;
}

//_____________________________________________________________________________
Double_t SpdItsGeoMapperX::GetLayerRmax(Int_t nlayer) const
{
   if (nlayer < 1 || nlayer > GetNLayers())  return 0;
   
   const SpdParameter* par = GetParameter(Form("ItsLayer%dRmax",nlayer));
   Double_t rmax = 0;
   if (par) par->Value(rmax);
   return rmax;
}

//_____________________________________________________________________________
Double_t SpdItsGeoMapperX::GetLayerPhi(Int_t nlayer) const
{
   if (nlayer < 1 || nlayer > GetNLayers())  return 0;
   
   const SpdParameter* par = GetParameter(Form("ItsLayer%dPhi",nlayer));
   Double_t Phi = 0;
   if (par) par->Value(Phi);
   return Phi;
}

//_____________________________________________________________________________
Bool_t SpdItsGeoMapperX::IsGeoTypeDefined(Int_t gtype) const
{ 
   return (gtype > 0 && gtype < 4); 
}

//_____________________________________________________________________________
Bool_t SpdItsGeoMapperX::AreEndcapsEnabled() const
{
   const SpdParameter* par = GetParameter("ItsEnableEndcaps");
   if (!par) return kFALSE;
   Bool_t v;
   par->Value(v);
   return v;
}

//_____________________________________________________________________________
void SpdItsGeoMapperX::EnableEndcaps(Bool_t on)
{
   SpdParameter* par;    
   par = GetParameter("ItsEnableEndcaps");
   if (par) { *par = on; }
   else { fParams->Add(new SpdParameter("ItsEnableEndcaps",on)); } 
}

//_____________________________________________________________________________
Bool_t SpdItsGeoMapperX::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-W- <SpdItsGeoMapperX::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
 
   if (!fParams) return kFALSE;
   
   if (fGeoTable) fGeoTable->Clear();
   if (fGeoVolPars) fGeoVolPars->Clear();
    
   if (!CheckGeoType(gtype,"ItsGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
     
   par = GetParameter("ItsBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetItsBaseMaterial(); }
   else { fParams->Add(new SpdParameter("ItsBaseMaterial",mapper->GetItsBaseMaterial())); } 
   
   par = GetParameter("ItsLadderMaterial");
   if (par) { if (reinit) *par = mapper->GetItsLadderMaterial(); }
   else { fParams->Add(new SpdParameter("ItsLadderMaterial",mapper->GetItsLadderMaterial())); }  
   
   par = GetParameter("ItsSensorMaterial");
   if (par) { if (reinit) *par = mapper->GetItsSensorMaterial(); }
   else { fParams->Add(new SpdParameter("ItsSensorMaterial",mapper->GetItsSensorMaterial())); }  
   
   par = GetParameter("ItsMinRadius");
   if (par) { if (reinit) *par = mapper->GetItsMinRadius();  }
   else { fParams->Add(new SpdParameter("ItsMinRadius",mapper->GetItsMinRadius())); } 
   
   par = GetParameter("ItsMaxRadius");
   if (par) { if (reinit) *par = mapper->GetItsMaxRadius();  }
   else { fParams->Add(new SpdParameter("ItsMaxRadius",mapper->GetItsMaxRadius())); } 
    
   par = GetParameter("ItsMaxLength");
   if (par) { if (reinit) *par = mapper->GetItsMaxLength();  }
   else { fParams->Add(new SpdParameter("ItsMaxLength",mapper->GetItsMaxLength())); }
   
   par = GetParameter("ItsNLayers");
   if (par) { if (reinit) *par = mapper->GetItsNLayers();  }
   else { fParams->Add(new SpdParameter("ItsNLayers",mapper->GetItsNLayers())); }  
   
   Bool_t endcaps = kTRUE; // its endcaps is enabled
   par = GetParameter("ItsEnableEndcaps");
   if (par) { if (reinit) *par = endcaps; }
   else { fParams->Add(new SpdParameter("ItsEnableEndcaps",endcaps)); } 
   
   //cout << "-I- <SpdItsGeoMapperX::InitGeometry> reinit: " << reinit << endl;
     
   GetMaxStepFactor();
    
   SetParameters(reinit);
 
   return kTRUE;
}

//_____________________________________________________________________________
void SpdItsGeoMapperX::SetParameters(Bool_t reinit)
{
   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* ++++++++++++ DEFINE LAYERS +++++++++++++ */ 
   /* ++++++++++++++++++++++++++++++++++++++++ */
   
   Int_t nlayers = GetNLayers();
   
   if (nlayers < 1) return;
   
   SpdItsLayer* layer[nlayers];
   
   Int_t nl = 0;
   for (Int_t i(1); i <= nlayers; i++) 
   {
        SpdItsLayer* l = InitLayer(i);
        if (!l) continue;
        
        l->SetMedia(GetBaseMaterial());
        l->SetActivity(0);
        
        l->SetFillColor(kOrange+8);
        l->SetLineColor(kOrange+8);
        l->SetTransparency(30);
        
        l->SetNCopies(1);
        l->MakePacking();
        
        fGeoVolPars->Add(l);
        fGeoTable->AddTableRec(0,l->GetName(),fMasterVolName,1,1);
        
        layer[nl] = l;
        
        nl++;
   }
   
   TString hash, vname;
   
   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* ++++++++++++ DEFINE LADDERS ++++++++++++ */
   /* ++++++++++++++++++++++++++++++++++++++++ */
     
   std::map<TString,SpdItsLadder*> table_l;  // set of unique ladders
   std::map<TString,SpdItsLadder*>::iterator it;
   
   SpdItsLadder* ladder;
   
   std::map<TString,SpdItsLadder*> ladders;
     
   for (Int_t l(0); l < nl; l++) {
        int nn = 0;
        for (Int_t i(0); i < layer[l]->GetNNodes(); i++) {
              
             ladder = (SpdItsLadder*)layer[l]->MakeNodeVolPars(i); 
             
             /* ATTENTION +++++ INIT LADDER PACKING +++++ ATTENTION */
             
             InitLadderPacking(l+1,ladder); 
             
             hash = ladder->HashString();
             it = table_l.find(hash);
             nn++;
           
             if (it != table_l.end()) {
                 layer[l]->SetVolName(i,it->second->GetName()); // fill layer vol. table
                 delete ladder;
                 continue;
             }
             
             vname = AddPrefix(Form("Ladder%li",table_l.size()+1));
             
             //cout << "<SpdItsGeoMapperX::SetParameters_1> Add new ladder: " 
             //      << vname << " / " << l << " " << i << " " << nn << " " << table_l.size() 
             //      << " nnodes = " << layer[l]->GetNNodes() << endl;
             
             ladders.insert(std::pair<TString,SpdItsLadder*>(vname,ladder));     
        
             layer[l]->SetVolName(i,vname); // fill module vol. table (node_num, vol_name)
             
             ladder->SetName(vname);
             ladder->SetMedia(GetLadderMaterial());
             ladder->SetActivity(0);
              
             ladder->SetLineColor(kOrange+9);
             ladder->SetFillColor(kOrange+9);
             ladder->SetTransparency(5);

             table_l.insert(std::pair<TString,SpdItsLadder*>(hash,ladder)); 
             
             /* ATTENTION +++++ PACK LADDER +++++ ATTENTION */
             
             ladder->MakePacking();
             
             /* ATTENTION +++++ Add unique LADDER to the List of VolPars +++++ ATTENTION */ 
             
             fGeoVolPars->Add(ladder);  
        }
   }
   
   /* ATTENTION +++++ Fill GeoTable (LEVEL, Vol.Name, Mother Vol.Name, First node, Nodes) +++++ ATTENTION */
   
   /* ++++++ NOTE SIMPLIFICATION: 1 layer <-> 1 ladder NOTE ++++++ */
   
   for (Int_t l(0); l<nl; l++) {
 
        vname = layer[l]->GetVolName(0); // ! NOTE !
 
        it = ladders.find(vname);
        if (it == ladders.end()) continue;
    
        ladder = it->second;
        if (!ladder) continue;
        
        Int_t nn = layer[l]->GetNNodes(vname);
        if (nn == 0) continue; // don't add empty layer to the table 
              
        fGeoTable->AddTableRec(1, vname, layer[l]->GetName(), ladder->GetNCopies()+1, nn);
        
        ladder->AddNCopies(nn);
   }
  
   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* ++++++++++++ DEFINE CHIPS ++++++++++++++ */
   /* ++++++++++++++++++++++++++++++++++++++++ */

   std::map<TString,SpdItsChip*> table_c;  // set of unique chips
   std::map<TString,SpdItsChip*>::iterator it_c;
   
   SpdItsChip* chip;
   
   std::map<TString,SpdItsChip*> chips;
   
   VVolumesTable* vvtable = new VVolumesTable;
   SpdGeoVVolumeBox2D *vvol; 
   
   for (it = table_l.begin(); it != table_l.end(); ++it) {
     
       ladder = it->second;      
       if (!ladder) continue;
       
       for (Int_t i(0); i < ladder->GetNNodes(); i++) {
         
            chip = (SpdItsChip*)ladder->MakeNodeVolPars(i); 
            
            hash = chip->HashString();
            it_c = table_c.find(hash);
              
            if (it_c != table_c.end()) {
                ladder->SetVolName(i,it_c->second->GetName()); // fill ladder vol. table
                delete chip;
                continue;
            }
                      
            vname = AddPrefix(Form("Chip%li",table_c.size()+1));
             
            chips.insert(std::pair<TString,SpdItsChip*>(vname,chip));    
         
            ladder->SetVolName(i,vname); // fill ladder vol. table
             
            chip->SetName(vname);
            chip->SetMedia(GetSensorMaterial());
            chip->SetActivity(1);
        
            chip->SetLineColor(kOrange+10);
            chip->SetFillColor(kOrange+10);
            chip->SetTransparency(50);
            
            table_c.insert(std::pair<TString,SpdItsChip*>(hash,chip));
             
            /* ATTENTION +++++ Add unique CHIP to the List of VolPars +++++ ATTENTION */ 
          
            fGeoVolPars->Add(chip); 
            
            /* ATTENTION +++++ Add Chip info into the VVolumes table ++++++ ATTENTION */ 
             
            vvol = FindVolume(chip->GetType(),fVVolumes);
            if (vvol) {
                vvol->SetMotherVolume(vname);
                vvol->SetSizes(chip->GetLz(),chip->GetLphi());
                vvol->SetAxes('z','x');
                vvtable->insert(std::pair<TString,SpdGeoVVolume*>(vname,vvol));
            }
       }  
   }
   
   if (!vvtable->empty()) {
       delete fVVolumes;
       fVVolumes = vvtable;
   }
   else {
       delete vvtable;
       DeleteVVolumes();
   } 
   
   /* ATTENTION +++++ Fill GeoTable (LEVEL, Vol.Name, Mother Vol.Name, First node, Nodes) +++++ ATTENTION */ 
   
    /* ++++++ NOTE SIMPLIFICATION: 1 ladder <-> 1 chip NOTE ++++++ */
    
   for (it = table_l.begin(); it != table_l.end(); ++it) {
     
        ladder = it->second;
        if (!ladder) continue;
        
        vname = ladder->GetVolName(0); // ! NOTE !
        
        it_c = chips.find(vname);
        if (it_c == chips.end()) continue;
         
        chip = it_c->second;
        if (!chip) continue;
         
        Int_t nn = ladder->GetNNodes(vname);
         
        if (nn == 0) continue; // don't add empty ladder to the table 

        fGeoTable->AddTableRec(2, vname, ladder->GetName(), chip->GetNCopies()+1, nn);

        chip->AddNCopies(nn);
   }  
  
   cout << "-I- <SpdItsGeoMapperX::SetParameters> Define " << nl << "/" << nlayers << " layers " << endl;
  
   if (AreEndcapsEnabled()) SetEndcaps();
   
   fGeoTable->CloseTable();
}
//_____________________________________________________________________________
void SpdItsGeoMapperX::SetEndcaps()
{
   //cout << "-I- <SpdItsGeoMapperX::SetEndcaps> " << endl;
   
   Double_t r = GetMinRadius();
   Int_t nlayers = GetNLayers();
 
   Double_t len, rmin, rmax; 
   
   for (Int_t i = 1; i <= nlayers; i++)    
   {
        len = GetLayerLength(i);
        //rmax = GetLayerRmax(i);
        //rmin = r;
        rmin = 3.5; // Zamyatin, TB meeting 21.04.2022
        rmax = 22.0; // cm, from TDR
   
        //cout << i << " [1] length = " << len << " rmax = " << rmax << " rmin = " << rmin <<  endl;
        
        //cout << i << " [2] length = " << len << " rmax = " << rmax << " rmin = " << rmin <<  endl;
    
        SpdItsEClayer* layer = new SpdItsEClayer(AddPrefix(Form("EClayer%d",i)));
        
        layer->SetLayerPars(0.3*mm_,rmin,rmax);
        
        layer->SetMedia(GetLadderMaterial());
        layer->SetActivity(1);
        
        layer->SetFillColor(kOrange+8);
        layer->SetLineColor(kOrange+8);
        layer->SetTransparency(30);
        
        layer->SetNCopies(1);
        
        fGeoVolPars->Add(layer);
        fGeoTable->AddTableRec(0,layer->GetName(),fMasterVolName,1,2);
   }
}

//_____________________________________________________________________________
Bool_t SpdItsGeoMapperX::GetLayerPackingPars(Int_t nlayer, Int_t& nladders, 
                                             Double_t& Rlayer, Double_t& Rmin, Double_t& Rmax,
                                             Double_t& Lz, Double_t& Lphi, Double_t& Lr, 
                                             Double_t& angle, Double_t& Phi)
{
  
   if (!IsGeoTypeDefined(fGeoType)) {
       cout << "-E- <SpdItsGeoMapperX::GetLayerPackingPars>"
            << " Unknown geometry type: " << fGeoType << endl;
       return kFALSE; 
   }
   
   if (nlayer < 1) return kFALSE;
   
   SpdParameter* par;
   TString parname;
   
   // Default settings 
   // WARNING set corresponding values in SpdTsTBGeoMapper::SetLayerPackingPars also

   Int_t    nladders_def = 0;
   Double_t Rlayer_def;
   Double_t Lz_def, Lphi_def, Lr_def; 
   Double_t angle_def(0.), Phi_def(0.);
   
   if (nlayer > 0 && nlayer < 6) 
   {
       Rlayer_def = Rl_(fGeoType,nlayer);  
       Lz_def     = Lz_(fGeoType,nlayer);
       Lphi_def   = Lphi_(fGeoType,nlayer);
       Lr_def     = Lr_(fGeoType,nlayer);
       angle_def  = angle_(fGeoType,nlayer);
       Phi_def    = Phi_(fGeoType,nlayer);
       
       if (fGeoType < 3) {
           nladders_def = Ceil(Pi()/ASin(0.5*Lphi_def/Rlayer_def));
           Rlayer_def  = 0.5*Lphi_def/Tan(Pi()/nladders_def) + 0.5*Lr_def; 
       }
       else {
           nladders_def = Nl_(fGeoType,nlayer);  
       }
   }
   else 
   {
       cout << "-W- <SpdItsGeoMapperX::GetLayerPackingPars>"
            << " No default parameters for layer: " << nlayer << endl;
   }
     
   //----------------------------------------------------
       
   Int_t npars = 0;
   
   parname = Form("ItsLayer%dNLadders",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(nladders); npars++; }
   else { fParams->Add(new SpdParameter(parname,nladders_def)); nladders = nladders_def; } 
   
   if (nladders == 0) {
     
       parname = Form("ItsLayer%dRmin",nlayer);
       par = GetParameter(parname);  
       if (!par) return kFALSE;
       else par->Value(Rmin);
       
       parname = Form("ItsLayer%dRmax",nlayer);
       par = GetParameter(parname);  
       if (!par) return kFALSE;
       else par->Value(Rmax);
       
       parname = Form("ItsLayer%dRlayer",nlayer);
       par = GetParameter(parname);  
       if (par) par->Value(Rlayer);
       else {  
           Rlayer = 0.5*(Rmin + Rmax);
           fParams->Add(new SpdParameter(parname,Rlayer)); 
       }
       
       return kTRUE;
   }
      
   parname = Form("ItsLayer%dRlayer",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(Rlayer); npars++; }
   else { fParams->Add(new SpdParameter(parname,Rlayer_def)); Rlayer = Rlayer_def; }
    
   parname = Form("ItsLayer%dLz",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(Lz); npars++; }
   else { fParams->Add(new SpdParameter(parname,Lz_def)); Lz = Lz_def; } 
   
   parname = Form("ItsLayer%dLphi",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(Lphi); npars++; }
   else { fParams->Add(new SpdParameter(parname,Lphi_def)); Lphi = Lphi_def; } 
   
   parname = Form("ItsLayer%dLr",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(Lr); npars++; }
   else { fParams->Add(new SpdParameter(parname,Lr_def)); Lr = Lr_def; } 
   
   if (nlayer > 5 && npars < 5) {
       cout << "-E- <SpdItsGeoMapperX::GetLayerPackingPars>"
            << " No enough parameters was found: " << npars << " (required 5)" << endl;
       return kFALSE; 
   }
   
   //----------------------------------------------------
   
   parname = Form("ItsLayer%dAngle",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(angle); }
   else { fParams->Add(new SpdParameter(parname,angle_def)); angle = angle_def; } 
   
   parname = Form("ItsLayer%dPhi",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(Phi); }
   else { fParams->Add(new SpdParameter(parname,Phi_def)); Phi = Phi_def; } 
       
   //----------------------------------------------------
   
   Double_t a = angle*DegToRad();
   
   Double_t lr   = 0.5*Abs(Lphi*Sin(a) + Lr*Cos(a)); 
   Double_t lphi = 0.5*Abs(Lphi*Cos(a) - Lr*Sin(a)); 
   
   //cout << lphi << " " << lr << " / " << Lphi << " " << Lr << "/" << angle << endl;
    
   parname = Form("ItsLayer%dRmin",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(Rmin); }
   else { 
       Rmin = Rlayer-lr;
       fParams->Add(new SpdParameter(parname,Rmin)); 
   } 
   
   parname = Form("ItsLayer%dRmax",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(Rmax); }
   else { 
       Rmax = Sqrt((Rlayer+lr)*(Rlayer+lr) + lphi*lphi);  
       fParams->Add(new SpdParameter(parname,Rmax)); 
   } 
 
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdItsGeoMapperX::GetLadderPackingPars(Int_t nlayer, 
                                             Int_t& nz, Int_t& nphi,
                                             Double_t& lz, Double_t& lphi, 
                                             Double_t& dlz, Double_t& dlphi)
{
   if (!IsGeoTypeDefined(fGeoType)) {
       cout << "-E- <SpdItsGeoMapperX::GetLadderPackingPars>"
            << " Unknown geometry type: " << fGeoType << endl;
       return kFALSE; 
   }
   
   if (nlayer < 1) return kFALSE;
   
   SpdParameter* par;
   TString parname;
   
   // Default settings 
   // WARNING set corresponding values in SpdTsTBGeoMapper::SetLadderPackingPars also

   Double_t lz_def, lphi_def, dlz_def(0), dlphi_def(0); 
   
   if (nlayer > 0 && nlayer < 6) 
   {
       lz_def    = lz_(fGeoType,nlayer);
       lphi_def  = lphi_(fGeoType,nlayer);
       dlz_def   = dlz_(fGeoType,nlayer);
       dlphi_def = dlphi_(fGeoType,nlayer); 
   }
   else 
   {
       cout << "-W- <SpdItsGeoMapperX::GetLadderPackingPars>"
            << " No default parameters for layer: " << nlayer << endl;
   }
   
   //----------------------------------------------------
          
   parname = Form("ItsLayer%dChipLz",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(lz); }
   else { fParams->Add(new SpdParameter(parname,lz_def)); lz = lz_def; } 
   
   parname = Form("ItsLayer%dChipLphi",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(lphi); }
   else { fParams->Add(new SpdParameter(parname,lphi_def)); lphi = lphi_def; } 
   
   parname = Form("ItsLayer%dChipDLz",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(dlz); }
   else { fParams->Add(new SpdParameter(parname,dlz_def)); dlz = dlz_def; } 
   
   parname = Form("ItsLayer%dChipDLphi",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(dlphi); }
   else { fParams->Add(new SpdParameter(parname,dlphi_def)); dlphi = dlphi_def; } 
   
   //----------------------------------------------------
    
   Double_t Lz, Lphi;
     
   parname = Form("ItsLayer%dLz",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(Lz); }
   else Lz = Lz_(fGeoType,nlayer); 
   
   parname = Form("ItsLayer%dLphi",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(Lphi); }
   else Lphi = Lphi_(fGeoType,nlayer); 
   
   nz   = Int_t( (Lz + dlz)/(lz + dlz) );
   nphi = Int_t( (Lphi + dlphi)/(lphi + dlphi) );
   
   parname = Form("ItsLayer%dNChipsZ",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(nz); }
   else { fParams->Add(new SpdParameter(parname,nz,1));} 
   
   parname = Form("ItsLayer%dNChipsPhi",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(nphi); }
   else { fParams->Add(new SpdParameter(parname,nphi,1)); } 
  
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdItsGeoMapperX::GetChipPackingPars(Int_t nlayer, TString& cname)
{
   Int_t ctype(0), ncz(1), ncphi(1);
   
   if (nlayer > 0 && nlayer < 6) 
   {
       ncz   = ncz_(fGeoType,nlayer);  
       ncphi = ncphi_(fGeoType,nlayer);
       ctype = ctype_(fGeoType,nlayer);
   }
   else 
   {
       cout << "-W- <SpdItsGeoMapperX::GetChipPackingPars>"
            << " No default parameters for layer: " << nlayer << endl;
   }
    
   SpdParameter* par;
   TString parname;
   
   parname = Form("ItsLayer%dChipNCellz",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(ncz); }
   else { fParams->Add(new SpdParameter(parname,ncz));  } 
   
   parname = Form("ItsLayer%dChipNCellphi",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(ncphi);}
   else { fParams->Add(new SpdParameter(parname,ncphi)); } 
   
   if (ncz < 1 || ncphi < 1) ctype = 0;
   if (ncz < 2 && ncphi < 2) ctype = 0;
 
   parname = Form("ItsLayer%dChipType",nlayer);
   par = GetParameter(parname);  
   if (par) { par->Value(ctype); }
   else { fParams->Add(new SpdParameter(parname,ctype)); } 
   
   if (ctype == 0) {
       cname = "";
       return kTRUE;
   }
     
   cname = ChipName(ctype, ncz, ncphi);
   
   if (cname.IsWhitespace()) return kTRUE;
   
   parname = Form("ItsLayer%dChipName",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = cname; }
   else { fParams->Add(new SpdParameter(parname,cname,1)); } 
   
   SpdGeoVVolumeBox2D* vol = AddNewVolume(cname,fVVolumes);
   
   if (vol) {
       vol->SetName(cname);
       vol->SetCells(ncz,ncphi);
       vol->SetMaxStepFactor(GetMaxStepFactor());
   }
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdItsGeoMapperX::SetLadderPackingPars(Int_t nlayer, 
                                           Int_t chiptype, Int_t ncz, Int_t ncphi, 
                                           Double_t lz,  Double_t lphi, 
                                           Double_t dlz, Double_t dlphi)
{
   SpdParameter* par;
   TString parname;
   
   parname = Form("ItsLayer%dChipType",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = chiptype; }
   else { fParams->Add(new SpdParameter(parname,chiptype)); } 
   
   parname = Form("ItsLayer%dChipNCellz",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = ncz; }
   else { fParams->Add(new SpdParameter(parname,ncz)); } 
   
   parname = Form("ItsLayer%dChipNCellphi",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = ncphi; }
   else { fParams->Add(new SpdParameter(parname,ncphi)); } 
    
   parname = Form("ItsLayer%dChipLz",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = lz; }
   else { fParams->Add(new SpdParameter(parname,lz)); } 
   
   parname = Form("ItsLayer%dChipLphi",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = lphi; }
   else { fParams->Add(new SpdParameter(parname,lphi)); } 
   
   parname = Form("ItsLayer%dChipDLz",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = dlz; }
   else { fParams->Add(new SpdParameter(parname,dlz)); } 
   
   parname = Form("ItsLayer%dChipDLphi",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = dlphi; }
   else { fParams->Add(new SpdParameter(parname,dlphi)); } 
}
   
//_____________________________________________________________________________
void SpdItsGeoMapperX::SetLayerPackingPars(Int_t nlayer, Int_t nladders, 
                                           Double_t Rlayer, Double_t& Rmin,  Double_t& Rmax,
                                           Double_t Lz, Double_t Lphi, Double_t Lr, 
                                           Double_t angle, Double_t Phi) // protected !
{
   SpdParameter* par;
   TString parname;
    
   parname = Form("ItsLayer%dNLadders",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = nladders; }
   else { fParams->Add(new SpdParameter(parname,nladders)); } 
   
   parname = Form("ItsLayer%dRlayer",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = Rlayer; }
   else { fParams->Add(new SpdParameter(parname,Rlayer)); } 
   
   parname = Form("ItsLayer%dRmin",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = Rmin; }
   else { fParams->Add(new SpdParameter(parname,Rmin)); } 
   
   parname = Form("ItsLayer%dRmax",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = Rmax; }
   else { fParams->Add(new SpdParameter(parname,Rmax)); } 
   
   parname = Form("ItsLayer%dLz",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = Lz; }
   else { fParams->Add(new SpdParameter(parname,Lz)); } 
   
   parname = Form("ItsLayer%dLphi",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = Lphi; }
   else { fParams->Add(new SpdParameter(parname,Lphi)); } 
   
   parname = Form("ItsLayer%dLr",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = Lr; }
   else { fParams->Add(new SpdParameter(parname,Lr)); } 
   
   parname = Form("ItsLayer%dAngle",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = angle; }
   else { fParams->Add(new SpdParameter(parname,angle)); } 
   
   parname = Form("ItsLayer%dPhi",nlayer);
   par = GetParameter(parname);  
   if (par) { *par = Phi; }
   else { fParams->Add(new SpdParameter(parname,Phi)); } 
}

//_____________________________________________________________________________
void SpdItsGeoMapperX::SetLayerPackingPars2(Int_t nlayer, 
                                            Int_t Nladders, Double_t Rlayer, 
                                            Double_t Lz, Double_t Lphi, Double_t Lr, 
                                            Double_t angle, Double_t Phi)
{   
   Double_t a = angle*DegToRad();
   
   Double_t lr   = 0.5*Abs(Lphi*Sin(a) + Lr*Cos(a));
   Double_t lphi = 0.5*Abs(Lphi*Cos(a) - Lr*Sin(a)); 
   
   Double_t Rmin = Rlayer-lr;
   Double_t Rmax = Sqrt((Rlayer+lr)*(Rlayer+lr) + lphi*lphi);
   
   SetLayerPackingPars(nlayer,Nladders,Rlayer,Rmin,Rmax,Lz,Lphi,Lr,angle,Phi); 
}

//_____________________________________________________________________________
// Calculates nladders and Rlayer = 1/2*Lrph ...
void SpdItsGeoMapperX::SetLayerPackingPars3(Int_t nlayer, 
                                           Double_t Rlayer,
                                           Double_t Lz, Double_t Lphi, Double_t Lr,  
                                           Double_t angle, Double_t Phi)
{
 
   Int_t nladders = Ceil(Pi()/ASin(0.5*Lphi/Rlayer));
   Rlayer = 0.5*Lphi/Tan(Pi()/nladders) + 0.5*Lr;  
   
   SetLayerPackingPars2(nlayer,nladders,Rlayer,Lz,Lphi,Lr,angle,Phi);
}
                    
//_____________________________________________________________________________
void SpdItsGeoMapperX::SetGeometryPars(Int_t g1, Int_t g2)
{ 
   if (g2 < 1 || g2 > 5) g2 = g1;

   cout << "-I- <SpdItsGeoMapperX::SetGeometryPars> " << "( "<< g1 << ", " << g2 << " )" << endl;
   
   //----------------------------------------------------------
   if (g1 == 1)  
   {
      fGeoType = 1;
      SetNLayers(4);
      
      for (Int_t i(1); i < 6; i++) 
      {
           SetLayerPackingPars3(i,Rl_(g1,i),Lz_(g1,i),Lphi_(g1,i),Lr_(g1,i),
                                  angle_(g1,i),Phi_(g1,i));
           
           SetLadderPackingPars(i,ctype_(g2,i),ncz_(g2,i),ncphi_(g2,i),
                                  lz_(g2,i),lphi_(g2,i),dlz_(g2,i),dlphi_(g2,i));
      }    
   }
   //----------------------------------------------------------
   else if (g1 == 2) 
   {
      fGeoType = 2;
      SetNLayers(3);
      
      for (Int_t i(1); i < 6; i++) 
      {
           SetLayerPackingPars3(i,Rl_(g1,i),Lz_(g1,i),Lphi_(g1,i),Lr_(g1,i),
                                  angle_(g1,i),Phi_(g1,i));
           
           SetLadderPackingPars(i,ctype_(g2,i),ncz_(g2,i),ncphi_(g2,i),
                                  lz_(g2,i),lphi_(g2,i),dlz_(g2,i),dlphi_(g2,i)); 
      }
   }
   //----------------------------------------------------------
   else if (g1 == 3 || g1 == 4 || g1 == 5) 
   {
      fGeoType = 3;
      if (g1 == 3) {
         SetNLayers(3);
      } else {
         SetNLayers(5);
      }
      
      for (Int_t i(1); i < 6; i++) 
      {
           SetLayerPackingPars2(i,Nl_(g1,i),Rl_(g1,i),Lz_(g1,i),Lphi_(g1,i),Lr_(g1,i),
                                  angle_(g1,i),Phi_(g1,i));
           
           SetLadderPackingPars(i,ctype_(g2,i),ncz_(g2,i),ncphi_(g2,i),
                                  lz_(g2,i),lphi_(g2,i),dlz_(g2,i),dlphi_(g2,i));
      }
      
   }

   else {
      cout << "-W- <SpdItsGeoMapperX::SetGeometryPars>"
           << " Unknown set of parameters: " << g1 << ", " << g2 << endl;
   }
}

//_____________________________________________________________________________
SpdItsLayer* SpdItsGeoMapperX::InitLayer(Int_t nlayer)
{
   Int_t    nladders;
   Double_t Rlayer, Rmin, Rmax;
   Double_t Lz, Lphi, Lr;
   Double_t angle, Phi;
   
   // searching for layer parameters
   if (!GetLayerPackingPars(nlayer, nladders,
                            Rlayer, Rmin, Rmax,
                            Lz, Lphi, Lr, 
                            angle, Phi)) return 0;
   
   SpdItsLayer* layer = new SpdItsLayer(AddPrefix(Form("Layer%d",nlayer)));
   layer->SetLayerPars(Lz,Rmin,Rmax);
   
   if (nladders == 0) return layer;
   
   layer->SetPackingPars(nladders,Rlayer,Lz,Lphi,Lr,angle);
  
   return layer;
}

//_____________________________________________________________________________
void SpdItsGeoMapperX::InitLadderPacking(Int_t nlayer, SpdItsLadder* ladder)
{
   Int_t nz, nphi;
   Double_t lz, lphi, dlz, dlphi;
   
   // searching for ladder parameters
   if (!GetLadderPackingPars(nlayer, nz, nphi, lz, lphi, dlz, dlphi)) return;
   
   TString chip_name;
  
   // searching for chip parameters
   if (!GetChipPackingPars(nlayer, chip_name)) return;
       
   ladder->SetPackingPars(chip_name,nz,nphi,lz,lphi,dlz,dlphi);
}

//_____________________________________________________________________________
TString SpdItsGeoMapperX::ChipName(Int_t type, Int_t ncz, Int_t ncphi) const
{
        if (type <  1) return "";
   else if (type == 1) return Form("MAPS_%dx%d",ncz,ncphi);
   else if (type == 2) return Form("DSSD_%dx%d",ncz,ncphi);
   
   if (type < 1000) return Form("X%03d_%dx%d",type,ncz,ncphi);
 
   return Form("X%d_%dx%d",type,ncz,ncphi);
}
 
//_____________________________________________________________________________
TGeoMatrix* SpdItsGeoMapperX::GetNodeMatrix(const SpdGeoTableRec* rec, Int_t loc_i) const
{   
   if (!rec) return 0;
     
   if (rec->GetMotherVolName() != fMasterVolName) {                  
       const SpdGeoVolPars* pars = GetVolPars(rec->GetMotherVolName()); 
       if (!pars) return 0;
       return pars->MakeNodeMatrix(rec->GetVolName(),loc_i);
   }
   
   /* (top level) layer matrix definition */
   
   TString name = rec->GetVolName();
   Int_t part;
   
   if (name.BeginsWith("ItsLayer")) { name.Remove(0,8); part = 0; }
   else if (name.BeginsWith("ItsEClayer")) { name.Remove(0,10); part = 1; }
   else return 0;
   
   if (!name.IsDigit()) return 0;
   Int_t nlayer = name.Atoi();
  
   //cout << "<SpdItsGeoMapperX::GetNodeMatrix> " << rec->GetVolName() << " " << nlayer << endl;
   
   if (part == 0) {
       TGeoRotation* r = new TGeoRotation();
       r->RotateZ(GetLayerPhi(nlayer));
       return r;
   }
   
   if (part == 1) {
       //Double_t shift = 0.5*(GetLayerLength(nlayer) + GetLayerWidth(nlayer));
       Double_t shift;
       if (nlayer == 1 ) shift = 41.45;
       if (nlayer == 2 ) shift = 51.45;
       if (nlayer == 3 ) shift = 61.45;
       if (loc_i == 0) return new TGeoTranslation(0,0, shift);
       if (loc_i == 1) return new TGeoTranslation(0,0,-shift);
       return 0;
   }
  
   return 0;
}

//_____________________________________________________________________________
void SpdItsGeoMapperX::Print(Option_t*) const
{
   cout << "-I- <SpdItsGeoMapperX::Print>" << "\n\n";
   SpdGeoMapper::Print("");
   
   PrintVVolumes();
}

//_____________________________________________________________________________
TString SpdItsGeoMapperX::GetPrintVolParsCap() const
{
   return Form("%3s  %2s  %4s  %4s    %9s %9s %9s   %9s %9s  %-20s %-20s %-15s  %6s %6s", 
          "I", "A", "NC", "NN",
          "Lz  ", "Rmin/Lphi", "Rmax/Lr",
          "R/nphi", "angle/nz",
          "Class", "Name", "Media",
          "NN", "NV"); 
}



