// $Id$
// Author: artur   2018/02/01

#ifndef __SPDTSSBGEOMAPPER_H__
#define __SPDTSSBGEOMAPPER_H__

#include <TObjArray.h>
#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsSBGeoMapper                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsSBGeoMapper: public SpdGeoMapper {

public:
  
    SpdTsSBGeoMapper();
    SpdTsSBGeoMapper(TString prefix);
    
    virtual ~SpdTsSBGeoMapper();
    
    static SpdTsSBGeoMapper* Instance();
    static SpdTsSBGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
     
    virtual void Print(Option_t*) const;
    
    Int_t      GetNSectors() const;
    Double_t   GetSecAngle() const; // deg
    Double_t   GetSecAngleOver2() const; // deg
    Double_t   GetClearance2() const;
    TString    GetBaseMaterial() const;
    TString    GetBaseStrawMaterial() const;
    
    virtual TGeoMatrix* GetNodeMatrix(const SpdGeoTableRec* rec, Int_t loc_i) const;
    
    Int_t GetStrawModuleNPacking(Int_t nmodule) const;
        
    void SetStrawModulePars(Double_t angle /*deg*/, Char_t popt /* 'o' or 'e' */,
                            Double_t size = 1. /*cm*/, Double_t gap = 0./*cm*/);
       
    void SetStrawLayerPars(Double_t angle /*deg*/, Char_t popt /* 'o' or 'e' */,
                           Double_t size = 1. /*cm*/, Double_t gap = 0./*cm*/);
    
    void SetDefaultStrawModulePars(Int_t geotype);
    void SetDefaultStrawLayerPars(Int_t geotype);
    
protected:
  
    /* data members */
    static SpdTsSBGeoMapper* fInstance;
    
    Bool_t GetStrawModulePars(Int_t nmodule /* module number: 1,2,3 */, 
           Double_t& angle /*deg*/, Char_t& popt /* 'o' or 'e' */,
	       Double_t& size /*cm*/, Double_t& gap /*cm*/);
    
    Bool_t GetStrawLayerPars(Int_t nmodule /* module number: 1,2,3 */, Int_t npack,
           Double_t& angle /*deg*/, Char_t& popt /* 'o' or 'e' */,
	       Double_t& size /*cm*/, Double_t& gap /*cm*/);
    
    Bool_t InitStrawModulePacking(SpdGeoVolPars* module, Int_t nmodule);
    Bool_t InitStrawLayerPacking(Int_t nmodule, SpdGeoVolPars* module, Int_t ilayer);
    
    virtual TString GetPrintVolParsCap() const;
        
private:
  
    void SetParameters_1(Bool_t reinit);
     
    Double_t fRShift[3]; // module radial shift, cm
    Double_t fZShift[3]; // module axial shift, cm
  
    ClassDef(SpdTsSBGeoMapper,1) 
};

#endif  /* __SPDTSSBGEOMAPPER_H__ */


