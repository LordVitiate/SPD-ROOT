// $Id$
// Author: artur   2018/05/11
//         ruslan  2023/02/17

#ifndef __SPDTSBVOLPARS_H__
#define __SPDTSBVOLPARS_H__

#include "SpdGeoVolPars.h"

//___________________________________________________________________________//
//                                                                           //
// SpdTsBStrawInternal                                                       //
// SpdTsBStraw                                                               //
// SpdTsBStrawDLayer                                                         //
// SpdTsBStrawModuleH                                                        //
//                                                                           //
// SpdTsBStrawLayer                                                          //
// SpdTsBStrawModule                                                         //
//___________________________________________________________________________//


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsBStrawInternal                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsBStrawInternal: public SpdGeoVolPars {

public:

    SpdTsBStrawInternal();
    SpdTsBStrawInternal(const Char_t* name);
    virtual ~SpdTsBStrawInternal();
    
    virtual void Clear();
    
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    inline void SetStrawInternalPars(Double_t length/*cm*/, Double_t oradius/*cm*/, Double_t width/*cm*/);
    
    inline void SetLength(Double_t l)      { fLength = l;      }
    inline void SetHalfLength(Double_t hl) { fLength = 2*hl;   }
    inline void SetOuterRadius(Double_t r) { fOuterRadius = r; }
    inline void SetWidth(Double_t w)       { fWidth = w;       }
 
    inline Double_t GetLength()      const { return fLength;      }
    inline Double_t GetHalfLength()  const { return .5*fLength;   }   
    inline Double_t GetOuterRadius() const { return fOuterRadius; }
    inline Double_t GetWidth()       const { return fWidth;       }
    
    inline Double_t GetInnerRadius() const { return fOuterRadius - fWidth; }
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    
protected:
      
    Double_t  fLength;      // straw length
    Double_t  fOuterRadius; // straw radius
    Double_t  fWidth;       // internal's width
  
    ClassDef(SpdTsBStrawInternal,1)
};

//------------------------------------------------------------------------------
inline void SpdTsBStrawInternal::SetStrawInternalPars(Double_t length, Double_t oradius, 
                                                      Double_t width)
{
    fLength = DRoundCM(length);
    fOuterRadius = DRoundCMBase(oradius);
    fWidth = DRoundCMBase(width);
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsBStraw                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsBStraw: public SpdGeoVolPars {

public:

    SpdTsBStraw();
    SpdTsBStraw(const Char_t* name);
    virtual ~SpdTsBStraw();
    
    virtual void Clear();
    
    inline void SetStrawPars(Double_t length /*cm*/, Double_t radius /*cm*/);
    
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    inline void SetLength(Double_t l)      { fLength = DRoundCM(l);    }
    inline void SetHalfLength(Double_t hl) { fLength = DRoundCM(2*hl); }
    inline void SetRadius(Double_t r)      { fRadius = DRoundCM(r);    }
   
    inline Double_t GetLength()     const { return fLength;    }
    inline Double_t GetHalfLength() const { return .5*fLength; }   
    inline Double_t GetRadius()     const { return fRadius;    }
    inline Double_t GetWidth()      const { return 2*fRadius;  }
    
    static void SetMinStrawSize(Double_t size) { fMinStrawSize = size; }
    static Double_t GetMinStrawSize() { return fMinStrawSize; }
   
    virtual TGeoMatrix*  MakeNodeMatrix(TString volname, Int_t loc_i) const;
    
    SpdTsBStrawInternal* MakeShell(TString volname, TString media, Double_t rmin, Double_t rmax) const;
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    
protected:
      
    Double_t  fLength; // straw length
    Double_t  fRadius; // straw radius
    
    static Double_t fMinStrawSize; 
      
    ClassDef(SpdTsBStraw,1)
};

//------------------------------------------------------------------------------
inline void SpdTsBStraw::SetStrawPars(Double_t length, Double_t radius)
{
    fLength = DRoundCM(length);
    fRadius = DRoundCMBase(radius);
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsBStrawDLayer                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsBStrawDLayer: public SpdGeoVolPars {

public:

    SpdTsBStrawDLayer();
    SpdTsBStrawDLayer(const Char_t* name);
    virtual ~SpdTsBStrawDLayer();
    
    virtual void Clear();
    
    void SetDLayerPars(Double_t length /*cm*/, Double_t height /*cm*/, 
                       Double_t width_bottom /*cm*/, Double_t width_top /*cm*/);
    
    Bool_t SetPackingPars(Double_t angle /*deg*/, Double_t w /*cm*/, 
                          Double_t gap /*cm*/, Double_t subgap /*cm*/, Char_t option /*'o','e'*/);
    Bool_t MakePacking();
      
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    static void SetMinLayerSize(Double_t size) { fMinLayerSize = size; }
    static Double_t GetMinLayerSize()  { return fMinLayerSize; }
    
    inline Double_t GetLength() const { return fLength; }
    inline Double_t GetHeight() const { return fHeight; }
    inline Double_t GetWidthBottom()   const { return fWidthBottom;   }
    inline Double_t GetWidthTop()      const { return fWidthTop;      }
    
    inline Double_t GetPackingStrawAngle() const { return fPackingStrawAngle; }
    inline Double_t GetPackingStrawWidth() const { return fPackingStrawWidth; }
    inline Double_t GetPackingStrawGap()   const { return fPackingStrawGap;   }
    inline Double_t GetPackingSublayerGap() const { return fPackingSublayerGap; }
    
    inline Char_t GetPackingStrawShiftOpt() const { return fPackingStrawShiftOpt; }
    
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t nnode) const;
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t nnode) const;
    
    virtual void Prn(Int_t i, Bool_t endn) const;
   
protected:
  
    Double_t  fLength;
    Double_t  fHeight;
    Double_t  fWidthBottom;
    Double_t  fWidthTop;
    
    Double_t  fPackingStrawAngle;
    Double_t  fPackingStrawWidth;
    Double_t  fPackingStrawGap;
    Double_t  fPackingSublayerGap;
    
    Char_t  fPackingStrawShiftOpt;
    
    static Double_t fMinLayerSize; 
    
    ClassDef(SpdTsBStrawDLayer,1)
};

