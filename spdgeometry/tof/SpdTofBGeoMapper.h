// $Id$
// Author: artur   2021/08/18

#ifndef __SPDTOFBGEOMAPPER_H__
#define __SPDTOFBGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTofBGeoMapper                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTofBGeoMapper: public SpdGeoMapper {

public:
  
    SpdTofBGeoMapper();
    SpdTofBGeoMapper(TString prefix);
    
    virtual ~SpdTofBGeoMapper();
    
    static SpdTofBGeoMapper* Instance();
    static SpdTofBGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void   UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
  
    TString  GetBaseMaterial() const;
  
    Double_t GetModuleLength() const;
    Double_t GetModuleSize() const;
    Double_t GetModuleWidth() const;

    //-------------------------------
    // Protvino geometry version

    Double_t GetTofBLengthModuleMRPCProtvino() const;          
    Double_t GetTofBSizeModuleMRPCProtvino()   const;     
    Double_t GetTofBWidthModuleMRPCProtvino()  const;      

    virtual void Print(Option_t*) const;

private:
  
    /* data members */
    static SpdTofBGeoMapper* fInstance;
    
    void SetParameters_1(Bool_t reinit);
    void SetParameters_2(Bool_t reinit);
    
    ClassDef(SpdTofBGeoMapper,1) 
};

#endif  /* __SPDTOFBGEOMAPPER_H__ */


