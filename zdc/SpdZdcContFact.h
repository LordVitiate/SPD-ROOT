
#ifndef __SPDZDCCONTFACT_H__
#define __SPDZDCCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdZdcContFact                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdZdcContFact : public FairContFact {
  
public:
  
   SpdZdcContFact();
  ~SpdZdcContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdZdcContFact,0) 
};


#endif /* __SPDZDCCONTFACT_H__ */
