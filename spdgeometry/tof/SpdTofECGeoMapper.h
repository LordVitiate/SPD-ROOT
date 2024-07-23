// $Id$
// Author: artur   2021/08/18

#ifndef __SPDTOFECGEOMAPPER_H__
#define __SPDTOFECGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTofECGeoMapper                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTofECGeoMapper: public SpdGeoMapper {

public:
  
    SpdTofECGeoMapper();
    SpdTofECGeoMapper(TString prefix);
    
    virtual ~SpdTofECGeoMapper();
    
    static SpdTofECGeoMapper* Instance();
    static SpdTofECGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void   UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
    
    TString  GetBaseMaterial() const;
    
    Double_t GetMinDistance() const;
    Double_t GetModuleSize() const;
    Double_t GetModuleWidth() const;
    Double_t GetModuleThickness() const;
    
    //-------------------------------
    // Tsinghua geometry version
    
    Double_t GetTofECLengthModuleMRPCTsinghua() const;
    Double_t GetTofECSizeModuleMRPCTsinghua()   const;        
    Double_t GetTofECWidth1ModuleMRPCTsinghua() const;  
    Double_t GetTofECWidth2ModuleMRPCTsinghua() const; 
   
    virtual void Print(Option_t*) const;
      
private:
  
    /* data members */
    static SpdTofECGeoMapper* fInstance;
    
    void SetParameters_1(Bool_t reinit);
    void SetParameters_2(Bool_t reinit);

    ClassDef(SpdTofECGeoMapper,1) 
};

#endif  /* __SPDTOFECGEOMAPPER_H__ */


