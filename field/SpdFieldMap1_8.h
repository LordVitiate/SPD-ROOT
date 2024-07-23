//
//  Created 29/09/17  by A. Ivanov
//
//  Redesign:
//          18/01/17 (A. Tkachenko)
//
//  Adapted for 1/8 of the field map by I. Denisenko
//    DOES NOT SUPPORT TRANSLATIONS, exception will be thrown

#ifndef __SPDFIELDMAP1_8_H__
#define __SPDFIELDMAP1_8_H__

#include "SpdField.h"
#include "SpdFieldMapData1_8.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdFieldMap1_8                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdFieldPar;

class SpdFieldMap1_8 : public SpdField, public SpdFieldMapData1_8 {

public:

    SpdFieldMap1_8();
    SpdFieldMap1_8(const Char_t* name, const Char_t* type = "");
    SpdFieldMap1_8(SpdFieldPar* fieldPar, const Char_t* name = "SpdFieldMap1_8");

    virtual ~SpdFieldMap1_8() {}

    virtual void Clear();
    virtual void ResetParameters();

    virtual Bool_t InitData(TString fileName, TString path = "");

    virtual void Init() {};

    /* ----- transform field data ----- */

    virtual void MultiplyField(Double_t v);
    virtual void MultiplyField(Double_t vx, Double_t vy, Double_t vz);
    virtual void ShiftField(Double_t dx, Double_t dy, Double_t dz);

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

    inline Double_t GetXStep() const { return fXstep; }
    inline Double_t GetYStep() const { return fYstep; }
    inline Double_t GetZStep() const { return fZstep; }

    /* ------ print ------ */

    virtual void PrintFieldParameters() const;

protected:

    virtual Bool_t GetParameters(SpdFieldPar* pars);
    virtual Bool_t PutParameters(SpdFieldPar* pars);

    virtual Bool_t InitData();

    virtual void   FindCell(const Double_t& x, const Double_t& y, const Double_t& z);

    /* approximation methods */

    virtual Double_t Approx_0(const Float_t* array) const;
    virtual Double_t Approx_1(const Float_t* array) const;
    virtual Double_t Approx_2(const Float_t* array) const;

    Int_t   fApproxMethod;

    /* const memebers (DATA MAP) */

    Double_t fXmin;
    Double_t fYmin;
    Double_t fZmin;

    Double_t fXstep;
    Double_t fYstep;
    Double_t fZstep;

    Double_t fRevCellSize;

    const Float_t* fFx;
    const Float_t* fFy;
    const Float_t* fFz;

    /* auxiliary memebers */

    Double_t fDx1, fDy1, fDz1,
             fDx2, fDy2, fDz2;

    Int_t fI_000, fI_001, fI_010, fI_011,
          fI_100, fI_101, fI_110, fI_111;

    ClassDef(SpdFieldMap1_8,1)

};

#endif /*__SPDFIELDMAP_H__ */
