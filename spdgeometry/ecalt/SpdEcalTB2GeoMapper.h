// $Id$
// Author: andre/artur   2020/07/31

#ifndef __SPDECALTB2GEOMAPPER_H__
#define __SPDECALTB2GEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"
#include "SpdCommonGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTB2GeoMapper                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTB2GeoMapper: public SpdGeoMapper {

public:
  
    SpdEcalTB2GeoMapper();
    SpdEcalTB2GeoMapper(TString prefix);
    
    virtual ~SpdEcalTB2GeoMapper();
    
    static SpdEcalTB2GeoMapper* Instance();
    static SpdEcalTB2GeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    /* Setters */
    
    virtual void UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    void SetForceInsideBasket(Bool_t value);
    void SetForceCellSize(Bool_t value);

    void SetCellZSize(Double_t value);
    void SetCellInnerPhiSize(Double_t value);
    void SetCellOuterPhiSize(Double_t value);

    void SetNLayers(Int_t value);

    void SetAbsorberLayerThickness(Double_t value);
    void SetScintLayerThickness(Double_t value);

    void SetCellClearance(Double_t value);
    void SetModuleClearance(Double_t value);
    
    void SetBasketNModulesZ(Int_t value);
    void SetBasketNModulesPhi(Int_t value);

    /* Getters */
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
    
    Int_t    GetNLayers() const;
    Int_t    GetNSectors() const;
    
    Double_t GetBarrelLength() const;
    Double_t GetBarrelMaxTheta() const;
    Double_t GetBarrelWidth() const;
    Double_t GetBarrelOuterSize() const;
    
    Double_t GetSectorClearance() const;
    Double_t GetBasketClearance() const;
    Double_t GetModuleClearance() const;
    Double_t GetCellClearance()   const;

    Double_t GetBarrelModuleRadialSize() const;
      
    Double_t GetLayerInnerSizeTheta() const;
    Double_t GetLayerInnerSizePhi() const;
    Double_t GetLayerOuterSizeTheta() const;
    Double_t GetLayerOuterSizePhi() const;
    Double_t GetLayer1SizeZ() const;
    Double_t GetLayer2SizeZ() const;
    
    Bool_t   IsForceCellSize() const;
    Bool_t   IsTrimModuleLength() const;
    
    TString  GetBaseMedium() const;
    TString  GetAbsorberMedium() const;
    TString  GetScintMedium() const;
    
    TString  GetAbsorberVolName() const;
    TString  GetScintVolName() const;
    
    virtual void Print(Option_t*) const;
            
protected:
    
    /* data members */
    static SpdEcalTB2GeoMapper* fInstance;
    
    TString fMasterVolName;
    
    void AddParameterFromCommon(TString parname, Int_t accessFunc(SpdCommonGeoMapper*), Bool_t reinit);
    void AddParameterFromCommon(TString parname, Double_t accessFunc(SpdCommonGeoMapper*), Bool_t reinit);

    ClassDef(SpdEcalTB2GeoMapper,1) 
};

#endif  /* __SPDECALTB2GEOMAPPER_H__ */


