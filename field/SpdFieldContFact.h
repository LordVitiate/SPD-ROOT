// $Id$
// Author: artur   2017/11/29

#ifndef __SPDFIELDCONTFACT_H__
#define __SPDFIELDCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdFieldContFact                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdFieldContFact: public  FairContFact {

public:

    SpdFieldContFact();
   ~SpdFieldContFact() {}
  
    virtual FairParSet* createContainer(FairContainer*);

private:
  
    void setAllContainers();

    ClassDef(SpdFieldContFact,1)
};

#endif  /* __SPDFIELDCONTFACT_H__ */

