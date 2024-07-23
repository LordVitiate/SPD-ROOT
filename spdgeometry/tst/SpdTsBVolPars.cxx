// $Id$
// Author: artur   2018/05/11
//         ruslan  2023/02/21

//_____________________________________________________________________________
//
// SpdTsBStrawInternal
// SpdTsBStraw
// SpdTsBStrawDLayer
// SpdTsBStrawModuleH
//
// SpdTsBStrawLayer
// SpdTsBStrawModule
//_____________________________________________________________________________

#include "SpdTsBVolPars.h"

using namespace TMath;

#include <iostream>
using std::cout;
using std::endl;

#include <TGeoMatrix.h>

ClassImp(SpdTsBStrawInternal)
ClassImp(SpdTsBStraw)
ClassImp(SpdTsBStrawDLayer)
ClassImp(SpdTsBStrawModuleH)

Double_t SpdTsBStraw::fMinStrawSize = 10.; //cm

Double_t SpdTsBStrawDLayer::fMinLayerSize = 0.1; //cm


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsBStrawInternal                                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsBStrawInternal::SpdTsBStrawInternal():SpdGeoVolPars("") 
{
  Clear();
}

//_____________________________________________________________________________
SpdTsBStrawInternal::SpdTsBStrawInternal(const Char_t* name):SpdGeoVolPars(name) 
{
 
}

//_____________________________________________________________________________
SpdTsBStrawInternal::~SpdTsBStrawInternal() 
{
 
}

//_____________________________________________________________________________
void SpdTsBStrawInternal::Clear()
{
  SpdGeoVolPars::Clear();
      
  fLength = 0;
  fOuterRadius = 0;
  fWidth = 0;
}

//_____________________________________________________________________________
TString SpdTsBStrawInternal::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
  
  hash += Form("%d",IRoundCM(fLength));
  hash += Form("%d",IRoundCMBase(fOuterRadius));
  hash += Form("%d",IRoundCMBase(fWidth));
  
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawInternal::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsBStrawInternal* p = dynamic_cast<SpdTsBStrawInternal*>(pars);
  if (!p) return kFALSE;
  
  if (IRoundCM(fLength) != IRoundCM(p->GetLength())) return kFALSE;
  if (IRoundCMBase(fOuterRadius) != IRoundCMBase(p->GetOuterRadius())) return kFALSE;
  if (IRoundCMBase(fWidth) != IRoundCMBase(p->GetWidth())) return kFALSE;
    
  return kTRUE; 
}

//_____________________________________________________________________________
void SpdTsBStrawInternal::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9.4f %9.4f %9.4f] (%9.4f %9.4f) %-20s %-20s %-15s %6d %6d", 
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
// SpdTsBStraw                                                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsBStraw::SpdTsBStraw():SpdGeoVolPars(""),fLength(0),fRadius(0)
{
    
}

//_____________________________________________________________________________
SpdTsBStraw::SpdTsBStraw(const Char_t* name):SpdGeoVolPars(name) 
{
 
}

//_____________________________________________________________________________
SpdTsBStraw::~SpdTsBStraw() 
{
 
}

//_____________________________________________________________________________
void SpdTsBStraw::Clear()
{
  SpdGeoVolPars::Clear();
      
  fLength = 0;
  fRadius = 0;
}

//_____________________________________________________________________________
TString SpdTsBStraw::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
  
  hash += Form("%d",IRoundCM(fLength));
  hash += Form("%d",IRoundCMBase(fRadius));
  
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsBStraw::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsBStraw* p = dynamic_cast<SpdTsBStraw*>(pars);
  if (!p) return kFALSE;
  
  if (IRoundCM(fLength) != IRoundCM(p->GetLength())) return kFALSE;
  if (IRoundCMBase(fRadius) != IRoundCMBase(p->GetRadius())) return kFALSE;
    
  return kTRUE; 
}

//_____________________________________________________________________________
TGeoMatrix* SpdTsBStraw::MakeNodeMatrix(TString volname, Int_t loc_i) const
{
  return 0;
  //return new TGeoCombiTrans();
}
    
//_____________________________________________________________________________
SpdTsBStrawInternal* SpdTsBStraw::MakeShell(TString volname, TString media, 
                                            Double_t rmin, Double_t rmax) const
{
  SpdTsBStrawInternal* pars = new SpdTsBStrawInternal(volname);
  
  Double_t oradius, w;
  (rmax > fRadius) ? oradius = fRadius : oradius = rmax;
  (rmax > rmin) ? w = rmax-rmin : w = 0;
  
  pars->SetStrawInternalPars(fLength,oradius,w);
  pars->SetMedia(media);
  
  return pars;
}
  
