// $Id$
// Author: artur   2018/02/01

#ifndef __SPDRSSECPARSET_H__
#define __SPDRSSECPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsSECGeoPar                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsSECParSet : public SpdParSet {
  
public:
  
    SpdRsSECParSet(const char* name = "SpdRsSECParSet",
                   const char* title = "RsSEC parameters",
                   const char* context = "TestDefaultContext");
   
   ~SpdRsSECParSet();
    
private:
    
    ClassDef(SpdRsSECParSet,1)
};

#endif /* __SPDRSSECPARSET_H__ */
