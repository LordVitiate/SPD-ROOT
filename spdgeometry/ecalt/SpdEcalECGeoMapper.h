// $Id$
// Author: andre/artur   2020/07/31

#ifndef __SPDECALECECGEOMAPPER_H__
#define __SPDECALECECGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"
#include "SpdCommonGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalECGeoMapper                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


class SpdEcalECGeoMapper: public SpdGeoMapper {

public:
  
    SpdEcalECGeoMapper();
    SpdEcalECGeoMapper(TString prefix);
    
    virtual ~SpdEcalECGeoMapper();
    
    static SpdEcalECGeoMapper* Instance();
    static SpdEcalECGeoMapper* GetMapper();
    
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

    Double_t GetCellSize() const;
    Int_t    GetNLayers() const;
    Double_t GetAbsorberLayerThickness() const;
    Double_t GetScintLayerThickness() const;
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
    static SpdEcalECGeoMapper* fInstance;
    
    TString fMasterVolName;
     
    void AddParameterFromCommon(TString parname, Int_t accessFunc(SpdCommonGeoMapper*), Bool_t reinit);
    void AddParameterFromCommon(TString parname, Double_t accessFunc(SpdCommonGeoMapper*), Bool_t reinit);
    
    ClassDef(SpdEcalECGeoMapper,1) 
};

#endif  /* __SPDECALECECGEOMAPPER_H__ */