//_____________________________________________________________________________
void SpdTsBStraw::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9.4f %9.4f %9.4f] (%9.4f %9.4f) %-20s %-20s %-15s %6d %6d", 
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
// SpdTsBStrawDLayer                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsBStrawDLayer::SpdTsBStrawDLayer() : SpdGeoVolPars(""),
    fLength(0), fHeight(0), fWidthBottom(0), fWidthTop(0), 
    fPackingStrawAngle(0), fPackingStrawWidth(-10), 
    fPackingStrawGap(-10), fPackingSublayerGap(-10), 
    fPackingStrawShiftOpt('o')
{

}

//_____________________________________________________________________________
SpdTsBStrawDLayer::SpdTsBStrawDLayer(const Char_t* name) : SpdGeoVolPars(name),
    fLength(0), fHeight(0), fWidthBottom(0), fWidthTop(0), 
    fPackingStrawAngle(0), fPackingStrawWidth(-10), 
    fPackingStrawGap(-10), fPackingSublayerGap(-10), 
    fPackingStrawShiftOpt('o')
{

}

//_____________________________________________________________________________
SpdTsBStrawDLayer::~SpdTsBStrawDLayer() 
{

}

//_____________________________________________________________________________
void SpdTsBStrawDLayer::Clear()
{
  SpdGeoVolPars::Clear();
  
  fLength = 0;
  fHeight = 0;
  fWidthBottom = 0;
  fWidthTop = 0;
   
  fPackingStrawAngle = 0;
  fPackingStrawWidth = -10;
  fPackingStrawGap   = -10;
  fPackingSublayerGap = -10;
  
  fPackingStrawShiftOpt = 'o';
  
  ClearNodes();
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawDLayer::SetPackingPars(Double_t angle, Double_t w,
                                         Double_t gap, Double_t subgap, Char_t option)
{ 
  ClearNodes();
  
  fPackingStrawAngle = 0;
  fPackingStrawWidth = -10;
  fPackingStrawGap   = -10;
  fPackingSublayerGap = -10;
   
  fPackingStrawAngle = DRoundDEG(angle);
  fPackingStrawWidth = DRoundCMBase(w);
  
  fPackingStrawGap   = DRoundCMBase(gap);
  fPackingSublayerGap = DRoundCMBase(subgap);
  
  fPackingStrawShiftOpt = option;
  
  if (!(option == 'o' || option == 'e')) {
      cout << "-E- <SpdTsBStrawLayer::SetPackingPars> " << "Bad parameter value" << endl;
      return kFALSE;
  }

  return kTRUE;
}

//_____________________________________________________________________________
TString SpdTsBStrawDLayer::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
  
  hash += Form("%d",IRoundCMBase(fLength));
  hash += Form("%d",IRoundCMBase(fHeight));
  hash += Form("%d",IRoundCMBase(fWidthBottom));
  hash += Form("%d",IRoundCMBase(fWidthTop));
 
  hash += Form("%d",IRoundDEG(fPackingStrawAngle));
  hash += Form("%d",IRoundCMBase(fPackingStrawWidth));
  hash += Form("%d",IRoundCMBase(fPackingStrawGap));
  hash += Form("%d",IRoundCMBase(fPackingSublayerGap));
  
  hash += Form("%d",Int_t(fPackingStrawShiftOpt));
 
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawDLayer::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsBStrawDLayer* p = dynamic_cast<SpdTsBStrawDLayer*>(pars);
  if (!p) return kFALSE;
  
  if (IRoundCMBase(fLength)      != IRoundCMBase(p->GetLength()))      return kFALSE;
  if (IRoundCMBase(fHeight)      != IRoundCMBase(p->GetHeight()))      return kFALSE;
  if (IRoundCMBase(fWidthBottom) != IRoundCMBase(p->GetWidthBottom())) return kFALSE;
  if (IRoundCMBase(fWidthTop)    != IRoundCMBase(p->GetWidthTop()))    return kFALSE;
     
  if (IRoundDEG(fPackingStrawAngle)     != IRoundDEG(p->GetPackingStrawAngle()))     return kFALSE;
  if (IRoundCMBase(fPackingStrawWidth)  != IRoundCMBase(p->GetPackingStrawWidth()))  return kFALSE;
  if (IRoundCMBase(fPackingStrawGap)    != IRoundCMBase(p->GetPackingStrawGap()))    return kFALSE;
  if (IRoundCMBase(fPackingSublayerGap) != IRoundCMBase(p->GetPackingSublayerGap())) return kFALSE;
  
  if (fPackingStrawShiftOpt != p->GetPackingStrawShiftOpt()) return kFALSE;

  return kTRUE; 
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawDLayer::MakePacking()
{
  if (!(fPackingStrawWidth > 0) || (fPackingStrawGap < 0)) {
      cout << "-E- <SpdTsBStrawLayer::MakePacking> Bad packing parameter " 
           << "(width, gap: " << fPackingStrawWidth << "," << fPackingStrawGap << ")" 
           << endl;
      return kFALSE;
  }
  
  ClearNodes();
  
  Double_t r, g, a, yu, yd;
  r = 0.5*fPackingStrawWidth;
  g = fPackingStrawGap;
  a = r * sqrt(4 - pow(1+g/2/r,2));  // distance between central planes of sublayers
  a += fPackingSublayerGap;          //
  yu =  0.5*a;      // y displacement of the upper sublayer
  yd = -0.5*a;      // y displacement of the lower sublayer
  
  Double_t k, xmid, xu, xd;
  k = (fWidthTop - fWidthBottom)/2./fHeight;
  xmid = (fWidthTop + fWidthBottom)/4.;
  if (fPackingStrawAngle == 0.) {
    xu = xmid + k*yu - r*(sqrt(k*k+1.)-1.);
    xd = xmid + k*yd - r*(sqrt(k*k+1.)-1.);
  }
  else {
    xu = Min(xmid + k*(yu - r), xmid + k*(yu + r));
    xd = Min(xmid + k*(yd - r), xmid + k*(yd + r));
  }
  
  // Shift in sublayers
  Double_t s = -0.5*(2.*r + g);
  Double_t shift_u, shift_d;
  if (fPackingStrawShiftOpt == 'e') {
      shift_d = 0;
      shift_u = s;
  }
  else if (fPackingStrawShiftOpt == 'o') {
      shift_d = s;
      shift_u = 0;
  }
  else {
      return kFALSE;
  }
  
  Double_t vert_u[8] = { -0.5*fLength,  xu, 
                         -0.5*fLength, -xu, 
                          0.5*fLength, -xu, 
                          0.5*fLength,  xu, 
                       };
  std::vector<std::vector<Double_t>> pars_u;
  Packing(pars_u, vert_u, SpdTsBStraw::GetMinStrawSize(), shift_u, fPackingStrawAngle, 2.*r, g);
  
  Double_t vert_d[8] = { -0.5*fLength,  xd, 
                         -0.5*fLength, -xd, 
                          0.5*fLength, -xd, 
                          0.5*fLength,  xd, 
                       };
  std::vector<std::vector<Double_t>> pars_d;
  Packing(pars_d, vert_d, SpdTsBStraw::GetMinStrawSize(), shift_d, fPackingStrawAngle, 2.*r, g);
  
  // add y displacements of the sublayers as pars[i][4]
  for(auto& p : pars_u) {
      p.push_back(yu);
  }
  for(auto& p : pars_d) {
      p.push_back(yd);
  }
  
  Int_t nd = pars_d.size();
  Int_t nu = pars_u.size();
  // cout << "-I- <SpdTsBStrawDLayer::MakePacking> " << GetName() << " Number of straws: " << nd << " + " << nu << " = " << nd+nu << endl;
  
  // copy pars_u and pars_d to fNodesPars
  fNodesPars.insert(fNodesPars.end(), std::make_move_iterator(pars_u.begin()), std::make_move_iterator(pars_u.end()));
  fNodesPars.insert(fNodesPars.end(), std::make_move_iterator(pars_d.begin()), std::make_move_iterator(pars_d.end()));
  
  return kTRUE;
}

//_____________________________________________________________________________
SpdGeoVolPars* SpdTsBStrawDLayer::MakeNodeVolPars(Int_t nnode) const // protected
{ 
  if ( !(nnode <fNodesPars.size()) ) return 0;

  const std::vector<Double_t>& pars = fNodesPars[nnode];
  
  SpdTsBStraw* straw = new SpdTsBStraw();
  straw->SetStrawPars(pars[3], 0.5*fPackingStrawWidth);
  
  return straw; 
}

//_____________________________________________________________________________
TGeoMatrix* SpdTsBStrawDLayer::MakeNodeMatrix(Int_t nnode)  const // protected
{ 
  if ( !(nnode <fNodesPars.size()) ) return 0;

  const std::vector<Double_t>& pars = fNodesPars[nnode];
  
  if (pars.empty()) return 0;
  
  TGeoTranslation trans(pars[2], pars[4], pars[1]); 
  
  TGeoRotation rot;
  rot.RotateY(fPackingStrawAngle);
  
  return new TGeoCombiTrans(trans,rot);  
}

//_____________________________________________________________________________
void SpdTsBStrawDLayer::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9.4f %9.4f %9.4f] (%9.4f %9.4f) %-20s %-20s %-15s %6d %6d", 
          i, fActivity, fNCopies,
          fLength, fHeight, fWidthBottom, fWidthTop, 
          fPackingStrawAngle, fPackingStrawGap, 
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}   


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsBStrawModuleH                                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsBStrawModuleH::SpdTsBStrawModuleH():SpdGeoVolPars(""),
fLength(0),fHbot(0),fHtop(0),fLbot(0),fLmid(0),fLtop(0),
fPackingLayerWidth(-10),fPackingLayerGap(-10),fPackingLayerShift(0)
{
 
}

