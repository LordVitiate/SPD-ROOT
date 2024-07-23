// $Id$
// Author: artur   2019/03/07

#ifndef __SPDITSVOLPARS_H__
#define __SPDITSVOLPARS_H__

#include "SpdGeoVolPars.h"

//___________________________________________________________________________//
//                                                                           //
// SpdItsChip                                                                //
// SpdItsLadder                                                              //
// SpdItsLayer                                                               //
// SpdItsEClayer                                                             //
//___________________________________________________________________________//


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsChip                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdItsChip: public SpdGeoVolPars {

public:

    SpdItsChip();
    SpdItsChip(const Char_t* name);
    
    virtual ~SpdItsChip();
    
    virtual void Clear();
    
    void SetChipPars(Double_t Lz /*cm*/, Double_t Lphi /*cm*/, Double_t Lr /*cm*/, TString type);
   
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
   
    inline  Double_t GetLz()    const { return fLz;   }
    inline  Double_t GetLphi()  const { return fLphi; }
    inline  Double_t GetLr()    const { return fLr;   }
    inline  TString  GetType()  const { return fType; }
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    
private:

    Double_t  fLz;
    Double_t  fLphi;
    Double_t  fLr;
    TString   fType; 
    
    ClassDef(SpdItsChip,1)
};

//------------------------------------------------------------------------------
inline void SpdItsChip::SetChipPars(Double_t Lz /*cm*/, Double_t Lphi /*cm*/, Double_t Lr /*cm*/, TString type)
{
   fLz   = DRoundCMBase2(Lz);
   fLphi = DRoundCMBase2(Lphi);
   fLr   = DRoundCMBase2(Lr);
   fType = type;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsLadder                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdItsLadder: public SpdGeoVolPars {

public:

    SpdItsLadder();
    SpdItsLadder(const Char_t* name);
    
    virtual ~SpdItsLadder();
    
    virtual void Clear();
    
    void SetLadderPars(Double_t Length /*cm*/, Double_t Lphi /*cm*/, Double_t Lr /*cm*/);
    
    Bool_t  SetPackingPars(TString chip, Int_t nz, Int_t nphi,
                           Double_t lz /*cm*/, Double_t lphi /*cm*/, 
                           Double_t dlz = 0./*cm*/, Double_t dlphi = 0. /*cm*/);
      
    Bool_t  MakePacking();
    
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    inline  Double_t GetLength() const { return fLength; }
    inline  Double_t GetLphi()   const { return fLphi;   }
    inline  Double_t GetLr()     const { return fLr;     }
    
    inline  TString  GetPackingChip()      const { return fPackingChip;      }
    inline  Int_t    GetPackingNzChips()   const { return fPackingNzChips;   }
    inline  Int_t    GetPackingNphiChips() const { return fPackingNphiChips; }
    inline  Int_t    GetNPackingChips()    const { return fPackingNphiChips*fPackingNzChips; }  
 
    inline  Double_t GetPackingChipLz()    const { return fPackingChipLz;    }
    inline  Double_t GetPackingChipLphi()  const { return fPackingChipLphi;  }
    inline  Double_t GetPackingChipDLz ()  const { return fPackingChipDLz;   }  
    inline  Double_t GetPackingChipDLphi() const { return fPackingChipDLphi; } 
   
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t nnode) const;
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t nnode) const;
    
    virtual TGeoMatrix*    MakeNodeMatrix(TString volname, Int_t loc_i) const;
    virtual void           SetVolName(Int_t nnode, TString volname);
    
    virtual Int_t    GetNNodes() const; 
    virtual Int_t    GetNVolumes() const;
    virtual Int_t    GetNNodes(TString volname) const; 
    virtual TString  GetVolName(Int_t nnode) const; 
    
    virtual void Prn(Int_t i, Bool_t endn) const;

private:
  
    Double_t  fLength;
    Double_t  fLphi;
    Double_t  fLr;

    TString   fPackingChip;
    Int_t     fPackingNphiChips;
    Int_t     fPackingNzChips;
   
    Double_t  fPackingChipLz;
    Double_t  fPackingChipLphi;
   
    Double_t  fPackingChipDLz;
    Double_t  fPackingChipDLphi;
   
    ClassDef(SpdItsLadder,1)
};

