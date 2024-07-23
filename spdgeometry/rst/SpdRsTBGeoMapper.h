// $Id$
// Author: artur   2018/02/01

#ifndef __SPDRSTBGEOMAPPER_H__
#define __SPDRSTBGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTBGeoMapper                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsTBGeoMapper: public SpdGeoMapper {

public:
  
    SpdRsTBGeoMapper();
    SpdRsTBGeoMapper(TString prefix);
    
    virtual ~SpdRsTBGeoMapper();
    
    static SpdRsTBGeoMapper* Instance();
    static SpdRsTBGeoMapper* GetMapper();
    
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
    static SpdRsTBGeoMapper* fInstance;
    
    void SetParameters_1(Bool_t reinit);
    void SetParameters_2(Bool_t reinit); 
      
    ClassDef(SpdRsTBGeoMapper,1) 
};

#endif  /* __SPDRSTBGEOMAPPER_H__ */


