// $Id$
// Author: artur   2021/01/20

#ifndef __SPDSETPARSET_H__
#define __SPDSETPARSET_H__

#include <TObjArray.h>
#include "FairParGenericSet.h"
#include "SpdFieldPar.h"
#include "SpdPrimGenParSet.h"
#include "SpdPassiveGeoParSet.h"
#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdSetParSet                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdSetParSet: public FairParGenericSet {

public:

    SpdSetParSet();
    virtual ~SpdSetParSet();
    
    // Setters
    void SetFieldPars(FairParGenericSet* pars)      { fFieldPars      = dynamic_cast<SpdFieldPar*>(pars);         }
    void SetPrimGenPars(FairParGenericSet* pars)    { fPrimGenPars    = dynamic_cast<SpdPrimGenParSet*>(pars);    }
    void SetPassiveGeoPars(FairParGenericSet* pars) { fPassiveGeoPars = dynamic_cast<SpdPassiveGeoParSet*>(pars); }
    
    void AddActiveGeoPars(FairParGenericSet* pars); 
    
    void AddHitMakerPars(FairParGenericSet* pars);
    void AddObjMakerPars(FairParGenericSet* pars);
 
    // Getters
    SpdFieldPar*         GetFieldPars()      const { return fFieldPars;   }
    SpdPrimGenParSet*    GetPrimGenPars()    const { return fPrimGenPars; }
    SpdPassiveGeoParSet* GetPassiveGeoPars() const { return fPassiveGeoPars; }
    
    SpdParSet*           GetActiveGeoPars(TString pars_name) const;
    
    SpdParSet* GetHitMakerPars(TString pars_name) const;
    SpdParSet* GetObjMakerPars(TString pars_name) const;
    
    FairParGenericSet*   FindParameters(TString pars_name) const;
 
    // Print
    void PrintParameters(Int_t opt = 1) const;
      
    virtual void   putParams(FairParamList*) {}
    virtual Bool_t getParams(FairParamList*) { return true; }
    
private:

    // 1st level parameters (primary MC)
    SpdFieldPar*         fFieldPars;
    SpdPrimGenParSet*    fPrimGenPars;
    
          /* geometry parameters */
    SpdPassiveGeoParSet* fPassiveGeoPars;
    TObjArray*           fActiveGeoPars;
    
    // 2nd level parameters
    TObjArray*           fHitMakerPars;
    
    // 3rd level parameters
    TObjArray*           fObjMakerPars;
   
    ClassDef(SpdSetParSet,1)
};

#endif  /* __SPDSETPARSET_H__ */

