// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALSBGEOMAPPER_H__
#define __SPDECALSBGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalSBGeoMapper                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalSBGeoMapper: public SpdGeoMapper {

public:
  
    SpdEcalSBGeoMapper();
    SpdEcalSBGeoMapper(TString prefix);
    
    virtual ~SpdEcalSBGeoMapper();
    
    static SpdEcalSBGeoMapper* Instance();
    static SpdEcalSBGeoMapper* GetMapper();
    
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
    static SpdEcalSBGeoMapper* fInstance;
    
    void SetParameters_1(Bool_t reinit);
    void SetParameters_2(Bool_t reinit);
      
    ClassDef(SpdEcalSBGeoMapper,1) 
};



#endif  /* __SPDECALSBGEOMAPPER_H__ */


