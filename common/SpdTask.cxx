// $Id$
// Author: artur   2020/10/02


//_____________________________________________________________________________
//
// SpdTask
//_____________________________________________________________________________

#include "SpdTask.h"

#include "FairRootManager.h"
#include "SpdRootFileSink.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdTask)

//_____________________________________________________________________________
SpdTask::SpdTask(const char* name):FairTask(name),
fEventStatus(true),fSkipEventToSave(false)
{
 
}

//_____________________________________________________________________________
SpdTask::~SpdTask() 
{

}

//_____________________________________________________________________________
void SpdTask::SkipEventToSave()
{
    if (!fEventStatus) { 
        fSkipEventToSave = true; 
        return;
    }
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm) return;
    SpdRootFileSink* sink = dynamic_cast<SpdRootFileSink*>(rm->GetSink()); 
    if (sink) fEventStatus = sink->SetEventStatus(false); 
    if (!fEventStatus) fSkipEventToSave = true;
}

//_____________________________________________________________________________
Bool_t SpdTask::GetEventStatus() const
{   
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm) return true;
    SpdRootFileSink* sink = dynamic_cast<SpdRootFileSink*>(rm->GetSink());      
    return (sink) ? sink->GetEventStatus() : true; 
}

//_____________________________________________________________________________
void SpdTask::FinishEvent()
{
    if (fSkipEventToSave) {
        ChangeEventStatus(true);
        fSkipEventToSave = false;
    }
}

//_____________________________________________________________________________
Bool_t SpdTask::ChangeEventStatus(Bool_t status)
{
    if (status == fEventStatus) return fEventStatus;
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm) return fEventStatus;
    SpdRootFileSink* sink = dynamic_cast<SpdRootFileSink*>(rm->GetSink()); 
    fEventStatus = (sink) ? sink->SetEventStatus(status) : true; 
    return fEventStatus;
}

