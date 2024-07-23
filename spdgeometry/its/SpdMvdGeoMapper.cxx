// $Id$
// Author: artur   2021/11/12

//_____________________________________________________________________________
//
// SpdMvdGeoMapper
//_____________________________________________________________________________

#include "SpdMvdGeoMapper.h"
#include "SpdCommonGeoMapper.h"

#include <TMath.h>
#include <map>

#include <iostream>

using std::cout;
using std::endl;

using namespace TMath;

ClassImp(SpdMvdGeoMapper)

SpdMvdGeoMapper* SpdMvdGeoMapper::fInstance = 0;

SpdMvdGeoMapper* SpdMvdGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdMvdGeoMapper(); }

SpdMvdGeoMapper* SpdMvdGeoMapper::GetMapper() 
{ return fInstance; }   
    
//_____________________________________________________________________________
SpdMvdGeoMapper::SpdMvdGeoMapper():SpdGeoMapper("Mvd")
{
   if (fInstance) {
       Fatal("SpdMvdGeoMapper", "Singleton instance already exists.");
       return;
   }
   
   fInstance = this;
  
   fParams = new TObjArray();
  
   fGeoType = 1; //SpdCommonGeoMapper::Instance()->GetMvdDefGeoType();
}

//_____________________________________________________________________________
SpdMvdGeoMapper::SpdMvdGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
   if (fInstance) {
       Fatal("SpdMvdGeoMapper", "Singleton instance already exists.");
       return;
   }
   
   fInstance = this;
  
   fParams = new TObjArray();
  
   fGeoType = 1; //SpdCommonGeoMapper::Instance()->GetMvdDefGeoType();
}

//_____________________________________________________________________________
SpdMvdGeoMapper::~SpdMvdGeoMapper() 
{
   ClearGeometry();
}

//_____________________________________________________________________________
void SpdMvdGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdMvdGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }
  
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX"; }
   else if (option == "air")       { fUnsetMedia = "airX";    } 
   else                            { fUnsetMedia = option;    }
   
   //cout << "-I- <SpdMvdGeoMapper::UnsetMaterials> Material: " << fUnsetMedia << endl;
}

//_____________________________________________________________________________
Bool_t SpdMvdGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
   return (gtype > 0 && gtype < 4); 
}

//_____________________________________________________________________________
void SpdMvdGeoMapper::SetActiveMaterial(TString material)
{
   SpdParameter* par = GetParameter("MvdActiveMaterial");
   if (par) *par = material; 
   else fParams->Add(new SpdParameter("MvdActiveMaterial",material));  
}

