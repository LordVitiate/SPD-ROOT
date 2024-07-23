// $Id$
// Author: artur   2018/11/29

#ifndef __SPDRUNANA_H__
#define __SPDRUNANA_H__

#include "FairRunAna.h"
#include "SpdSetParSet.h"
#include "FairParGenericSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRunAna                                                                  //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTask;
class FairField;

class SpdRunAna: public FairRunAna {

public:

    SpdRunAna();
    virtual ~SpdRunAna();
    
    virtual void Initialize();
    virtual void FinishTask();
    
    void DeleteParSet();
    
    // Setters
    void UnloadBaseGeoParSet() { fLoadGeoParSet = false; fLoadBaseParSet = false; }
    void LoadGeoParSet(Bool_t load = kTRUE)  { fLoadGeoParSet  = load; }
    void LoadBaseParSet(Bool_t load = kTRUE) { fLoadBaseParSet = load; }
    
    void KeepParameters(Bool_t save = true)  { fKeepParameters = save; }

    // Getters
    SpdSetParSet*       GetParSetAssembly(); 
    FairParGenericSet*  GetParameters(TString pars_name);
    
    void PrintEntries();
    
private:
    
    SpdSetParSet*  fParSet; // assembly of parameters sets
 
    void   LoadParameters();
    void   SaveParameters();
    
    Bool_t fKeepParameters;
    
    Bool_t fLoadGeoParSet;
    Bool_t fLoadBaseParSet;
    
    FairParSet* fGeoParSet;
    FairParSet* fBaseParSet;
    
    Bool_t fIsParsSaved;
    
    void  RunEntry(Int_t entry); 
    
    friend class SpdMCDataIterator;
    
    ClassDef(SpdRunAna,1)
};

#endif  /* __SPDRUNANA_H__ */

