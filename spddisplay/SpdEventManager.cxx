// $Id$
// Author: artur   2018/08/27


//_____________________________________________________________________________
//
// SpdEventManager
//_____________________________________________________________________________

#include "SpdEventManager.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdEventManager)

//______________________________________________________________________________
SpdEventManager::SpdEventManager():
fFairEventManager(0),fSpdEveEventViewer(0),
fVisLevel(2),fMaxVisNodes(10000),fVisOption(1),fManagerType(kFairManager)
{
    fFairEventManager = SpdFairEventManager::Instance();
    if (!fFairEventManager) fFairEventManager = new SpdFairEventManager();  
}

//______________________________________________________________________________
SpdEventManager::SpdEventManager(Int_t VisLevel, KEventManagerType manager):
fFairEventManager(0),fSpdEveEventViewer(0),
fVisLevel(VisLevel),fMaxVisNodes(10000),fVisOption(1),fManagerType(kUndefined)
{
    if (manager == kFairManager) {
        fFairEventManager = SpdFairEventManager::Instance();
        if (!fFairEventManager) fFairEventManager = new SpdFairEventManager(); 
        fManagerType = manager;
    }
    else if (manager == kSpdEveViewer) {
        fSpdEveEventViewer = SpdEveEventViewer::Instance();
        if (!fSpdEveEventViewer) fSpdEveEventViewer = new SpdEveEventViewer();  
        fManagerType = manager;
    }
}

//______________________________________________________________________________
SpdEventManager::~SpdEventManager()
{
}

//______________________________________________________________________________
void SpdEventManager::Init()
{
    switch (fManagerType) {
        case kFairManager  : fFairEventManager->Init(fVisOption,fVisLevel,fMaxVisNodes); break;
        case kSpdEveViewer : fSpdEveEventViewer->Init(fVisOption,fVisLevel,fMaxVisNodes); break;
    }
}

//______________________________________________________________________________
void SpdEventManager::AddTask(FairTask* task)
{
    switch (fManagerType) {
        case kFairManager  : fFairEventManager->AddTask(task); break;
        case kSpdEveViewer : fSpdEveEventViewer->AddTask(task); break;
    }
}

//______________________________________________________________________________
void SpdEventManager::GotoEvent(Int_t event)
{
    switch (fManagerType) {
        case kFairManager  : fFairEventManager->GotoEvent(event); break;
        case kSpdEveViewer : fSpdEveEventViewer->GotoEvent(event); break;
    }
}


