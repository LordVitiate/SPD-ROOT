// $Id$
// Author: artur   2021/11/12

#ifndef __SPDMVDCONTFACT_H__
#define __SPDMVDCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMvdContFact                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMvdContFact : public FairContFact {
  
public:
  
   SpdMvdContFact();
  ~SpdMvdContFact() {}
  
   virtual FairParSet* createContainer(FairContainer*);
    
private:
    
   void setAllContainers();
    
   ClassDef(SpdMvdContFact,0) 
};


#endif /* __SPDMVDCONTFACT_H__ */