//_____________________________________________________________________________
SpdTsBStrawModuleH::SpdTsBStrawModuleH(const Char_t* name):SpdGeoVolPars(name),
fLength(0),fHbot(0),fHtop(0),fLbot(0),fLmid(0),fLtop(0),
fPackingLayerWidth(-10),fPackingLayerGap(-10),fPackingLayerShift(0)
{
 
}

//_____________________________________________________________________________
SpdTsBStrawModuleH::~SpdTsBStrawModuleH()
{
 
}

//_____________________________________________________________________________
void SpdTsBStrawModuleH::Clear()
{
  SpdGeoVolPars::Clear();
   
  fLength = 0;
  fHbot = 0;
  fHtop = 0;
  fLbot = 0;
  fLmid = 0;
  fLtop = 0;
 
  fPackingLayerWidth = -10;
  fPackingLayerGap   = -10;
  fPackingLayerShift = 0;
  
  ClearNodes();
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawModuleH::SetPackingPars(Double_t w, Double_t dw, Char_t option)
{ 
  ClearNodes();
  
  fPackingLayerWidth = -10;
  fPackingLayerGap   = -10;
  fPackingLayerShift = 0;
  
  fPackingLayerWidth = DRoundCMBase(w);
  
  fPackingLayerGap   = DRoundCMBase(dw);
    
  if (option == 'o') fPackingLayerShift = 0;
  else if (option == 'e') {
      fPackingLayerShift = -0.5*(fPackingLayerWidth + fPackingLayerGap);
      fPackingLayerShift = DRoundCMBase(fPackingLayerShift);
  }
  else {
      cout << "-E- <SpdTsBStrawModuleH::SetPackingPars> " << "Bad parameter value" << endl;
      return kFALSE;
  }
 
  return kTRUE;
}

//_____________________________________________________________________________
TString SpdTsBStrawModuleH::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
  
  hash += Form("%d",IRoundCMBase(fLength));
  hash += Form("%d",IRoundCMBase(fHbot));
  hash += Form("%d",IRoundCMBase(fHtop));
  hash += Form("%d",IRoundCMBase(fLbot));
  hash += Form("%d",IRoundCMBase(fLmid));
  hash += Form("%d",IRoundCMBase(fLtop));
 
  hash += Form("%d",IRoundCMBase(fPackingLayerWidth));
  hash += Form("%d",IRoundCMBase(fPackingLayerGap));
  hash += Form("%d",IRoundCMBase(fPackingLayerShift));
 
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawModuleH::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsBStrawModuleH* p = dynamic_cast<SpdTsBStrawModuleH*>(pars);
  if (!p) return kFALSE;
  
  if (IRoundCMBase(fLength) != IRoundCMBase(p->GetLength())) return kFALSE;
  if (IRoundCMBase(fHbot)   != IRoundCMBase(p->GetHbot()))   return kFALSE;
  if (IRoundCMBase(fHtop)   != IRoundCMBase(p->GetHtop()))   return kFALSE;
  if (IRoundCMBase(fLbot)   != IRoundCMBase(p->GetLbot()))   return kFALSE;
  if (IRoundCMBase(fLmid)   != IRoundCMBase(p->GetLmid()))   return kFALSE;
  if (IRoundCMBase(fLtop)   != IRoundCMBase(p->GetLtop()))   return kFALSE;
     
  if (IRoundCMBase(fPackingLayerWidth) != IRoundCMBase(p->GetPackingLayerWidth()))  return kFALSE;
  if (IRoundCMBase(fPackingLayerGap)   != IRoundCMBase(p->GetPackingLayerGap()))    return kFALSE;
  if (IRoundCMBase(fPackingLayerShift) != IRoundCMBase(p->GetPackingLayerShift()))  return kFALSE;    
 
  return kTRUE; 
}

