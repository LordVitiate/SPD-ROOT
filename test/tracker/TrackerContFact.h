
#ifndef __TRACKERCONTFACT_H__
#define __TRACKERCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TrackerContFact                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairContainer;

class TrackerContFact : public FairContFact {
  
public:

    TrackerContFact();
   ~TrackerContFact() {}
   
    FairParSet* createContainer(FairContainer*);
  
private:

    void setAllContainers();
    
    ClassDef(TrackerContFact,0) 
};

#endif /* __TRACKERCONTFACT_H__ */
