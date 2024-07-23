
#ifndef __SPDBBCCONTFACT_H__
#define __SPDBBCCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdBbcContFact                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdBbcContFact : public FairContFact {
  
public:
  
   SpdBbcContFact();
  ~SpdBbcContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdBbcContFact,0) 
};


#endif /* __SPDBBCCONTFACT_H__ */
