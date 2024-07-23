// $Id$
// Author: artur   2018/02/01

#ifndef __SPDRSTECPARSET_H__
#define __SPDRSTECPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTECParSet                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsTECParSet : public SpdParSet {
  
public:
  
    SpdRsTECParSet(const char* name = "SpdRsTECParSet",
                   const char* title = "RsTEC parameters",
                   const char* context = "TestDefaultContext");
   
   ~SpdRsTECParSet();
    
private:
    
    ClassDef(SpdRsTECParSet,1)
};

#endif /* __SPDRSTECPARSET_H__ */