//_____________________________________________________________________________
Double_t SpdTsBStrawModuleH::Lx(Double_t y)
{
    Double_t h = 0.5*(fHbot + fHtop);
    Double_t ybot = h + y;
    Double_t ytop = h - y;
    Double_t lx = 0;
    if (ybot <= fHbot) {
        lx = fLbot + (fLmid - fLbot)*ybot/fHbot;
    }
    else if (ytop <= fHtop) {
        lx = fLtop + (fLmid - fLtop)*ytop/fHtop;
    }
    return lx;
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawModuleH::MakePacking()
{
  if (!(fPackingLayerWidth > 0) || (fPackingLayerGap < 0)) {
      cout << "-E- <SpdTsBStrawModuleH::MakePacking> Bad packing parameter " 
           << "(width, gap: " << fPackingLayerWidth << "," << fPackingLayerGap << ")" 
           << endl;
      return kFALSE;
  }
  
  ClearNodes();
  
  Double_t h = fHbot + fHtop;
  
  Double_t vert[8] = { 0.5*fLength, -0.5*h, //(z,y)[1]
                      -0.5*fLength, -0.5*h, //(z,y)[2]
                      -0.5*fLength,  0.5*h, //(z,y)[3]
                       0.5*fLength,  0.5*h  //(z,y)[4]
                     };
                      
  Packing(fNodesPars /* output result */, 
          vert, SpdTsBStrawDLayer::GetMinLayerSize(), 
          fPackingLayerShift, 0, fPackingLayerWidth, fPackingLayerGap);
  
  if (fNodesPars.empty()) return kTRUE;
  
  Int_t nl = fNodesPars.size();
  
  Double_t ww = 0.5*fPackingLayerWidth;
  Double_t y0, l1, l2, l;
  
  for (Int_t i(0); i<nl; i++) {
      
        y0 = fNodesPars[i][2];
        l1 = Lx(y0 + ww);
        l2 = Lx(y0 - ww);
        
        fNodesPars[i].push_back(l2);
        fNodesPars[i].push_back(l1);
  }        
   
  return kTRUE;
}

//_____________________________________________________________________________
SpdGeoVolPars* SpdTsBStrawModuleH::MakeNodeVolPars(Int_t nnode)  const 
{ 
  if ( !(nnode <fNodesPars.size()) ) return 0;

  const std::vector<Double_t>& pars = fNodesPars[nnode];
  
  if (pars.empty()) return 0;
  
  SpdTsBStrawDLayer* dlayer = new SpdTsBStrawDLayer();
  // set length, height, width_bottom, with_top of the double layer (dlayer)
  // pars[] are calculated in MakePacking() method
  dlayer->SetDLayerPars(pars[3], fPackingLayerWidth, pars[4], pars[5]);
  return dlayer; 

}

//_____________________________________________________________________________
TGeoMatrix* SpdTsBStrawModuleH::MakeNodeMatrix(Int_t nnode)  const 
{ 
  if ( !(nnode < fNodesPars.size()) ) return 0;

  const std::vector<Double_t>& pars = fNodesPars[nnode];
  
  if (pars.empty()) return 0;
  
  // set coordinates of the center of the double layer (dlayer)
  // pars[] are calculated in MakePacking() method
  return new TGeoTranslation(0, pars[2], pars[1]);
}

//_____________________________________________________________________________
void SpdTsBStrawModuleH::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9.4f %9.4f %9.4f %9.4f %9.4f] (%9.4f) %-20s %-20s %-15s %6d %6d", 
          i, fActivity, fNCopies,
          fLength, fHbot, fHtop, fLbot, fLmid, fLtop, 
          fPackingLayerShift,       
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}   



