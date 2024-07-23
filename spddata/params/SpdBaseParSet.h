// $Id$
// Author: artur   2018/10/29

#ifndef __SPDBASEPARSET_H__
#define __SPDBASEPARSET_H__

#include "FairParGenericSet.h"
#include "SpdParameter.h"
#include <TObjString.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdBaseParSet                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairParamList;
class TObjArray;

class SpdBaseParSet : public FairParGenericSet {
  
public:
  
    SpdBaseParSet(const char* name = "SpdBaseParSet",
                  const char* title = "Spd Parameters Set",
                  const char* context = "Default Context");
   
   ~SpdBaseParSet();
    
    virtual void clear();
    
    virtual void printParams();
    virtual void print(Int_t opt = 0);
    virtual void printByLabel(Int_t pr);
    
    void   SetName(const char* name)       { fName  = name;  }
    void   SetTitle(const char* title)     { fTitle = title; }
    void   SetContext(const char* context) { paramContext = context; }
    
    void   SetParameters(TObjArray* pars);            // add (and reset) all parameters
    void   SetParameters(TObjArray* pars, Int_t pr);  // add (and reset) parameters with a certian priority
    
    void   FillParameters(TObjArray* pars);           // add all parameters into external array 
    void   FillParameters(TObjArray* pars, Int_t pr); // add parameters with a certian priority into external array
    
    void   SetParameter(const Text_t* par_name, Bool_t value, Int_t p = 0);
    void   SetParameter(const Text_t* par_name, Char_t value, Int_t p = 0);
    void   SetParameter(const Text_t* par_name, Int_t value, Int_t p = 0);
    void   SetParameter(const Text_t* par_name, Long_t value, Int_t p = 0);
    void   SetParameter(const Text_t* par_name, Float_t value, Int_t p = 0);
    void   SetParameter(const Text_t* par_name, Double_t value, Int_t p = 0);
    void   SetParameter(const Text_t* par_name, const Char_t* value, Int_t p = 0);
    
    Bool_t GetParameter(const Text_t* par_name, Bool_t& value);
    Bool_t GetParameter(const Text_t* par_name, Char_t& value);
    Bool_t GetParameter(const Text_t* par_name, Int_t& value);
    Bool_t GetParameter(const Text_t* par_name, Long_t& value);
    Bool_t GetParameter(const Text_t* par_name, Float_t& value);
    Bool_t GetParameter(const Text_t* par_name, Double_t& value);
    Bool_t GetParameter(const Text_t* par_name, TString& value);
   
    Int_t  GetNParameters();
    
    inline TObjArray* GetParameters() const { return fParams; }
    
    SpdParameter*  GetParameter(const Text_t* par_name);
   
    Bool_t         FindParameter(const Text_t* par_name);
   
    virtual void   putParams(FairParamList* l); 
    virtual Bool_t getParams(FairParamList* l);
 
protected:
  
    TObjArray* fParams;
    
    ClassDef(SpdBaseParSet,1)
};

#endif  /* __SPDBASEPARSET_H__ */

