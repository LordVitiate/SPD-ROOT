// $Id$
// Author: artur   2017/10/10

#ifndef __SPDCONSTFIELD_H__
#define __SPDCONSTFIELD_H__

#include "SpdField.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdConstField                                                              //
//                                                                            //
// A constant (homogeneous) magnetic field                                    // 
//                                                                            //
///  @param: bX, bY, bZ  Field values [kG]                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdFieldPar;

class SpdConstField : public SpdField {

public:    

  SpdConstField();
  SpdConstField(const char* name);
  SpdConstField(SpdFieldPar* fieldPar, const Char_t* name = "SpdConstField");

  virtual ~SpdConstField() {}
  
  virtual void Init() {};

  /* ----- setters ----- */
  
  virtual void SetNFieldParameters(Int_t /* npar = 3 */);
  
  inline  void SetField(Double_t bX, Double_t bY, Double_t bZ);
  
  /* ----- getters ----- */
  virtual Double_t GetBx(Double_t x, Double_t y, Double_t z);
  virtual Double_t GetBy(Double_t x, Double_t y, Double_t z); 
  virtual Double_t GetBz(Double_t x, Double_t y, Double_t z);
    
  virtual Bool_t GetField(const Double_t point[3], Double_t* bField); 
    
  virtual Bool_t GetBx(Double_t& f, Double_t x, Double_t y, Double_t z);
  virtual Bool_t GetBy(Double_t& f, Double_t x, Double_t y, Double_t z);
  virtual Bool_t GetBz(Double_t& f, Double_t x, Double_t y, Double_t z);

  inline Double_t GetBx() const { return fParams[0]; }
  inline Double_t GetBy() const { return fParams[1]; }
  inline Double_t GetBz() const { return fParams[2]; }
  
  /* ------ print ------ */
    
  virtual void PrintFieldParameters() const;

private:

  ClassDef(SpdConstField, 1);

};

//_____________________________________________________________________________
inline void SpdConstField::SetField(Double_t bX, Double_t bY, Double_t bZ) 
{
    fParams[0] = bX; fParams[1] = bY; fParams[2] = bZ;
}



#endif
