// $Id$
// Author: artur   2018/02/01

#ifndef __SPDRSTB2GEOMAPPER_H__
#define __SPDRSTB2GEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTB2GeoMapper                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsTB2GeoMapper: public SpdGeoMapper {

public:
  
    SpdRsTB2GeoMapper();
    SpdRsTB2GeoMapper(TString prefix);
    
    virtual ~SpdRsTB2GeoMapper();
    
    static SpdRsTB2GeoMapper* Instance();
    static SpdRsTB2GeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    TString  GetBaseMaterial() const;
    TString  GetAbsorbMaterial() const;
    TString  GetPassiveMaterialMDT() const;
    TString  GetActiveMaterialMDT() const;

    Double_t GetModuleLength() const;
    Double_t GetModuleSize() const;
    Double_t GetModuleWidth() const;
    Int_t    GetNLayers() const;
    
    Double_t GetModuleHmin() const;
    Double_t GetModuleHmax() const;
    Double_t GetModuleLmin() const;
    Double_t GetModuleLmax() const;

    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
     
    virtual void Print(Option_t*) const;
    
    Int_t      GetNSectors() const; 
    Double_t   GetSecAngle() const; // deg
    
private:
  
    /* data members */
    static SpdRsTB2GeoMapper* fInstance;
    
    void SetParameters(Bool_t reinit);
      
    ClassDef(SpdRsTB2GeoMapper,1) 
};

#endif  /* __SPDRSTB2GEOMAPPER_H__ */


