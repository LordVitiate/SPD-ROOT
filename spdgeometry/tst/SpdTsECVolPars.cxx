// $Id$
// Author: artur   2018/05/11


//_____________________________________________________________________________
//
// SpdTsECStrawInternal
// SpdTsECStraw
// SpdTsECStrawLayer
// SpdTsECStrawModule
//_____________________________________________________________________________

#include "SpdTsECVolPars.h"

using namespace TMath;

#include <iostream>
using std::cout;
using std::endl;

#include <TGeoMatrix.h>

ClassImp(SpdTsECStrawInternal)
ClassImp(SpdTsECStraw)
ClassImp(SpdTsECStrawLayer)
ClassImp(SpdTsECStrawModule)
ClassImp(SpdTsECStrawModuleArb)
ClassImp(SpdTsECStrawModulePgon)

Double_t SpdTsECStraw::fMinStrawSize = 1.; //cm

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStrawInternal                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsECStrawInternal::SpdTsECStrawInternal():SpdGeoVolPars("") 
{
  Clear();
}

//_____________________________________________________________________________
SpdTsECStrawInternal::SpdTsECStrawInternal(const Char_t* name):SpdGeoVolPars(name) 
{
 
}

//_____________________________________________________________________________
SpdTsECStrawInternal::~SpdTsECStrawInternal() 
{
 
}

//_____________________________________________________________________________
void SpdTsECStrawInternal::Clear()
{
  SpdGeoVolPars::Clear();
      
  fLength = 0;
  fOuterRadius = 0;
  fWidth = 0;
}

//_____________________________________________________________________________
TString SpdTsECStrawInternal::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
  
  hash += Form("%d",IRoundCMBase2(fLength));
  hash += Form("%d",IRoundCMBase2(fOuterRadius));
  hash += Form("%d",IRoundCMBase2(fWidth));
  
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsECStrawInternal::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsECStrawInternal* p = dynamic_cast<SpdTsECStrawInternal*>(pars);
  if (!p) return kFALSE;
  
  if (IRoundCMBase2(fLength) != IRoundCMBase2(p->GetLength())) return kFALSE;
  if (IRoundCMBase2(fOuterRadius) != IRoundCMBase2(p->GetOuterRadius())) return kFALSE;
  if (IRoundCMBase2(fWidth) != IRoundCMBase2(p->GetWidth())) return kFALSE;
    
  return kTRUE; 
}

//_____________________________________________________________________________
void SpdTsECStrawInternal::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9.4f %9.4f %9.4f] (%9.4f %9.4f) %-25s %-15s %-15s %6d %6d", 
          i, fActivity, fNCopies,
          fLength, fWidth, GetInnerRadius(), GetOuterRadius(), 
          0., 0., 
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}   

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStraw                                                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsECStraw::SpdTsECStraw():SpdGeoVolPars(""),fType(0),fLength(0),fRadius(0)
{
    
}

//_____________________________________________________________________________
SpdTsECStraw::SpdTsECStraw(const Char_t* name):SpdGeoVolPars(name),
fType(0),fLength(0),fRadius(0)
{
 
}

//_____________________________________________________________________________
SpdTsECStraw::~SpdTsECStraw() 
{
 
}

//_____________________________________________________________________________
void SpdTsECStraw::Clear()
{
  SpdGeoVolPars::Clear();
  
  fType   = 0;
  fLength = 0;
  fRadius = 0;
}

//_____________________________________________________________________________
TString SpdTsECStraw::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
  
  hash += Form("%d",fType);
  
  hash += Form("%d",IRoundCMBase2(fLength));
  hash += Form("%d",IRoundCMBase2(fRadius));
  
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsECStraw::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsECStraw* p = dynamic_cast<SpdTsECStraw*>(pars);
  if (!p) return kFALSE;
  
  if (fType != p->GetType()) return kFALSE;
  
  if (IRoundCMBase2(fLength) != IRoundCMBase2(p->GetLength())) return kFALSE;
  if (IRoundCMBase2(fRadius) != IRoundCMBase2(p->GetRadius())) return kFALSE;
    
  return kTRUE; 
}

//_____________________________________________________________________________
TGeoMatrix* SpdTsECStraw::MakeNodeMatrix(TString volname, Int_t loc_i) const
{
  return 0;
  //return new TGeoCombiTrans();
}
    
