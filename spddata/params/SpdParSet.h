// $Id$
// Author: artur   2018/10/29

#ifndef __SPDPARSET_H__
#define __SPDPARSET_H__

#include "FairParGenericSet.h"
#include "SpdParameter.h"
#include <TObjString.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdParSet                                                                  //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairParamList;
class TObjArray;

class SpdParSet : public FairParGenericSet {
  
public:
  
    SpdParSet(const char* name = "SpdParSet",
              const char* title = "Spd Parameters Set",
              const char* context = "Default Context");
   
   ~SpdParSet();
    
    virtual void clear();
    
    virtual void printParams();
    virtual void print(Int_t opt = 0);
    virtual void printByLabel(Int_t pr);
    
    void SetParameters(TObjArray* pars);            // add (and re-set) all parameters
    void SetParameters(TObjArray* pars, Int_t pr);  // add (and re-set) parameters with a certian priority
    void SetMapperParameters(TObjArray* pars);      // add (and re-set) parameters with priority > -1
    
    void FillParameters(TObjArray* pars);           // add all parameters into external array 
    void FillParameters(TObjArray* pars, Int_t pr); // add parameters with a certian priority into external array
    void FillMapperParameters(TObjArray* pars);     // add parameters with priority > -1 into external array
    
    void SetParameter(const Text_t* par_name, Bool_t value, Int_t p = 0);
    void SetParameter(const Text_t* par_name, Char_t value, Int_t p = 0);
    void SetParameter(const Text_t* par_name, Int_t value, Int_t p = 0);
    void SetParameter(const Text_t* par_name, Long_t value, Int_t p = 0);
    void SetParameter(const Text_t* par_name, Float_t value, Int_t p = 0);
    void SetParameter(const Text_t* par_name, Double_t value, Int_t p = 0);
    void SetParameter(const Text_t* par_name, const Char_t* value, Int_t p = 0);
    
    Bool_t GetParameter(const Text_t* par_name, Bool_t& value);
    Bool_t GetParameter(const Text_t* par_name, Char_t& value);
    Bool_t GetParameter(const Text_t* par_name, Int_t& value);
    Bool_t GetParameter(const Text_t* par_name, Long_t& value);
    Bool_t GetParameter(const Text_t* par_name, Float_t& value);
    Bool_t GetParameter(const Text_t* par_name, Double_t& value);
    Bool_t GetParameter(const Text_t* par_name, TString& value);
   
    Int_t  GetNParameters();
    Int_t  GetNNodes();
    
    inline TObjArray* GetParameters() const { return fParams; }
    inline TObjArray* GetNodes()      const { return fNodes;  }
   
    SpdParameter* GetParameter(const Text_t* par_name);
    TString       GetNodeFullName(Int_t num);
   
    Bool_t        FindParameter(const Text_t* par_name);
    Int_t         FindNode(TObjString node);
    
    virtual void   putParams(FairParamList* l); 
    virtual Bool_t getParams(FairParamList* l);
 
protected:
  
    TObjArray* fParams;
    TObjArray* fNodes;
    
    ClassDef(SpdParSet,1)
};

#endif  /* __SPDPARSET_H__ */

