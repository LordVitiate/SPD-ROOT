// $Id$
// Author: artur   2019/09/16

#ifndef __SPDGEOLOADER_H__
#define __SPDGEOLOADER_H__

#include <TObject.h>
#include <TString.h>
#include <map>
#include <set>

#include "SpdDetectorList.h"

class TFile;
class SpdParSet;
class SpdPassiveGeoParSet;
class FairBaseParSet;
class FairModule;
class SpdDetector;
class SpdPassiveModule;
class SpdGeoMapper;
class SpdGeoBuilder;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGeoLoader                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdGeoLoader: public TObject {

public:

    SpdGeoLoader();
    virtual ~SpdGeoLoader();
    
    static   SpdGeoLoader* Instance() { return fInstance; }
    
    Bool_t   LoadGeometry(TString parfile); 
    Bool_t   LoadGeometry(); // use this method if parameters already have been loaded via runtime database 
   
    Bool_t   LoadModule(Int_t id, Bool_t unsetmat = false);
    Bool_t   LoadModule(TString name, Bool_t unsetmat = false); // name = module name, "all", "passives", "actives"
    
    void     LockGeometry(Bool_t lock = true) { fLockGeom = lock; }   // if true, prevent loading new modules via LoadModule
    
    /* Getters */
    Bool_t   IsGeometryLocked()  const { return fLockGeom; } 
    
    Int_t    GetNTotalModules()  const { return fModulesId.size(); }
    Int_t    GetNActualModules() const { return fModules.size();   }
    
    const std::set<Int_t>&  GetActualModules() const { return fModules; }
    
    Int_t    GetModuleId(TString mname) const;
    Bool_t   IsGeometryModule(Int_t id) const;
    Bool_t   IsModuleActual(Int_t id) const;
    Bool_t   IsModuleActive(Int_t id) const;
    
    SpdPassiveGeoParSet*  GetPassiveParameters();
    SpdParSet*            GetActiveParameters(Int_t id);
    FairModule*           GetModule(Int_t id);
    SpdPassiveModule*     GetPassive(Int_t id);
    SpdDetector*          GetActive(Int_t id); 
    SpdGeoMapper*         GetMapper(Int_t id);
    SpdGeoBuilder*        GetBuilder(Int_t id);
    
    /* Setters (static) */
  
    static void ForceTopGeoFile(TString fname);       // TOP (cave) geometry changing
    static void ForceMediaFile(TString fname);        // Media file changing
    static void UnsetMaterials(Bool_t m = false); 
    static void ResetMediaFromParams(Bool_t r = true);
    static void ResetPassivesMedia(Bool_t r = false);
    
    /* Draw/Print */
    
    void  DrawGeometry(Int_t level = 2, TString option = "ogl");

    void  PrintGeometry() const;
    void  PrintActualGeometry() const;
    
protected:
    
    Bool_t   fIsInit;
    Bool_t   fLockGeom;
    TFile*   fParFile;
    
    std::map<TString,Int_t> fModulesId; // module [name <-> id]
    std::set<Int_t>         fModules;   // list of modules were constructed
    
    static TString fTopGeoFile;         // default: "cave.geo"
    static TString fMediaFile;          // default: "media.geo"
    static Bool_t  fUnsetMaterials;     // default: false
    static Bool_t  fResetMediaFromPars; // default: true
    static Bool_t  fResetPassivesMedia; // default: false
    
    static SpdGeoLoader* fInstance;
    
    ClassDef(SpdGeoLoader,1)
};

#endif  /* __SPDGEOLOADER_H__ */

