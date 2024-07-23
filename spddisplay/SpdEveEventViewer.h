// $Id$
// Author: artur   2018/09/02

#ifndef __SPDEVEEVENTVIEWER_H__
#define __SPDEVEEVENTVIEWER_H__

#include <TEveEventManager.h>
#include <TEveManager.h>
#include <FairRunAna.h>           

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEveEventViewer                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEveEventViewer: public TEveEventManager {

public:

    SpdEveEventViewer();
    virtual ~SpdEveEventViewer();
    
    static SpdEveEventViewer* Instance()  { return fInstance; }
    
    virtual void AddTask(FairTask* task);
    
    virtual void Open();

    virtual void Init(Int_t visopt = 1, Int_t vislvl = 2, Int_t maxvisnds = 10000);
    
    virtual void GotoEvent(Long64_t nevent);
    virtual void NextEvent();
    virtual void PrevEvent();
    
    virtual void Close();
    
    Long_t GetCurrentEvent() const { return fNEvent; }

protected:
    
    static SpdEveEventViewer* fInstance;
    
    FairRootManager* fRootFileManager;
    FairRunAna*      fRunAnalyzer;
    
    Bool_t           fIsInit;
    Long64_t         fNEvent;

    ClassDef(SpdEveEventViewer,1)
};

#endif  /* __SPDEVEEVENTVIEWER_H__ */

