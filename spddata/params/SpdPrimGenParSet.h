// $Id$
// Author: artur   2018/12/05

#ifndef __SPDPRIMGENPARSET_H__
#define __SPDPRIMGENPARSET_H__

#include "FairParGenericSet.h"
#include "SpdParameter.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPrimGenParSet                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairParamList;

class SpdPrimGenParSet: public FairParGenericSet {

public:

    SpdPrimGenParSet(const char* name = "SpdPrimGenParSet",
                     const char* title = "Spd Primary Generator Parameters",
                     const char* context = "Default Context");
    
    virtual ~SpdPrimGenParSet();

    virtual void clear();
   
    virtual void printParams();
    virtual void print(Int_t opt = 0);
    virtual void printByLabel(Int_t pr);
    
    void   SetParameter(const Text_t* par_name, Bool_t value, Int_t p);
    void   SetParameter(const Text_t* par_name, Char_t value, Int_t p);
    void   SetParameter(const Text_t* par_name, Int_t value, Int_t p);
    void   SetParameter(const Text_t* par_name, Long_t value, Int_t p);
    void   SetParameter(const Text_t* par_name, Float_t value, Int_t p);
    void   SetParameter(const Text_t* par_name, Double_t value, Int_t p);
    void   SetParameter(const Text_t* par_name, const Char_t* value, Int_t p);
    
    Bool_t GetParameter(const Text_t* par_name, Bool_t& value, Int_t p);
    Bool_t GetParameter(const Text_t* par_name, Char_t& value, Int_t p);
    Bool_t GetParameter(const Text_t* par_name, Int_t& value, Int_t p);
    Bool_t GetParameter(const Text_t* par_name, Long_t& value, Int_t p);
    Bool_t GetParameter(const Text_t* par_name, Float_t& value, Int_t p);
    Bool_t GetParameter(const Text_t* par_name, Double_t& value, Int_t p);
    Bool_t GetParameter(const Text_t* par_name, TString& value, Int_t p);
    
    SpdParameter* GetParameter(const Text_t* par_name, Int_t p);
     
    Int_t  GetNParameters();
     
    inline TObjArray* GetParameters() const { return fParams; }
    
    virtual void   putParams(FairParamList* l);
    virtual Bool_t getParams(FairParamList* l);
 
protected:
  
    TObjArray* fParams;

    ClassDef(SpdPrimGenParSet,1)
};

#endif  /* __SPDPRIMGENPARSET_H__ */

