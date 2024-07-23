/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#ifndef __SPDSTTGEOPAR_H__
#define __SPDSTTGEOPAR_H__

#include "FairParGenericSet.h"
#include "SpdParameter.h"

class FairParamList;
class TObjArray;

class SpdSttGeoPar : public FairParGenericSet
{
  public:

    SpdSttGeoPar(const char* name = "SpdSttGeoPar",
                 const char* title = "SpdStt Geometry Parameters",
                 const char* context = "TestDefaultContext");
   
   ~SpdSttGeoPar();
    
    virtual void clear();
    virtual void printParams();
    virtual void print(Int_t opt = 0);
    
    inline void  SetGeometryType(Int_t geoType) { fGeoType = geoType; }
    inline Int_t GetGeometryType() { return fGeoType; }
    
    void SetParameters(TObjArray* pars);  // add (and reset) parameters
    void FillParameters(TObjArray* pars); // fill external array
    
    void SetParameter(const Text_t* par_name, Int_t value);
    void SetParameter(const Text_t* par_name, Double_t value);
    
    Bool_t GetParameter(const Text_t* par_name, Int_t& value);
    Bool_t GetParameter(const Text_t* par_name, Double_t& value);
   
    Int_t GetNParameters();
    inline TObjArray* GetParameters() { return fParams; }
    
    Bool_t        FindParameter(const Text_t* par_name);
    SpdParameter* GetParameter(const Text_t* par_name);
     
    virtual void   putParams(FairParamList* l); 
    virtual Bool_t getParams(FairParamList* l);

private:
   
    Int_t      fGeoType;
    TObjArray* fParams;
    
    ClassDef(SpdSttGeoPar,1)
};

#endif