//_____________________________________________________________________________
SpdTsECStrawInternal* SpdTsECStraw::MakeShell(TString volname, TString media, 
                                              Double_t rmin, Double_t rmax) const
{
  SpdTsECStrawInternal* pars = new SpdTsECStrawInternal(volname);
  
  Double_t oradius, w;
  (rmax > fRadius) ? oradius = fRadius : oradius = rmax;
  (rmax > rmin) ? w = rmax-rmin : w = 0;
  
  pars->SetStrawInternalPars(fLength,oradius,w);
  pars->SetMedia(media);
  
  return pars;
}
  
//_____________________________________________________________________________
void SpdTsECStraw::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9.4f %9.4f %9.4f] (%9.4f %9.4f) %-25s %-15s %-15s %6d %6d", 
          i, fActivity, fNCopies,
          fLength, 2*fRadius, 0., GetRadius(), 
          0., 0., 
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStrawModuleArb                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsECStrawModuleArb::SpdTsECStrawModuleArb():
SpdGeoVolPars(""),fLength(0),fH(0),fLmin(0),fLmax(0)
{
 
}

//_____________________________________________________________________________
SpdTsECStrawModuleArb::SpdTsECStrawModuleArb(const Char_t* name):
SpdGeoVolPars(name),fLength(0),fH(0),fLmin(0),fLmax(0)
{
 
}

//_____________________________________________________________________________
SpdTsECStrawModuleArb::~SpdTsECStrawModuleArb() 
{
 
}

//_____________________________________________________________________________
void SpdTsECStrawModuleArb::Clear()
{
  SpdGeoVolPars::Clear();
   
  fLength = 0;
  fH = 0;
  fLmin = 0;
  fLmax = 0;
  
  ClearNodes();
}

//_____________________________________________________________________________
TString SpdTsECStrawModuleArb::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
  
  hash += Form("%d",IRoundCMBase(fLength));
  hash += Form("%d",IRoundCMBase(fH));
  hash += Form("%d",IRoundCMBase(fLmin));
  hash += Form("%d",IRoundCMBase(fLmax));
 
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsECStrawModuleArb::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsECStrawModuleArb* p = dynamic_cast<SpdTsECStrawModuleArb*>(pars);
  if (!p) return kFALSE;
  
  if (IRoundCMBase(fLength) != IRoundCMBase(p->GetLength())) return kFALSE;
  if (IRoundCMBase(fH)      != IRoundCMBase(p->GetH()))      return kFALSE;
  if (IRoundCMBase(fLmin)   != IRoundCMBase(p->GetLmin()))   return kFALSE;
  if (IRoundCMBase(fLmax)   != IRoundCMBase(p->GetLmax()))   return kFALSE;
 
  return kTRUE; 
}

//_____________________________________________________________________________
void SpdTsECStrawModuleArb::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9.4f %9.4f %9.4f] (%9.4f %9.4f) %-25s %-15s %-15s %6d %6d", 
          i, fActivity, fNCopies,
          fLength, fH, fLmin, fLmax, 
          0., 0.,       
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}   

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStrawModulePgon                                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsECStrawModulePgon::SpdTsECStrawModulePgon():
SpdGeoVolPars(""),fNSectors(0),fLength(0),fHmin(0),fHmax(0)
{
 
}

//_____________________________________________________________________________
SpdTsECStrawModulePgon::SpdTsECStrawModulePgon(const Char_t* name):
SpdGeoVolPars(name),fNSectors(0),fLength(0),fHmin(0),fHmax(0)
{
 
}

//_____________________________________________________________________________
SpdTsECStrawModulePgon::~SpdTsECStrawModulePgon() 
{
 
}

//_____________________________________________________________________________
void SpdTsECStrawModulePgon::Clear()
{
  SpdGeoVolPars::Clear();
   
  fNSectors = 0;
  fLength = 0;
  fHmin = 0;
  fHmax = 0;
  
  ClearNodes();
}

//_____________________________________________________________________________
TString SpdTsECStrawModulePgon::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
  
  hash += Form("%d",fNSectors);
  
  hash += Form("%d",IRoundCMBase(fLength));
  hash += Form("%d",IRoundCMBase(fHmin));
  hash += Form("%d",IRoundCMBase(fHmax));
 
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsECStrawModulePgon::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsECStrawModulePgon* p = dynamic_cast<SpdTsECStrawModulePgon*>(pars);
  if (!p) return kFALSE;
  
  if (fNSectors != p->GetNSectors()) return kFALSE;

  if (IRoundCMBase(fLength) != IRoundCMBase(p->GetLength())) return kFALSE;
  if (IRoundCMBase(fHmin)   != IRoundCMBase(p->GetHmin()))   return kFALSE;
  if (IRoundCMBase(fHmax)   != IRoundCMBase(p->GetHmax()))   return kFALSE;
 
  return kTRUE; 
}

