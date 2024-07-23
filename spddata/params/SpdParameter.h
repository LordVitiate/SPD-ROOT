// $Id$
// Author: artur   2016/02/10

#ifndef __SPDPARAMETER_H__
#define __SPDPARAMETER_H__

#include <TNamed.h>
#include <TArrayC.h>
#include <TObjArray.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdParameter                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdParameter: public TNamed {

public:

    SpdParameter() {}   
    virtual ~SpdParameter() {}
    
    SpdParameter(const Text_t* name);
    SpdParameter(const Text_t* name, Bool_t v, Int_t p = 0);
    SpdParameter(const Text_t* name, Int_t v, Int_t p = 0);
    SpdParameter(const Text_t* name, Long_t v, Int_t p = 0);
    SpdParameter(const Text_t* name, Char_t v, Int_t p = 0);
    SpdParameter(const Text_t* name, Float_t v, Int_t p = 0);
    SpdParameter(const Text_t* name, Double_t v, Int_t p = 0);
    SpdParameter(const Text_t* name, const Char_t* v, Int_t p = 0);
    
    /* get-methods */
    TString Name() { return fName;  }
    TString Type() { return fTitle; }
    
    inline Bool_t CheckType(Bool_t)        const { return (fTitle == "Bool_t");   }
    inline Bool_t CheckType(Int_t)         const { return (fTitle == "Int_t");    } 
    inline Bool_t CheckType(Long_t)        const { return (fTitle == "Long_t");   } 
    inline Bool_t CheckType(Char_t)        const { return (fTitle == "Char_t");   } 
    inline Bool_t CheckType(Float_t)       const { return (fTitle == "Float_t");  }
    inline Bool_t CheckType(Double_t)      const { return (fTitle == "Double_t"); }
    inline Bool_t CheckType(const Char_t*) const { return (fTitle == "String");   }
    
    // set value with type checking
    Bool_t SetValue(Bool_t x);
    Bool_t SetValue(Int_t x);
    Bool_t SetValue(Long_t x);
    Bool_t SetValue(Char_t x);
    Bool_t SetValue(Float_t x);
    Bool_t SetValue(Double_t x);
    
    inline void SetPriority(Int_t p) { Priority_ = p; }
    
    // get value with type checking
    Bool_t Value(Bool_t& x) const;
    Bool_t Value(Int_t& x) const;
    Bool_t Value(Long_t& x) const;
    Bool_t Value(Char_t& x) const;
    Bool_t Value(Float_t& x) const;
    Bool_t Value(Double_t& x) const;
    const Char_t* Value() const;
    
    static const Char_t* GetParameter(TObjArray* pars, TString par_name);
    
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Bool_t& value);
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Int_t& value);
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Long_t& value);
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Char_t& value);
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Float_t& value);
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Double_t& value);
    
    static const Char_t* GetParameter(TObjArray* pars, TString par_name, Int_t p, const Char_t*);
    
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Int_t p, Bool_t& value);
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Int_t p, Int_t& value);
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Int_t p, Long_t& value);
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Int_t p, Char_t& value);
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Int_t p, Float_t& value);
    static Bool_t GetParameter(TObjArray* pars, TString par_name, Int_t p, Double_t& value);
    
    inline Int_t GetPriority() const { return Priority_; }

    // set value with and change it's type if needed 
    SpdParameter& operator=(Bool_t x);
    SpdParameter& operator=(Int_t x);
    SpdParameter& operator=(Long_t x);
    SpdParameter& operator=(Char_t x);
    SpdParameter& operator=(Float_t x);
    SpdParameter& operator=(Double_t x);
    SpdParameter& operator=(const Char_t* x);
    
    SpdParameter& operator=(const SpdParameter& x) { return *this; }
    
    void PrintParameter();
    
protected:
 
    static SpdParameter* FindObject(TObjArray* pars, const Char_t* name, const Int_t& p);
  
    TArrayC Value_;
    Int_t   Priority_;

    ClassDef(SpdParameter,1)
};



#endif  /* __SPDPARAMETER_H__ */

