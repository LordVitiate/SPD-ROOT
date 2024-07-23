// $Id$
// Author: artur   2019/09/09

#ifndef __SPDGFMAGNETICFIELD_H__
#define __SPDGFMAGNETICFIELD_H__

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGFMagneticField                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "AbsBField.h"
#include "SpdField.h"

class SpdRegion;

class SpdGFMagneticField: public genfit::AbsBField {

public:

    SpdGFMagneticField();
    SpdGFMagneticField(SpdField* f, Bool_t IsOwn = kFALSE);
    SpdGFMagneticField(TString filename, TString fieldname);
     
    virtual ~SpdGFMagneticField();
    
    virtual TVector3 get(const TVector3& position) const override;
    virtual void get(const double& posX, const double& posY, const double& posZ, 
                     double& Bx, double& By, double& Bz) const override;

    virtual void Clear();
    
    /* setters */
    
    SpdField*  SetField(SpdField* f, Bool_t IsOwn = kFALSE);
    SpdField*  SetFieldMap(TString filename, TString fieldname);
         
    /* getters */
    
    SpdField*  GetField() { return fField_; }
    Bool_t     GetIsOwn() { return fIsOwnField_; }
    
    SpdRegion* GetFieldRegion(TString region = ""); // get or create field region

private:

    SpdField* fField_;
    Bool_t    fIsOwnField_;

    ClassDef(SpdGFMagneticField,1)
};

#endif  /* __SPDGFMAGNETICFIELD_H__ */

