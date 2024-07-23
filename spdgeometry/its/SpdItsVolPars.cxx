// $Id$
// Author: artur   2019/03/07

//_____________________________________________________________________________
//
// SpdItsChip
// SpdItsLadder
// SpdItsLayer
//_____________________________________________________________________________

#include "SpdItsVolPars.h"

using namespace TMath;

#include <iostream>
using std::cout;
using std::endl;

#include <TGeoMatrix.h>
#include <vector>

ClassImp(SpdItsChip)
ClassImp(SpdItsLadder)
ClassImp(SpdItsLayer)

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsChip                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdItsChip::SpdItsChip():SpdGeoVolPars(""),
fLz(0),fLphi(0),fLr(0),fType("")
{
 
}

//_____________________________________________________________________________
SpdItsChip::SpdItsChip(const Char_t* name):SpdGeoVolPars(name),
fLz(0),fLphi(0),fLr(0),fType("")
{
 
}

//_____________________________________________________________________________
void SpdItsChip::Clear() 
{
  fLz = 0;
  fLphi = 0;
  fLr = 0;
  fType = "";
}

//_____________________________________________________________________________
TString SpdItsChip::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
 
  hash += Form("%d",IRoundCMBase2(fLz));
  hash += Form("%d",IRoundCMBase2(fLphi));
  hash += Form("%d",IRoundCMBase2(fLr));
  hash += fType;
  
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdItsChip::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdItsChip* p = dynamic_cast<SpdItsChip*>(pars);
  if (!p) return kFALSE;

  if (IRoundCMBase2(fLz)     != IRoundCMBase(p->GetLz()))     return kFALSE;  
  if (IRoundCMBase2(fLphi)   != IRoundCMBase(p->GetLphi()))   return kFALSE;
  if (IRoundCMBase2(fLr)     != IRoundCMBase(p->GetLr()))     return kFALSE;  

  if (fType != p->GetType()) return kFALSE;
    
  return kTRUE; 
}

//_____________________________________________________________________________
SpdItsChip::~SpdItsChip() 
{
 
}

//_____________________________________________________________________________
void SpdItsChip::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  %4d   [%9.4f %9.4f %9.4f] (  %-15s  ) %-20s %-20s %-15s %6d %6d", 
          i, fActivity, fNCopies, 0,
          fLz, fLphi, fLr, 
          fType.Data(),       
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsLadder                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdItsLadder::SpdItsLadder():SpdGeoVolPars(""),
fLength(0),fLphi(0),fLr(0),
fPackingChip("undefined"),
fPackingNzChips(0),fPackingNphiChips(0),
fPackingChipLz(0),fPackingChipLphi(0),
fPackingChipDLz(0),fPackingChipDLphi(0)
{
 
}

//_____________________________________________________________________________
SpdItsLadder::SpdItsLadder(const Char_t* name):SpdGeoVolPars(name),
fLength(0),fLphi(0),fLr(0),
fPackingChip("undefined"),
fPackingNzChips(0),fPackingNphiChips(0),
fPackingChipLz(0),fPackingChipLphi(0),
fPackingChipDLz(0),fPackingChipDLphi(0)
{
 
}

//_____________________________________________________________________________
SpdItsLadder::~SpdItsLadder() 
{
 
}

//_____________________________________________________________________________
void SpdItsLadder::Clear()
{
  SpdGeoVolPars::Clear();
  
  fLength = 0;
  fLphi = 0;
  fLr = 0;
  
  fPackingChip = "undefined";
 
  fPackingNzChips = 0;
  fPackingNphiChips = 0;
 
  fPackingChipLz = 0; 
  fPackingChipLphi = 0;
 
  fPackingChipDLz = 0;
  fPackingChipDLphi = 0;
    
  ClearNodes();
}

//_____________________________________________________________________________
Bool_t SpdItsLadder::SetPackingPars(TString chip, Int_t nz, Int_t nphi,
                                     Double_t lz,  Double_t lphi, 
                                     Double_t dlz, Double_t dlphi)
{
  ClearNodes();
  
  fPackingChip = chip;
 
  fPackingNzChips   = (nz < 1)   ? 1 : nz;
  fPackingNphiChips = (nphi < 1) ? 1 : nphi;
  
  fPackingChipLz    = DRoundCMBase2(lz); 
  fPackingChipLphi  = DRoundCMBase2(lphi);
 
  fPackingChipDLz   = (dlz   < 0) ? -1. : DRoundCMBase2(dlz);  
  fPackingChipDLphi = (dlphi < 0) ? -1. : DRoundCMBase2(dlphi);
 
  return kTRUE;
}

