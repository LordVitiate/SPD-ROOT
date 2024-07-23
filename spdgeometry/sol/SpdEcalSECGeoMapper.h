// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALSECGEOMAPPER_H__
#define __SPDECALSECGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalSECGeoMapper                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalSECGeoMapper: public SpdGeoMapper {

public:
  
    SpdEcalSECGeoMapper();
    SpdEcalSECGeoMapper(TString prefix);
    
    virtual ~SpdEcalSECGeoMapper();
    
    static SpdEcalSECGeoMapper* Instance();
    static SpdEcalSECGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
     
    virtual void Print(Option_t*) const;
    
    Int_t      GetNSectors();
    Double_t   GetSecAngle(); // deg
    Double_t   GetSecAngleOver2(); // deg
    
    virtual TString GetVolName(Int_t level, Int_t uid);
      
private:
  
    /* data members */
    static SpdEcalSECGeoMapper* fInstance;
    
    void SetParameters_1(Bool_t reinit);
      
    ClassDef(SpdEcalSECGeoMapper,1) 
};

#endif  /* __SPDECALSECGEOMAPPER_H__ */


