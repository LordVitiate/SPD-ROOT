// $Id$
// Author: artur   2017/11/29

#ifndef __SPDPARSETCONTFACT_H__
#define __SPDPARSETCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdParSetContFact                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdParSetContFact: public  FairContFact {

public:

    SpdParSetContFact();
   ~SpdParSetContFact() {}
  
    virtual FairParSet* createContainer(FairContainer*);

private:
  
    void setAllContainers();

    ClassDef(SpdParSetContFact,1)
};

#endif  /* __SPDPARSETCONTFACT_H__ */