//_____________________________________________________________________________
TString SpdMvdGeoMapper::GetActiveMaterial() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;    
   const SpdParameter* par = GetParameter("MvdActiveMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Double_t SpdMvdGeoMapper::GetMinRadius() const
{
   const SpdParameter* par = GetParameter("ItsMinRadius");
   Double_t rmin = 0;
   if (par) par->Value(rmin);
   return rmin;
}

//_____________________________________________________________________________
Double_t SpdMvdGeoMapper::GetMaxRadius() const
{
   const SpdParameter* par = GetParameter("ItsMaxRadius");
   Double_t rmax = 0;
   if (par) par->Value(rmax);
   return rmax;
}

//_____________________________________________________________________________
Double_t SpdMvdGeoMapper::GetMaxLength() const
{
   const SpdParameter* par = GetParameter("ItsMaxLength");
   Double_t lmax = 0;
   if (par) par->Value(lmax);
   return lmax;
}

//_____________________________________________________________________________
TString SpdMvdGeoMapper::GetBaseMaterial() const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;    
   const SpdParameter* par = GetParameter("ItsBaseMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Int_t SpdMvdGeoMapper::GetNLayers() const
{
   const SpdParameter* par = GetParameter("MvdNLayers");
   Int_t nl = 0;
   if (par) par->Value(nl);
   return nl;
}

//_____________________________________________________________________________
Double_t SpdMvdGeoMapper::GetLayerLength(Int_t nlayer) const
{
   const SpdParameter* par = GetParameter(Form("MvdLayer%dLength",nlayer));
   Double_t len = 0;
   if (par) par->Value(len);
   return len; 
}

//_____________________________________________________________________________
Double_t SpdMvdGeoMapper::GetLayerRmin(Int_t nlayer) const
{
   const SpdParameter* par = GetParameter(Form("MvdLayer%dRmin",nlayer));
   Double_t rmin = 0;
   if (par) par->Value(rmin);
   return rmin;     
}

//_____________________________________________________________________________
Double_t SpdMvdGeoMapper::GetLayerRmax(Int_t nlayer) const
{
   const SpdParameter* par = GetParameter(Form("MvdLayer%dRmax",nlayer));
   Double_t rmax = 0;
   if (par) par->Value(rmax);
   return rmax;    
}

//_____________________________________________________________________________
TString SpdMvdGeoMapper::GetLayerBaseMaterial(Int_t nlayer) const
{
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;   
   const SpdParameter* par = GetParameter(Form("MvdLayer%dBaseMaterial",nlayer));
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Bool_t SpdMvdGeoMapper::IsLayerActive(Int_t nlayer) const
{
   const SpdParameter* par = GetParameter(Form("MvdLayer%dActivity",nlayer));
   Bool_t act = 0;
   if (par) par->Value(act);
   return act;  
}

//_____________________________________________________________________________
Double_t SpdMvdGeoMapper::GetLayerPhi(Int_t nlayer) const
{
   const SpdParameter* par = GetParameter(Form("MvdLayer%dPhi",nlayer));
   Double_t phi = 0;
   if (par) par->Value(phi);
   return phi; 
}

//_____________________________________________________________________________
Double_t SpdMvdGeoMapper::GetLayerZpos(Int_t nlayer) const
{
   const SpdParameter* par = GetParameter(Form("MvdLayer%dZpos",nlayer));
   Double_t zpos = 0;
   if (par) par->Value(zpos);
   return zpos;   
}

//_____________________________________________________________________________
Int_t SpdMvdGeoMapper::GetNSublayers(Int_t nlayer) const
{
   const SpdParameter* par = GetParameter(Form("MvdLayer%dMother",nlayer));
   
   if (!par) {
       par = GetParameter(Form("MvdLayer%dNSublayers",nlayer));  
       Int_t nsl = 0;
       if (par) par->Value(nsl);
       return nsl;   
   }
   
   Int_t mother_nlayer = 0;
   par->Value(mother_nlayer);
   return GetNSublayers(mother_nlayer);
}

//_____________________________________________________________________________
Double_t SpdMvdGeoMapper::GetSublayerRmin(Int_t nlayer, Int_t nsublayer) const
{
   const SpdParameter* par = GetParameter(Form("MvdLayer%dMother",nlayer));
   
   if (!par) {
       par = GetParameter(Form("MvdLayer%dSublayer%dRmin",nlayer,nsublayer));  
       Double_t rmin = 0;
       if (par) par->Value(rmin);
       return rmin;   
   }
   
   Int_t mother_nlayer = 0;
   par->Value(mother_nlayer);
   
   Double_t d = GetLayerRmin(nlayer)-GetLayerRmin(mother_nlayer);
   return GetSublayerRmin(mother_nlayer,nsublayer)+d; 
}

//_____________________________________________________________________________
Double_t SpdMvdGeoMapper::GetSublayerWidth(Int_t nlayer, Int_t nsublayer) const
{
   const SpdParameter* par = GetParameter(Form("MvdLayer%dMother",nlayer));
   
   if (!par) {
       par = GetParameter(Form("MvdLayer%dSublayer%dWidth",nlayer,nsublayer));  
       Double_t  width = 0;
       if (par) par->Value(width);
       return width;   
   }
   
   Int_t mother_nlayer = 0;
   par->Value(mother_nlayer);
   return GetSublayerWidth(mother_nlayer,nsublayer);    
}

//_____________________________________________________________________________
TString SpdMvdGeoMapper::GetSublayerMaterial(Int_t nlayer, Int_t nsublayer) const
{  
   if (!fUnsetMedia.IsWhitespace()) return fUnsetMedia;   
   
   const SpdParameter* par = GetParameter(Form("MvdLayer%dMother",nlayer));
   
   if (!par) {
       par = GetParameter(Form("MvdLayer%dSublayer%dMaterial",nlayer,nsublayer));  
       return (par) ?  par->Value() : "";
   }
   
   Int_t mother_nlayer = 0;
   par->Value(mother_nlayer);
   return GetSublayerMaterial(mother_nlayer,nsublayer); 
}
    
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t SpdMvdGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-E- <SpdMvdGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
 
   if (!fParams) return kFALSE;
   
   if (!CheckGeoType(gtype,"MvdGeoType")) return kFALSE;
    
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
     
   SpdParameter* par;

   // Base material
   par = GetParameter("ItsBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetItsBaseMaterial(); }
   else { fParams->Add(new SpdParameter("ItsBaseMaterial",mapper->GetItsBaseMaterial())); } 
   
   // GLOBAL SIZES
   //------------------------------------------------------------
   Double_t Rmin = mapper->GetItsMinRadius();
   par = GetParameter("ItsMinRadius");
   if (par) { if (reinit) *par = Rmin;  }
   else { fParams->Add(new SpdParameter("ItsMinRadius",Rmin)); } 
   
   Double_t Rmax = mapper->GetItsMaxRadius();
   par = GetParameter("ItsMaxRadius");
   if (par) { if (reinit) *par = Rmax;  }
   else { fParams->Add(new SpdParameter("ItsMaxRadius",Rmax)); } 
    
   Double_t Lmax = mapper->GetItsMaxLength();
   par = GetParameter("ItsMaxLength");
   if (par) { if (reinit) *par = Lmax;  }
   else { fParams->Add(new SpdParameter("ItsMaxLength",Lmax)); }
   //------------------------------------------------------------
   
   TString ActiveMaterial = "argon";
   par = GetParameter("MvdActiveMaterial");
   if (par) { if (reinit) *par = ActiveMaterial;  }
   else { fParams->Add(new SpdParameter("MvdActiveMaterial",ActiveMaterial)); } 
   
   //cout << "-I- <SpdMvdGeoMapper::InitGeometry> reinit: " << reinit << endl;
      
        if (fGeoType == 1) SetParameters_1(reinit);
   else if (fGeoType == 2) SetParameters_2(reinit);
   else if (fGeoType == 3) SetParameters_3(reinit);
   
   LockGeometry();
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdMvdGeoMapper::SetParameters_1(Bool_t reinit) // private
{
   cout << "-I- <SpdMvdGeoMapper::SetParameters_1> " << endl;
   
   ClearGeometry();
   
   //---------------------------------------
   // BUILD LAYERS
   
   Int_t l0, l1;
   
   l0 = DefineLayer(5.0,80.0);            SetLayerActivity(l0,true);   
   
        AddSublayer(l0,0.01750,"FR4");
        AddSublayer(l0,0.00190,"copper");
        AddSublayer(l0,0.01350,"kapton2");
        AddSublayer(l0,0.40000,"argon");
        AddSublayer(l0,0.00055,"copper");
        AddSublayer(l0,0.02400,"kapton2");
   
   l1 = DefineLayerCopy(l0,5.5);  
   l1 = DefineLayerCopy(l0,6.0);   
  
   l1 = DefineLayerCopy(l0,12.0);         SetLayerActivity(l1,true);
   l1 = DefineLayerCopy(l0,12.5);

   l1 = DefineLayerCopy2(l0,19.0,160.0);  SetLayerActivity(l1,true);
   l1 = DefineLayerCopy2(l0,19.5,160.0);
  
   //---------------------------------------   
   if (!CheckGeometry()) {
       cout << "-I- <SpdMvdGeoMapper::SetParameters_1> Bad geometry definition" << endl;
       ClearGeometry();
       return;
   }
   
   FillParameters(true);
}

//_____________________________________________________________________________
void SpdMvdGeoMapper::SetParameters_2(Bool_t reinit) // private
{
   cout << "-I- <SpdMvdGeoMapper::SetParameters_2> " << endl;
   
   ClearGeometry();
    
   //---------------------------------------
   // BUILD LAYERS
   
   Int_t l0, l1;
   
   l0 = DefineLayer(5.0,80.0);            SetLayerActivity(l0,true);   
   
        AddSublayer(l0,0.01750,"FR4");
        AddSublayer(l0,0.00190,"copper");
        AddSublayer(l0,0.01350,"kapton2");
        AddSublayer(l0,0.40000,"argon");
        AddSublayer(l0,0.00055,"copper");
        AddSublayer(l0,0.02400,"kapton2");
   
   l1 = DefineLayerCopy(l0,5.5);  
   l1 = DefineLayerCopy(l0,6.0);   

   l1 = DefineLayerCopy2(l0,19.0,160.0);  SetLayerActivity(l1,true);
   l1 = DefineLayerCopy2(l0,19.5,160.0); 
   l1 = DefineLayerCopy2(l0,20.0,160.0); 
   
   //---------------------------------------
   if (!CheckGeometry()) {
       cout << "-I- <SpdMvdGeoMapper::SetParameters_2> Bad geometry definition" << endl;
       ClearGeometry();
       return;
   }
   
   FillParameters(true);
}

//_____________________________________________________________________________
void SpdMvdGeoMapper::SetParameters_3(Bool_t reinit) // private
{
   cout << "-I- <SpdMvdGeoMapper::SetParameters_3> " << endl;
   
   if (!CheckGeometry()) {
       cout << "-I- <SpdMvdGeoMapper::SetParameters_3> Bad geometry definition" << endl;
       ClearGeometry();
       return;
   }
   
   FillParameters(true);
}

//_____________________________________________________________________________
void SpdMvdGeoMapper::ClearGeometry()
{
   if (fBLayers.empty()) return;
   for (mvd_blayer* layer : fBLayers) delete layer;
   fBLayers.clear();
}

//_____________________________________________________________________________
Int_t SpdMvdGeoMapper::DefineLayer(Double_t Rmin, Double_t Length, 
                                   Double_t Phi, Double_t Zpos,
                                   Double_t dead_zone, TString dzone_material,
                                   TString base_material)
{ 
    if (fLockGeometry) {
        cout << "-E- <SpdMvdGeoMapper::DefineLayer> Geometry is locked " << endl;
        return -1;
    }
   
    if (!(Rmin > 0) || !(Length > 0)) {
        cout << "-W- <SpdMvdGeoMapper::DefineLayer> Bad layer size parameters" << endl;
        return -1;
    }
    
    if (dead_zone < 0) dead_zone = 0;
        
    mvd_blayer* layer = new mvd_blayer();
    
    layer->setRmin(Rmin);
    
    layer->length    = Length;
    layer->phi       = Phi;
    layer->zpos      = Zpos;
    layer->dzone     = dead_zone;
    layer->dzone_mat = dzone_material;
    layer->base_mat  = base_material;
    
    fBLayers.push_back(layer);
    
    return fBLayers.size()-1;
}

//_____________________________________________________________________________
Int_t SpdMvdGeoMapper::DefineLayerCopy(Int_t layer_label, Double_t Rmin, 
                                       Double_t Phi, Double_t Zpos)
{
    if (fLockGeometry) {
        cout << "-E- <SpdMvdGeoMapper::DefineLayerCopy> Geometry is locked " << endl;
        return -1;
    } 
    
    if (layer_label < 0 || layer_label >= fBLayers.size()) {
        cout << "-E- <SpdMvdGeoMapper::DefineLayerCopy> No such layer: " << layer_label << endl;
        return -1;
    }
    
    if (!(Rmin > 0)) {
        cout << "-E- <SpdMvdGeoMapper::DefineLayerCopy> Bad layer min. radius" << endl;
        return -1;
    }
    
    mvd_blayer* layer = fBLayers[layer_label]->makeCopy(Rmin);
    
    layer->phi    = Phi;
    layer->zpos   = Zpos;
    
    layer->mother_layer = fBLayers[layer_label];
    
    fBLayers.push_back(layer);
    
    return fBLayers.size()-1;
}

//_____________________________________________________________________________
Int_t SpdMvdGeoMapper::DefineLayerCopy2(Int_t layer_label, 
                                        Double_t Rmin, Double_t Length, 
                                        Double_t Phi, Double_t Zpos)
{
    if (fLockGeometry) {
        cout << "-E- <SpdMvdGeoMapper::DefineLayerCopy2> Geometry is locked " << endl;
        return -1;
    } 
    
    if (layer_label < 0 || layer_label >= fBLayers.size()) {
        cout << "-E- <SpdMvdGeoMapper::DefineLayerCopy2> No such layer: " << layer_label << endl;
        return -1;
    }
    
    if (!(Rmin > 0) || !(Length > 0)) {
        cout << "-E- <SpdMvdGeoMapper::DefineLayerCopy2> Bad layer size parameters" << endl;
        return -1;
    }
    
    mvd_blayer* layer = fBLayers[layer_label]->makeCopy(Rmin);
    
    layer->length = Length;
    layer->phi    = Phi;
    layer->zpos   = Zpos;
    
    layer->mother_layer = fBLayers[layer_label];
    
    fBLayers.push_back(layer);
    
    return fBLayers.size()-1;
}

//_____________________________________________________________________________
void SpdMvdGeoMapper::SetLayerActivity(Int_t layer_label, Bool_t is_active)
{
    if (fLockGeometry) {
        cout << "-E- <SpdMvdGeoMapper::SetLayerActivity> Geometry is locked " << endl;
        return;
    }
    
    if (layer_label < 0 || layer_label >= fBLayers.size()) {
        cout << "-W- <SpdMvdGeoMapper::SetLayerActivity> No such layer: " << layer_label << endl;
        return;
    }
    
    fBLayers[layer_label]->is_active = is_active;
}

//_____________________________________________________________________________
Double_t SpdMvdGeoMapper::AddSublayer(Int_t layer_label, Double_t width, TString material)
{
    if (fLockGeometry) {
        cout << "-E- <SpdMvdGeoMapper::AddSublayer> Geometry is locked " << endl;
        return 0;
    }
    
    if (layer_label < 0 || layer_label >= fBLayers.size()) {
        cout << "-W- <SpdMvdGeoMapper::AddSublayer> No such layer: " << layer_label << endl;
        return 0;
    }
    
    if (!(width > 0)) {
        cout << "-W- <SpdMvdGeoMapper::Addsublayer> Bad sublayer width" << endl;
        return 0;
    }
    
    mvd_blayer* layer = fBLayers[layer_label];
    
    (material.IsWhitespace()) ? layer->addLayer(width) : layer->addLayer(width,material);
    
    return layer->getRmax();   
}

//_____________________________________________________________________________
Bool_t SpdMvdGeoMapper::CheckGeometry()
{
   cout << "-I- <SpdMvdGeoMapper::CheckGeometry> ... " << endl;
     
   Int_t nlayers = fBLayers.size();
   
   if (nlayers < 1) return true;
 
   // sort layers by min. radius magnitude (if nlayers > 1)
   
   if (nlayers > 1) 
   {
       std::multimap<Double_t, mvd_blayer*> ll; 
       Int_t n(0);
       for (mvd_blayer* l : fBLayers) ll.insert(std::pair<Double_t,mvd_blayer*>(l->getRmin(),l));
       for (auto ix : ll) {
            fBLayers[n] = ix.second;
            fBLayers[n]->label = n+1; // ATTENTION label >= 1!
            n++;
       }
   }
  
   // global size checking
   
   Double_t Rmin = GetMinRadius();
   Double_t Rmax = GetMaxRadius(); 
   
   if (fBLayers[0]->getRmin() < Rmin)  {
       cout << "-E- <SpdMvdGeoMapper::CheckGeometry>"
            << " Layer(0) min.radius: " << fBLayers[0]->getRmin() << " < " << Rmin << endl;
       return false;     
   }
   
   if (fBLayers[nlayers-1]->getRmax() > Rmax)  {
       cout << "-E- <SpdMvdGeoMapper::CheckGeometry>"
            << " Layer(" << nlayers-1 << ") max.radius: " 
            << fBLayers[nlayers-1]->getRmax() << " > " << Rmax << endl;
       return false;     
   }
            
   // check layers one by one
   
   Double_t Len  = GetMaxLength(); 
   
   for (UInt_t i(0); i<nlayers; i++) 
   {
        // layer length checking
        if (fBLayers[i]->length > Len)  {
            cout << "-E- <SpdMvdGeoMapper::CheckGeometry>"
                 << " Layer("<< i <<") length: " << fBLayers[i]->length << " > " << Len << endl;
            return false;     
        }
        
        // radial layer size checking
        if (!fBLayers[i]->checkRSize()) {
            cout << "-E- <SpdMvdGeoMapper::CheckGeometry>"
                 << " Bad layer(" << i << ") radial size, r(min,max) = " 
                 << fBLayers[i]->getRmin() << ", " << fBLayers[i]->getRmax() 
                 << endl;
            return false;
        }
        
        // layers overlap checking
        if ((i < nlayers-1) && (fBLayers[i]->getRmax() > fBLayers[i+1]->getRmin())) {
            cout << "-E- <SpdMvdGeoMapper::CheckGeometry>"
                 << " Layers are overlap Rmax(" << i << ") > Rmin(" << i+1 << ")" 
                 << ", " << fBLayers[i]->getRmax() << " > " << fBLayers[i+1]->getRmin() << endl;
                 
            Print("");     
            return false;
        }
    
        if (!CheckLayer(i)) return false;
   }
   
   cout << "-I- <SpdMvdGeoMapper::CheckGeometry> ... successfull " << endl;
   
   return true;
}

//_____________________________________________________________________________
Bool_t SpdMvdGeoMapper::CheckLayer(Int_t nlayer)
{
   // check sublayers
   return true;
}

//_____________________________________________________________________________
void SpdMvdGeoMapper::FillParameters(Bool_t reinit)
{
   if (fBLayers.empty()) return;
     
   Double_t dpar;
   Int_t ipar;
   Bool_t bpar;
   TString spar;
   
   SpdParameter* par;
   TString parname;

   mvd_blayer* mlayer;
   
   ipar = fBLayers.size();
   parname = "MvdNLayers";
   par = GetParameter(parname);  
   if (par) { if (reinit) *par = ipar; }
   else { fParams->Add(new SpdParameter(parname,ipar)); }

   Int_t nlayer(0);
   for (mvd_blayer* layer : fBLayers) 
   {
       nlayer++;
       
       dpar = layer->length;
       parname = Form("MvdLayer%dLength",nlayer);
       par = GetParameter(parname);  
       if (par) { if (reinit) *par = dpar; }
       else { fParams->Add(new SpdParameter(parname,dpar)); } 
       
       dpar = layer->getRmin();
       parname = Form("MvdLayer%dRmin",nlayer);
       par = GetParameter(parname);  
       if (par) { if (reinit) *par = dpar; }
       else { fParams->Add(new SpdParameter(parname,dpar)); } 
       
       dpar = layer->getRmax();
       parname = Form("MvdLayer%dRmax",nlayer);
       par = GetParameter(parname);  
       if (par) { if (reinit) *par = dpar; }
       else { fParams->Add(new SpdParameter(parname,dpar)); } 
       
       dpar = layer->zpos;
       parname = Form("MvdLayer%dZpos",nlayer);
       par = GetParameter(parname);  
       if (par) { if (reinit) *par = dpar; }
       else { fParams->Add(new SpdParameter(parname,dpar)); } 
       
       dpar = layer->phi;
       parname = Form("MvdLayer%dPhi",nlayer);
       par = GetParameter(parname);  
       if (par) { if (reinit) *par = dpar; }
       else { fParams->Add(new SpdParameter(parname,dpar)); } 
       
       spar = layer->getBaseMaterial();
       parname = Form("MvdLayer%dBaseMaterial",nlayer);
       par = GetParameter(parname);  
       if (par) { if (reinit) *par = spar; }
       else { fParams->Add(new SpdParameter(parname,spar)); } 
       
       bpar = layer->is_active;
       parname = Form("MvdLayer%dActivity",nlayer);
       par = GetParameter(parname);  
       if (par) { if (reinit) *par = bpar; }
       else { fParams->Add(new SpdParameter(parname,bpar)); } 
       
       mlayer = layer->mother_layer;
       
       if (mlayer) {
           
           ipar = mlayer->label;
           parname = Form("MvdLayer%dMother",nlayer);
           par = GetParameter(parname);  
           if (par) { if (reinit) *par = ipar; }
           else { fParams->Add(new SpdParameter(parname,ipar)); }
           
           continue; // ATTENTION
       }
       
       ipar = layer->getNLayers();
       parname = Form("MvdLayer%dNSublayers",nlayer);
       par = GetParameter(parname);  
       if (par) { if (reinit) *par = ipar; }
       else { fParams->Add(new SpdParameter(parname,ipar)); } 
       
       // add sublayers parameters
       for (Int_t i(0); i<layer->getNLayers(); i++) 
       {
            dpar = layer->getRmin(i);
            parname = Form("MvdLayer%dSublayer%dRmin",nlayer,i+1);
            par = GetParameter(parname);  
            if (par) { if (reinit) *par = dpar; }
            else { fParams->Add(new SpdParameter(parname,dpar)); } 
            
            dpar = layer->getWidth(i);
            parname = Form("MvdLayer%dSublayer%dWidth",nlayer,i+1);
            par = GetParameter(parname);  
            if (par) { if (reinit) *par = dpar; }
            else { fParams->Add(new SpdParameter(parname,dpar)); } 
            
            spar = layer->getMaterial(i);
            parname = Form("MvdLayer%dSublayer%dMaterial",nlayer,i+1);
            par = GetParameter(parname);  
            if (par) { if (reinit) *par = spar; }
            else { fParams->Add(new SpdParameter(parname,spar)); } 
       }
   }
}

//_____________________________________________________________________________
void SpdMvdGeoMapper::Print(Option_t* opt) const
{
   cout << "-I- <SpdMvdGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
       
   if (strcmp(opt,"layers")) return;
   
   Int_t nl = GetNLayers();
   if (nl < 1) return;
   
   Int_t nsl; 
   
   printf("\t------------------------------------------------------------------------------------------------\n");
   printf("\t%5s %3s %10s %3s  %10s %10s %10s %15s %10s %10s \n","L","A",
          "Len, cm","l","R_min, cm","R_max, cm","Width, cm","Material","phi, deg","Z0, cm");

   Double_t Rmin, Rmax, W, rmin, rmax, w;
   
   for (Int_t i(1); i<=nl; i++) 
   {
        printf("\t------------------------------------------------------------------------------------------------\n");
        nsl = GetNSublayers(i);
        Rmin = GetLayerRmin(i);
        Rmax = GetLayerRmax(i);
        W = Rmax-Rmin;
        printf("\t%5d %3d %10.3f %3d: %10.5f %10.5f %10.5f %15s %10.3f %10.3f\n",i,
               IsLayerActive(i),GetLayerLength(i),nsl,Rmin,Rmax,W,
               GetLayerBaseMaterial(i).Data(),GetLayerPhi(i),GetLayerZpos(i));
        
        printf("\t%20s----------------------------------------------------------------------------\n","");
        for (Int_t j(1); j <= nsl; j++) {
             rmin = GetSublayerRmin(i,j);
             w = GetSublayerWidth(i,j);
             rmax = rmin + w;
             printf("\t%20s %3d  %10.5f %10.5f %10.5f %15s\n","",j,
                   rmin,rmax,w,GetSublayerMaterial(i,j).Data());    
        }
        printf("\n");
   }
   
   cout << endl;
}

//_____________________________________________________________________________
void SpdMvdGeoMapper::Print2(Option_t*) const
{
   cout << "-I- <SpdMvdGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
       
   if (fBLayers.empty()) return;
   
   Int_t n(0), nl; 
   
   printf("\t------------------------------------------------------------------------------------------------\n");
   printf("\t%5s %3s %10s %3s  %10s %10s %10s %15s %10s %10s \n","L","A",
          "Len, cm","l","R_min, cm","R_max, cm","Width, cm","Material","phi, deg","Z0, cm");

   for (mvd_blayer* l : fBLayers) 
   {
        printf("\t------------------------------------------------------------------------------------------------\n");
        nl = l->getNLayers();
        printf("\t%5d %3d %10.3f %3d: %10.5f %10.5f %10.5f %15s %10.3f %10.3f\n",l->label,l->is_active,
               l->length,nl,l->getRmin(),l->getRmax(),l->getWidth(),l->base_mat.Data(),l->phi,l->zpos);
        
        printf("\t%20s----------------------------------------------------------------------------\n","");
        for (Int_t i(0); i < nl; i++) {
            printf("\t%20s %3d  %10.5f %10.5f %10.5f %15s\n","",i+1,
                   l->getRmin(i),l->getRmax(i),l->getWidth(i),l->getMaterial(i).Data());    
        }
        printf("\n");
        n++;
   }
   
   cout << endl;
}


