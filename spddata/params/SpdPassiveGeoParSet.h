// $Id$
// Author: artur   2018/01/23

#ifndef __SPDPASSIVEGEOPARSET_H__
#define __SPDPASSIVEGEOPARSET_H__

#include "FairParGenericSet.h"
#include "SpdParameter.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPassiveGeoParSet                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairParamList;
class TObjArray;

class SpdPassiveGeoParSet : public FairParGenericSet  {
  
public:
  
    SpdPassiveGeoParSet(const char* name = "SpdPassiveGeoParSet",
                     const char* title = "Spd Passive Geometry Parameters Set",
                     const char* context = "TestDefaultContext");
   
   ~SpdPassiveGeoParSet();
    
    virtual void clear();
    virtual void printParams();
    virtual void print(Int_t opt = 0);
    
    void SetParameters(TObjArray* pars);  // add (and reset) parameters
    void FillParameters(TObjArray* pars); // fill external array
    
    void SetParameter(const Text_t* par_name, Int_t value);
    void SetParameter(const Text_t* par_name, Double_t value);
    void SetParameter(const Text_t* par_name, const Char_t* value);
    
    Bool_t GetParameter(const Text_t* par_name, Int_t& value);
    Bool_t GetParameter(const Text_t* par_name, Double_t& value);
    Bool_t GetParameter(const Text_t* par_name, TString& value);
   
    Int_t  GetNParameters();
    inline TObjArray* GetParameters() { return fParams; }
    
    Bool_t        FindParameter(const Text_t* par_name);
    SpdParameter* GetParameter(const Text_t* par_name);
     
    virtual void   putParams(FairParamList* l); 
    virtual Bool_t getParams(FairParamList* l);
 
private:
   
    TObjArray* fParams;
    
    ClassDef(SpdPassiveGeoParSet,1)
};

#endif
