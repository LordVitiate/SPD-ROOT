// $Id$
// Author: artur   2018/01/31

#ifndef __SPDGEOMAPPER_H__
#define __SPDGEOMAPPER_H__

#include <TObject.h>

#include "SpdGeoVolPars.h"
#include "SpdGeoVVolume.h"
#include "SpdGeoTable.h"
#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGeoMapper                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TObjArray;
class SpdGeoVVolume;

class SpdGeoMapper: public TObject {

public:

    SpdGeoMapper();
    SpdGeoMapper(TString prefix);
    
    virtual ~SpdGeoMapper();
    
    virtual void DeleteParameters(); 
    virtual void DeleteVolPars(); 
    virtual void DeleteGeoTable(); 
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE) { return kFALSE; }
    
    /* ------ setters ------*/
    
    virtual void   SetGeoType(Int_t gtype);
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const { return (gtype > 0); }
     
    virtual SpdParameter* AddParameter(TString par_name);
    
    virtual void  LockGeometry()   { fLockGeometry = kTRUE;  }
    virtual void  UnlockGeometry() { fLockGeometry = kFALSE; }
    
    virtual void  UnsetMaterials(Bool_t /*precise = kTRUE*/, TString /*option = "base"*/) {}
    virtual void  UnsetMaterials(TString option = "base") { UnsetMaterials(0,option); } 
       
    /* ------ getters ------ */
  
    Int_t         GetGeoType()       const { return fGeoType;    } 
    TString       GetGeoPrefix()     const { return fGeoPrefix;  }
    TString       GetUnsetMedia()    const { return fUnsetMedia; }
    TObjArray*    GetVolPars()       const { return fGeoVolPars; }
    SpdGeoTable*  GetGeoTable()      const { return fGeoTable;   }
    
    TString       GetMasterVolName() const { return fMasterVolName; }
    
    virtual       Int_t GetVolActivity(TString vol_name) const; 
    
    virtual const SpdGeoVolPars* GetVolPars(TString vol_name) const; 
    
    virtual const SpdParameter*  GetParameter(TString par_name) const;
    virtual       SpdParameter*  GetParameter(TString par_name);
   
    virtual TString GetVolName(Int_t /*level*/, Int_t /*uid*/) { return ""; }
    
    virtual SpdGeoVVolume* GetVVolume(TString /* volume name */) { return 0; }
    virtual SpdGeoVVolume* GetVVolume(Int_t /* node id */) { return 0; }
    
    TObjArray*    GetMediums() const;
    
    Bool_t        IsDigit(const Text_t* par_name);
    Bool_t        IsGeometryLocked() { return fLockGeometry; }
   
    virtual TString AddPrefix(TString name, Bool_t add_uscore = kFALSE) const;
     
    virtual TGeoMatrix* GetNodeMatrix(const SpdGeoTableRec* /*rec*/, Int_t /*loc_i*/) const { return 0; }
    
    virtual Long_t   GetNodeId(const TString& nodepath);
    virtual TString  GetNodeFullName(Long_t id);
    
    virtual void  FillParametersIn(SpdParSet* params);
    virtual void  LoadParametersFrom(SpdParSet* params);
    
    virtual void  ResetMediaFromParams();
    
    virtual void  Print(Option_t*) const;
    virtual void  PrintVolPars(Int_t nvolumes = 0) const;
    
    /* ------ GEOMETRY TABLE ------ */
    
    virtual void  PrintGeoTable(Int_t l = 0) const;

protected:
   
    virtual Bool_t  CheckGeoType(Int_t geotype, TString parname);
    
    virtual Int_t   AddVolPars(SpdGeoVolPars* pars);
  
    Int_t           fGeoType;        // Geometry type
    TString         fGeoPrefix;      // Name prefix
    TString         fMasterVolName;  // Master volume name 
    TString         fUnsetMedia;     // Global media to unset materials 
    TObjArray*      fParams;         // Set of Geometry parameters    
    TObjArray*      fGeoVolPars;     // Set of volume's paramaters
    SpdGeoTable*    fGeoTable;       // Geometry structure table
   
    Bool_t          fLockGeometry;   //!
    
    virtual TString GetPrintVolParsCap() const { return ""; }
    
    ClassDef(SpdGeoMapper,1)
};

#endif  /* __SPDGEOMAPPER_H__ */