//_____________________________________________________________________________
void SpdTsECStrawModulePgon::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9d %9.4f %9.4f] (%9.4f %9.4f) %-25s %-15s %-15s %6d %6d", 
          i, fActivity, fNCopies,
          fLength, fNSectors, fHmin, fHmax, 
          0., 0.,       
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}   

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStrawModule                                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsECStrawModule::SpdTsECStrawModule():SpdGeoVolPars(""),
fType(0),fLength(0),fRmin(0),fRmax(0),
fPackingLayerType(0),fPackingNLayers(0),fPackingLayerGap(0),fPackingLayerDphi(0),
fPackingLayerLength(0),fPackingLayerRadius(0),fPackingLayerBSize(0)
{
 
}

//_____________________________________________________________________________
SpdTsECStrawModule::SpdTsECStrawModule(const Char_t* name):SpdGeoVolPars(name),
fType(0),fLength(0),fRmin(0),fRmax(0),
fPackingLayerType(0),fPackingNLayers(0),fPackingLayerGap(0),fPackingLayerDphi(0),
fPackingLayerLength(0),fPackingLayerRadius(0),fPackingLayerBSize(0)
{
 
}

//_____________________________________________________________________________
SpdTsECStrawModule::~SpdTsECStrawModule() 
{
 
}

//_____________________________________________________________________________
void SpdTsECStrawModule::Clear()
{
  SpdGeoVolPars::Clear();
  
  fType = 0;
  fLength = 0;
  fRmin = 0;
  fRmax = 0;
  
  fPackingLayerType = 0;
  fPackingNLayers = 0;
  fPackingLayerGap = 0;
  fPackingLayerDphi = 0;
  
  fPackingLayerLength = 0;
  fPackingLayerRadius = 0;
  fPackingLayerBSize = 0;
  
  ClearNodes();
}

//_____________________________________________________________________________
Bool_t SpdTsECStrawModule::SetPackingPars(Int_t ltype, Int_t nlayers, 
                           Double_t lgap /*cm*/, Double_t dphi /*deg*/,  
                           Double_t l /*cm*/, Double_t r/*cm*/, Double_t b/*cm*/)
{
  ClearNodes();
  
  fPackingLayerType   = ltype;
  fPackingNLayers     = nlayers;
  fPackingLayerGap    = DRoundCMBase2(lgap);
  fPackingLayerDphi   = DRoundDEG(dphi);
  
  fPackingLayerLength = DRoundCMBase2(l); 
  fPackingLayerRadius = DRoundCMBase2(r);
  fPackingLayerBSize  = DRoundCMBase2(b);
  
  return kTRUE;
}
     
//_____________________________________________________________________________
TString SpdTsECStrawModule::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
 
  TString hash(ClassName());
 
  hash += Form("%d",fType);
  
  hash += Form("%d",IRoundCMBase2(fLength));
  hash += Form("%d",IRoundCMBase2(fRmin));
  hash += Form("%d",IRoundCMBase2(fRmax));
  
  hash += Form("%d",fPackingLayerType);
  
  hash += Form("%d",IRoundCMBase2(fPackingNLayers)); 
  hash += Form("%d",IRoundCMBase2(fPackingLayerGap));
  hash += Form("%d",IRoundDEG(fPackingLayerDphi));
  
  hash += Form("%d",IRoundCMBase2(fPackingLayerLength)); 
  hash += Form("%d",IRoundCMBase2(fPackingLayerRadius));
  hash += Form("%d",IRoundCMBase2(fPackingLayerBSize));
 
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsECStrawModule::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsECStrawModule* p = dynamic_cast<SpdTsECStrawModule*>(pars);
  if (!p) return kFALSE;
  
  if (fType != p->GetType()) return kFALSE;
  
  if (IRoundCMBase2(fLength) != IRoundCMBase2(p->GetLength())) return kFALSE;
  if (IRoundCMBase2(fRmin)   != IRoundCMBase2(p->GetRmin()))   return kFALSE;
  if (IRoundCMBase2(fRmax)   != IRoundCMBase2(p->GetRmax()))   return kFALSE;
  
  if (fPackingLayerType != p->GetPackingLayerType()) return kFALSE;
  
  if (IRoundCMBase2(fPackingNLayers)   !=  p->GetPackingNLayers())   return kFALSE;
  if (IRoundCMBase2(fPackingLayerGap)  !=  p->GetPackingLayerGap())  return kFALSE;
  if (IRoundCMBase2(fPackingLayerDphi) !=  p->GetPackingLayerDphi()) return kFALSE;
  
  if (IRoundCMBase2(fPackingLayerLength)  != p->GetPackingLayerLength()) return kFALSE;  
  if (IRoundCMBase2(fPackingLayerRadius) != p->GetPackingLayerRadius())  return kFALSE; 
  if (IRoundCMBase2(fPackingLayerBSize)  != p->GetPackingLayerBSize())   return kFALSE;  
 
  return kTRUE; 
}

