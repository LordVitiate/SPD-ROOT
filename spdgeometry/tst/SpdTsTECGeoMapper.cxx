// $Id$
// Author: artur   2019/07/04

//_____________________________________________________________________________
//
// SpdTsTECGeoMapper
//_____________________________________________________________________________


#include "SpdCommonGeoMapper.h"
#include "SpdTsTECGeoMapper.h"
#include "SpdTsECVolPars.h"

#include <TMath.h>

#include <iostream>

using std::cout;
using std::endl;

using std::pair;

using namespace TMath;

SpdTsTECGeoMapper* SpdTsTECGeoMapper::fInstance = 0;

SpdTsTECGeoMapper* SpdTsTECGeoMapper::Instance() 
{ return (fInstance) ? fInstance : new SpdTsTECGeoMapper(); }

SpdTsTECGeoMapper* SpdTsTECGeoMapper::GetMapper() 
{ return fInstance; }

#define mm_ 0.1

//----------------------------------------------------------------------------------
// List of default MODULE parameters 
//----------------------------------------------------------------------------------
// (1) Ltype     : type of LAYER (integer)
// (2) NLayers   : number of LAYERS inside the MODULE (integer)
// (3) Lgapsize  : clearance between two nearest LAYERS inside the MODULE (>=0)
// (4) Dphi      : local rotation angle step for LAYERS (integer i, dphi = i*45)
//----------------------------------------------------------------------------------

Double_t SpdTsTECGeoMapper::theModulePars[NDEFMODULES][4] = 
{
    {  1,     8,   10.*mm_,  1  },  //  module type 1  
    {  2,     8,   10.*mm_,  1  },  //  module type 2
};

//----------------------------------------------------------------------------------
// List of default LAYER parameters 
//----------------------------------------------------------------------------------
// (1) Stype     : type of STRAW (integer i)
// (2) Zgapsize  : z-clearance between two nearest STRAW inside the LAYER 
// (3) Rgapsize  : r-clearance between two nearest STRAW inside the LAYER (>=0)
// (4) Poption   : LAYER packing option : 0 = "eo", 1 = "oe"
// (5) DRadius   : LAYER radial offset inside the layer
// (6) DBoresize : LAYER bore offset inside the layer
// (7) Radius    : LAYER max. size (circumscribed circle radius); R < 0 => R = MODULE_MAX_RADIUS
// (8) Boresize  : LAYER bore size (inscribed circle radius); B < 0 => B = MODULE_BORE_SIZE
//----------------------------------------------------------------------------------

Double_t SpdTsTECGeoMapper::theLayerPars[NDEFLAYERS][8] = 
{
    {  1,  0*mm_,  0.*mm_,  0,  0.*mm_,  0.*mm_,  -1.*mm_,  -1.*mm_  },  //  layer type 1
    {  1,  0*mm_,  0.*mm_,  1,  0.*mm_,  0.*mm_,  -1.*mm_,  -1.*mm_  },  //  layer type 2
};

//-----------------------------------
// List of default STRAW parameters 
//-----------------------------------
// (1) Rstraw  : straw max. radius
// (2) Wshell  : shell width
// (3) Rwire   : wire radius
//-----------------------------------

Double_t SpdTsTECGeoMapper::theStrawPars[NDEFSTRAWS][3] =
{
    {  5.*mm_,  0.06*mm_,  0.015*mm_  },  //  straw type 1
};

/* +++++++++++++++++++ static getters +++++++++++++++++++ */

Int_t    SpdTsTECGeoMapper::Lt_(Int_t nmodule)   { return theModulePars[nmodule-1][0]; }
Int_t    SpdTsTECGeoMapper::Nl_(Int_t nmodule)   { return theModulePars[nmodule-1][1]; }
Double_t SpdTsTECGeoMapper::Lg_(Int_t nmodule)   { return theModulePars[nmodule-1][2]; }
Int_t    SpdTsTECGeoMapper::dphi_(Int_t nmodule) { return theModulePars[nmodule-1][3]; }

Int_t    SpdTsTECGeoMapper::St_(Int_t nlayer)    { return theLayerPars[nlayer-1][0]; }
Double_t SpdTsTECGeoMapper::Zg_(Int_t nlayer)    { return theLayerPars[nlayer-1][1]; }
Double_t SpdTsTECGeoMapper::Rg_(Int_t nlayer)    { return theLayerPars[nlayer-1][2]; }
Int_t    SpdTsTECGeoMapper::opt_(Int_t nlayer)   { return theLayerPars[nlayer-1][3]; }
Double_t SpdTsTECGeoMapper::dR_(Int_t nlayer)    { return theLayerPars[nlayer-1][4]; }
Double_t SpdTsTECGeoMapper::dB_(Int_t nlayer)    { return theLayerPars[nlayer-1][5]; }
Double_t SpdTsTECGeoMapper::R_(Int_t nlayer)     { return theLayerPars[nlayer-1][6]; }
Double_t SpdTsTECGeoMapper::B_(Int_t nlayer)     { return theLayerPars[nlayer-1][7]; }

Double_t SpdTsTECGeoMapper::Rs_(Int_t nstraw)    { return theStrawPars[nstraw-1][0]; }
Double_t SpdTsTECGeoMapper::Ws_(Int_t nstraw)    { return theStrawPars[nstraw-1][1]; }
Double_t SpdTsTECGeoMapper::Rw_(Int_t nstraw)    { return theStrawPars[nstraw-1][2]; }

/* +++++++++++++++++++ static setters  +++++++++++++++++++ */

void SpdTsTECGeoMapper::SetDefaultModulePars(Int_t m, Int_t p, Double_t value)
{
   if (m < 1 || m > NDEFMODULES) return;
   if (p < 0 || p > 3) return;
   
   theModulePars[m-1][p] = value;
}

void SpdTsTECGeoMapper::SetDefaultLayerPars(Int_t l, Int_t p, Double_t value)
{
   if (l < 1 || l > NDEFLAYERS) return;
   if (p < 0 || p > 7) return;
   
   theLayerPars[l-1][p] = value;
}

void SpdTsTECGeoMapper::SetDefaultStrawPars(Int_t s, Int_t p, Double_t value) 
{
   if (s < 1 || s > NDEFSTRAWS) return;
   if (p < 0 || p > 2) return;
   
   theStrawPars[s-1][p] = value;
}

ClassImp(SpdTsTECGeoMapper)

//_____________________________________________________________________________
SpdTsTECGeoMapper::SpdTsTECGeoMapper():SpdGeoMapper("TsTEC")
{
   if (fInstance) {
       Fatal("SpdTsTECGeoMapper", "Singleton instance already exists.");
       return;
   }
    
   fInstance = this;
   
   fParams = new TObjArray();
   fGeoVolPars = new TObjArray();  
   fGeoTable = new SpdGeoTable(); 
   
   fGeoType = SpdCommonGeoMapper::Instance()->GetTsTECDefGeoType();
   fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
}

//_____________________________________________________________________________
SpdTsTECGeoMapper::SpdTsTECGeoMapper(TString prefix):SpdGeoMapper(prefix)
{
   if (fInstance) {
       Fatal("SpdTsTECGeoMapper", "Singleton instance already exists.");
       return;
   }
    
   fInstance = this;
   
   fParams = new TObjArray();
   fGeoVolPars = new TObjArray();  
   fGeoTable = new SpdGeoTable(); 
   
   fGeoType = SpdCommonGeoMapper::Instance()->GetTsTECDefGeoType();
   fMasterVolName = SpdCommonGeoMapper::Instance()->GetMasterVolumeName();
}

