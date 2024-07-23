// $Id$
// Author: andre/artur   2020/07/31

#ifndef __SPDECALTEC2ECGEOMAPPER_H__
#define __SPDECALTEC2ECGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"
#include "SpdCommonGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTEC2GeoMapper                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


class SpdEcalTEC2GeoMapper: public SpdGeoMapper {

public:
  
    SpdEcalTEC2GeoMapper();
    SpdEcalTEC2GeoMapper(TString prefix);
    
    virtual ~SpdEcalTEC2GeoMapper();
    
    static SpdEcalTEC2GeoMapper* Instance();
    static SpdEcalTEC2GeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    /* Setters */
    
    virtual void UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    void SetCellSize(Double_t value);
    void SetNLayers(Int_t value);
    void SetAbsorberLayerThickness(Double_t value);
    void SetScintLayerThickness(Double_t value);
    void SetCellClearance(Double_t value);
    void SetModuleClearance(Double_t value);
    void SetNModulesX(Int_t value);
    void SetNModulesY(Int_t value);
    
    /* Getters */
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;

    Double_t GetLayer1SizeZ() const;
    Double_t GetLayer2SizeZ() const;
    Double_t GetCellSize() const;
    Int_t    GetNLayers() const;
    Double_t GetCellClearance() const;
    Double_t GetModuleClearance() const;
    Double_t GetEndcapMinDist() const;
    Double_t GetEndcapThickness() const;
    Double_t GetEndcapSize() const;
    Double_t GetEndcapWidth() const;
    
    TString  GetBaseMedium() const;
    TString  GetAbsorberMedium() const;
    TString  GetScintMedium() const;
    
    TString  GetAbsorberVolName() const;
    TString  GetScintVolName() const;
    
    virtual void Print(Option_t*) const;
    
protected:
    
    /* data members */
    static SpdEcalTEC2GeoMapper* fInstance;
    
    TString fMasterVolName;
     
    void AddParameterFromCommon(TString parname, Int_t accessFunc(SpdCommonGeoMapper*), Bool_t reinit);
    void AddParameterFromCommon(TString parname, Double_t accessFunc(SpdCommonGeoMapper*), Bool_t reinit);
    
    ClassDef(SpdEcalTEC2GeoMapper,1) 
};

#endif  /* __SPDECALTEC2ECGEOMAPPER_H__ */


