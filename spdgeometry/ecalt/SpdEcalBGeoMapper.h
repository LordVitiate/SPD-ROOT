// $Id$
// Author: andre/artur   2020/07/31

#ifndef __SPDECALBGEOMAPPER_H__
#define __SPDECALBGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"
#include "SpdCommonGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalBGeoMapper                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalBGeoMapper: public SpdGeoMapper {

public:
  
    SpdEcalBGeoMapper();
    SpdEcalBGeoMapper(TString prefix);
    
    virtual ~SpdEcalBGeoMapper();
    
    static SpdEcalBGeoMapper* Instance();
    static SpdEcalBGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    /* Setters */
    
    virtual void UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    

    void SetNZBaskets(Int_t value);
    void SetNLayers(Int_t value);
    void SetCellZSize(Double_t value);
    void SetCellInnerPhiSize(Double_t value);
    void SetAbsorberLayerThickness(Double_t value);
    void SetScintLayerThickness(Double_t value);

    void SetCellClearance(Double_t value);
    void SetModuleClearance(Double_t value);
    void SetBasketZClearance(Double_t value);
    void SetBasketPhiClearance(Double_t value);
    
    void SetUseZBasketGaps(Bool_t value);
    void SetZBasketPlugMedium(TString value);


    //these values are calculated only after building geometry
    void SetCalcCellZSize(Double_t value);
    void SetCalcCellInnerPhiSize(Double_t value);
    void SetCalcCellOuterPhiSize(Double_t value);

    void SetBasketNModulesZ(Int_t value);
    void SetBasketNModulesPhi(Int_t value);


    /* Getters */
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
    
    Double_t GetBarrelLength() const;
    Double_t GetBarrelWidth() const;
    Double_t GetBarrelOuterSize() const;
    Double_t GetBarrelRadius() const;
    Int_t    GetNSectors() const;
    Double_t GetBarrelModuleRadialSize() const;

    Int_t    GetNZBaskets() const;
    Int_t    GetNLayers() const;
    Double_t GetCellZSize() const;
    Double_t GetCellInnerSizePhi() const;
    Double_t GetCellOuterSizePhi() const;
    Double_t GetAbsorberLayerThickness() const;
    Double_t GetScintLayerThickness() const;
    
    Double_t GetCellClearance()   const;
    Double_t GetModuleClearance() const;
    Double_t GetBasketZClearance() const;
    Double_t GetBasketPhiClearance() const;
    Double_t GetSectorClearance() const;

    Bool_t   IsUsingZBasketGapsPlugs() const;
    TString  GetBaseMedium() const;
    TString  GetAbsorberMedium() const;
    TString  GetScintMedium() const;
    TString  GetZBasketPlugMedium() const;
    
    TString  GetAbsorberVolName() const;
    TString  GetScintVolName() const;
    
    virtual void Print(Option_t*) const;
            
protected:
    
    /* data members */
    static SpdEcalBGeoMapper* fInstance;
    
    TString fMasterVolName;
    
    void AddParameterFromCommon(TString parname, Int_t accessFunc(SpdCommonGeoMapper*), Bool_t reinit);
    void AddParameterFromCommon(TString parname, Double_t accessFunc(SpdCommonGeoMapper*), Bool_t reinit);

    ClassDef(SpdEcalBGeoMapper,1) 
};

#endif  /* __SPDECALBGEOMAPPER_H__ */


