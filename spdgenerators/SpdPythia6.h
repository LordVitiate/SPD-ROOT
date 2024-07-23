// $Id$
// Author: artur   2018/12/14

#ifndef __SPDPYTHIA6_H__
#define __SPDPYTHIA6_H__

#include <TPythia6.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPythia6                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdParameter;
class SpdPrimGenParSet;

class SpdPythia6: public TPythia6 {

public:

    SpdPythia6(Int_t storage_index = -10);    
    virtual ~SpdPythia6();
    
    void  SetStorageIndex(Int_t index) { fStorageIndex = index; }
    Int_t GetStorageIndex() const { return fStorageIndex; } 
    
    virtual void  SelectParSet(Int_t n);
    virtual Int_t GetParSetNumber() const { return fParSetNumber; } 
    
    /* --------- set/get Pythia6 parameters --------- */
    
    // ****** /PYINT2/
    
    int     GetISET(int i)        { return fPyint2->ISET[i-1]; }    
    int     GetKFPR(int i, int j) { return fPyint2->KFPR[j-1][i-1]; } 
    double  GetCOEF(int i, int j) { return fPyint2->COEF[j-1][i-1]; } 
    
    void    SetISET(int i, int x)           { fPyint2->ISET[i-1] = x; }    
    void    SetKFPR(int i, int j, int k)    { fPyint2->KFPR[j-1][i-1] = k; } 
    void    SetCOEF(int i, int j, double c) { fPyint2->COEF[j-1][i-1] = c; } 
    
    // ****** /PYINT3/
    
    double  GetXSFX(int i, int j) { return fPyint3->XSFX[j-1][i-1]; }    
    int     GetISIG(int i, int j) { return fPyint3->ISIG[j-1][i-1]; } 
    double  GetSIGH(int i)        { return fPyint3->SIGH[i-1]; }  
    
    void    SetXSFX(int i, int j, double x) { fPyint3->XSFX[j-1][i-1] = x; }    
    void    SetISIG(int i, int j, int x)    { fPyint3->ISIG[j-1][i-1] = x; } 
    void    SetSIGH(int i, double s)        { fPyint3->SIGH[i-1] = s; }  
        
    /* ------------- set/get parameters ------------- */
    
    virtual Bool_t FillParsIn(SpdPrimGenParSet* params);
    virtual Bool_t LoadParsFrom(SpdPrimGenParSet* params);   
    
    void   SetParameter(TString par_name, Bool_t value);
    void   SetParameter(TString par_name, Char_t value);
    void   SetParameter(TString par_name, Int_t value);
    void   SetParameter(TString par_name, Long_t value);
    void   SetParameter(TString par_name, Float_t value);
    void   SetParameter(TString par_name, Double_t value);
    void   SetParameter(TString par_name, const Char_t* value);
    
    Bool_t GetParameter(TString par_name, Bool_t& value);
    Bool_t GetParameter(TString par_name, Char_t& value);
    Bool_t GetParameter(TString par_name, Int_t& value);
    Bool_t GetParameter(TString par_name, Long_t& value);
    Bool_t GetParameter(TString par_name, Float_t& value);
    Bool_t GetParameter(TString par_name, Double_t& value);
    Bool_t GetParameter(TString par_name, TString& value);
    
    SpdParameter* GetPar(TString par_name);
    
    virtual void Print(Option_t* opt = "") const;
    
protected:
   
    virtual Bool_t CheckParName(TString& parname);
    
    virtual void  SetParameters_1();
    virtual void  FillParameters_1(SpdPrimGenParSet* params);
    virtual void  LoadParameters_1(SpdPrimGenParSet* params);
    
    Int_t      fStorageIndex;
    TObjArray* fParams;
    
    Int_t      fParSetNumber;
    
    // auxiliary

    Bool_t     fCheckParName;
    
    ClassDef(SpdPythia6,1)
};

#endif  /* __SPDPYTHIA6_H__ */

