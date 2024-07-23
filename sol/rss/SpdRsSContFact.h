
#ifndef __SPDRSSCONTFACT_H__
#define __SPDRSSCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsSContFact                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsSContFact : public FairContFact {
  
public:
  
   SpdRsSContFact();
  ~SpdRsSContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdRsSContFact,0) 
};

#endif /* __SPDRSSCONTFACT_H__ */
