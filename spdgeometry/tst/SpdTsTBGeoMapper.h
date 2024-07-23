// $Id$
// Author: artur   2018/02/01
//         ruslan  2023/02/21

#ifndef __SPDTSTBGEOMAPPER_H__
#define __SPDTSTBGEOMAPPER_H__

#include "SpdGeoMapper.h"

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsTBGeoMapper                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsTBGeoMapper: public SpdGeoMapper {

public:
  
    SpdTsTBGeoMapper();
    SpdTsTBGeoMapper(TString prefix);
    
    virtual ~SpdTsTBGeoMapper();
    
    static SpdTsTBGeoMapper* Instance();
    static SpdTsTBGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
 
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
   
    virtual void Print(Option_t*) const;
    
    Int_t      GetNSectors() const;
    Double_t   GetSecAngle() const; // deg
    Double_t   GetSecAngleOver2() const; // deg
    Double_t   GetClearance() const;
    Double_t   GetClearance2() const;
    TString    GetBaseMaterial() const;
    TString    GetBaseStrawMaterial() const;
    
    Int_t      GetStrawModuleNPacking(Int_t nmodule) const;
    
    virtual TGeoMatrix* GetNodeMatrix(const SpdGeoTableRec* rec, Int_t loc_i) const;
        
    void SetStrawModulePars(Int_t nmodule /* module number: 1 */, 
                            Double_t size /*cm*/, Double_t gap = 0. /*cm*/, Char_t popt = 'o' /* 'o' or 'e' */);
       
    void SetStrawLayerPars(Int_t nmodule /* module number: 1 */, 
                           Double_t angle /*deg*/, Double_t size /*cm*/,
	                       Double_t gap = 0. /*cm*/, Double_t subgap = 0. /*cm*/,
                           Char_t popt = 'o' /* 'o' or 'e' */);
    
    void SetStrawModulePars(Int_t geometry);
    void SetStrawLayerPars(Int_t geometry);
    
    static Double_t GetLayerWidthFromStrawDiameter(Double_t d, Double_t gap=0., Double_t sgap=0.);
    
protected:
  
    /* data members */
    static SpdTsTBGeoMapper* fInstance;
    
    /* methods */ 
    Bool_t GetStrawModulePars(Int_t nmodule /* module number: 1 */, 
                              Double_t& size /*cm*/, Double_t& gap /*cm*/, Char_t& popt /* 'o' or 'e' */);
    
    Bool_t GetStrawLayerPars(Int_t nmodule /* module number: 1 */, Int_t npack,
                             Double_t& angle /*deg*/, Double_t& size /*cm*/, 
                             Double_t& gap /*cm*/, Double_t& subgap /*cm*/,
                             Char_t& popt /* 'o' or 'e' */);
    
    Bool_t InitStrawLayerPacking(Int_t nmodule, SpdGeoVolPars* module, Int_t ilayer);
    
    virtual TString GetPrintVolParsCap() const;
        
private:
  
    void SetParameters_3(Bool_t reinit);
     
    Double_t fRShift; // module radial shift, cm
    Double_t fZShift; // module axial shift, cm
  
    ClassDef(SpdTsTBGeoMapper,1) 
};

#endif  /* __SPDTSTBGEOMAPPER_H__ */


