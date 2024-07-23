
#ifndef __SPDITSCONTFACT_H__
#define __SPDITSCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsContFact                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdItsContFact : public FairContFact {
  
public:
  
   SpdItsContFact();
  ~SpdItsContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdItsContFact,0) 
};


#endif /* __SPDITSCONTFACT_H__ */
