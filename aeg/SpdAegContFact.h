
#ifndef __SPDAEGCONTFACT_H__
#define __SPDAEGCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdAegContFact                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdAegContFact : public FairContFact {
  
public:
  
   SpdAegContFact();
  ~SpdAegContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdAegContFact,0) 
};


#endif /* __SPDAEGCONTFACT_H__ */