//------------------------------------------------------------------------------
inline void SpdItsLadder::SetLadderPars(Double_t Length, Double_t Lphi, Double_t Lr)
{
    fLength = DRoundCMBase2(Length);
    fLphi   = DRoundCMBase2(Lphi);
    fLr     = DRoundCMBase2(Lr);
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItLayers                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdItsLayer: public SpdGeoVolPars {

public:

    SpdItsLayer();
    SpdItsLayer(const Char_t* name);
    
    virtual ~SpdItsLayer();
       
    virtual void Clear();
    
    void SetLayerPars(Double_t length /*cm*/, Double_t rmin /*cm*/, Double_t rmax /*cm*/);
    
    Bool_t SetPackingPars(Int_t nplates, Double_t rlayer /*cm*/, 
                          Double_t lz /*cm*/, Double_t lphi /*cm*/, Double_t lr /*cm*/, 
                          Double_t angle = 0. /*deg*/);
                         
    Bool_t MakePacking();
    
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    inline  Double_t GetLength() const { return fLength; }
    inline  Double_t GetRmin()   const { return fRmin;   }
    inline  Double_t GetRmax()   const { return fRmax;   }
    
    inline  Double_t GetPackingNLadders()     const { return fPackingNLadders;     }
    inline  Double_t GetPackingLadderRadius() const { return fPackingLadderRadius; }
    inline  Double_t GetPackingLadderLz()     const { return fPackingLadderLz;     }
    inline  Double_t GetPackingLadderLphi()   const { return fPackingLadderLphi;   }
    inline  Double_t GetPackingLadderLr()     const { return fPackingLadderLr;     }
    inline  Double_t GetPackingLadderAngle()  const { return fPackingLadderAngle;  }
     
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t nnode) const;
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t nnode) const;
   
    virtual TGeoMatrix* MakeNodeMatrix(TString volname, Int_t loc_i) const;
    virtual void        SetVolName(Int_t nnode, TString volname);
     
    virtual Int_t    GetNNodes() const; 
    virtual Int_t    GetNVolumes() const;
    virtual Int_t    GetNNodes(TString volname) const; 
    virtual TString  GetVolName(Int_t nnode) const; 
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    
protected:
  
    Double_t  fLength;
    Double_t  fRmin;
    Double_t  fRmax;
    
    Int_t     fPackingNLadders;
    Double_t  fPackingLadderRadius;
    Double_t  fPackingLadderLz;
    Double_t  fPackingLadderLphi;
    Double_t  fPackingLadderLr; 
    Double_t  fPackingLadderAngle;
     
private:

    ClassDef(SpdItsLayer,1)
};

//------------------------------------------------------------------------------
inline void SpdItsLayer::SetLayerPars(Double_t length, Double_t rmin, Double_t rmax)
{
    fLength = length;
    fRmin   = rmin;
    fRmax   = rmax;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsEClayer                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdItsEClayer: public SpdGeoVolPars {

public:

    SpdItsEClayer();
    SpdItsEClayer(const Char_t* name);
    virtual ~SpdItsEClayer();
    
    virtual void Clear();
    
    void    SetLayerPars(Double_t length /*cm*/, 
                         Double_t rmin /*cm*/, Double_t rmax /*cm*/);
       
    virtual TString  HashString(Int_t type = 0) const;
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const;
    
    inline  Double_t GetLength() const { return fLength; }
    inline  Double_t GetRmin()   const { return fRmin;   }
    inline  Double_t GetRmax()   const { return fRmax;   }
    
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t nnode) const { return 0; }
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t nnode)  const { return 0; }
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    
protected:
  
    Double_t  fLength;
    Double_t  fRmin;
    Double_t  fRmax; 
    
        
    ClassDef(SpdItsEClayer,1)
};

//------------------------------------------------------------------------------
inline void SpdItsEClayer::SetLayerPars(Double_t length, 
                                        Double_t rmin, Double_t rmax)
{  
    fLength = length;
    fRmin   = rmin;
    fRmax   = rmax;
}



#endif  /* __SPDITSVOLPARS_H__ */

