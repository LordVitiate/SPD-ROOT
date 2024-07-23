// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTECGEOMAPPER_H__
#define __SPDECALTECGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTECGeoMapper                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTECGeoMapper: public SpdGeoMapper {

public:
  
    SpdEcalTECGeoMapper();
    SpdEcalTECGeoMapper(TString prefix);
    
    virtual ~SpdEcalTECGeoMapper();
    
    static SpdEcalTECGeoMapper* Instance();
    static SpdEcalTECGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
    
    virtual void Print(Option_t*) const;
    
    Int_t      GetNSectors();
    Double_t   GetSecAngle(); // deg
    Double_t   GetSecAngleOver2(); // deg
    
    virtual TString GetVolName(Int_t level, Int_t uid);
      
protected:
  
    /* data members */
    static SpdEcalTECGeoMapper* fInstance;
    
    virtual void SetParameters_1(Bool_t reinit);
    virtual void SetParameters_2(Bool_t reinit);
   
    ClassDef(SpdEcalTECGeoMapper,1) 
};

#endif  /* __SPDECALTECGEOMAPPER_H__ */