//_____________________________________________________________________________      
Bool_t SpdItsLadder::MakePacking()
{
  if ( !(fPackingNzChips > 0)  || !(fPackingNphiChips > 0) ) {
      cout << "-E- <SpdItsLadder::MakePacking> Bad packing parameters " << endl;
      return kFALSE;
  }
  
  if ( !(fPackingChipLphi > 0)  || !(fPackingChipLz > 0) ) {
      cout << "-E- <SpdItsLadder::MakePacking> Bad packing parameters " << endl;
      return kFALSE;
  }
  
  ClearNodes();
  
  std::vector<Double_t> v(4,0);
  
  v[0] = -1; // "status"
  v[1] = fPackingChipLz; 
  v[2] = fPackingChipLphi;
  v[3] = fLr;
  
  fNodesPars.push_back(v);
  
  return kTRUE;
}

//_____________________________________________________________________________   
TString SpdItsLadder::HashString(Int_t type) const
{
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
    
  hash += Form("%d",IRoundCMBase2(fLength));
  hash += Form("%d",IRoundCMBase2(fLphi));
  hash += Form("%d",IRoundCMBase2(fLr));
  
  hash += fPackingChip;
   
  hash += Form("%d",fPackingNzChips);
  hash += Form("%d",fPackingNphiChips);
 
  hash += Form("%d",IRoundCMBase2(fPackingChipLz));
  hash += Form("%d",IRoundCMBase2(fPackingChipLphi));
 
  hash += Form("%d",IRoundCMBase2(fPackingChipDLz));
  hash += Form("%d",IRoundCMBase2(fPackingChipDLphi));
   
  return hash.MD5(); 
}
    
//_____________________________________________________________________________       
Bool_t SpdItsLadder::Compare(SpdGeoVolPars* pars, Int_t type) const
{
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdItsLadder* p = dynamic_cast<SpdItsLadder*>(pars);
  if (!p) return kFALSE;
  
  if (IRoundCMBase2(fLength) != IRoundCMBase(p->GetLength())) return kFALSE;
  if (IRoundCMBase2(fLphi)   != IRoundCMBase(p->GetLphi()))   return kFALSE;
  if (IRoundCMBase2(fLr)     != IRoundCMBase(p->GetLr()))     return kFALSE;
  
  if (fPackingChip != p->GetPackingChip()) return kFALSE;
  
  if (fPackingNzChips != p->GetPackingNzChips()) return kFALSE;
  if (fPackingNphiChips != p->GetPackingNphiChips()) return kFALSE;
 
  if (IRoundCMBase2(fPackingChipLz)    != IRoundCMBase2(p->GetPackingChipLz()))    return kFALSE;
  if (IRoundCMBase2(fPackingChipLphi)  != IRoundCMBase2(p->GetPackingChipLphi()))  return kFALSE;
 
  if (IRoundCMBase2(fPackingChipDLz)   != IRoundCMBase2(p->GetPackingChipDLz()))   return kFALSE; 
  if (IRoundCMBase2(fPackingChipDLphi) != IRoundCMBase2(p->GetPackingChipDLphi())) return kFALSE;

  return kTRUE; 
}

//_____________________________________________________________________________
SpdGeoVolPars* SpdItsLadder::MakeNodeVolPars(Int_t nnode)  const 
{ 
  if (nnode < 0 || nnode >= GetNPackingChips() && fNodesPars.empty()) return 0;
 
  const std::vector<Double_t>& pars = fNodesPars[0];
  
  if (pars.empty()) return 0;
  
  SpdItsChip* chip = new SpdItsChip();
  chip->SetChipPars(pars[1],pars[2],pars[3],fPackingChip);
  
  return chip; 
}

//_____________________________________________________________________________
TGeoMatrix* SpdItsLadder::MakeNodeMatrix(Int_t nnode) const 
{ 
  //return 0;
  
  if (nnode < 0 || nnode >= GetNPackingChips() && fNodesPars.empty()) return 0;
 
  const std::vector<Double_t>& pars = fNodesPars[0];
   
  if (pars.empty()) return 0; 
  
  Double_t zshift   = fPackingChipLz   + fPackingChipDLz;
  Double_t phishift = fPackingChipLphi + fPackingChipDLphi;
  
  Double_t z0   = -0.5 * (fPackingNzChips - 1) * zshift;
  Double_t phi0 = -0.5 * (fPackingNphiChips - 1) * phishift;
  
  Double_t z   = z0   + Int_t(nnode/fPackingNphiChips) * zshift;                            
  Double_t phi = phi0 + Int_t(nnode%fPackingNphiChips) * phishift;
  
  return new TGeoTranslation(phi,0,z);
}