//_____________________________________________________________________________
SpdTsTECGeoMapper::~SpdTsTECGeoMapper() 
{
   fInstance = 0;
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::UnsetMaterials(Bool_t precise, TString option)
{
   if (fLockGeometry) {
       cout << "-E- <SpdTsTECGeoMapper::UnsetMaterials> Geometry is locked " << endl;
       return;
   }      
   
        if (option.IsWhitespace()) { fUnsetMedia = "vacuumX2"; }
   else if (option == "base")      { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX2"; }
   else if (option == "vacuum")    { (precise) ? fUnsetMedia = "vacuum2" : fUnsetMedia = "vacuumX2"; }
   else if (option == "air")       { fUnsetMedia = "airX2";    } 
   else                            { fUnsetMedia = option;     }
}

//_____________________________________________________________________________
Int_t SpdTsTECGeoMapper::GetNSectors() const
{
   const SpdParameter* par = GetParameter("TsTECNSectors");
   if (!par) return 0;
   Int_t nsec;
   par->Value(nsec);
   return nsec;
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetSecAngle() const
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 360./nsec: 0;
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetSecAngleOver2() const
{
   Int_t nsec = GetNSectors();
   return (nsec > 0) ? 180./nsec: 0;
}

//_____________________________________________________________________________
TString SpdTsTECGeoMapper::GetBaseMaterial() const
{
   const SpdParameter* par = GetParameter("TsTECBaseMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
TString SpdTsTECGeoMapper::GetLayerMaterial() const
{
   const SpdParameter* par = GetParameter("TsTECLayerMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
TString SpdTsTECGeoMapper::GetBaseStrawMaterial() const
{
   const SpdParameter* par = GetParameter("TsTECBaseStrawMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
TString SpdTsTECGeoMapper::GetStrawShellMaterial() const
{
   const SpdParameter* par = GetParameter("TsTECStrawShellMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
TString SpdTsTECGeoMapper::GetStrawWireMaterial() const
{
   const SpdParameter* par = GetParameter("TsTECStrawWireMaterial");
   return (par) ?  par->Value() : "";
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetSize() const
{
   Double_t size = 0;
   const SpdParameter* par = GetParameter("TsTECSize");
   if (par) par->Value(size);
   return size;
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetMaxR() const
{
   Double_t rmax = 0;
   const SpdParameter* par = GetParameter("TsTECMaxR");
   if (par) par->Value(rmax);
   return rmax;
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetWidth() const
{
   Double_t width = 0;
   const SpdParameter* par = GetParameter("TsTECWidth");
   if (par) par->Value(width);
   return width;
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetLength() const
{
   Double_t length = 0;
   const SpdParameter* par = GetParameter("TsTECLength");
   if (par) par->Value(length);
   return length;
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetMinDistance() const
{
   Double_t dist = 0;
   const SpdParameter* par = GetParameter("TsTECMinDistance");
   if (par) par->Value(dist);
   return dist;
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetMaxDistance() const
{
   Double_t dist = 0;
   const SpdParameter* par = GetParameter("TsTECMaxDistance");
   if (par) par->Value(dist);
   return dist;
}

//_____________________________________________________________________________
Int_t SpdTsTECGeoMapper::GetNModules() const
{
   const SpdParameter* par = GetParameter("TsTECNModules");
   Int_t nmodules = 0;
   if (par) par->Value(nmodules);
   return nmodules;
}

//_____________________________________________________________________________
Int_t SpdTsTECGeoMapper::GetNModules(Int_t opt /*0,1,-1*/) const
{
   Int_t nm = 0;
   const SpdParameter* par;
    
   if (opt == 1) {
       par = GetParameter("TsTECNForwardModules");
       if (par) par->Value(nm);
       return nm;
   }
   
   if (opt == -1) {
       par = GetParameter("TsTECNBackwardModules");
       if (par) par->Value(nm);
       return nm;
   }
   
   if (opt == 0) {
       Int_t n = 0;
       par = GetParameter("TsTECNForwardModules");
       if (par) par->Value(nm);
       n += nm;
       par = GetParameter("TsTECNBackwardModules");
       if (par) par->Value(nm);
       n += nm;
       return n;
   }
   
   return nm;
}

//_____________________________________________________________________________
Int_t SpdTsTECGeoMapper::GetModuleType(Int_t opt /*1,-1*/, Int_t nmodule) const
{
   const SpdParameter* par = 0;
  
   if (opt == 1) par = GetParameter(Form("TsTECFModule%dType",nmodule));  
   else if (opt == -1) par = GetParameter(Form("TsTECBModule%dType",nmodule));  
   else return 0;

   Int_t nm = 0;
   if (par) par->Value(nm);
   return nm;
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetModuleLength(Int_t opt /*1,-1*/, Int_t nmodule) 
{
   Int_t mt = GetModuleType(opt,nmodule);
   return (mt == 0) ? 0 : GetModuleLength(mt);
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetModuleDist(Int_t opt /*1,-1*/, Int_t nmodule) const
{
   Int_t mt = GetModuleType(opt,nmodule);
   if (mt == 0) return 0;
   
   const SpdParameter* par = 0;
   Double_t dist(0);
  
   if (opt == 1) par = GetParameter(Form("TsTECFModule%dDist",nmodule));  
   else if (opt == -1) par = GetParameter(Form("TsTECBModule%dDist",nmodule)); 
   else return 0;
   
   if (par) par->Value(dist);
   return dist;
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetModulePhi(Int_t opt /*1,-1*/, Int_t nmodule) const
{
   Int_t mt = GetModuleType(opt,nmodule);
   if (mt == 0) return 0;
   
   const SpdParameter* par = 0;
   Double_t phi(0);
  
   if (opt == 1) par = GetParameter(Form("TsTECFModule%dPhi",nmodule));  
   else if (opt == -1) par = GetParameter(Form("TsTECBModule%dPhi",nmodule));  
   else return 0;
   
   if (par) par->Value(phi); 
   
   return phi;
}

//_____________________________________________________________________________
Int_t SpdTsTECGeoMapper::GetLayerType(Int_t mtype) const
{
   const SpdParameter* par = GetParameter(Form("TsTECModuleType%dLtype",mtype));
   if (!par) return 0;
   Int_t ltype;
   par->Value(ltype);
   return ltype; 
}

//_____________________________________________________________________________
Int_t SpdTsTECGeoMapper::GetStrawType(Int_t ltype) const
{
   const SpdParameter* par = GetParameter(Form("TsTECLayerType%dStype",ltype));
   if (!par) return 0;
   Int_t stype;
   par->Value(stype);
   return stype; 
}
    
//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetModuleLength(Int_t mtype) 
{
   const SpdParameter* par;
   TString parname;
       
   parname = Form("TsTECModuleType%dLtype",mtype);
   par = GetParameter(parname);
   if (!par) {
       cout << "-W- <SpdTsTECGeoMapper::GetModuleLength> Module of type " << mtype  
            << ": unknown layer type " << endl;
       return 0;
   } 
   
   Int_t ltype;
   par->Value(ltype);
 
   parname = Form("TsTECModuleType%dNlayers",mtype);
   par = GetParameter(parname);
   if (!par) {
       cout << "-W- <SpdTsTECGeoMapper::GetModuleLength> Module of type " << mtype  
            << ": unknown number of layers " << endl;
       return 0;
   } 
   
   Int_t nlayers;
   par->Value(nlayers);
   
   if (nlayers < 1) {
       cout << "-W- <SpdTsTECGeoMapper::GetModuleLength> Module of type " << mtype  
            << ": no layers inside " << endl;
       return 0;
   }
   
   Double_t llen = GetLayerLength(ltype);
   
   if (!(llen > 0)) {
       cout << "-W- <SpdTsTECGeoMapper::GetModuleLength> Module of type " << mtype  
            << ": zero layer size " << endl;
       return 0;     
   }
   
   if (nlayers == 1) return llen;
   
   Double_t lgapsize = 0;

   parname = Form("TsTECModuleType%dLgapsize",mtype);
   par = GetParameter(parname);
   if (!par) {
       cout << "-W- <SpdTsTECGeoMapper::GetModuleLength> Module of type " << mtype  
            << ": unknown layer gapsize " << endl;
       return 0;
   } 
   par->Value(lgapsize);
   
   return nlayers*llen + (nlayers-1)*lgapsize;
}
   
//_____________________________________________________________________________   
Double_t SpdTsTECGeoMapper::GetLayerLength(Int_t ltype) 
{
   const SpdParameter* par;
   TString parname;
       
   parname = Form("TsTECLayerType%dStype",ltype);
   par = GetParameter(parname);
   if (!par) {
       cout << "-W- <SpdTsTECGeoMapper::GetLayerLength> Layer of type " << ltype  
            << ": unknown straw type " << endl;
       return 0;
   } 
   
   Int_t stype;
   par->Value(stype);
   
   Double_t slen = 2.*GetStrawRadius(stype);
   
   if (!(slen > 0)) {
       cout << "-W- <SpdTsTECGeoMapper::GetLayerLength> Layer of type " << ltype  
            << ": zero straw size " << endl;
       return 0;     
   }
 
   Double_t zgapsize;
  
   if (!CheckLayerZgapsize(ltype,zgapsize)) {
       cout << "-W- <SpdTsTECGeoMapper::GetLayerLength> Layer of type " << ltype  
            << ": unknown straw zgapsize " << endl;
       return 0;
   } 
 
   return 2*slen + zgapsize;  
}

//_____________________________________________________________________________   
Double_t SpdTsTECGeoMapper::GetStrawRadius(Int_t stype) const
{  
   const SpdParameter* par;
     
   TString parname = Form("TsTECStrawType%dRstraw",stype);
   par = GetParameter(parname);
   if (!par) {
       cout << "-W- <SpdTsTECGeoMapper::GetStrawRadius> Straw of type " << stype  
            << ": zero straw size " << endl;
       return 0;
   } 
   
   Double_t r = 0;
   par->Value(r);   
   
   return r; 
}
//_____________________________________________________________________________   
void SpdTsTECGeoMapper::GetStrawInners(Int_t stype, Double_t& wshell, Double_t& rwire) const
{
   wshell = 0;
   rwire = 0;
 
   const SpdParameter* par;  
   TString parname;
   
   parname = Form("TsTECStrawType%dWshell",stype);
   par = GetParameter(parname);
   if (!par) {
       cout << "-W- <SpdTsTECGeoMapper::GetStrawRadius> Straw of type " << stype  
            << ": zero straw shell size " << endl;
   } 
   else par->Value(wshell);
   
   parname = Form("TsTECStrawType%dRwire",stype);
   par = GetParameter(parname);
   if (!par) {
       cout << "-W- <SpdTsTECGeoMapper::GetStrawRadius> Straw of type " << stype  
            << ": zero straw wire size " << endl;
   } 
   else par->Value(rwire);
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetModuleSizePar(Int_t opt /*1,-1*/, Int_t nmodule) const
{
   const SpdParameter* par;
   
   if (opt == 1) par = GetParameter(Form("TsTECFModule%dSize",nmodule));
   else if (opt == -1) par = GetParameter(Form("TsTECBModule%dSize",nmodule));
   else return 0;
   
   if (!par) return 0;
   
   Double_t size = 0;
   par->Value(size);
   return size;
}

//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetModuleLengthPar(Int_t opt /*1,-1*/, Int_t nmodule) const
{
   const SpdParameter* par;
   
   if (opt == 1) par = GetParameter(Form("TsTECFModule%dLength",nmodule));
   else if (opt == -1) par = GetParameter(Form("TsTECBModule%dLength",nmodule));
   else return 0; 
   
   if (!par) return 0;
  
   Double_t length = 0;
   par->Value(length);
   return length;
}
//_____________________________________________________________________________
Double_t SpdTsTECGeoMapper::GetModulePos(Int_t opt /*1,-1*/, Int_t nmodule) const
{
   if (opt != 1 && opt != -1) return 0;
   Double_t pos = GetModuleSizePar(opt,nmodule)-0.5*GetModuleLengthPar(opt,nmodule);
   return (opt == 1) ? pos : -pos;
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::AddIdentModules(Int_t opt, Int_t mtype, Int_t nmodules, Double_t angle)
{
         if (opt ==  0) for (Int_t i(0); i<nmodules; i++) AddModules(mtype,0,angle);
    else if (opt ==  1) for (Int_t i(0); i<nmodules; i++) AddForwardModule(mtype,0,angle);
    else if (opt == -1) for (Int_t i(0); i<nmodules; i++) AddBackwardModule(mtype,0,angle);   
   
   RecalculateDistance(opt);
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::AddModules(Int_t mtype, Double_t distance, Double_t angle)
{
   AddForwardModule(mtype,distance,angle);
   AddBackwardModule(mtype,distance,angle);
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::AddForwardModule(Int_t mtype, Double_t distance, Double_t angle)
{
   Int_t nm = 0;
   SpdParameter* par;
   TString parname;

   par = GetParameter("TsTECNForwardModules");
   if (par) { 
       par->Value(nm); 
       *par = ++nm;
   }
   else { fParams->Add(new SpdParameter("TsTECNForwardModules",++nm)); }
   
   parname = Form("TsTECFModule%dType",nm);
   par = GetParameter(parname);  
   if (par) { *par = mtype; }
   else { fParams->Add(new SpdParameter(parname,mtype)); }
   
   parname = Form("TsTECFModule%dDist",nm);
   par = GetParameter(parname);  
   if (par) { *par = distance; }
   else { fParams->Add(new SpdParameter(parname,distance)); }
   
   parname = Form("TsTECFModule%dPhi",nm);
   par = GetParameter(parname);  
   if (par) { *par = angle; }
   else { fParams->Add(new SpdParameter(parname,angle)); }
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::AddBackwardModule(Int_t mtype, Double_t distance, Double_t angle)
{
   Int_t nm = 0;
   SpdParameter* par;
   TString parname;

   par = GetParameter("TsTECNBackwardModules");
   if (par) { 
       par->Value(nm); 
       *par = ++nm;
   }
   else { fParams->Add(new SpdParameter("TsTECNBackwardModules",++nm)); }
   
   parname = Form("TsTECBModule%dType",nm);
   par = GetParameter(parname);  
   if (par) { *par = mtype; }
   else { fParams->Add(new SpdParameter(parname,mtype)); }
   
   parname = Form("TsTECBModule%dDist",nm);
   par = GetParameter(parname);  
   if (par) { *par = distance; }
   else { fParams->Add(new SpdParameter(parname,distance)); }
   
   parname = Form("TsTECBModule%dPhi",nm);
   par = GetParameter(parname);  
   if (par) { *par = angle; }
   else { fParams->Add(new SpdParameter(parname,angle)); }
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::RecalculateDistance(Int_t opt /*0,1,-1*/) 
{
   if (opt == 0) {
       RecalculateDistance(1);
       RecalculateDistance(-1);
       return;
   }
   
   if (opt != -1 && opt != 1) return;
     
   Int_t nm = GetNModules(opt);
   if (nm < 2) return;
  
   Double_t suml(0.);
   for (Int_t i(1); i<=nm; i++) suml += GetModuleLength(opt,i);
   
   Double_t len = (GetLength()- suml);
   
   if (len < 0) len = 0;
   else len /= (nm-1);
   
   const Char_t* mask;
   if (opt == 1) mask = "TsTECFModule";
   else if (opt == -1) mask = "TsTECBModule";
    
   SpdParameter* par;
   
   TString parname = Form("%s1Dist",mask);
   par = GetParameter(parname);     
   if (par) *par = 0.;
   
   for (Int_t i(2); i<=nm; i++) {
        parname = Form("%s%dDist",mask,i);
        par = GetParameter(parname);     
        if (par) *par = len;
   }
}

//_____________________________________________________________________________
Int_t SpdTsTECGeoMapper::GetNMaxLayers(Int_t ltype, Double_t lgapsize)
{
   const SpdParameter* par;
   TString parname;
   
   parname = Form("TsTECLayerType%dStype",ltype);
   par = GetParameter(parname);
   if (!par) {
       cout << "-W- <SpdTsTECGeoMapper::GetNMaxLayers> Layer of type " << ltype  
            << ": unknown straw type " << endl;
       return 0;
   } 
   
   Int_t stype;
   par->Value(stype);
   
   Double_t slen = GetStrawRadius(stype);
   
   if (!(slen > 0)) {
       cout << "-W- <SpdTsTECGeoMapper::GetNMaxLayers> Layer of type " << ltype  
            << ": zero straw size " << endl;
       return 0;     
   }
   
   Double_t zgapsize;
  
   if (!CheckLayerZgapsize(ltype,zgapsize)) {
       cout << "-W- <SpdTsTECGeoMapper::GetLayerLength> Layer of type " << ltype  
            << ": unknown straw zgapsize " << endl;
       return 0;
   } 
    
   if (lgapsize < 0) lgapsize = 0;
   
   return Int_t((GetLength() + lgapsize)/(4*slen + zgapsize + lgapsize));
}

//_____________________________________________________________________________
Bool_t SpdTsTECGeoMapper::IsGeoTypeDefined(Int_t gtype) const
{ 
   return (gtype == 3);
}
       
//_____________________________________________________________________________
Bool_t SpdTsTECGeoMapper::InitGeometry(Int_t gtype, Bool_t reinit) 
{ 
   if (fLockGeometry) {
       cout << "-W- <SpdTsTECGeoMapper::InitGeometry> Geometry is locked " << endl;
       return kFALSE;
   }
 
   if (!fParams) return kFALSE;
   
   if (fGeoTable) fGeoTable->Clear();
   if (fGeoVolPars) fGeoVolPars->Clear();
   
   fVolumesMap.clear();
       
   if (!CheckGeoType(gtype,"TsTECGeoType")) return kFALSE;
         
   //cout << "-I- <SpdTsTECGeoMapper::InitGeometry> reinit: " << reinit << endl;
      
   if (fGeoType == 3) SetParameters_3(reinit);
 
   return kTRUE;
}


//_____________________________________________________________________________
void SpdTsTECGeoMapper::SetParameters_3(Bool_t reinit)
{   
   //cout << "-I- <SpdTsTECGeoMapper::SetParameters_3> " << endl;
  
   static SpdCommonGeoMapper* mapper = SpdCommonGeoMapper::Instance();
   
   SpdParameter* par;
   
   Double_t Length = mapper->GetTsTECLength3();
   par = GetParameter("TsTECLength");  
   if (par) { if (reinit) *par = Length; else par->Value(Length); }
   else { if (Length > 0) fParams->Add(new SpdParameter("TsTECLength",Length)); }
   
   if (!(Length > 0)) 
   {
       cout << "-W- <SpdTsTECGeoMapper::SetParameters_3> "
            << "Module length = 0; " 
            << "(GeoType " << fGeoType << "->0)" << endl;
       
       fGeoType = 0;
       par = GetParameter("TsTECGeoType");  
       if (par) { *par = fGeoType; }
       else { fParams->Add(new SpdParameter("TsTECGeoType",fGeoType)); }
       
       return;
   }
           
   Double_t Size = mapper->GetTsTECSize();
   par = GetParameter("TsTECSize");  
   if (par) { if (reinit) *par = Size; else par->Value(Size); }
   else { fParams->Add(new SpdParameter("TsTECSize",Size)); }
   
   Double_t Width = mapper->GetTsTECWidth();
   par = GetParameter("TsTECWidth");
   if (par) { if (reinit) *par = Width; else par->Value(Width);  }
   else { fParams->Add(new SpdParameter("TsTECWidth",Width)); } 
   
   Double_t ZminDist = mapper->GetTsTECMinDist3();
   par = GetParameter("TsTECMinDistance");
   if (par) { if (reinit) *par = ZminDist; else par->Value(ZminDist);  }
   else { fParams->Add(new SpdParameter("TsTECMinDistance",ZminDist)); } 
   
   /* define materials */
    
   par = GetParameter("TsTECBaseMaterial");
   if (par) { if (reinit) *par = mapper->GetTsTECBaseMaterial(); }
   else { fParams->Add(new SpdParameter("TsTECBaseMaterial",mapper->GetTsTECBaseMaterial())); } 
   
    par = GetParameter("TsTECLayerMaterial");
   if (par) { if (reinit) *par = mapper->GetTsTECLayerMaterial(); }
   else { fParams->Add(new SpdParameter("TsTECLayerMaterial",mapper->GetTsTECLayerMaterial())); }  
    
   TString mat_straw_base = "arco27030";  // FIXME
   par = GetParameter("TsTECBaseStrawMaterial");
   if (par) { if (reinit) *par = mat_straw_base; else mat_straw_base = par->Value(); }
   else { fParams->Add(new SpdParameter("TsTECBaseStrawMaterial",mat_straw_base)); } 
  
   TString mat_straw_shell = "kapton2"; // FIXME
   par = GetParameter("TsTECStrawShellMaterial");
   if (par) { if (reinit) *par = mat_straw_shell; else mat_straw_shell = par->Value(); }
   else { fParams->Add(new SpdParameter("TsTECStrawShellMaterial",mat_straw_shell)); } 
   
   TString mat_straw_wire = "tungsten";   // FIXME
   par = GetParameter("TsTECStrawWireMaterial");
   if (par) { if (reinit) *par = mat_straw_wire; else mat_straw_wire = par->Value(); }
   else { fParams->Add(new SpdParameter("TsTECStrawWireMaterial",mat_straw_wire)); }
   
   /* check number of modules */
    
   Int_t nfmodules = 0;
   par = GetParameter("TsTECNForwardModules");
   if (par) { if (reinit) *par = nfmodules; else par->Value(nfmodules); }
   else { fParams->Add(new SpdParameter("TsTECNForwardModules",nfmodules)); } 
   
   Int_t nbmodules = 0;
   par = GetParameter("TsTECNBackwardModules");
   if (par) { if (reinit) *par = nbmodules; else par->Value(nbmodules);}
   else { fParams->Add(new SpdParameter("TsTECNBackwardModules",nbmodules)); }  
    
   /* define secondary parameters */
   
   Double_t ZmaxDist = ZminDist + Length;
   par = GetParameter("TsTECMaxDistance");  
   if (par) { *par = ZmaxDist; }
   else { fParams->Add(new SpdParameter("TsTECMaxDistance",ZmaxDist,1)); }
   
   Double_t MaxR = Size/Cos(Pi()/8.);
   par = GetParameter("TsTECMaxR");  
   if (par) { *par = MaxR; }
   else { fParams->Add(new SpdParameter("TsTECMaxR",MaxR,1)); }
     
   Double_t BoreSize = Size - Width;  // < 0 ?
   par = GetParameter("TsTECBoreSize");  
   if (par) { *par = BoreSize; }
   else { fParams->Add(new SpdParameter("TsTECBoreSize",BoreSize,1)); }
   
   /* ++++++++++ Build Geometry ++++++++++ */
   
   Int_t nmodules = nfmodules + nbmodules;
   
   if (nmodules == 0) {
       cout << "-I- <SpdTsTECGeoMapper::SetParameters_3> Create default geometry " << endl;
       SetGeometryPars(0);
   }
   
   nfmodules = GetNModules(1);
   nbmodules = GetNModules(-1);
    
   nmodules = CheckGeometry(nfmodules,nbmodules);
   
   par = GetParameter("TsTECNForwardModules");
   *par = nfmodules; 
   par = GetParameter("TsTECNBackwardModules");
   *par = nbmodules; 
   
   par = GetParameter("TsTECNModules");
   if (par) { *par = nmodules; }
   else { fParams->Add(new SpdParameter("TsTECNModules",nmodules,1)); }  
   
   if (nmodules == 0) {
       
       cout << "-E- <SpdTsTECGeoMapper::SetParameters_3> Geometry is empty" << endl;
       
       fGeoType = 0;
       par = GetParameter("TsTECGeoType");  
       if (par) { *par = fGeoType; }
       else { fParams->Add(new SpdParameter("TsTECGeoType",fGeoType)); }
       
       return;
   }
  
   TString hash, vname;
  
   TObjArray* Refuse = new TObjArray();
  
   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* ++++++++++++ DEFINE MODULES ++++++++++++ */ 
   /* ++++++++++++++++++++++++++++++++++++++++ */
   
   std::map<TString,SpdTsECStrawModule*> table_m;  // set of unique modules
   std::map<TString,SpdTsECStrawModule*>::iterator itm;
   
   SpdTsECStrawModule* module;
   
   std::map<TString,SpdTsECStrawModule*> modules;
   
   Int_t opt[2] = {1,-1};
   
   for (Int_t j(0); j < 2; j++) {
        
        int nm = GetNModules(opt[j]);
        
        for (Int_t i(1); i <= nm; i++) {
         
             /* ATTENTION +++++ INIT MODULE +++++ ATTENTION */
             
             module = InitStrawModulePacking(opt[j],i);
             
             if (!module) {
                 cout << "-E- <SpdTsTECGeoMapper::SetParameters_3> Bad module " 
                      << nm << " parameters " << endl;
                      continue;
             }
        
             hash = module->HashString();
             itm = table_m.find(hash);
          
             if (itm != table_m.end()) {
                 AddVolumeModule(itm->second->GetName(),opt[j]*i);
                 delete module;
                 continue;
             }
             
             vname = AddPrefix(Form("SModule%li",table_m.size()+1));
             
             AddVolumeModule(vname,opt[j]*i);
                 
             modules.insert(std::pair<TString,SpdTsECStrawModule*>(vname,module));     
        
             module->SetName(vname);
             module->SetMedia(GetBaseMaterial());
             module->SetActivity(0);  // FIXME set volume activity 
              
             module->SetLineColor(kYellow);
             module->SetFillColor(kYellow);
             module->SetTransparency(50);
             
             table_m.insert(std::pair<TString,SpdTsECStrawModule*>(hash,module)); 
             
             /* ATTENTION +++++ PACK MODULE +++++ ATTENTION */
             
             module->MakePacking();
             
             /* ATTENTION +++++ Add unique MODULE to the List of VolPars +++++ ATTENTION */ 
             
             fGeoVolPars->Add(module);  
        }
   }
   
   /* ATTENTION +++++ Fill GeoTable (LEVEL, Vol.Name, Mother Vol.Name, First node, Nodes) +++++ ATTENTION */
   
   for (itm = modules.begin(); itm != modules.end(); itm++)
   {
        module = itm->second;
        vname = module->GetName();
        Int_t nn = GetVolumeNModules(vname);
        fGeoTable->AddTableRec(0,vname,fMasterVolName,1,nn);
        module->AddNCopies(nn);
   }
   
   //fGeoTable->CloseTable(); return;
   
   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* ++++++++++++ DEFINE LAYERS ++++++++++++ */
   /* ++++++++++++++++++++++++++++++++++++++++ */
     
   std::map<TString,SpdTsECStrawLayer*> table_l;  // set of unique layers
   std::map<TString,SpdTsECStrawLayer*>::iterator itl;
   
   SpdTsECStrawLayer* layer;
   
   std::map<TString,SpdTsECStrawLayer*> layers;
   
   module = 0;
   
   for (itm = table_m.begin(); itm != table_m.end(); itm++) {
  
        module = itm->second;
              
        for (Int_t i(0); i < module->GetNNodes(); i++) {
                     
             layer = (SpdTsECStrawLayer*)module->MakeNodeVolPars(i);
                       
             /* ATTENTION +++++ INIT LAYER PACKING +++++ ATTENTION */
             
             if (!InitStrawLayerPacking(layer)) {
                 cout << "-E- <SpdTsTECGeoMapper::SetParameters_3> Bad layer (type: " 
                      << layer->GetType() << ") parameters " << endl;
                 continue;
             }
             
             hash = layer->HashString();
             itl = table_l.find(hash);
         
             if (itl != table_l.end()) {
                 module->SetVolName(i,itl->second->GetName()); // fill module vol. table (node_num, vol_name)
                 delete layer;
                 continue;
             }
             
             vname = AddPrefix(Form("SLayer%li",table_l.size()+1));
             
             layers.insert(std::pair<TString,SpdTsECStrawLayer*>(vname,layer));     
        
             module->SetVolName(i,vname); // fill module vol. table (node_num, vol_name)
             
             layer->SetName(vname);
             layer->SetMedia(GetLayerMaterial());
             layer->SetActivity(0);  // FIXME set volume activity 
              
             layer->SetLineColor(kRed+1);
             layer->SetFillColor(kRed+1);
             layer->SetTransparency(50);

             table_l.insert(std::pair<TString,SpdTsECStrawLayer*>(hash,layer)); 
             
             /* ATTENTION +++++ PACK LAYER +++++ ATTENTION */
 
             layer->MakePacking();
             
             /* ATTENTION +++++ Add unique LAYER to the List of VolPars +++++ ATTENTION */ 
             
             if (layer->GetNNodes() != 0) {
                 fGeoVolPars->Add(layer);   // WARNING add not empty layers only WARNING  
             }
             else Refuse->Add(layer);
             
        }
   }
      
   /* ATTENTION +++++ Fill GeoTable (LEVEL, Vol.Name, Mother Vol.Name, First node, Nodes) +++++ ATTENTION */
    
   /* ++++++ NOTE SIMPLIFICATION: 1 module <-> 1 layer NOTE ++++++ */
   
   for (itm = table_m.begin(); itm != table_m.end(); itm++) {
   
        module = itm->second;
  
        vname = module->GetVolName(0); // ! NOTE !
 
        itl = layers.find(vname);
        if (itl == layers.end()) continue;
    
        layer = itl->second;
        if (!layer) continue;
        
        Int_t nn = module->GetNNodes(vname);
        if (nn == 0) continue; // don't add empty layer to the table 
              
        fGeoTable->AddTableRec(1, vname, module->GetName(), layer->GetNCopies()+1, nn);
        
        layer->AddNCopies(nn);
   }
   
   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* ++++++++++++ DEFINE STRAWS +++++++++++++ */
   /* ++++++++++++++++++++++++++++++++++++++++ */

   std::map<TString,SpdTsECStraw*> table_s;  // set of unique straws
   std::map<TString,SpdTsECStraw*>::iterator its;
   
   SpdTsECStraw* straw;
   
   std::map<TString,SpdTsECStraw*> straws;
   
   for (itl = table_l.begin(); itl != table_l.end(); ++itl) {
     
       layer = itl->second;      
       if (!layer) continue;
       
       for (Int_t i(0); i < layer->GetNNodes(); i++) {
    
            straw = (SpdTsECStraw*)layer->MakeNodeVolPars(i); 
                
            hash = straw->HashString();
            its = table_s.find(hash);
              
            if (its != table_s.end()) {
                layer->SetVolName(i,its->second->GetName()); // fill layer vol. table (node_num, vol_name)
                delete straw;
                continue;
            }
                      
            vname = AddPrefix(Form("Straw%li",table_s.size()+1));
            
            straws.insert(std::pair<TString,SpdTsECStraw*>(vname,straw));    
        
            layer->SetVolName(i,vname); // fill layer vol. table (node_num, vol_name)
            
            straw->SetName(vname);
            straw->SetMedia(GetBaseStrawMaterial());
            straw->SetActivity(1);  // FIXME set volume activity 
        
            straw->SetLineColor(kRed);
            straw->SetFillColor(kRed);
            straw->SetTransparency(30);
            
            table_s.insert(std::pair<TString,SpdTsECStraw*>(hash,straw));
            
            /* ATTENTION +++++ Add unique STRAW to the List of VolPars +++++ ATTENTION */ 
         
            fGeoVolPars->Add(straw); 
       }  
   }
   
   /* ATTENTION +++++ Fill GeoTable (LEVEL, Vol.Name, Mother Vol.Name, First node, Nodes) +++++ ATTENTION */ 
   
   for (itl = table_l.begin(); itl != table_l.end(); ++itl) {
     
        layer = itl->second; 

        if (!layer) continue;

        const SpdGeoVolPars_VolNodes& volumes = layer->GetVolNodes();
        SpdGeoVolPars_VolNodes::const_iterator itx = volumes.begin();
   
        for (; itx != volumes.end(); itx++) 
        {
              vname = itx->first;
              
              its = straws.find(vname);
              if (its == straws.end()) continue;
              
              straw = its->second;
              
              Int_t nn = itx->second.size();     
              if (nn < 1) continue;
                   
              fGeoTable->AddTableRec(2, vname, layer->GetName(), straw->GetNCopies()+1, nn);
              
              straw->AddNCopies(nn);
        }
   } 
   
   /* ++++++++++++++++++++++++++++++++++++++++ */
   /* +++++++++ DEFINE STRAW SHELLS ++++++++++ */
   /* ++++++++++++++++++++++++++++++++++++++++ */
   
   SpdTsECStrawInternal* straw_shell;
   TString shellname;
   Double_t rmin, rmax;
   Double_t straw_shell_w, straw_wire_r;
   
   for (its = table_s.begin(); its != table_s.end(); ++its) {
     
        straw = its->second; 
        if (!straw) continue;
        
        GetStrawInners(straw->GetType(),straw_shell_w,straw_wire_r);
        
        // ----- make straw shell ----- 
        shellname = straw->GetName();
        shellname += "Shell";
        rmax = straw->GetRadius();
        rmin = rmax - straw_shell_w;
        
        straw_shell = straw->MakeShell(shellname,mat_straw_shell,rmin,rmax);
        straw_shell->SetActivity(0); // FIXME set volume activity 
       
        straw_shell->SetLineColor(kMagenta+2);
        straw_shell->SetFillColor(kMagenta+2);
        straw_shell->SetTransparency(60);
            
        fGeoVolPars->Add(straw_shell); // Add Shell to the List of VolPars
        fGeoTable->AddTableRec(3, shellname, straw->GetName(), 1, 1); // Fill GeoTable 
        
        //  ----- make straw wire ----- 
        shellname = straw->GetName();
        shellname += "Wire";
        
        straw_shell = straw->MakeShell(shellname,mat_straw_wire,0,straw_wire_r);
        straw_shell->SetActivity(0); // FIXME set volume activity 
         
        straw_shell->SetLineColor(kMagenta+5);
        straw_shell->SetFillColor(kMagenta+5);
        straw_shell->SetTransparency(0);
        
        fGeoVolPars->Add(straw_shell); // Add Wire to the List of VolPars
        fGeoTable->AddTableRec(3, shellname, straw->GetName(), 1, 1); // Fill GeoTable 
   }
   
   cout << "-I- <SpdTsTECGeoMapper::SetParameters_3> Volumes (add/reject): " 
        << fGeoVolPars->GetEntries() << "/" << Refuse->GetEntries() << endl;
        
   Refuse->Delete();
   delete Refuse;
  
   /* close geometry */   
   
   fGeoTable->CloseTable();
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::SetModulePars(Int_t mtype, Int_t ltype, Int_t nlayers,
                                      Double_t lgapsize /*cm*/, Int_t dphi)
{
   SpdParameter* par;
   TString parname;
   
   if (lgapsize < 0) lgapsize = 0.;
   
   /* check/add "global" parameters */

   par = GetParameter("TsTECLength");  
   if (!par) fParams->Add(new SpdParameter("TsTECLength",SpdCommonGeoMapper::Instance()->GetTsTECLength3())); 
           
   par = GetParameter("TsTECSize");  
   if (!par) fParams->Add(new SpdParameter("TsTECSize",SpdCommonGeoMapper::Instance()->GetTsTECSize()));
   
   par = GetParameter("TsTECWidth");
   if (!par) fParams->Add(new SpdParameter("TsTECWidth",SpdCommonGeoMapper::Instance()->GetTsTECWidth())); 
   
   /* set parameters */
    
   parname = Form("TsTECModuleType%dLtype",mtype);
   par = GetParameter(parname);  
   if (par) { *par = ltype; }
   else { fParams->Add(new SpdParameter(parname,ltype)); }
     
   parname = Form("TsTECModuleType%dLgapsize",mtype);
   par = GetParameter(parname);  
   if (par) { *par = lgapsize; }
   else { fParams->Add(new SpdParameter(parname,lgapsize)); }
   
   parname = Form("TsTECModuleType%dDphi",mtype);
   par = GetParameter(parname);  
   if (par) { *par = dphi; }
   else { fParams->Add(new SpdParameter(parname,dphi)); }
   
   if (nlayers < 1) nlayers = GetNMaxLayers(ltype,lgapsize); // ATTENTION calculate "on quite"
   
   parname = Form("TsTECModuleType%dNlayers",mtype);
   par = GetParameter(parname);      
   if (par) { *par = nlayers; }
   else { fParams->Add(new SpdParameter(parname,nlayers)); }
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::SetLayerPars(Int_t ltype, Int_t stype, 
                                     Double_t zgapsize /*cm*/, Double_t rgapsize/*cm*/,
                                     Int_t popt /*0 = "eo", 1 = "oe"*/,
                                     Double_t dR /*cm*/, Double_t dB/*cm*/,
                                     Double_t R /*cm*/, Double_t B/*cm*/)
{
   SpdParameter* par;
   TString parname;

   //if (zgapsize < 0) zgapsize = 0.;
   if (rgapsize < 0) rgapsize = 0.;
   
   if (dR < 0) dR = 0;
   if (dB < 0) dB = 0;
      
   // check layer bore size
   Double_t B_def =  GetSize()-GetWidth();
   if (!(B_def > 0)) B_def = SpdCommonGeoMapper::Instance()->GetTsTECSize()
                           - SpdCommonGeoMapper::Instance()->GetTsTECWidth();
   if (!(B > 0)) B = B_def;

   // check layer max. radius
   Double_t R_def = GetMaxR();
   if (!(R_def > B_def)) R_def = (B_def + SpdCommonGeoMapper::Instance()->GetTsTECWidth())/Cos(Pi()/8.);
   if (!(R > B) || (R > R_def)) R = R_def;
   
   parname = Form("TsTECLayerType%dStype",ltype);
   par = GetParameter(parname);  
   if (par) { *par = stype; }
   else { fParams->Add(new SpdParameter(parname,stype)); }
   
   parname = Form("TsTECLayerType%dZgapsize",ltype);
   par = GetParameter(parname);  
   if (par) { *par = zgapsize; }
   else { fParams->Add(new SpdParameter(parname,zgapsize)); }
   
   parname = Form("TsTECLayerType%dRgapsize",ltype);
   par = GetParameter(parname);  
   if (par) { *par = rgapsize; }
   else { fParams->Add(new SpdParameter(parname,rgapsize)); }
   
   parname = Form("TsTECLayerType%dPopt",ltype);
   par = GetParameter(parname);  
   if (par) { *par = popt; }
   else { fParams->Add(new SpdParameter(parname,popt)); }
   
   parname = Form("TsTECLayerType%dDRadius",ltype);
   par = GetParameter(parname);  
   if (par) { *par = dR; }
   else { fParams->Add(new SpdParameter(parname,dR)); }
   
   parname = Form("TsTECLayerType%dDBoresize",ltype);
   par = GetParameter(parname);  
   if (par) { *par = dB; }
   else { fParams->Add(new SpdParameter(parname,dB)); }
   
   parname = Form("TsTECLayerType%dRadius",ltype);
   par = GetParameter(parname);  
   if (par) { *par = R; }
   else { fParams->Add(new SpdParameter(parname,R)); }
   
   parname = Form("TsTECLayerType%dBoresize",ltype);
   par = GetParameter(parname);  
   if (par) { *par = B; }
   else { fParams->Add(new SpdParameter(parname,B)); }
   
   CheckLayerZgapsize(ltype,zgapsize);
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::SetStrawPars(Int_t stype, Double_t rstraw /*cm*/, 
                                     Double_t wshell /*cm*/, Double_t rwire /*cm*/)
{
   SpdParameter* par;
   TString parname;
   
   parname = Form("TsTECStrawType%dRstraw",stype);
   par = GetParameter(parname);  
   if (par) { *par = rstraw; }
   else { fParams->Add(new SpdParameter(parname,rstraw)); }
   
   parname = Form("TsTECStrawType%dWshell",stype);
   par = GetParameter(parname);  
   if (par) { *par = wshell; }
   else { fParams->Add(new SpdParameter(parname,wshell)); }  
   
   parname = Form("TsTECStrawType%dRwire",stype);
   par = GetParameter(parname);  
   if (par) { *par = rwire; }
   else { fParams->Add(new SpdParameter(parname,rwire)); }
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::SetParameters(Int_t module, Int_t layer, Int_t straw)
{
   if (module < 1) {
       cout << "-W- <SpdTsTECGeoMapper::SetParameters> "
                << "Bad module type " << module << endl;
       return;
   }
  
   SpdParameter* par;
   TString parname;
   
   Bool_t mdef(kFALSE), ldef(kFALSE), sdef(kFALSE);
   
   if (layer < 1 && module > 0 && module <= NDEFMODULES) layer = Lt_(module);
   if (straw < 1 && layer > 0 && layer <= NDEFLAYERS) straw = St_(layer);
       
   // check straw 
   if (straw < 1) {
       parname = Form("TsTECLayerType%dStype",layer);
       par = GetParameter(parname);  
       if (par) par->Value(straw);
       if (!par || straw < 1) { 
           cout << "-W- <SpdTsTECGeoMapper::SetParameters> "
                << "Undefined or bad parameter: " << parname << endl; 
           return;
       }
   }
      
   if (straw > NDEFSTRAWS) {
       parname = Form("TsTECStrawType%dRwire",straw);
       par = GetParameter(parname);  
       if (!par) {
           cout << "-W- <SpdTsTECGeoMapper::SetParameters> "
                << "Undefined parameter: " << parname << endl; 
           return;
       }
       sdef = kTRUE;
   }
   
   // check layer
   if (layer < 1) {
       parname = Form("TsTECModuleType%dLtype",module);
       par = GetParameter(parname); 
       par->Value(layer);
       if (!par || layer < 1) {
           cout << "-W- <SpdTsTECGeoMapper::SetParameters> "
                << "Undefined or bad parameter: " << parname << endl; 
           return;
       }
   }
   
   if (layer > NDEFLAYERS) {
       parname = Form("TsTECLayerType%dStype",layer);
       par = GetParameter(parname);  
       if (!par) { 
           cout << "-W- <SpdTsTECGeoMapper::SetParameters> "
                << "Undefined parameter: " << parname << endl; 
           return;
       }
       *par = straw;
       ldef = kTRUE;
   }
   
   // check module 
   if (module > NDEFMODULES) {
       parname = Form("TsTECModuleType%dLtype",layer);
       par = GetParameter(parname);  
       if (!par) {
           cout << "-W- <SpdTsTECGeoMapper::SetParameters> "
                << "Undefined parameter: " << parname << endl;    
           return;
       }
       *par = layer;
       mdef = kTRUE;
   }   
   
   cout << "-I- <SpdTsTECGeoMapper::SetParameters> module/layer/straw: "
                << module << "/" << layer << "/" << straw << endl; 
                
   if (!sdef) SetStrawPars(straw,Rs_(straw),Ws_(straw),Rw_(straw));
   if (!ldef) SetLayerPars(layer,straw,Zg_(layer),Rg_(layer),opt_(layer),
                                       dR_(layer),dB_(layer),R_(layer),B_(layer));
   if (!mdef) SetModulePars(module,layer,Nl_(module),Lg_(module),dphi_(module));
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::SetGeometryPars(Int_t g)
{
   if (g == 0)
   {
       Int_t nlayers = 8;
       Double_t lgap = 1.; // cm
       SetDefaultModulePars(1, 1, nlayers);
       SetDefaultModulePars(2, 1, nlayers);
       SetDefaultModulePars(1, 2, lgap);
       SetDefaultModulePars(2, 2, lgap);
       
       SetParameters(1);
       SetParameters(2);
       
       AddForwardModule(1, 0, 0);
       AddBackwardModule(2, 0, 0);
   }
   else {
      cout << "-W- <SpdTsTECGeoMapper::SetGeometryPars>"
           << " Unknown set of parameters: " << g << endl;
   }
}

//_____________________________________________________________________________
Int_t SpdTsTECGeoMapper::CheckGeometry(Int_t& nfmods, Int_t& nbmods)
{   
   cout << "-I- <SpdTsTECGeoMapper::CheckGeometry> Check forward/backward modules: " 
        << nfmods << "/" << nbmods << endl; 
        
   SpdParameter* par;
   TString parname;
   Double_t ssize, length, dist;
   Int_t mtype, im;
   
   /* --------------- check forward modules --------------- */
   
   ssize = GetMinDistance();
   im = 0;
   Int_t nmods;
   
   nmods = nfmods;
   while (im < nmods) 
   { 
      im++;
      
      parname = Form("TsTECFModule%dType",im);
      par = GetParameter(parname);
      if (!par) {
          cout << "-E- <SpdTsTECGeoMapper::CheckGeometry> "
               << "Unknown forward module " << im << " type " << endl;
          nfmods = 0;
          break;
      }
   
      par->Value(mtype);
      
      parname = Form("TsTECFModule%dDist",im);
      par = GetParameter(parname);
      if (!par) {
          cout << "-E- <SpdTsTECGeoMapper::CheckGeometry> "
               << "Forward module: parameter \"" << parname << "\" is not difined" << endl;
          nfmods = 0;
          break;
      }
      
      par->Value(dist);
      ssize += dist;
      
      parname = Form("TsTECFModule%dPhi",im);
      par = GetParameter(parname);
      if (!par) {
          cout << "-E- <SpdTsTECGeoMapper::CheckGeometry> "
               << "Forward module: parameter \"" << parname << "\" is not difined" << endl;
          nfmods = 0;
          break;
      }
      
      length = GetModuleLength(mtype);
     
      if (!(length > 0)) {
          cout << "-E- <SpdTsTECGeoMapper::CheckGeometry> "
               << "Forward module: parameter \"" << parname <<" \" is not difined" << endl;
          nfmods = 0;
          break;
      }
      
      ssize += length;
      
      if ((ssize - GetMaxDistance()) > 1.e-12) {
          cout << "-W- <SpdTsTECGeoMapper::CheckGeometry> "
               << "Forward module "<< im << " is out of range: " 
               << ssize << " > " << GetMaxDistance()  << " (delta = " << ssize-GetMaxDistance() << ")" << endl;
          nfmods--;
          continue;
      }
     
      /* define secondary parameters: length, size */
      
      parname = Form("TsTECFModule%dLength",im);
      par = GetParameter(parname);
      if (par) *par = length;
      else fParams->Add(new SpdParameter(parname,length,1));
        
      parname = Form("TsTECFModule%dSize",im);
      par = GetParameter(parname);
      if (par) *par = ssize;
      else fParams->Add(new SpdParameter(parname,ssize,1));
  
      cout << "-I- <SpdTsTECGeoMapper::CheckGeometry> Register forward module: " 
           << im << "; type: " << mtype 
           << "; length: " << length << "; size: " << ssize 
           << " (max: "<< GetMaxDistance() << ")" << endl;
   }
   
   /* --------------- check backward modules --------------- */
   
   ssize = GetMinDistance();
   im = 0;
   
   nmods = nbmods;
   while (im < nmods) 
   { 
      im++;
      
      parname = Form("TsTECBModule%dType",im);
      par = GetParameter(parname);
      if (!par) {
          cout << "-E- <SpdTsTECGeoMapper::CheckGeometry> "
               << "Unwnown backward module " << im << " type " << endl;
          nbmods = 0;
          break;
      }
   
      par->Value(mtype);
      
      parname = Form("TsTECBModule%dDist",im);
      par = GetParameter(parname);
      if (!par) {
          cout << "-E- <SpdTsTECGeoMapper::CheckGeometry> "
               << "Backward module parameter \"" << parname << "\" is not difined" << endl;
          nbmods = 0;
          break;
      }
      
      par->Value(dist);
      ssize += dist;
      
      parname = Form("TsTECBModule%dPhi",im);
      par = GetParameter(parname);
      if (!par) {
          cout << "-E- <SpdTsTECGeoMapper::CheckGeometry> "
               << "Backward module parameter \"" << parname << "\" is not difined" << endl;
          nbmods = 0;
          break;
      }
      
      length = GetModuleLength(mtype);
     
      if (!(length > 0)) {
          cout << "-E- <SpdTsTECGeoMapper::CheckGeometry> "
               << "Backward module: parameter \"" << parname << "\" is not difined" << endl;
          nbmods = 0;
          break;
      }
      
      ssize += length;
      
      if ((ssize - GetMaxDistance()) > 1.e-12) {
          cout << "-W- <SpdTsTECGeoMapper::CheckGeometry> "
               << "Backward module " << im << " is out of range: " 
               << ssize << " > " << GetMaxDistance() << " (delta = " << ssize-GetMaxDistance() << ")" << endl;
          nbmods--;
          continue;
      }
     
      /* define secondary parameters: length, size */
     
      parname = Form("TsTECBModule%dLength",im);
      par = GetParameter(parname);
      if (par) *par = length;
      else fParams->Add(new SpdParameter(parname,length,1));
        
      parname = Form("TsTECBModule%dSize",im);
      par = GetParameter(parname);
      if (par) *par = ssize;
      else fParams->Add(new SpdParameter(parname,ssize,1));
      
      cout << "-I- <SpdTsTECGeoMapper::CheckGeometry> Register backward module: number " 
           << im << "; type: " << mtype 
           << "; length: " << length << "; size: " << ssize 
           << " (max: "<< GetMaxDistance() << ")"<< endl;     
   }
   
   cout << "-I- <SpdTsTECGeoMapper::CheckGeometry> Add forward/backward modules: " 
        << nfmods << "/" << nbmods << endl; 
        
   return nfmods + nbmods;
}

//_____________________________________________________________________________
SpdTsECStrawModule* SpdTsTECGeoMapper::InitStrawModulePacking(Int_t opt /*1,-1*/, Int_t nmodule)
{
   Int_t mtype = GetModuleType(opt,nmodule);
    
   if (mtype < 1) {
       cout << "<SpdTsTECGeoMapper::InitStrawModulePacking> Unknown " 
            << ((opt == 1) ? "forward" : "backward")
            << "module type: "<< mtype << endl;
       return 0;
   }
   
   Double_t mlength = GetModuleLength(mtype);
   Double_t rmax = GetMaxR();
   Double_t rmin = GetSize() - GetWidth();
   
   if (!(mlength > 0) || (rmin < 0) || !(rmax > rmin)) {
       cout << "<SpdTsTECGeoMapper::InitStrawModulePacking> "
            << "Bad parameters for module of type: "<< mtype << endl;
       return 0;
   }
   
   Int_t ltype, nlayers;
   Double_t lgap, ldphi, llength, lradius, lbsize;   

   // searching for module packing parameters
    
   if (!GetModulePackingPars(mtype, ltype, nlayers, lgap, ldphi, llength, lradius, lbsize)) return 0;
   
   SpdTsECStrawModule* module = new SpdTsECStrawModule();
   
   module->SetModulePars(mtype,mlength,rmin,rmax); 
   
   if (!module->SetPackingPars(ltype,nlayers,lgap,ldphi,llength,lradius,lbsize)) return 0;
  
   return module;
}

//_____________________________________________________________________________
Bool_t SpdTsTECGeoMapper::GetModulePackingPars(Int_t mtype, Int_t& ltype, Int_t& nlayers, 
                          Double_t& lgap, Double_t& ldphi, 
                          Double_t& llength, Double_t& lradius, Double_t& lbsize)
{
   if (!IsGeoTypeDefined(fGeoType)) {
       cout << "-E- <SpdTsTECGeoMapper::GetModulePackingPars>"
            << " Unknown geometry type: " << fGeoType << endl;
       return kFALSE; 
   }
   
   if (mtype < 1) return kFALSE;
   
   SpdParameter* par;
   TString parname;
   
   // Default settings 
   // WARNING see corresponding values in SpdTsTECGeoMapper::SetModulePars 
   
   if (mtype > NDEFMODULES) {
       cout << "-I- <SpdTsTECGeoMapper::GetModulePackingPars>"
            << " Set parameters for (nondefault) module of type: " << mtype << endl;
   }
  
   parname = Form("TsTECModuleType%dLtype",mtype);
   par = GetParameter(parname);  
   if (par) { par->Value(ltype); }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetModulePackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   } 
   
   parname = Form("TsTECModuleType%dNlayers",mtype);
   par = GetParameter(parname);  
   if (par) { par->Value(nlayers); }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetModulePackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   } 
   
   parname = Form("TsTECModuleType%dLgapsize",mtype);
   par = GetParameter(parname);  
   if (par) { par->Value(lgap); }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetModulePackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   } 
   
   parname = Form("TsTECModuleType%dDphi",mtype);
   par = GetParameter(parname);  
   if (par) { Int_t dphi; par->Value(dphi); ldphi = dphi*45.; }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetModulePackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   } 
      
   parname = Form("TsTECLayerType%dRadius",ltype);
   par = GetParameter(parname);  
   if (par) { par->Value(lradius); }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetModulePackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   }
   
   parname = Form("TsTECLayerType%dBoresize",ltype);
   par = GetParameter(parname);  
   if (par) { par->Value(lbsize); }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetModulePackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   }
   
   /* define/add layer length as secondary parameter */
  
   parname = Form("TsTECLayerType%dLength",ltype);
   par = GetParameter(parname);  
   if (par) { par->Value(llength); }
   else { 
       llength = GetLayerLength(ltype);
       if (!(llength > 0)) {
           cout << "-E- <SpdTsTECGeoMapper::GetModulePackingPars> Unknown parameter " 
                <<"\"" << parname << "\" value\n"; 
           return kFALSE;
       }
       fParams->Add(new SpdParameter(parname,llength,1));
   } 
   
   return kTRUE; 
}

//_____________________________________________________________________________
Bool_t SpdTsTECGeoMapper::InitStrawLayerPacking(SpdTsECStrawLayer* layer)
{  
   Int_t ltype = layer->GetType();
   
   if (ltype < 1) {
       cout << "<SpdTsTECGeoMapper::InitStrawLayerPacking> Unknown layer type: " << ltype << endl;
       return kFALSE;
   }
   
   Int_t stype, popt;
   Double_t sradius, lzgap, lrgap, ldr, ldb;
  
   // searching for layer parameters
   if (!GetLayerPackingPars(ltype,stype,popt,sradius,lzgap,lrgap,ldr,ldb)) return kFALSE;
  
   if (!layer->SetPackingPars(stype,popt,sradius,lzgap,lrgap,ldr,ldb))  return kFALSE;
  
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdTsTECGeoMapper::GetLayerPackingPars(Int_t ltype, Int_t& stype, Int_t& popt,
                          Double_t& sradius, Double_t& lzgap, Double_t& lrgap,
                          Double_t& ldr, Double_t& ldb)
{
   if (ltype < 1) return kFALSE;
   
   SpdParameter* par;
   TString parname;
   
   // Default settings 
   // WARNING see corresponding values in SpdTsTECGeoMapper::SetLayerPars 
  
   parname = Form("TsTECLayerType%dStype",ltype);
   par = GetParameter(parname);  
   if (par) { par->Value(stype); }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetLayerPackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   }
   
   if (ltype > NDEFLAYERS) {
       cout << "-I- <SpdTsTECGeoMapper::GetLayerPackingPars>"
            << " Set parameters for (nondefault) layer of type: " << ltype << endl;
   }
   
   parname = Form("TsTECLayerType%dPopt",ltype);
   par = GetParameter(parname);  
   if (par) { par->Value(popt); }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetLayerPackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   } 
  
   if (!CheckLayerZgapsize(ltype,lzgap)) {
       cout << "-E- <SpdTsTECGeoMapper::GetLayerPackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   } 
  
   parname = Form("TsTECLayerType%dRgapsize",ltype);
   par = GetParameter(parname);  
   if (par) { par->Value(lrgap); }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetLayerPackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   } 
   
   parname = Form("TsTECLayerType%dDRadius",ltype);
   par = GetParameter(parname);  
   if (par) { par->Value(ldr); }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetLayerPackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   } 
   
   parname = Form("TsTECLayerType%dDBoresize",ltype);
   par = GetParameter(parname);  
   if (par) { par->Value(ldb); }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetLayerPackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   } 
   
   parname = Form("TsTECStrawType%dRstraw",stype);
   par = GetParameter(parname);  
   if (par) { par->Value(sradius); }
   else { 
       cout << "-E- <SpdTsTECGeoMapper::GetLayerPackingPars> Unknown parameter " 
            <<"\"" << parname << "\" value\n"; 
       return kFALSE;
   }
 
   return kTRUE;
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::Print(Option_t*) const
{
   cout << "-I- <SpdTsTECGeoMapper::Print>" << "\n\n";
   SpdGeoMapper::Print("");
}

//_____________________________________________________________________________
TString SpdTsTECGeoMapper::GetPrintVolParsCap() const
{
   return Form("%3s  %2s  %4s   %9s %9s %9s %9s   %9s %9s  %-25s %-15s %-15s %6s %6s", 
          "I", "A", "NC", 
          "Len/Width", "Size/N", "L/H(min)", "L/H(max)", 
          "xxxxx", "xxxxx",
          "Class", "Name", "Media",
          "NN", "NV"); 
}

//_____________________________________________________________________________
TGeoMatrix* SpdTsTECGeoMapper::GetNodeMatrix(const SpdGeoTableRec* rec, Int_t loc_i) const
{
   if (!rec) return 0;
     
   if (rec->GetMotherVolName() != fMasterVolName) {                  
       const SpdGeoVolPars* pars = GetVolPars(rec->GetMotherVolName()); 
       if (!pars) return 0;
       return pars->MakeNodeMatrix(rec->GetVolName(),loc_i);
   }
   
   /* define top level (module) matrix */
   
   TGeoCombiTrans* m = 0;
   
   TString mname  = rec->GetVolName();
  
   if (fGeoType == 3) {
      
       Int_t nmodule = GetVolumeNModule(mname,loc_i);
       if (nmodule == 0) return 0;
       
       if (nmodule > 0) mname = AddPrefix(Form("FModule%d",Abs(nmodule)));
       else mname = AddPrefix(Form("BModule%d",Abs(nmodule)));
    
       Double_t pos = 0, x;
       
       const SpdParameter* par;
       TString parname;
       
       parname = mname + "Size";
       par = GetParameter(parname);
       if (!par) return 0;
       par->Value(x);
       
       pos += x;
       
       parname = mname + "Length";
       par = GetParameter(parname);
       if (!par) return 0;
       par->Value(x);
       
       pos -= 0.5*x;
       
       parname = mname + "Phi";
       par = GetParameter(parname);
       if (!par) return 0;
       par->Value(x);
       
       if (parname.Contains("TsTECBModule")) pos = -pos;
       
       m = new TGeoCombiTrans(0,0,pos,0);
       m->RotateZ(x);
       
       return m;
   }
 
   cout << "-E- <SpdTsTECGeoMapper::GetModuleGeoMatrix> "
        << "Unknown geometry type: " << fGeoType << endl;
 
   return m;
}

//_____________________________________________________________________________
void SpdTsTECGeoMapper::AddVolumeModule(TString volname, Int_t nmodule)
{
   /* add secondary parameter */
     
   TString parname; 
   if (nmodule > 0) parname = Form("TsTECFModule%dVolName",nmodule);
   else parname = Form("TsTECBModule%dVolName",Abs(nmodule));
   
   SpdParameter* par = GetParameter(parname);
   if (par) *par = volname;
   else fParams->Add(new SpdParameter(parname,volname,1));
   
   /* add module */
    
   std::map < TString, std::vector<Int_t> >::iterator it = fVolumesMap.find(volname);
   
   if (it != fVolumesMap.end()) {
       it->second.push_back(nmodule);
       return;
   }
   else {
       std::vector<Int_t> v;
       v.push_back(nmodule);
       fVolumesMap.insert(std::pair< TString, std::vector<Int_t> > (volname,v)); 
   }
}

//_____________________________________________________________________________
Int_t SpdTsTECGeoMapper::GetVolumeNModules(TString volname) const
{
   std::map < TString, std::vector<Int_t> >::const_iterator it = fVolumesMap.find(volname);
   return (it != fVolumesMap.end()) ? it->second.size() : 0;
}

//_____________________________________________________________________________
Int_t SpdTsTECGeoMapper::GetVolumeNModule(TString volname, Int_t ncopy) const
{
   std::map < TString, std::vector<Int_t> >::const_iterator it = fVolumesMap.find(volname);
   if (it == fVolumesMap.end()) return 0;
   return (ncopy < it->second.size()) ? it->second[ncopy] : 0;
}

//_____________________________________________________________________________
Bool_t SpdTsTECGeoMapper::CheckLayerZgapsize(Int_t ltype, Double_t& zgapsize)
{  
   zgapsize = 0;
  
   SpdParameter* zgap = GetParameter(Form("TsTECLayerType%dZgapsize",ltype));
   if (!zgap) {
       cout << "-W- <SpdTsTECGeoMapper::CheckLayerZgapsize> Layer of type " << ltype  
            << ": unknown straw zgapsize " << endl;
       return  kFALSE;
   } 
  
   zgap->Value(zgapsize);
   
   if (!(zgapsize < 0)) return kTRUE;
   
   const SpdParameter* par;
   TString parname;
  
   parname = Form("TsTECLayerType%dRgapsize",ltype);
   par = GetParameter(parname);  
   if (!par) {
       cout << "-E- <SpdTsTECGeoMapper::CheckLayerZgapsize>  Layer of type " << ltype  
             << ": unknown straw rgapsize " << endl;
       return kFALSE;
   } 
   
   Double_t dr;
   par->Value(dr);
   
   parname = Form("TsTECLayerType%dStype",ltype);
   par = GetParameter(parname);
   if (!par) {
       cout << "-W- <SpdTsTECGeoMapper::CheckLayerZgapsize> Layer of type " << ltype  
            << ": unknown straw type " << endl;
       return kFALSE;
   } 
   
   Int_t stype;
   par->Value(stype);
      
   Double_t r = GetStrawRadius(stype);
   
   if (!(r > 0)) {
       cout << "-W- <SpdTsTECGeoMapper::CheckLayerZgapsize> Layer of type " << ltype  
            << ": zero straw radius " << endl;
       return kFALSE;     
   }
   
   Double_t dzmin = -2*r;
   if (dr < 2*r) dzmin += TMath::Sqrt(4*r*r - (r+0.5*dr)*(r+0.5*dr));
   
   //cout << "-W- <SpdTsTECGeoMapper::CheckLayerZgapsize> "
   //     << "zgapsize = " << zgapsize << ", dzmin = " << dzmin << endl;
            
   if (zgapsize < dzmin) {
       //cout << "-W- <SpdTsTECGeoMapper::CheckLayerZgapsize> gapsize is changed"
       //     << " from " << zgapsize << " to " << dzmin << endl;
       zgapsize = dzmin;    
   }
   
   //zgapsize = 0;
   *zgap = zgapsize;

   return kTRUE;
}
