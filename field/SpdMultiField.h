// $Id$
// Author: artur   2017/12/20

#ifndef __SPDMULTIFIELD_H__
#define __SPDMULTIFIELD_H__

#include "SpdField.h"
#include <map>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMultiField                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdFieldPar;

class SpdMultiField : public FairField {

public:

    SpdMultiField(); 
    virtual ~SpdMultiField();
    
    virtual void Init() {}
    virtual void FillParContainer();
    virtual void FillParContainer(SpdFieldPar* Pars); 
    
    virtual void GetParameters(SpdFieldPar* Pars);
    
    Bool_t AddField(SpdField* field, Bool_t switch_on = kTRUE);
    
    Bool_t SwitchField(TString name, Bool_t switch_on = kTRUE);
    
    inline void SetOverlapped(Bool_t overlap = kTRUE)  { fIsOverlapped  = overlap;  }
    inline void SetOverlapSumMode(Int_t mode = 0)      { fOverlapSumMode = mode; }
    //inline void SetFieldsOwner(Bool_t isowner = kTRUE) { fIsFieldsOwner = isowner; }
    
    /* ----- getters ----- */
    
    SpdField*  GetField(TString name);
    
    virtual TString GetFieldTypeName() const { return GetTitle(); }
    
    virtual Bool_t IsInsideRegion(Double_t x, Double_t y, Double_t z);
    virtual Bool_t IsInsideRegion(Double_t r, Double_t z);
    
    virtual Double_t GetBx(Double_t x, Double_t y, Double_t z); 
    virtual Double_t GetBy(Double_t x, Double_t y, Double_t z);
    virtual Double_t GetBz(Double_t x, Double_t y, Double_t z);
    
    virtual void GetFieldValue(const Double_t point[3], Double_t* bField); 
    virtual void GetBxyz(const Double_t point[3], Double_t* bField); 
    
    /* ----- print ----- */
  
    virtual void Print(Option_t* option) const;

private:
  
    std::multimap<Int_t, SpdField*> fFields;
    std::multimap<Int_t, SpdField*>::iterator fFieldsIter;
    
    Bool_t   fIsOverlapped;    // default: false
    Bool_t   fIsFieldsOwner;   // default: true
    
    Int_t    fOverlapSumMode;  // 0: averaged sum nonzeros (default); 1: just sum all 
    
    ClassDef(SpdMultiField,1)
};

#endif  /* __SPDMULTIFIELD_H__ */

