
#ifndef __SPDRSTCONTFACT_H__
#define __SPDRSTCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTContFact                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsTContFact : public FairContFact {
  
public:
  
   SpdRsTContFact();
  ~SpdRsTContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdRsTContFact,0) 
};


#endif /* __SPDRSTCONTFACT_H__ */
