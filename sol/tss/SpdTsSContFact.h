
#ifndef __SPDTSSCONTFACT_H__
#define __SPDTSSCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsSContFact                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsSContFact : public FairContFact {
  
public:
  
   SpdTsSContFact();
  ~SpdTsSContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdTsSContFact,0) 
};


#endif /* __SPDTSSCONTFACT_H__ */