//------------------------------------------------------------------------------
inline void SpdTsBStrawDLayer::SetDLayerPars(Double_t length, Double_t height, 
                                             Double_t width_bottom, Double_t width_top)
{
    fLength = DRoundCMBase(length);
    fHeight = DRoundCMBase(height);
    fWidthBottom = DRoundCMBase(width_bottom);
    fWidthTop    = DRoundCMBase(width_top);
}


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsBStrawModuleH                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsBStrawModuleH: public SpdGeoVolPars {

public:

    SpdTsBStrawModuleH();
    SpdTsBStrawModuleH(const Char_t* name);
    virtual ~SpdTsBStrawModuleH();
    
    virtual void Clear();
    
    void SetModulePars(Double_t length /*cm*/, Double_t hbot /*cm*/, Double_t htop /*cm*/, 
                       Double_t lbot /*cm*/, Double_t lmid /*cm*/, Double_t ltop /*cm*/);
    
    Bool_t SetPackingPars(Double_t w /*cm*/, Double_t dw /*cm*/, Char_t option /*'o','e'*/);
    Bool_t MakePacking();
    
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    inline  Double_t GetLength() const { return fLength; }
    inline  Double_t GetHbot()   const { return fHbot;   }
    inline  Double_t GetHtop()   const { return fHtop;   }
    inline  Double_t GetLbot()   const { return fLbot;   }
    inline  Double_t GetLmid()   const { return fLmid;   }
    inline  Double_t GetLtop()   const { return fLtop;   }
    
    inline  Double_t GetH()      const { return 0.5*(fHbot + fHtop); }
    
    inline  Double_t GetPackingLayerWidth() const { return fPackingLayerWidth; }
    inline  Double_t GetPackingLayerGap()   const { return fPackingLayerGap;   }
    inline  Double_t GetPackingLayerShift() const { return fPackingLayerShift; }
    
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t nnode) const;
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t nnode) const;
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    
protected:
  
    Double_t  fLength;
    Double_t  fHbot;
    Double_t  fHtop;
    Double_t  fLbot;
    Double_t  fLmid;
    Double_t  fLtop;
    
    Double_t  fPackingLayerWidth;
    Double_t  fPackingLayerGap;
    Double_t  fPackingLayerShift;
     
private:
    
    Double_t Lx(Double_t y);
    
    ClassDef(SpdTsBStrawModuleH,1)
};

//------------------------------------------------------------------------------
inline void SpdTsBStrawModuleH::SetModulePars(Double_t length, Double_t hbot, Double_t htop, 
                                             Double_t lbot, Double_t lmid, Double_t ltop)
{
    fLength = DRoundCMBase(length);
    fHbot   = DRoundCMBase(hbot);
    fHtop   = DRoundCMBase(htop);
    fLbot   = DRoundCMBase(lbot);
    fLmid   = DRoundCMBase(lmid);
    fLtop   = DRoundCMBase(ltop);
}



