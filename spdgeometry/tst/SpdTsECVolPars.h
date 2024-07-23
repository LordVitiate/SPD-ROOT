// $Id$
// Author: artur   2019/07/08

#ifndef __SPDTSECVOLPARS_H__
#define __SPDTSECVOLPARS_H__

#include "SpdGeoVolPars.h"

//___________________________________________________________________________//
//                                                                           //
// SpdTsECStrawInternal                                                      //
// SpdTsECStraw                                                              //
// SpdTsECStrawLayer                                                         //
// SpdTsECStrawModule                                                        //
// SpdTsECStrawModuleArb                                                     //
// SpdTsECStrawModulePgon                                                    //
//___________________________________________________________________________//


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStrawInternal                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsECStrawInternal: public SpdGeoVolPars {

public:

    SpdTsECStrawInternal();
    SpdTsECStrawInternal(const Char_t* name);
    virtual ~SpdTsECStrawInternal();
    
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
  
    ClassDef(SpdTsECStrawInternal,1)
};

//------------------------------------------------------------------------------
inline void SpdTsECStrawInternal::SetStrawInternalPars(Double_t length, Double_t oradius, 
                                                       Double_t width)
{
    fLength = DRoundCMBase2(length);
    fOuterRadius = DRoundCMBase2(oradius);
    fWidth = DRoundCMBase2(width);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStraw                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsECStraw: public SpdGeoVolPars {

public:

    SpdTsECStraw();
    SpdTsECStraw(const Char_t* name);
    virtual ~SpdTsECStraw();
    
    virtual void Clear();
    
    inline void SetStrawPars(Int_t type, Double_t length /*cm*/, Double_t radius /*cm*/);
    
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    inline void SetLength(Double_t l)      { fLength = DRoundCM(l);    }
    inline void SetHalfLength(Double_t hl) { fLength = DRoundCM(2*hl); }
    inline void SetRadius(Double_t r)      { fRadius = DRoundCM(r);    }
   
    inline Double_t GetType()       const { return fType;    }
    inline Double_t GetLength()     const { return fLength;    }
    inline Double_t GetHalfLength() const { return .5*fLength; }   
    inline Double_t GetRadius()     const { return fRadius;    }
    inline Double_t GetWidth()      const { return 2*fRadius;  }
    
    static void SetMinStrawSize(Double_t size) { fMinStrawSize = size; }
    static Double_t GetMinStrawSize() { return fMinStrawSize; }
   
    virtual TGeoMatrix*  MakeNodeMatrix(TString volname, Int_t loc_i) const;
    
    SpdTsECStrawInternal* MakeShell(TString volname, TString media, Double_t rmin, Double_t rmax) const;
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    
protected:
      
    Int_t     fType;   // straw type
    Double_t  fLength; // straw length
    Double_t  fRadius; // straw radius
    
    static Double_t fMinStrawSize; 
      
    ClassDef(SpdTsECStraw,1)
};

//------------------------------------------------------------------------------
inline void SpdTsECStraw::SetStrawPars(Int_t type, Double_t length, Double_t radius)
{
    fType   = type;
    fLength = DRoundCMBase2(length);
    fRadius = DRoundCMBase2(radius);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStrawModuleArb                                                      //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsECStrawModuleArb: public SpdGeoVolPars {

public:

    SpdTsECStrawModuleArb();
    SpdTsECStrawModuleArb(const Char_t* name);
    virtual ~SpdTsECStrawModuleArb();
    
    virtual void Clear();
    
    void SetModulePars(Double_t length /*cm*/, Double_t h /*cm*/, 
                       Double_t lmin /*cm*/, Double_t lmax /*cm*/);
    
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    inline  Double_t GetLength() const { return fLength; }
    inline  Double_t GetH()      const { return fH;      }
    inline  Double_t GetLmin()   const { return fLmin;   }
    inline  Double_t GetLmax()   const { return fLmax;   }
        
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t nnode) const { return 0; }
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t nnode) const { return 0; }
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    
protected:
  
    Double_t  fLength;
    Double_t  fH;
    Double_t  fLmin;
    Double_t  fLmax;  
        
    ClassDef(SpdTsECStrawModuleArb,1)
};

