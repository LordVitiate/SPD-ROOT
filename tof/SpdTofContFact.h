
#ifndef __SPDTOFCONTFACT_H__
#define __SPDTOFCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTOFContFact                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTofContFact : public FairContFact {
  
public:
  
   SpdTofContFact();
  ~SpdTofContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdTofContFact,0) 
};


#endif /* __SPDTOFCONTFACT_H__ */
