// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTBGEOMAPPER_H__
#define __SPDECALTBGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTBGeoMapper                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTBGeoMapper: public SpdGeoMapper {

public:
  
    SpdEcalTBGeoMapper();
    SpdEcalTBGeoMapper(TString prefix);
    
    virtual ~SpdEcalTBGeoMapper();
    
    static SpdEcalTBGeoMapper* Instance();
    static SpdEcalTBGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
   
    virtual void Print(Option_t*) const;
    
    Int_t      GetNSectors();
    Double_t   GetSecAngle(); // deg
    Double_t   GetSecAngleOver2(); // deg
    Double_t   GetClearance2();
    
    virtual TString GetVolName(Int_t level, Int_t uid);
      
private:
  
    /* data members */
    static SpdEcalTBGeoMapper* fInstance;
    
    void SetParameters_1(Bool_t reinit);
    void SetParameters_2(Bool_t reinit); 
      
    ClassDef(SpdEcalTBGeoMapper,1) 
};

#endif  /* __SPDECALTBGEOMAPPER_H__ */


