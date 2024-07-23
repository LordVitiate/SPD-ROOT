// $Id$
// Author: artur   2021/09/21

#ifndef __SPDZDCGEOMAPPER_H__
#define __SPDZDCGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdZdcGeoMapper                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdZdcGeoMapper: public SpdGeoMapper {

public:
  
    SpdZdcGeoMapper();
    SpdZdcGeoMapper(TString prefix);
    
    virtual ~SpdZdcGeoMapper();
    
    static SpdZdcGeoMapper* Instance();
    static SpdZdcGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = false);
    
    virtual void   UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
  
    TString  GetBaseMaterial() const;
  
    Double_t GetMinDistance() const;
    Double_t GetModuleLength() const;
    Double_t GetModuleWidth() const;
    Double_t GetModuleHmin() const;
    Double_t GetModuleHmax() const;
  
    virtual void Print(Option_t*) const;

private:
  
    /* data members */
    static SpdZdcGeoMapper* fInstance;
    
    void SetParameters_1(Bool_t reinit);
      
    ClassDef(SpdZdcGeoMapper,1) 
};

#endif  /* __SPDZDCGEOMAPPER_H__ */


