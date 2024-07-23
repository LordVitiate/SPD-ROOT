// $Id$
// Author: artur   2017/11/24

#ifndef __SPDRADIALFIELD_H__
#define __SPDRADIALFIELD_H__

#include "SpdField.h"
#include <TMath.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRadialField                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdFieldPar;

class SpdRadialField : public SpdField {

public:

    SpdRadialField();
    SpdRadialField(const Char_t* name);
    SpdRadialField(SpdFieldPar* fieldPar, const Char_t* name = "SpdRadialField");
    
    virtual ~SpdRadialField() {}
    
    virtual void Init() {};
    
    /* ----- setters ----- */

    inline  void SetRadialFunctionType(int type) { fRadialFunctionType = type; }
     
    /* ----- getters ----- */
    virtual Bool_t GetField(const Double_t point[3], Double_t* bField); 
    
    virtual Bool_t GetBx(Double_t& f, Double_t x, Double_t y, Double_t z);
    virtual Bool_t GetBy(Double_t& f, Double_t x, Double_t y, Double_t z);
    virtual Bool_t GetBz(Double_t& f, Double_t x, Double_t y, Double_t z);
    
    /* ------ print ------ */
    
    virtual void PrintFieldParameters() const;
    
    /* -----  for user definition (optionaly) ----- */

    virtual Double_t Radial_1(const Int_t exception) const; 

protected:
    
    virtual Bool_t GetParameters(SpdFieldPar* pars);
    virtual Bool_t PutParameters(SpdFieldPar* pars);
      
    Int_t fRadialFunctionType;
    
    Double_t Bx() const;
    Double_t By() const;
     
    Double_t fX;
    Double_t fY;
    Double_t fZ;
    Double_t fR;
  
    ClassDef(SpdRadialField,1)
};


#endif  /* __SPDRADIALFIELD_H__ */