//_____________________________________________________________________________
Bool_t SpdTsECStrawModule::MakePacking()
{
  if ( !(fPackingLayerType > 0)   || !(fPackingNLayers > 0)     || (fPackingLayerGap < 0)  ||
       !(fPackingLayerLength > 0) || !(fPackingLayerRadius > 0) || !(fPackingLayerBSize > 0) 
       ) 
  {
      cout << "-E- <SpdTsECStrawModule::MakePacking> Bad packing parameters " << endl;
      return kFALSE;
  }
 
  ClearNodes();
 
  Double_t suml = fPackingLayerLength*fPackingNLayers 
                + fPackingLayerGap*(fPackingNLayers-1);
  
  suml = DRoundCMBase2(suml);
  
  if (suml > fLength){
      cout << "-E- <SpdTsECStrawModule::MakePacking> Bad packing parameter: " 
           << " total length " << fLength << " < " << " sum. length " << suml << endl;
      return kFALSE;
  }
  
  std::vector<Double_t> v(4,0);
  
  v[0] = -1; // "status"
  v[1] = fPackingLayerLength;
  v[2] = fPackingLayerRadius;  
  v[3] = fPackingLayerBSize;  
  
  fNodesPars.push_back(v);
  
  return kTRUE;    
}
//_____________________________________________________________________________
SpdGeoVolPars* SpdTsECStrawModule::MakeNodeVolPars(Int_t nnode) const 
{ 
  if (nnode < 0 || nnode >= 2*fPackingNLayers && fNodesPars.empty()) return 0;
 
  const std::vector<Double_t>& pars = fNodesPars[0];
   
  if (pars.empty()) return 0;
   
  SpdTsECStrawLayer* layer = new SpdTsECStrawLayer();
  layer->SetLayerPars(fPackingLayerType,pars[1],pars[2],pars[3]);
  return layer; 
}

//_____________________________________________________________________________
TGeoMatrix* SpdTsECStrawModule::MakeNodeMatrix(Int_t nnode) const 
{ 
  if (nnode < 0 || nnode >= 2*fPackingNLayers && fNodesPars.empty()) return 0;
 
  const std::vector<Double_t>& pars = fNodesPars[0];
   
  if (pars.empty()) return 0;

  Int_t nl = Int_t(nnode/2);
  
  Double_t shift = 0;
  if (fPackingNLayers == 1) shift = -0.5*fPackingLayerLength;
  else shift = -0.5*(fPackingNLayers*fPackingLayerLength + (fPackingNLayers-1)*fPackingLayerGap);
  shift += (nl+0.5)*fPackingLayerLength + nl*fPackingLayerGap;
  
  TGeoTranslation trans(0,0,shift);
  
  TGeoRotation rot;
  if (nnode & 1) rot.RotateZ(Int_t(fPackingLayerDphi*nl)%180+180); // odd node
  else rot.RotateZ(Int_t(fPackingLayerDphi*nl)%180);   // even node
    
  return new TGeoCombiTrans(trans,rot);
  
  return 0; 
}

//_____________________________________________________________________________
void SpdTsECStrawModule::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9d %9.4f %9.4f] (%9.4f %9.4f) %-25s %-15s %-15s %6d %6d", 
          i, fActivity, fNCopies,
          fLength, fPackingNLayers, fRmin, fRmax, 
          fPackingLayerLength, fPackingLayerGap,       
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}   