//------------------------------------------------------------------------------
inline void SpdTsECStrawModuleArb::SetModulePars(Double_t length, Double_t h, 
                                             Double_t lmin, Double_t lmax)
{
    fLength = DRoundCMBase(length);
    fH      = DRoundCMBase(h);
    fLmin   = DRoundCMBase(lmin);
    fLmax   = DRoundCMBase(lmax);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStrawModulePgon                                                     //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsECStrawModulePgon: public SpdGeoVolPars {

public:

    SpdTsECStrawModulePgon();
    SpdTsECStrawModulePgon(const Char_t* name);
    virtual ~SpdTsECStrawModulePgon();
    
    virtual void Clear();
    
    void SetModulePars(Int_t nsectors, Double_t length /*cm*/, 
                       Double_t hmin /*cm*/, Double_t hmax /*cm*/);
    
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    inline  Double_t GetNSectors() const { return fNSectors; }
    inline  Double_t GetLength()   const { return fLength;   }
    inline  Double_t GetHmin()     const { return fHmin;     }
    inline  Double_t GetHmax()     const { return fHmax;     }
        
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t nnode) const { return 0; }
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t nnode)  const { return 0; }
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    
protected:
  
    Int_t     fNSectors;
    Double_t  fLength;
    Double_t  fHmin;
    Double_t  fHmax;  
        
    ClassDef(SpdTsECStrawModulePgon,1)
};

//------------------------------------------------------------------------------
inline void SpdTsECStrawModulePgon::SetModulePars(Int_t nsectors, Double_t length, 
                                                  Double_t hmin, Double_t hmax)
{  
    fNSectors = nsectors;
    fLength   = DRoundCMBase(length);
    fHmin     = DRoundCMBase(hmin);
    fHmax     = DRoundCMBase(hmax);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStrawModule                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsECStrawModule: public SpdGeoVolPars {

public:

    SpdTsECStrawModule();
    SpdTsECStrawModule(const Char_t* name);
    virtual ~SpdTsECStrawModule();
    
    virtual void Clear();
    
    void   SetModulePars(Int_t mtype, Double_t length /*cm*/, 
                         Double_t rmin /*cm*/, Double_t rmax /*cm*/);
  
    Bool_t SetPackingPars(Int_t ltype, Int_t nlayers, 
                          Double_t lgap /*cm*/, Double_t dphi /*deg*/,   
                          Double_t l /*cm*/, Double_t r/*cm*/, Double_t b/*cm*/);
       
    Bool_t MakePacking();
    
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    inline  Double_t GetType()   const { return fType;   }
    inline  Double_t GetLength() const { return fLength; }
    inline  Double_t GetRmin()   const { return fRmin;   }
    inline  Double_t GetRmax()   const { return fRmax;   }
   
    inline  Int_t    GetPackingLayerType()   const { return fPackingLayerType;    }
    inline  Int_t    GetPackingNLayers()     const { return fPackingNLayers;      }
    inline  Double_t GetPackingLayerGap()    const { return fPackingLayerGap;     }
    inline  Double_t GetPackingLayerDphi()   const { return fPackingLayerDphi;    }
    
    inline  Double_t GetPackingLayerLength() const { return fPackingLayerLength;  }
    inline  Double_t GetPackingLayerRadius() const { return fPackingLayerRadius;  }
    inline  Double_t GetPackingLayerBSize()  const { return fPackingLayerBSize;   }
    
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t nnode) const;
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t nnode)  const;
    
    virtual TGeoMatrix* MakeNodeMatrix(TString volname, Int_t loc_i) const;
    virtual void        SetVolName(Int_t nnode, TString volname);
    
    virtual Int_t    GetNNodes() const; 
    virtual Int_t    GetNVolumes() const;
    virtual Int_t    GetNNodes(TString volname) const; 
    virtual TString  GetVolName(Int_t nnode) const;   
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    
protected:
  
    Int_t     fType;
    Double_t  fLength;
    Double_t  fRmin;
    Double_t  fRmax; 
    
    Int_t     fPackingLayerType;
    Int_t     fPackingNLayers;
    Double_t  fPackingLayerGap;
    Double_t  fPackingLayerDphi;
    
    Double_t  fPackingLayerLength;
    Double_t  fPackingLayerRadius;
    Double_t  fPackingLayerBSize;
        
    ClassDef(SpdTsECStrawModule,1)
};

