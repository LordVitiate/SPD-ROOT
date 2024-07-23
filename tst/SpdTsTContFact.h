
#ifndef __SPDTSTCONTFACT_H__
#define __SPDTSTCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsTContFact                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsTContFact : public FairContFact {
  
public:
  
   SpdTsTContFact();
  ~SpdTsTContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdTsTContFact,0) 
};


#endif /* __SPDTSCONTFACT_H__ */
