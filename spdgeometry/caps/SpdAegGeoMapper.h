// $Id$
// Author: artur   2021/11/26

#ifndef __SPDAEGGEOMAPPER_H__
#define __SPDAEGGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdAegGeoMapper                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdAegGeoMapper: public SpdGeoMapper {

public:
  
    SpdAegGeoMapper();
    SpdAegGeoMapper(TString prefix);
    
    virtual ~SpdAegGeoMapper();
    
    static SpdAegGeoMapper* Instance();
    static SpdAegGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void   UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
    
    TString  GetBaseMaterial() const;
    
    Double_t GetMinDistance() const;
    Double_t GetModuleSize() const;
    Double_t GetModuleWidth() const;
    Double_t GetModuleThickness() const;
     
    virtual void Print(Option_t*) const;
      
private:
  
    /* data members */
    static SpdAegGeoMapper* fInstance;
    
    void SetParameters_1(Bool_t reinit);

    ClassDef(SpdAegGeoMapper,1) 
};

#endif  /* __SPDAEGGEOMAPPER_H__ */