//_____________________________________________________________________________
void SpdItsLadder::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  %4d   [%9.4f %9.4f %9.4f] (%9d %9d) %-20s %-20s %-15s %6d %6d", 
          i, fActivity, fNCopies, GetNPackingChips(),
          fLength, fLphi, fLr, 
          fPackingNphiChips, fPackingNzChips, 
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}   

//_____________________________________________________________________________
TGeoMatrix* SpdItsLadder::MakeNodeMatrix(TString volname, Int_t loc_i) const
{
   return (fVolNodes.begin()->first == volname) ? MakeNodeMatrix(loc_i) : 0; 
}

//_____________________________________________________________________________
void SpdItsLadder::SetVolName(Int_t nnode, TString volname)
{
   if (nnode >= GetNPackingChips()) return;
   
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
Int_t SpdItsLadder::GetNNodes() const
{
   return GetNPackingChips();
}

//_____________________________________________________________________________
Int_t SpdItsLadder::GetNVolumes() const
{
   return 1; 
}

//_____________________________________________________________________________
Int_t SpdItsLadder::GetNNodes(TString volname) const
{
   if (fVolNodes.empty()) return 0;
   return (fVolNodes.begin()->first == volname) ? GetNPackingChips() : 0;
}

//_____________________________________________________________________________
TString SpdItsLadder::GetVolName(Int_t nnode) const
{
   if (nnode < 0 || nnode > GetNPackingChips()) return "";
   return fVolNodes.begin()->first;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsLayer                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdItsLayer::SpdItsLayer():SpdGeoVolPars(""),
fLength(0),fRmin(0),fRmax(0),fPackingNLadders(0),fPackingLadderRadius(0),
fPackingLadderLz(0),fPackingLadderLphi(0),fPackingLadderLr(0),fPackingLadderAngle(0)
{
  
}

//_____________________________________________________________________________
SpdItsLayer::SpdItsLayer(const Char_t* name):SpdGeoVolPars(name), 
fLength(0),fRmin(0),fRmax(0),fPackingNLadders(0),fPackingLadderRadius(0),
fPackingLadderLz(0),fPackingLadderLphi(0),fPackingLadderLr(0),fPackingLadderAngle(0)
{
  
}

//_____________________________________________________________________________
SpdItsLayer::~SpdItsLayer() 
{
 
}

//_____________________________________________________________________________
void SpdItsLayer::Clear()
{
  SpdGeoVolPars::Clear();
  
  fLength = 0;
  fRmin = 0;
  fRmax = 0;
 
  fPackingNLadders = 0;
  fPackingLadderRadius = 0;
  fPackingLadderLz = 0;
  fPackingLadderLphi = 0;
  fPackingLadderLr = 0; 
  fPackingLadderAngle = 0;
  
  ClearNodes();
}

//_____________________________________________________________________________
Bool_t SpdItsLayer::SetPackingPars(Int_t nplates, Double_t rlayer, 
                                   Double_t lz, Double_t lphi, Double_t lr, 
                                   Double_t angle)
{
  ClearNodes();
  
  if (nplates < 1) return kFALSE;
  
  fPackingNLadders = nplates;
  fPackingLadderRadius = rlayer;
  fPackingLadderLz = lz;
  fPackingLadderLphi = lphi;
  fPackingLadderLr = lr; 
  fPackingLadderAngle = angle;
  
  return kTRUE;
}

//_____________________________________________________________________________
TString SpdItsLayer::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  return fName;
}

//_____________________________________________________________________________
Bool_t SpdItsLayer::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  return (fName == pars->GetName());
}

//_____________________________________________________________________________
Bool_t SpdItsLayer::MakePacking()
{
  if ( !(fPackingLadderRadius > 0) || !(fPackingLadderLz > 0) ||
       !(fPackingLadderLphi > 0) || !(fPackingLadderLr > 0) ) 
  {
      cout << "-E- <SpdItsLayer::MakePacking> Bad packing parameters " << endl;
      return kFALSE;
  }
  
  ClearNodes();
 
  std::vector<Double_t> v(4,0);
  
  v[0] = -1; // "status"
  v[1] = fPackingLadderLz;
  v[2] = fPackingLadderLphi;  
  v[3] = fPackingLadderLr;  
  
  fNodesPars.push_back(v);
  
  return kTRUE;
}