/* ****************************  OBSOLETE CLASSES  ****************************
 *                                                                            *
 * They are still usedd by classes for the very old solenoidal geometry       *
 * (TsSBGeoMapper, TsSBGeoBuilder), that's why they are kept here.            *
 * We need a major cleanup!!!                                                 *
 *                                                                            *
 * ****************************************************************************
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsBStrawLayer                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsBStrawLayer: public SpdGeoVolPars {

public:

    SpdTsBStrawLayer();
    SpdTsBStrawLayer(const Char_t* name);
    virtual ~SpdTsBStrawLayer();
    
    virtual void Clear();
    
    void SetLayerPars(Double_t length /*cm*/, Double_t h /*cm*/, 
                      Double_t lmin /*cm*/, Double_t lmax /*cm*/);
    
    Bool_t SetPackingPars(Double_t angle /*deg*/, Double_t w /*cm*/, 
                          Double_t dw /*cm*/, Char_t option /*'o','e'*/);
    Bool_t MakePacking();
      
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    static void SetMinLayerSize(Double_t size) { fMinLayerSize = size; }
    static Double_t GetMinLayerSize()  { return fMinLayerSize; }
    
    inline  Double_t GetLength() const { return fLength; }
    inline  Double_t GetH()      const { return fH;      }
    inline  Double_t GetLmin()   const { return fLmin;   }
    inline  Double_t GetLmax()   const { return fLmax;   }
    
    inline  Double_t GetPackingStrawAngle() const { return fPackingStrawAngle; }
    inline  Double_t GetPackingStrawWidth() const { return fPackingStrawWidth; }
    inline  Double_t GetPackingStrawGap()   const { return fPackingStrawGap;   }
    inline  Double_t GetPackingStrawShift() const { return fPackingStrawShift; }
    
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t nnode) const;
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t nnode) const;
    
    virtual void Prn(Int_t i, Bool_t endn) const;
   
protected:
  
    Double_t  fLength;
    Double_t  fH;
    Double_t  fLmin;
    Double_t  fLmax;
    
    Double_t  fPackingStrawAngle;
    Double_t  fPackingStrawWidth;
    Double_t  fPackingStrawGap;
    Double_t  fPackingStrawShift;
    
    static Double_t fMinLayerSize; 
    
    ClassDef(SpdTsBStrawLayer,1)
};

//------------------------------------------------------------------------------
inline void SpdTsBStrawLayer::SetLayerPars(Double_t length, Double_t h, 
                                           Double_t lmin, Double_t lmax)
{
    fLength = DRoundCMBase(length);
    fH      = DRoundCMBase(h);
    fLmin   = DRoundCMBase(lmin);
    fLmax   = DRoundCMBase(lmax);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsBStrawModule                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsBStrawModule: public SpdGeoVolPars {

public:

    SpdTsBStrawModule();
    SpdTsBStrawModule(const Char_t* name);
    virtual ~SpdTsBStrawModule();
    
    virtual void Clear();
    
    void SetModulePars(Double_t length /*cm*/, Double_t h /*cm*/, 
                       Double_t lmin /*cm*/, Double_t lmax /*cm*/);
    
    Bool_t SetPackingPars(Double_t angle /*deg*/, Double_t w /*cm*/, 
                          Double_t dw /*cm*/, Char_t option /*'o','e'*/);
    Bool_t MakePacking();
    
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    inline  Double_t GetLength() const { return fLength; }
    inline  Double_t GetH()      const { return fH;      }
    inline  Double_t GetLmin()   const { return fLmin;   }
    inline  Double_t GetLmax()   const { return fLmax;   }
    
    inline  Double_t GetPackingLayerAngle() const { return fPackingLayerAngle; }
    inline  Double_t GetPackingLayerWidth() const { return fPackingLayerWidth; }
    inline  Double_t GetPackingLayerGap()   const { return fPackingLayerGap;   }
    inline  Double_t GetPackingLayerShift() const { return fPackingLayerShift; }
    
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t nnode) const;
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t nnode) const;
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    
protected:
  
    Double_t  fLength;
    Double_t  fH;
    Double_t  fLmin;
    Double_t  fLmax;
    
    Double_t  fPackingLayerAngle;
    Double_t  fPackingLayerWidth;
    Double_t  fPackingLayerGap;
    Double_t  fPackingLayerShift;
     
private:
  
    Double_t fModuleAngle;
        
    ClassDef(SpdTsBStrawModule,1)
};

//------------------------------------------------------------------------------
inline void SpdTsBStrawModule::SetModulePars(Double_t length, Double_t h, 
                                             Double_t lmin, Double_t lmax)
{
    fLength = DRoundCMBase(length);
    fH      = DRoundCMBase(h);
    fLmin   = DRoundCMBase(lmin);
    fLmax   = DRoundCMBase(lmax);
    
    fModuleAngle = TMath::ATan(0.5*(lmax-lmin)/h)*TMath::RadToDeg();
}


#endif  /* __SPDTSBVOLPARS_H__ */