//_____________________________________________________________________________
TGeoMatrix* SpdTsECStrawModule::MakeNodeMatrix(TString volname, Int_t loc_i) const
{
   return (fVolNodes.begin()->first == volname) ? MakeNodeMatrix(loc_i) : 0; 
}

//_____________________________________________________________________________
void SpdTsECStrawModule::SetVolName(Int_t nnode, TString volname)
{
   if (nnode >= 2*fPackingNLayers) return;
   
   if (fNodesPars.empty()) return;
   
   nnode = 0;
   
   if (fNodesPars[nnode][0] == 1) return;
  
   fNodesPars[nnode][0] = 1; 
   
   if (fVolNodes.empty()) {
       std::vector<Int_t> v;
       v.push_back(nnode);
       fVolNodes.insert(std::pair< TString, std::vector<Int_t> >(volname,v));
       return;
   }
}

//_____________________________________________________________________________
Int_t SpdTsECStrawModule::GetNNodes() const
{
   return 2*fPackingNLayers;
}

//_____________________________________________________________________________
Int_t SpdTsECStrawModule::GetNVolumes() const
{
   return 1; 
}

//_____________________________________________________________________________
Int_t SpdTsECStrawModule::GetNNodes(TString volname) const
{
   if (fVolNodes.empty()) return 0;
   return (fVolNodes.begin()->first == volname) ? 2*fPackingNLayers : 0;
}

