
#ifndef __SPDECALSCONTFACT_H__
#define __SPDECALSCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalSContFact                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalSContFact : public FairContFact {
  
public:
  
   SpdEcalSContFact();
  ~SpdEcalSContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdEcalSContFact,0) 
};


#endif /* __SPDECALSCONTFACT_H__ */
