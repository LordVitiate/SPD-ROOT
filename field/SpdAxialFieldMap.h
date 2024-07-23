// $Id$
// Author: artur   2020/07/04

#ifndef __SPDAXIALFIELDMAP_H__
#define __SPDAXIALFIELDMAP_H__

#include "SpdField.h"
#include "SpdAxialFieldMapData.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdAxialFieldMap                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdFieldPar;

class SpdAxialFieldMap : public SpdField, public SpdAxialFieldMapData {
  
public:
  
    SpdAxialFieldMap();
    SpdAxialFieldMap(const Char_t* name, const Char_t* type = "");
    SpdAxialFieldMap(SpdFieldPar* fieldPar, const Char_t* name = "SpdAxialFieldMap");
    
    virtual ~SpdAxialFieldMap() {}
    
    virtual void Clear();
    virtual void ResetParameters();
    
    virtual Bool_t InitData(TString fileName, TString path = "");
    
    virtual void Init() {};
    
    /* ----- transform field data ----- */
   
    virtual void MultiplyField(Double_t v);
    virtual void MultiplyField(Double_t vr, Double_t vz);
    
    virtual void RotateField(Double_t /*angle*/, Double_t /*theta*/, Double_t /*phi*/) {}
       
    /* ----- setters ----- */
  
    virtual void SetNFieldParameters(Int_t n /*number of parameters*/);
    
    void SetApproxMethod(Int_t method);
    
    /* ----- getters ----- */
    
    virtual Bool_t IsInsideRegion(Double_t x, Double_t y, Double_t z);
    virtual Bool_t IsInsideRegion(Double_t r, Double_t z);
    
    inline  Int_t  GetApproxMethod() { return fApproxMethod; }
    
    virtual Bool_t GetField(const Double_t point[3], Double_t* bField); 
    
    virtual Bool_t GetBx(Double_t& f, Double_t x, Double_t y, Double_t z);
    virtual Bool_t GetBy(Double_t& f, Double_t x, Double_t y, Double_t z);
    virtual Bool_t GetBz(Double_t& f, Double_t x, Double_t y, Double_t z);
    
    inline Double_t  GetRStep() const { return fRstep; }
    inline Double_t  GetZStep() const { return fZstep; }
    
    virtual Double_t GetZmin()  const { return -SpdAxialFieldMapData::GetZmax(); }
    virtual Double_t GetZmax()  const { return  SpdAxialFieldMapData::GetZmax(); }
    
    virtual Int_t    GetNz()    const { return 2*fNz-1; }
    
    inline Double_t  GetXstep() const { return fRstep; }
    inline Double_t  GetYstep() const { return fRstep; }
    inline Int_t     GetNx()    const { return 2*fNr-1; }
    inline Int_t     GetNy()    const { return 2*fNr-1; }
    inline Double_t  GetXmin()  const { return -GetRmax(); }
    inline Double_t  GetYmin()  const { return -GetRmax(); }
    inline Double_t  GetXmax()  const { return  GetRmax(); }
    inline Double_t  GetYmax()  const { return  GetRmax(); }
    
    /* ------ print ------ */
    
    virtual void PrintFieldParameters() const;
    
protected:
  
    virtual Bool_t GetParameters(SpdFieldPar* pars);
    virtual Bool_t PutParameters(SpdFieldPar* pars);
    
    virtual Bool_t InitData();
    
    virtual void   FindCell(const Double_t& r, const Double_t& z); 
    
    /* approximation methods */
    
    virtual Double_t Approx_0(const Double_t* array) const;
    virtual Double_t Approx_1(const Double_t* array) const;
    virtual Double_t Approx_2(const Double_t* array) const;
    
    Int_t   fApproxMethod;
    
    /* const memebers (DATA MAP) */
    
    Double_t fRmin;
    Double_t fZmin;
    
    Double_t fRstep;
    Double_t fZstep;
    
    Double_t fRevCellSize;
    
    const Double_t* fFr;
    const Double_t* fFz;
    
    /* auxiliary memebers */
    
    Double_t fDz1, fDr1, fDz2, fDr2;
    Int_t    fI_00, fI_10, fI_01, fI_11;
           
    ClassDef(SpdAxialFieldMap,1) 

}; 

#endif /*__SPDAXIALFIELDMAP_H__ */
