// $Id$
// Author: artur   2017/11/28

#ifndef __SPDFIELD_H__
#define __SPDFIELD_H__

#include <TObject.h>
#include <TString.h>
#include <TMath.h>
#include <set>

#include "FairField.h"
#include "SpdFieldMapData.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdField                                                                   //
//                                                                            //
// FIELD TYPES:                                                               //
//                                                                            //
///@param fType:                                                              //
//                                                                            //
//   -1 = "zero" field, B = (0,0,0) [kG]                                      //
//    0 = constant field                                                      //
//    1 = radial field                                                        //
//    2 = field map                                                           //
//    4 = field map1_8                                                        //
//   >2 = unknown field type treated as -1                                    //
//                                                                            //
// all coordinates should be in [cm] !                                        //
//                                                                            //
// FIELD REGION TYPES:                                                        //
//                                                                            //
///@params :                                                                  //
//                                                                            //
//    "box"                                                                   //
//    "tube"                                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRegion;
class SpdFieldPar;

class SpdField: public FairField {

public:
  
    SpdField();
    SpdField(const Char_t* name);
    SpdField(SpdFieldPar* fieldPar, const Char_t* name = "SpdField");
    
    virtual ~SpdField();
    
    static void    SetFieldParType(TString partype);
    static TString GetFieldParType();
    
    virtual void ResetParameters();
    
    virtual void Init() {}
    virtual void FillParContainer();
    virtual void FillParContainer(SpdFieldPar* Pars); 
        
    virtual SpdRegion* CreateFieldRegion(TString type);
    virtual Bool_t     SwitchFieldRegion(Bool_t in = kTRUE);
    
    /* ----- setters ----- */
     
    virtual void SetFieldParameter(Int_t npar /*parameter number*/, Double_t value /* value*/);
    
    /* ----- getters ----- */
    
    virtual TString    GetFieldTypeName() const { return GetTitle(); }
    
    virtual SpdRegion* GetFieldRegion() const { return fFieldRegion; }
    virtual TString    GetFieldRegionType() const;
    
    virtual Bool_t IsInsideRegion(Double_t x, Double_t y, Double_t z);
    virtual Bool_t IsInsideRegion(Double_t r, Double_t z);
    
    virtual Double_t GetBx(Double_t x, Double_t y, Double_t z);
    virtual Double_t GetBy(Double_t x, Double_t y, Double_t z); 
    virtual Double_t GetBz(Double_t x, Double_t y, Double_t z);
    
    virtual void GetFieldValue(const Double_t point[3], Double_t* bField); 
    virtual void GetBxyz(const Double_t point[3], Double_t* bField); 
    
    virtual Bool_t GetField(const Double_t point[3], Double_t* bField); //TODO should be overloaded in derived class
    
    virtual Bool_t GetBx(Double_t& f, Double_t x, Double_t y, Double_t z) { return kFALSE; } //TODO
    virtual Bool_t GetBy(Double_t& f, Double_t x, Double_t y, Double_t z) { return kFALSE; } //TODO
    virtual Bool_t GetBz(Double_t& f, Double_t x, Double_t y, Double_t z) { return kFALSE; } //TODO
     
    virtual Double_t Value(Double_t x, Double_t y, Double_t z, TString value, Double_t eps = 0);
    
    /* ------ create field data map ------ */
    
    SpdFieldMapData* CreateFieldMapData(SpdFieldMapData* mdata = 0, 
                                        Int_t Nx = 101, Int_t Ny = 101, Int_t Nz = 101);
    
    /* ------ print ------ */
    virtual void Print(Option_t* option = "") const;
    virtual void PrintListOfFields() const;
    virtual void PrintFieldParameters() const {} //TODO
    
    static Bool_t FindFieldName(TString name);
    
protected:
  
    virtual void SetNFieldParameters(Int_t n /*number of parameters*/);
    
    virtual Bool_t GetParameters(SpdFieldPar* pars) { return kTRUE; } //TODO should be implemented in derived class
    virtual Bool_t PutParameters(SpdFieldPar* pars) { return kTRUE; } //TODO should be implemented in derived class
    
    static TString            fFieldParType;
    static std::set<TString>* fUFieldNames;
  
    SpdRegion* fFieldRegion;
    Bool_t     fSwitchFieldRegion;
    
    Int_t      fNpars;
    Double_t*  fParams;
    
    virtual Bool_t GetFieldParameters(SpdFieldPar* pars);
    virtual Bool_t PutFieldParameters(SpdFieldPar* pars); 
        
    virtual Bool_t GetRegionParameters(SpdFieldPar* pars); 
    virtual Bool_t PutRegionParameters(SpdFieldPar* pars); 
    
    virtual void AssignFieldUniqueName(TString name);
    
    inline const Char_t* Add_Name(TString name) 
           { name = fName + "/" + name; return name.Data(); } 
    inline const Char_t* Cut_Name(TString name) 
           { name.Remove(0,name.Last('/')+1); return name.Data(); }
    
    ClassDef(SpdField,1)
};

#endif  /* __SPDFIELD_H__ */

