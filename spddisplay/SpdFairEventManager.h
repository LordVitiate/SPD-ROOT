// $Id$
// Author: artur   2018/08/31

#ifndef __SPDFAIREVENTMANAGER_H__
#define __SPDFAIREVENTMANAGER_H__

#include "FairEventManager.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdFairEventManager                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdFairEventManager: public FairEventManager {

public:

    SpdFairEventManager();
    virtual ~SpdFairEventManager();
    
    static SpdFairEventManager* Instance() { return fInstance; }
    
    virtual void Init(Int_t visopt = 1, Int_t vislvl = 2, Int_t maxvisnds = 10000);
    
    void AddTask(FairTask* task);
    
    void UpdateEditor();

private:
    
    static SpdFairEventManager* fInstance; //!
    
    Bool_t fIsInit;

    ClassDef(SpdFairEventManager,1)
};

#endif  /* __SPDFAIREVENTMANAGER_H__ */