/* ****************************  OBSOLETE CLASSES  ****************************
 *                                                                            *
 * They are still usedd by classes for the very old solenoidal geometry       *
 * (TsSBGeoMapper, TsSBGeoBuilder), that's why they are kept here.            *
 * We need a major cleanup!!!                                                 *
 *                                                                            *
 * ****************************************************************************
 */

ClassImp(SpdTsBStrawLayer)
ClassImp(SpdTsBStrawModule)

Double_t SpdTsBStrawLayer::fMinLayerSize = 0.1; //cm

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsBStrawLayer                                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsBStrawLayer::SpdTsBStrawLayer():SpdGeoVolPars(""),
fLength(0),fH(0),fLmin(0),fLmax(0),fPackingStrawAngle(0),
fPackingStrawWidth(-10),fPackingStrawGap(-10),fPackingStrawShift(0)
{
 
}

//_____________________________________________________________________________
SpdTsBStrawLayer::SpdTsBStrawLayer(const Char_t* name):SpdGeoVolPars(name),
fLength(0),fH(0),fLmin(0),fLmax(0),fPackingStrawAngle(0),
fPackingStrawWidth(-10),fPackingStrawGap(-10),fPackingStrawShift(0)
{
 
}

//_____________________________________________________________________________
SpdTsBStrawLayer::~SpdTsBStrawLayer() 
{
 
}

