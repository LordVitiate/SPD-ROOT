// $Id$
// Author: artur   2017/11/29

#ifndef __SPDFIELDPAR_H__
#define __SPDFIELDPAR_H__

#include "FairParGenericSet.h"

#include <TString.h>
#include <map>
#include <string>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdFieldPar                                                                //
//                                                                            //
// FIELD TYPES:                                                               //
//                                                                            //
///@param fFieldType:                                                         //
//                                                                            //
//   -1 = "zero" field, B = (0,0,0)                                           //
//    0 = constant field                                                      //
//    1 = radial field                                                        //
//    2 = field map                                                           //
//   >2 = unknown field type, treated as -1                                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairParamList;
class TObjArray;
class SpdParameter;

class SpdFieldPar : public FairParGenericSet {

public:
  
    SpdFieldPar(const char* name = "SpdFieldPar", 
                const char* title = "SPD field parameters", 
                const char* context = "Default");
   ~SpdFieldPar();

    virtual void clear();
    virtual void printParams();
    virtual void print(Int_t opt = 0);
    
    Bool_t AddField(TString name, Int_t type);
    Bool_t FindField(TString name, Int_t& type) const;
    
    Int_t   GetNFields();
    Bool_t  GetFieldNameType(TString& name, Int_t& type) const;  // return first one
    TString GetFieldName() const;  // return first one
    const std::map<std::string,Int_t>& GetListOfFields() const { return fFields; }
    
    void SetParameters(TObjArray* pars);  // add (and reset) parameters
    void FillParameters(TObjArray* pars); // fill external array
    
    void SetParameter(const Text_t* par_name, Int_t value);
    void SetParameter(const Text_t* par_name, Double_t value);
    void SetParameter(const Text_t* par_name, const Char_t* value);
      
    Bool_t GetParameter(const Text_t* par_name, Int_t& value);
    Bool_t GetParameter(const Text_t* par_name, Double_t& value);
    Bool_t GetParameter(const Text_t* par_name, TString& value);
   
    Int_t GetNParameters();
    inline TObjArray* GetParameters() { return fParams; }
    
    Bool_t        FindParameter(const Text_t* par_name);
    SpdParameter* GetParameter(const Text_t* par_name);
     
    virtual void   putParams(FairParamList* l); 
    virtual Bool_t getParams(FairParamList* l);
    
private:
   
    std::map<std::string,Int_t> fFields; // <name,type>
    
    TObjArray* fParams;
       
    ClassDef(SpdFieldPar,1);
};

#endif /* __SPDFIELDPAR_H__ */
