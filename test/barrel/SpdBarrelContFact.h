
#ifndef __SPDBARRELCONTFACT_H__
#define __SPDBARRELCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdBarrelContFact                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairContainer;

class SpdBarrelContFact : public FairContFact {
  
public:

    SpdBarrelContFact();
   ~SpdBarrelContFact() {}
   
    virtual FairParSet* createContainer(FairContainer*);
  
private:

    void setAllContainers();
    
    ClassDef(SpdBarrelContFact,0) 
};

#endif /* __SPDBARRELCONTFACT_H__ */
