// $Id$
// Author: alver   2020/07/17

#ifndef __SPDRSTEC2GEOMAPPER_H__
#define __SPDRSTEC2GEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTEC2GeoMapper                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsTEC2GeoMapper: public SpdGeoMapper {

public:
  
    SpdRsTEC2GeoMapper();
    SpdRsTEC2GeoMapper(TString prefix);
    
    virtual ~SpdRsTEC2GeoMapper();
    
    static SpdRsTEC2GeoMapper* Instance();
    static SpdRsTEC2GeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
     
    virtual void Print(Option_t*) const;
    
    Int_t      GetNSectors() const;
    Double_t   GetSecAngleOver2() const; // deg
    Double_t   GetModuleInnerRadius() const;
    Double_t   GetModuleOuterRadius() const;
    Double_t   GetModuleThickness() const;
    Double_t   GetDist() const;
    Int_t      GetNLayers() const;

    TString  GetBaseMaterial() const;     
    TString  GetAbsorbMaterial() const;     
    TString  GetPassiveMaterialMDT() const;     
    TString  GetActiveMaterialMDT() const;

private:
  
    /* data members */
    static SpdRsTEC2GeoMapper* fInstance;
    
    void SetParameters(Bool_t reinit);
      
    ClassDef(SpdRsTEC2GeoMapper,1) 
};

#endif  /* __SPDRSTEC2GEOMAPPER_H__ */


