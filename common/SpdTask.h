// $Id$
// Author: artur   2020/10/02

#ifndef __SPDTASK_H__
#define __SPDTASK_H__

#include "FairTask.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTask                                                                    //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTask: public FairTask {

public:

    SpdTask():FairTask() {}
    
    SpdTask(const char* name);
    
    virtual ~SpdTask();

    void    SkipEventToSave();
    Bool_t  GetEventStatus() const;

    virtual void FinishEvent();  
    
private:

    Bool_t  fEventStatus; 
    Bool_t  fSkipEventToSave; 

    // return current event status
    Bool_t  ChangeEventStatus(Bool_t status);
    
    ClassDef(SpdTask,1)
};

#endif  /* __SPDTASK_H__ */