//_____________________________________________________________________________
void SpdTsBStrawLayer::Clear()
{
  SpdGeoVolPars::Clear();
  
  fLength = 0;
  fH = 0;
  fLmin = 0;
  fLmax = 0;
   
  fPackingStrawAngle = 0;
  fPackingStrawWidth = -10;
  fPackingStrawGap   = -10;
  fPackingStrawShift = 0;
  
  ClearNodes();
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawLayer::SetPackingPars(Double_t angle, Double_t w, 
                                        Double_t dw, Char_t option)
{ 
  ClearNodes();
  
  fPackingStrawAngle = 0;
  fPackingStrawWidth = -10;
  fPackingStrawGap   = -10;
  fPackingStrawShift = 0;
   
  fPackingStrawAngle = DRoundDEG(angle);
  fPackingStrawWidth = DRoundCMBase(w);
  
  fPackingStrawGap   = DRoundCMBase(dw);
  
  if (option == 'o') fPackingStrawShift = 0;
  else if (option == 'e') {
      fPackingStrawShift = -0.5*(fPackingStrawWidth + fPackingStrawGap);
      fPackingStrawShift = DRoundCMBase(fPackingStrawShift);
  }
  else {
      cout << "-E- <SpdTsBStrawLayer::SetPackingPars> " << "Bad parameter value" << endl;
      return kFALSE;
  }

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawLayer::MakePacking()
{
  if (!(fPackingStrawWidth > 0) || (fPackingStrawGap < 0)) {
      cout << "-E- <SpdTsBStrawLayer::MakePacking> Bad packing parameter " 
           << "(width, gap: " << fPackingStrawWidth << "," << fPackingStrawGap << ")" 
           << endl;
      return kFALSE;
  }
  
  ClearNodes();
  
  Double_t vert[8] = { -0.5*fLength,  0.5*fLmin, //(x,y)[1]
                       -0.5*fLength, -0.5*fLmin, //(x,y)[2]      
                        0.5*fLength, -0.5*fLmax, //(x,y)[3]                                
                        0.5*fLength,  0.5*fLmax  //(x,y)[4]
                     };             
  
  //cout << "\n-I- <SpdTsBStrawLayer::MakePacking> \n\n";
  
  Packing(fNodesPars /* output result */, vert/* ordered array of size 4*(x,y) */, 
          SpdTsBStraw::GetMinStrawSize(), fPackingStrawShift,
          fPackingStrawAngle, fPackingStrawWidth, fPackingStrawGap);
  
  return kTRUE;
}

//_____________________________________________________________________________
TString SpdTsBStrawLayer::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
  
  hash += Form("%d",IRoundCMBase(fLength));
  hash += Form("%d",IRoundCMBase(fH));
  hash += Form("%d",IRoundCMBase(fLmin));
  hash += Form("%d",IRoundCMBase(fLmax));
 
  hash += Form("%d",IRoundDEG(fPackingStrawAngle));
  hash += Form("%d",IRoundCMBase(fPackingStrawWidth));
  
  hash += Form("%d",IRoundCMBase(fPackingStrawGap));
  hash += Form("%d",IRoundCMBase(fPackingStrawShift));
 
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawLayer::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsBStrawLayer* p = dynamic_cast<SpdTsBStrawLayer*>(pars);
  if (!p) return kFALSE;
  
  if (IRoundCMBase(fLength) != IRoundCMBase(p->GetLength())) return kFALSE;
  if (IRoundCMBase(fH)      != IRoundCMBase(p->GetH()))      return kFALSE;
  if (IRoundCMBase(fLmin)   != IRoundCMBase(p->GetLmin()))   return kFALSE;
  if (IRoundCMBase(fLmax)   != IRoundCMBase(p->GetLmax()))   return kFALSE;    
     
  if (IRoundDEG(fPackingStrawAngle) != IRoundDEG(p->GetPackingStrawAngle())) return kFALSE;
  if (IRoundCMBase(fPackingStrawWidth) != IRoundCMBase(p->GetPackingStrawWidth()))  return kFALSE;
  
  if (IRoundCMBase(fPackingStrawGap)   != IRoundCMBase(p->GetPackingStrawGap()))   return kFALSE;
  if (IRoundCMBase(fPackingStrawShift) != IRoundCMBase(p->GetPackingStrawShift())) return kFALSE;    
 
  return kTRUE; 
}

//_____________________________________________________________________________
SpdGeoVolPars* SpdTsBStrawLayer::MakeNodeVolPars(Int_t nnode) const // protected
{ 
  if ( !(nnode <fNodesPars.size()) ) return 0;

  const std::vector<Double_t>& pars = fNodesPars[nnode];
  
  SpdTsBStraw* straw = new SpdTsBStraw();
  straw->SetStrawPars(pars[3],0.5*fPackingStrawWidth);
  
  return straw; 
}

//_____________________________________________________________________________
TGeoMatrix* SpdTsBStrawLayer::MakeNodeMatrix(Int_t nnode)  const // protected
{ 
  if ( !(nnode <fNodesPars.size()) ) return 0;

  const std::vector<Double_t>& pars = fNodesPars[nnode];
  
  if (pars.empty()) return 0;
  
  TGeoTranslation trans(pars[2],0,pars[1]); 
  
  TGeoRotation rot;
  rot.RotateY(fPackingStrawAngle);
  
  return new TGeoCombiTrans(trans,rot);  
}

//_____________________________________________________________________________
void SpdTsBStrawLayer::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9.4f %9.4f %9.4f] (%9.4f %9.4f) %-20s %-20s %-15s %6d %6d", 
          i, fActivity, fNCopies,
          fLength, fH, fLmin, fLmax, 
          fPackingStrawAngle, fPackingStrawShift, 
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}   


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsBStrawModule                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//_____________________________________________________________________________
SpdTsBStrawModule::SpdTsBStrawModule():SpdGeoVolPars(""),
fLength(0),fH(0),fLmin(0),fLmax(0),fPackingLayerAngle(0),
fPackingLayerWidth(-10),fPackingLayerGap(-10),fPackingLayerShift(0),
fModuleAngle(0)
{
 
}

