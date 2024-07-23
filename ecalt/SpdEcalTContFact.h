
#ifndef __SPDECALTCONTFACT_H__
#define __SPDECALTCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTContFact                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTContFact : public FairContFact {
  
public:
  
   SpdEcalTContFact();
  ~SpdEcalTContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdEcalTContFact,0) 
};


#endif /* __SPDECALTCONTFACT_H__ */