//_____________________________________________________________________________
SpdGeoVolPars* SpdItsLayer::MakeNodeVolPars(Int_t nnode)  const 
{ 
  if (nnode < 0 || nnode >= fPackingNLadders && fNodesPars.empty()) return 0;
 
  const std::vector<Double_t>& pars = fNodesPars[0];
   
  if (pars.empty()) return 0;
   
  SpdItsLadder* ladder = new SpdItsLadder();
  ladder->SetLadderPars(pars[1],pars[2],pars[3]);
  return ladder; 
}

//_____________________________________________________________________________
TGeoMatrix* SpdItsLayer::MakeNodeMatrix(Int_t nnode) const 
{ 
  if (nnode < 0 || nnode >= fPackingNLadders && fNodesPars.empty()) return 0;
 
  const std::vector<Double_t>& pars = fNodesPars[0];
   
  if (pars.empty()) return 0;

  TGeoTranslation trans(0,fPackingLadderRadius,0);
  
  TGeoRotation rot;
  rot.RotateZ(fPackingLadderAngle);
   
  TGeoCombiTrans* m = new TGeoCombiTrans(trans,rot);
  m->RotateZ(nnode*360./fPackingNLadders);
  
  return m;
}

//_____________________________________________________________________________
void SpdItsLayer::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  %4d   [%9.4f %9.4f %9.4f] (%9.4f %9.4f) %-20s %-20s %-15s %6d %6d", 
          i, fActivity, fNCopies, fPackingNLadders,
          fLength, fRmin, fRmax, 
          fPackingLadderRadius, fPackingLadderAngle,       
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
} 

//_____________________________________________________________________________
TGeoMatrix* SpdItsLayer::MakeNodeMatrix(TString volname, Int_t loc_i) const
{
   return (fVolNodes.begin()->first == volname) ? MakeNodeMatrix(loc_i) : 0; 
}

//_____________________________________________________________________________
void SpdItsLayer::SetVolName(Int_t nnode, TString volname)
{
   if (nnode >= fPackingNLadders) return;
   
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
Int_t SpdItsLayer::GetNNodes() const
{
   return fPackingNLadders;
}

//_____________________________________________________________________________
Int_t SpdItsLayer::GetNVolumes() const
{
   return 1; 
}

//_____________________________________________________________________________
Int_t SpdItsLayer::GetNNodes(TString volname) const
{
   if (fVolNodes.empty()) return 0;
   return (fVolNodes.begin()->first == volname) ? fPackingNLadders : 0;
}

//_____________________________________________________________________________
TString SpdItsLayer::GetVolName(Int_t nnode) const
{
   if (nnode < 0 || nnode > fPackingNLadders) return "";
   return fVolNodes.begin()->first;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsEClayer                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdItsEClayer::SpdItsEClayer():SpdGeoVolPars(""),
fLength(0),fRmin(0),fRmax(0)
{
 
}

//_____________________________________________________________________________
SpdItsEClayer::SpdItsEClayer(const Char_t* name):SpdGeoVolPars(name),
fLength(0),fRmin(0),fRmax(0)
{
 
}

//_____________________________________________________________________________
SpdItsEClayer::~SpdItsEClayer() 
{
 
}

//_____________________________________________________________________________
void SpdItsEClayer::Clear()
{
  SpdGeoVolPars::Clear();
 
  fLength = 0;
  fRmin = 0;
  fRmax = 0;
  
  ClearNodes();
}

//_____________________________________________________________________________
TString SpdItsEClayer::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
 
  TString hash(ClassName());
 
  hash += Form("%d",IRoundCMBase2(fLength));
  hash += Form("%d",IRoundCMBase2(fRmin));
  hash += Form("%d",IRoundCMBase2(fRmax));
  
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdItsEClayer::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdItsEClayer* p = dynamic_cast<SpdItsEClayer*>(pars);
  if (!p) return kFALSE;
  
  if (IRoundCMBase2(fLength) != IRoundCMBase2(p->GetLength())) return kFALSE;
  if (IRoundCMBase2(fRmin)   != IRoundCMBase2(p->GetRmin()))   return kFALSE;
  if (IRoundCMBase2(fRmax)   != IRoundCMBase2(p->GetRmax()))   return kFALSE;
 
  return kTRUE; 
}

//_____________________________________________________________________________
void SpdItsEClayer::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  %4d   [%9.4f %9.4f %9.4f] (%9.4f %9.4f) %-20s %-20s %-15s %6d %6d", 
          i, fActivity, fNCopies, 0,
          fLength, fRmin, fRmax, 
          0., 0.,       
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}   