//_____________________________________________________________________________
SpdTsBStrawModule::SpdTsBStrawModule(const Char_t* name):SpdGeoVolPars(name),
fLength(0),fH(0),fLmin(0),fLmax(0),fPackingLayerAngle(0),
fPackingLayerWidth(-10),fPackingLayerGap(-10),fPackingLayerShift(0),
fModuleAngle(0)
{
 
}

//_____________________________________________________________________________
SpdTsBStrawModule::~SpdTsBStrawModule() 
{
 
}

//_____________________________________________________________________________
void SpdTsBStrawModule::Clear()
{
  SpdGeoVolPars::Clear();
   
  fLength = 0;
  fH = 0;
  fLmin = 0;
  fLmax = 0;
 
  fPackingLayerAngle = 0;
  fPackingLayerWidth = -10;
  fPackingLayerGap   = -10;
  fPackingLayerShift = 0;
  
  fModuleAngle = 0;
  
  ClearNodes();
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawModule::SetPackingPars(Double_t angle, Double_t w, 
                                         Double_t dw, Char_t option)
{ 
  ClearNodes();
  
  fPackingLayerAngle = 0;
  fPackingLayerWidth = -10;
  fPackingLayerGap   = -10;
  fPackingLayerShift = 0;
  
  fPackingLayerAngle = DRoundDEG(angle);
  fPackingLayerWidth = DRoundCMBase(w);
  
  fPackingLayerGap   = DRoundCMBase(dw);
    
  if (option == 'o') fPackingLayerShift = 0;
  else if (option == 'e') {
      fPackingLayerShift = -0.5*(fPackingLayerWidth + fPackingLayerGap);
      fPackingLayerShift = DRoundCMBase(fPackingLayerShift);
  }
  else {
      cout << "-E- <SpdTsBStrawModule::SetPackingPars> " << "Bad parameter value" << endl;
      return kFALSE;
  }
 
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawModule::MakePacking()
{
  if (!(fPackingLayerWidth > 0) || (fPackingLayerGap < 0)) {
      cout << "-E- <SpdTsBStrawModule::MakePacking> Bad packing parameter " 
           << "(width, gap: " << fPackingLayerWidth << "," << fPackingLayerGap << ")" 
           << endl;
      return kFALSE;
  }
  
  ClearNodes();
  
  Double_t vert[8] = { 0.5*fLength, -0.5*fH, //(z,y)[1]
                      -0.5*fLength, -0.5*fH, //(z,y)[2]
                      -0.5*fLength,  0.5*fH, //(z,y)[3]
                       0.5*fLength,  0.5*fH  //(z,y)[4]
                     };
                      
  Packing(fNodesPars /* output result */, vert/* ordered array of size 4*(x,y) */, 
          SpdTsBStrawLayer::GetMinLayerSize(), fPackingLayerShift,
          fPackingLayerAngle, fPackingLayerWidth, fPackingLayerGap);
  
  //cout << "\n-I- <SpdTsBStrawModule::MakePacking> \n\n";
  
  if (fNodesPars.empty()) return kTRUE;
  
  Int_t nl = fNodesPars.size();
  
  Double_t aa = fPackingLayerAngle*DegToRad();
  Double_t ctb = 1./Tan(PiOver2()- fModuleAngle*DegToRad());
  
  Double_t ww, ll, hh, y1, y2, y0;
  Double_t l1, l2, lmin /*z- direction*/, lmax /*z+ direction*/;
  
  //cout << "\t module angle = " << fModuleAngle << "\n\n";
  
  for (Int_t i(0); i<nl; i++) {
    
       ww = 0.5*fPackingLayerWidth;
       ll = 0.5*fNodesPars[i][3];
       hh = 0.5*fH;
       
       y0 = fNodesPars[i][2];
    
       y1 = ll*Sin(aa) + ww*Cos(aa);
       y2 = ll*Sin(aa) - ww*Cos(aa);
                
       l1 = fLmin + 2 * ctb * Abs(y0 - y1 + hh);
       l2 = fLmin + 2 * ctb * Abs(y0 - y2 + hh);
       
       (l1 < l2) ? lmin = l1 : lmin = l2;
       
       //cout << "\t [1] "<< fLmin << " " << fLmax << "  l(1,2) = " << l1 << " " << l2 
       //     << "     " << Abs(y0 + y1 + hh) << " " << Abs(y0 + y2 + hh) << endl;
      
       l1 = fLmin + 2 * ctb * Abs(y0 + y1 + hh);
       l2 = fLmin + 2 * ctb * Abs(y0 + y2 + hh);
       
       (l1 < l2) ? lmax = l1 : lmax = l2;
       
       //cout << "\t [2] "<< fLmin << " " << fLmax << "  l(1,2) = " << l1 << " " << l2 
       //     << "     " << Abs(y0 - y1 + hh) << " " << Abs(y0 - y2 + hh) << endl;
      
       fNodesPars[i].push_back(lmin);
       fNodesPars[i].push_back(lmax);
       
       //cout << "\t" << i << "  x, y = (" << fNodesPars[i][1] << ", " << y0 
       //     << ")   h = " << fNodesPars[i][3] << " w = " <<  fPackingLayerWidth
       //     << " l(min, max) = " << lmin << ", " << lmax << "\n";
  }
   
  return kTRUE;
}

//_____________________________________________________________________________
TString SpdTsBStrawModule::HashString(Int_t type) const 
{ 
  if (type < 0) return "";
  if (!fName.IsWhitespace()) return fName;
  
  TString hash(ClassName());
  
  hash += Form("%d",IRoundCMBase(fLength));
  hash += Form("%d",IRoundCMBase(fH));
  hash += Form("%d",IRoundCMBase(fLmin));
  hash += Form("%d",IRoundCMBase(fLmax));
 
  hash += Form("%d",IRoundDEG(fPackingLayerAngle));
  hash += Form("%d",IRoundCMBase(fPackingLayerWidth));
  
  hash += Form("%d",IRoundCMBase(fPackingLayerGap));
  hash += Form("%d",IRoundCMBase(fPackingLayerShift));
 
  return hash.MD5(); 
}

//_____________________________________________________________________________
Bool_t SpdTsBStrawModule::Compare(SpdGeoVolPars* pars, Int_t type) const 
{ 
  if (type < 0) return kFALSE;
  if (!pars) return kFALSE;
  
  if (!fName.IsWhitespace()) return (fName == pars->GetName());
  
  SpdTsBStrawModule* p = dynamic_cast<SpdTsBStrawModule*>(pars);
  if (!p) return kFALSE;
  
  if (IRoundCMBase(fLength) != IRoundCMBase(p->GetLength())) return kFALSE;
  if (IRoundCMBase(fH)      != IRoundCMBase(p->GetH()))      return kFALSE;
  if (IRoundCMBase(fLmin)   != IRoundCMBase(p->GetLmin()))   return kFALSE;
  if (IRoundCMBase(fLmax)   != IRoundCMBase(p->GetLmax()))   return kFALSE;
     
  if (IRoundDEG(fPackingLayerAngle) != IRoundDEG(p->GetPackingLayerAngle())) return kFALSE;
  if (IRoundCMBase(fPackingLayerWidth) != IRoundCMBase(p->GetPackingLayerWidth()))  return kFALSE;
  
  if (IRoundCMBase(fPackingLayerGap)   != IRoundCMBase(p->GetPackingLayerGap()))    return kFALSE;
  if (IRoundCMBase(fPackingLayerShift) != IRoundCMBase(p->GetPackingLayerShift()))  return kFALSE;    
 
  return kTRUE; 
}

//_____________________________________________________________________________
SpdGeoVolPars* SpdTsBStrawModule::MakeNodeVolPars(Int_t nnode)  const 
{ 
  if ( !(nnode <fNodesPars.size()) ) return 0;

  const std::vector<Double_t>& pars = fNodesPars[nnode];
  
  if (pars.empty()) return 0;
  
  SpdTsBStrawLayer* layer = new SpdTsBStrawLayer();
  layer->SetLayerPars(pars[3], fPackingLayerWidth, pars[4], pars[5]);
  return layer; 
}

//_____________________________________________________________________________
TGeoMatrix* SpdTsBStrawModule::MakeNodeMatrix(Int_t nnode)  const 
{ 
  if ( !(nnode < fNodesPars.size()) ) return 0;

  const std::vector<Double_t>& pars = fNodesPars[nnode];
  
  if (pars.empty()) return 0;
  
  TGeoTranslation trans(0,pars[2],pars[1]); 
  TGeoRotation rot;
  rot.RotateX(-fPackingLayerAngle);
  
  return new TGeoCombiTrans(trans,rot);  
}

//_____________________________________________________________________________
void SpdTsBStrawModule::Prn(Int_t i, Bool_t endn) const
{
  printf("%3d  %2d  %4d  [%9.4f %9.4f %9.4f %9.4f] (%9.4f %9.4f) %-20s %-20s %-15s %6d %6d", 
          i, fActivity, fNCopies,
          fLength, fH, fLmin, fLmax, 
          fPackingLayerAngle, fPackingLayerShift,       
          ClassName(), fName.Data(), fMedia.Data(),
          GetNNodes(), GetNVolumes()
         );
  if (endn) printf("\n");
}   