//------------------------------------------------------------------------------
inline void SpdTsECStrawModule::SetModulePars(Int_t mtype, Double_t length, 
                                              Double_t rmin, Double_t rmax)
{  
    fType   = mtype;
    fLength = length;
    fRmin   = rmin;
    fRmax   = rmax;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsECStrawLayer                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsECStrawLayer: public SpdGeoVolPars {

public:

    SpdTsECStrawLayer();
    SpdTsECStrawLayer(const Char_t* name);
    virtual ~SpdTsECStrawLayer();
    
    virtual void Clear();
    
    void    SetLayerPars(Int_t type, Double_t length /*cm*/, 
                         Double_t radius /*cm*/, Double_t bsize /*cm*/);
    
    Bool_t  SetPackingPars(Int_t stype, Int_t option, Double_t sradius /*cm*/, 
                           Double_t zgap /*cm*/, Double_t rgap /*cm*/,   
                           Double_t dr = 0/*cm*/, Double_t db = 0/*cm*/);
    
    Bool_t  MakePacking();
      
    virtual TString   HashString(Int_t type = 0) const;
    virtual Bool_t    Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
       
    inline  Double_t  GetType()      const { return fType;     }
    inline  Double_t  GetLength()    const { return fLength;   }
    inline  Double_t  GetRadius()    const { return fRadius;   }
    inline  Double_t  GetBoreSize()  const { return fBoreSize; }
    
    inline  Int_t           GetNVertices() const { return NVERTICES; }
    inline  const Double_t* GetXVertices() const { return fXVertices; }
    inline  const Double_t* GetYVertices() const { return fYVertices; }
    
    inline  Int_t     GetPackingStrawType()   const { return fPackingStrawType;   }
    inline  Int_t     GetPackingStrawOption() const { return fPackingStrawOption; }
    inline  Double_t  GetPackingStrawRadius() const { return fPackingStrawRadius; }
    inline  Double_t  GetPackingStrawZGap()   const { return fPackingStrawZGap;   }
    inline  Double_t  GetPackingStrawRGap()   const { return fPackingStrawRGap;   }
    
    inline  Double_t  GetPackingLayerDR()     const { return fPackingLayerDR; }
    inline  Double_t  GetPackingLayerDB()     const { return fPackingLayerDB; }
    
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t nnode) const;
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t nnode) const;
    
    virtual void Prn(Int_t i, Bool_t endn) const;
   
protected:
  
    Int_t     fType;
    Double_t  fLength;
    Double_t  fRadius;
    Double_t  fBoreSize;
         
    Int_t     fPackingStrawType;
    Int_t     fPackingStrawOption;
    Double_t  fPackingStrawRadius;
    Double_t  fPackingStrawZGap;
    Double_t  fPackingStrawRGap;
    
    Double_t  fPackingLayerDR;
    Double_t  fPackingLayerDB;  
    
    static const Int_t NVERTICES = 6;
    Double_t  fXVertices[NVERTICES];
    Double_t  fYVertices[NVERTICES];
    
    ClassDef(SpdTsECStrawLayer,1)
};

#endif  /* __SPDTSECVOLPARS_H__ */
