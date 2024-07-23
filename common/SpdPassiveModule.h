// $Id$
// Author: artur   2018/01/30

#ifndef __SPDPASSIVEMODULE_H__
#define __SPDPASSIVEMODULE_H__

#include "FairModule.h"
#include "TGeoMedium.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPassiveModule                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdPassiveGeoParSet;

class SpdPassiveModule: public FairModule {

public:

    SpdPassiveModule();
    SpdPassiveModule(const char* Name, const char* Prefix, Int_t ModId);
    
    virtual ~SpdPassiveModule();
    
    // Create geometry
    virtual void ConstructGeometry() {}
    
    /* ----- getters ------ */
    
    virtual void SetGeometryType(Int_t/* type*/) {}
    
    virtual void UnsetMaterials(TString /*media*/) {}
    virtual void ResetMaterials(/*set dafault materials*/) {}
     
    /* ----- getters ------ */
    
    virtual Bool_t IsGeometryLocked() const { return kFALSE;    }
    virtual Int_t  GetGeoType()       const { return fGeoType;  }
    virtual Int_t  GetId()            const { return fModuleId; }
    
    virtual Double_t GetCapacity() const { return 0; } // cm^3
    virtual Double_t GetMass()     const { return 0; } // g
            Double_t GetDensity()  const;  // g/cm^3
          
    const Char_t*  GetPrefix() const { return fTitle.Data(); }      
    
    virtual void   Print(Option_t*) const;
    
    virtual Bool_t FillParsIn(SpdPassiveGeoParSet* params);
    virtual Bool_t LoadParsFrom(SpdPassiveGeoParSet* params);
    
protected:
  
    SpdPassiveGeoParSet* GetParameters();
    
    virtual TString     FullName(TString name, Bool_t add_uscore = kFALSE);
    virtual TGeoMedium* FindMedium(TString& medname, TString default_med);
    
    Int_t       fGeoType;
    Int_t       fModuleId;
    TGeoVolume* fMasterVolume; //!
    
    TString     fUnsetMedia; //! 
     
    ClassDef(SpdPassiveModule,1)
};

#endif  /* __SPDPASSIVEMODULE_H__ */

