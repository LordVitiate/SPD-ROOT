// $Id$
// Author: artur   2018/05/07

#ifndef __SPDGEOVOLPARS_H__
#define __SPDGEOVOLPARS_H__

#include <TNamed.h>
#include <TMath.h>
#include <TGeoMatrix.h>
#include <vector>
#include <map>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGeoVolPars                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

typedef std::vector< std::vector<Double_t> >    SpdGeoVolPars_NodesPars;
typedef std::map< TString, std::vector<Int_t> > SpdGeoVolPars_VolNodes;

class SpdGeoVolPars: public TNamed {

public:
    
    SpdGeoVolPars();
    SpdGeoVolPars(const Char_t* volname);
    
    virtual ~SpdGeoVolPars();
    
    virtual void   Clear();  
    virtual void   ClearNodes();  
    
    // ---------------------------------------------- //
    
    virtual SpdGeoVolPars* MakeNodeVolPars(Int_t /*nnode*/) const { return 0; } // : abstract
    virtual TGeoMatrix*    MakeNodeMatrix(Int_t /*nnode*/)  const { return 0; } // : abstract
    
    virtual TGeoMatrix* MakeNodeMatrix(TString volname, Int_t loc_i) const;
     
    virtual void SetVolName(Int_t nnode, TString volname);
    
    virtual Int_t    GetNNodes() const;
    virtual Int_t    GetNVolumes() const;
    virtual Int_t    GetNNodes(TString volname) const; // fast method
    virtual TString  GetVolName(Int_t nnode) const; // slow method
    
    // ---------------------------------------------- //
     
    inline  void SetMedia(const Char_t* media) { fMedia = media;  }
    inline  void SetActivity(Int_t act)        { fActivity = act; }
    inline  void SetNCopies(Int_t nn)          { fNCopies = nn;   }
    
    inline  void AddNCopies(Int_t n = 1) { fNCopies += n; }
    
    inline  void SetLineColor(Int_t color) { fLineColor = color; } 
    inline  void SetFillColor(Int_t color) { fFillColor = color; } 
    inline  void SetTransparency(Int_t tp) { fTransparency = tp; } 
    
    inline  TString  GetMedia()     const { return fMedia;     }
    inline  Int_t    GetActivity()  const { return fActivity;  }
    inline  Int_t    GetNCopies()   const { return fNCopies;   }
       
    inline  const SpdGeoVolPars_NodesPars& GetNodesPars() const { return fNodesPars; }
    inline  const SpdGeoVolPars_VolNodes&  GetVolNodes()  const { return fVolNodes;  }
            
    inline  Int_t    GetLineColor()    const { return fLineColor; } 
    inline  Int_t    GetFillColor()    const { return fFillColor; } 
    inline  Char_t   GetTransparency() const { return fTransparency; }         
   
    virtual TString  HashString(Int_t type = 0) const { return ""; } // : abstract
    virtual Bool_t   Compare(SpdGeoVolPars* pars, Int_t type = 0) const { return kFALSE; } //: abstract
    
    static void SetSizeRounding(Double_t x)  { fRoundingCM  = x; fIRoundingCM  = 1./x;} 
    static void SetAngleRounding(Double_t x) { fRoundingDEG = x; fIRoundingDEG = 1./x;}
    
    virtual void Prn(Int_t i, Bool_t endn) const;
    virtual void PrintGeoVolPars(Int_t option = 0) const;
    
    /* build methods */
    
    static void Packing(std::vector< std::vector<Double_t> >& pars /* output result */,
                        Double_t vertex[] /* ordered array of size 4*(x,y) */, 
                        Double_t min_size, Double_t ds,
                        Double_t angle, Double_t w, Double_t dw);
    
protected:
     
    TString   fMedia;
    Int_t     fActivity;
    Int_t     fNCopies;
     
    std::vector< std::vector<Double_t> >    fNodesPars;  // node number -> list of parameters (shape parameters, matrix, etc.) 
    std::map< TString, std::vector<Int_t> > fVolNodes;   // volume -> list of curresponding nodes (numbers)
    
    Int_t     fLineColor;
    Int_t     fFillColor;
    Char_t    fTransparency;
    
    inline static Double_t DRoundCM(Double_t x /*cm*/)      { return fRoundingCM*Int_t(x*fIRoundingCM);   }
    inline static Double_t DRoundDEG(Double_t x /*deg*/)    { return fRoundingDEG*Int_t(x*fIRoundingDEG); }
    inline static Double_t DRoundCMBase(Double_t x /*cm*/)  { return fRoundingCMBase*Int_t(x*fIRoundingCMBase); }
    inline static Double_t DRoundCMBase2(Double_t x /*cm*/) { return fRoundingCMBase2*Int_t(x*fIRoundingCMBase2); }
    inline static Int_t    IRoundCM(Double_t x /*cm*/)      { return Int_t(x*fIRoundingCM); }
    inline static Int_t    IRoundDEG(Double_t x /*deg*/)    { return Int_t(x*fIRoundingDEG); }
    inline static Int_t    IRoundCMBase(Double_t x /*cm*/)  { return Int_t(x*fIRoundingCMBase); }
    inline static Int_t    IRoundCMBase2(Double_t x /*cm*/) { return Int_t(x*fIRoundingCMBase2); }
    
private:
  
    static Double_t fRoundingCM;     
    static Double_t fRoundingDEG; 
    static Double_t fRoundingCMBase;
    static Double_t fRoundingCMBase2;
    static Double_t fIRoundingCM;
    static Double_t fIRoundingDEG; 
    static Double_t fIRoundingCMBase;
    static Double_t fIRoundingCMBase2;
  
    ClassDef(SpdGeoVolPars,1)
};

#endif  /* __SPDGEOVOLPARS_H__ */

