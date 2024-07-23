// $Id$
// Author: artur   2018/08/27

#ifndef __SPDEVENTMANAGER_H__
#define __SPDEVENTMANAGER_H__

#include <TString.h> 
#include "SpdFairEventManager.h"
#include "SpdEveEventViewer.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEventManager                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

enum KEventManagerType {kFairManager, kSpdEveViewer, kUndefined};

class SpdEventManager : public TObject  {

public:
    
    SpdEventManager();
    SpdEventManager(Int_t VisLevel, KEventManagerType = kFairManager);
    
    virtual ~SpdEventManager();
    
    virtual void Init();
    
    virtual void AddTask(FairTask* task);
    
    virtual void GotoEvent(Int_t event);
    
    virtual void SetVisLevel(Int_t level)   { fVisLevel = level;   } 
    virtual void SetMaxVisNodes(Int_t maxn) { fMaxVisNodes = maxn; } 
    virtual void SetVisOption(Int_t option) { fVisOption = option; } 
    
private:
    
    SpdFairEventManager* fFairEventManager;
    SpdEveEventViewer*   fSpdEveEventViewer;
    
    Int_t fVisLevel;
    Int_t fMaxVisNodes;
    Int_t fVisOption;
    
    KEventManagerType fManagerType;

    ClassDef(SpdEventManager,1);
};


#endif  /* __SPDEVENTMANAGER_H__ */