//_____________________________________________________________________________
TString SpdTsECStrawModule::GetVolName(Int_t nnode) const
{
   if (nnode < 0 || nnode > 2*fPackingNLayers) return "";
   return fVolNodes.begin()->first;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStrawLayer                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsECStrawLayer::SpdTsECStrawLayer():SpdGeoVolPars(""),
fType(0),fLength(0),fRadius(0),fBoreSize(0),
fPackingStrawType(0),fPackingStrawOption(0),fPackingStrawRadius(0),
fPackingStrawZGap(0),fPackingStrawRGap(0),
fPackingLayerDR(0),fPackingLayerDB(0)
{
  memset(fXVertices, 0, NVERTICES*sizeof(Double_t));
  memset(fYVertices, 0, NVERTICES*sizeof(Double_t));
}

//_____________________________________________________________________________
SpdTsECStrawLayer::SpdTsECStrawLayer(const Char_t* name):SpdGeoVolPars(name),
fType(0),fLength(0),fRadius(0),fBoreSize(0),
fPackingStrawType(0),fPackingStrawOption(0),fPackingStrawRadius(0),
fPackingStrawZGap(0),fPackingStrawRGap(0),
fPackingLayerDR(0),fPackingLayerDB(0)
{
  memset(fXVertices, 0, NVERTICES*sizeof(Double_t));
  memset(fYVertices, 0, NVERTICES*sizeof(Double_t));
}

//_____________________________________________________________________________
SpdTsECStrawLayer::~SpdTsECStrawLayer() 
{
  
}

//_____________________________________________________________________________
void SpdTsECStrawLayer::Clear()
{
  SpdGeoVolPars::Clear();
  
  fType = 0;
  fLength = 0;
  fRadius = 0;
  fBoreSize = 0;
  
  fPackingStrawType = 0;
  fPackingStrawOption = 0;
  fPackingStrawRadius = 0;
  fPackingStrawZGap = 0;
  fPackingStrawRGap = 0;
  
  fPackingLayerDR = 0;
  fPackingLayerDB = 0;  
    
  memset(fXVertices, 0, NVERTICES*sizeof(Double_t));
  memset(fYVertices, 0, NVERTICES*sizeof(Double_t));
  
  ClearNodes();
}

//_____________________________________________________________________________
void SpdTsECStrawLayer::SetLayerPars(Int_t type, Double_t length, 
                                     Double_t radius , Double_t bsize)
{
  fType     = type;
  fLength   = length;
  fRadius   = radius;
  fBoreSize = bsize;
  
  const Double_t phi0 = Pi()/8.;
  
  fXVertices[0] = fBoreSize;          fYVertices[0] = fRadius*Cos(phi0);
  fXVertices[1] = fRadius*Sin(phi0);  fYVertices[1] = fRadius*Cos(phi0);
  fXVertices[2] = fRadius*Cos(phi0);  fYVertices[2] = fRadius*Sin(phi0);
  fXVertices[3] = fXVertices[2];      fYVertices[3] = -fYVertices[2];
  fXVertices[4] = fXVertices[1];      fYVertices[4] = -fYVertices[1];
  fXVertices[5] = fXVertices[0];      fYVertices[5] = -fYVertices[0];
}

//_____________________________________________________________________________
Bool_t SpdTsECStrawLayer::SetPackingPars(Int_t stype, Int_t option, Double_t sradius, 
                                         Double_t zgap, Double_t rgap,   
                                         Double_t dr, Double_t db)
{ 
  ClearNodes();
 
  fPackingStrawType   = stype;
  fPackingStrawOption = (option > 0) ? 1 : 0;
  
  fPackingStrawRadius = DRoundCMBase2(sradius);
  fPackingStrawZGap   = DRoundCMBase2(zgap);
  fPackingStrawRGap   = DRoundCMBase2(rgap);
  
  fPackingLayerDR     = DRoundCMBase2(dr);
  fPackingLayerDB     = DRoundCMBase2(db);  
  
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdTsECStrawLayer::MakePacking()
{  
    if ( !(fPackingStrawType > 0) || !(fPackingStrawRadius > 0) || 
        (fPackingStrawRGap < 0) || (fPackingLayerDR < 0) || (fPackingLayerDB < 0) 
    )   
    {
        cout << "-E- <SpdTsECStrawLayer::MakePacking> Bad packing parameters " << endl;
        return kFALSE;
    }
    
    Double_t r = fPackingStrawRadius;
    Double_t dr = fPackingStrawRGap;
    Double_t dzmin = -2*r;
    if (dr < 2*r) dzmin += TMath::Sqrt(4*r*r - (r+0.5*dr)*(r+0.5*dr));
    
    if (dzmin > fPackingStrawZGap) {
        cout << "-E- <SpdTsECStrawLayer::MakePacking> Bad packing parameter " 
             << "ZGAP = " << fPackingStrawZGap << " < min. value = " << dzmin << endl;
        return kFALSE;
    }
    
    ClearNodes();
    
    Double_t xv[NVERTICES];
    Double_t yv[NVERTICES]; 
    
    const Double_t phi0 = Pi()/8.;
    const Double_t R = fRadius - fPackingLayerDR/Cos(phi0);
    const Double_t B = fBoreSize + fPackingLayerDB;
    
    xv[0] = B;            yv[0] = R*Cos(phi0);
    xv[1] = R*Sin(phi0);  yv[1] = R*Cos(phi0);
    xv[2] = R*Cos(phi0);  yv[2] = R*Sin(phi0);
    xv[3] = xv[2];        yv[3] = -yv[2];
    xv[4] = xv[1];        yv[4] = -yv[1];
    xv[5] = xv[0];        yv[5] = -yv[0];
    
    Double_t dz = fPackingStrawZGap;
    Int_t option = fPackingStrawOption;
    Double_t min_size = SpdTsECStraw::GetMinStrawSize();
    
    Double_t L = xv[2] - xv[0];
    Double_t l = 2*r + dr;
    Double_t ds = 0.5*l;
    Int_t nn = Int_t((L+dr)/l);
    Int_t ns = 2*nn-1;
    
    Double_t xx = xv[0] + r;
    
    Double_t au = (yv[2]-yv[1])/(xv[2]-xv[1]);
    Double_t bu = yv[1] - xv[1]*au;
    
    Double_t al = (yv[3]-yv[4])/(xv[3]-xv[4]);
    Double_t bl = yv[4] - xv[4]*al;
    
    Double_t x[2] = {xx - r, xx + r};
    Double_t y[2], yl, yu, yy;
    Double_t h;
    
    Double_t zz = r + 0.5*dz;
    
    if (nn & 1) { // max. odd
        if (option == 0) zz = -zz;
    }
    else {       // max. even 
        if (option != 0) zz = -zz;
    }
    
    for (Int_t i(0); i<ns; i++) 
    {
        // Lower tube end 
        for (Int_t j(0); j<2; j++) {
                 if (xv[0] <= x[j] && x[j] <= xv[1]) y[j] = yv[5];
            else if (xv[1] <= x[j] && x[j] <= xv[2]) y[j] = al*x[j] + bl; 
            else y[j] = 0;
        }
        yl = (y[0] > y[1]) ? y[0] : y[1];
        
        // Upper tube end
        for (Int_t j(0); j<2; j++) {
                 if (xv[0] <= x[j] && x[j] <= xv[1]) y[j] = yv[0];
            else if (xv[1] <= x[j] && x[j] <= xv[2]) y[j] = au*x[j] + bu; 
            else y[j] = 0;
        }
        yu = (y[0] < y[1]) ? y[0] : y[1];
        
        h = yu - yl;
        
        if (!(h < min_size)) 
        {
            // Tube center
            xx = x[0] + r;
            yy = 0.5*(yl + yu);
            
            std::vector<Double_t> v(5,0);
            v[0] = -1;  // "status"
            v[1] = xx;  // -> x-shift
            v[2] = yy;  // -> y-shift
            v[3] = zz;  // -> z-shift    
            v[4] = h;   // length 
            fNodesPars.push_back(v);
        }
        
        x[0] += ds;
        x[1] += ds; 
        zz = -zz;
    }
    
    return kTRUE;
}

//_____________________________________________________________________________
TString SpdTsECStrawLayer::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
  
  hash += Form("%d",fType);
  
  hash += Form("%d",IRoundCMBase2(fLength));
  hash += Form("%d",IRoundCMBase2(fRadius));
  hash += Form("%d",IRoundCMBase2(fBoreSize));
  
  hash += Form("%d",fPackingStrawType);
  hash += Form("%d",fPackingStrawOption);
  
  hash += Form("%d",IRoundCMBase2(fPackingStrawRadius));
  hash += Form("%d",IRoundCMBase2(fPackingStrawZGap)); 
  hash += Form("%d",IRoundCMBase2(fPackingStrawRGap));
  
  hash += Form("%d",IRoundCMBase2(fPackingLayerDR));
  hash += Form("%d",IRoundCMBase2(fPackingLayerDB));
 
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsECStrawLayer::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsECStrawLayer* p = dynamic_cast<SpdTsECStrawLayer*>(pars);
  if (!p) return kFALSE;
  
  if (fType != p->GetType()) return kFALSE;
  
  if (IRoundCMBase2(fLength)   != IRoundCMBase2(p->GetLength()))   return kFALSE;
  if (IRoundCMBase2(fRadius)   != IRoundCMBase2(p->GetRadius()))   return kFALSE;
  if (IRoundCMBase2(fBoreSize) != IRoundCMBase2(p->GetBoreSize())) return kFALSE;
  
  if (fPackingStrawType   != p->GetPackingStrawType())   return kFALSE;
  if (fPackingStrawOption != p->GetPackingStrawOption()) return kFALSE;
  
  if (IRoundCMBase2(fPackingStrawRadius) !=  p->GetPackingStrawRadius()) return kFALSE;
  if (IRoundCMBase2(fPackingStrawZGap)   !=  p->GetPackingStrawZGap())   return kFALSE;
  if (IRoundCMBase2(fPackingStrawRGap)   !=  p->GetPackingStrawRGap())   return kFALSE;
  
  if (IRoundCMBase2(fPackingLayerDR) != p->GetPackingLayerDR()) return kFALSE; 
  if (IRoundCMBase2(fPackingLayerDB) != p->GetPackingLayerDB()) return kFALSE;
 
  return kTRUE; 
}

//_____________________________________________________________________________
SpdGeoVolPars* SpdTsECStrawLayer::MakeNodeVolPars(Int_t nnode) const // protected
{ 
  if ( !(nnode <fNodesPars.size()) ) return 0;
  
  const std::vector<Double_t>& pars = fNodesPars[nnode];
  
  SpdTsECStraw* straw = new SpdTsECStraw();
  straw->SetStrawPars(fPackingStrawType,pars[4],fPackingStrawRadius);
  
  return straw; 
}

//_____________________________________________________________________________
TGeoMatrix* SpdTsECStrawLayer::MakeNodeMatrix(Int_t nnode)  const // protected
{ 
  if ( !(nnode <fNodesPars.size()) ) return 0;

  const std::vector<Double_t>& pars = fNodesPars[nnode];
  
  if (pars.empty()) return 0;
  
  TGeoTranslation trans(pars[1],pars[2],pars[3]); 
  TGeoRotation rot;
  rot.RotateX(90);
  
  return new TGeoCombiTrans(trans,rot);  
}

//_____________________________________________________________________________
void SpdTsECStrawLayer::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9d %9.4f %9.4f] (%9.4f %9.4f) %-25s %-15s %-15s %6d %6d", 
          i, fActivity, fNCopies,
          fLength, GetNNodes(), fBoreSize, fRadius, 
          fPackingStrawRadius, fPackingStrawZGap,       
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}   



    
