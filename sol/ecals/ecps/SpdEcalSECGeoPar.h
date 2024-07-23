// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTECGEOPAR_H__
#define __SPDECALTECGEOPAR_H__

#include "FairParGenericSet.h"
#include "SpdParameter.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalSECGeoPar                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairParamList;
class TObjArray;

class SpdEcalSECGeoPar : public FairParGenericSet {
  
public:
  
    SpdEcalSECGeoPar(const char* name = "SpdEcalSECGeoPar",
                     const char* title = "Ecal SEC Geometry Parameters",
                     const char* context = "TestDefaultContext");
   
   ~SpdEcalSECGeoPar();
    
    virtual void clear();
    virtual void printParams();
    virtual void print(Int_t opt = 0);
    
    inline void  SetGeometryType(Int_t geoType) { fGeoType = geoType; }
    inline Int_t GetGeometryType() { return fGeoType; }
    
    void SetParameters(TObjArray* pars);  // add (and reset) parameters
    void FillParameters(TObjArray* pars); // fill external array
    
    void SetParameter(const Text_t* par_name, Int_t value, Int_t p = 0);
    void SetParameter(const Text_t* par_name, Double_t value, Int_t p = 0);
    void SetParameter(const Text_t* par_name, const Char_t* value, Int_t p = 0);
    
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
  
    Int_t      fGeoType;
    TObjArray* fParams;
    
    ClassDef(SpdEcalSECGeoPar,1)
};

#endif /* __SPDECALTECGEOPAR_H__ */
