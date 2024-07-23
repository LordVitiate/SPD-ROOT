// $Id$
// Author: artur   2021/11/26

#ifndef __SPDBBCGEOMAPPER_H__
#define __SPDBBCGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdBbcGeoMapper                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdBbcGeoMapper: public SpdGeoMapper {

public:
  
    SpdBbcGeoMapper();
    SpdBbcGeoMapper(TString prefix);
    
    virtual ~SpdBbcGeoMapper();
    
    static SpdBbcGeoMapper* Instance();
    static SpdBbcGeoMapper* GetMapper();
    
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
    static SpdBbcGeoMapper* fInstance;
    
    void SetParameters_1(Bool_t reinit);

    ClassDef(SpdBbcGeoMapper,1) 
};

#endif  /* __SPDBBCGEOMAPPER_H__ */


