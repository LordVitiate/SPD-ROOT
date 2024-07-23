// $Id$
// Author: artur   2018/02/01

#ifndef __SPDTSTECPARSET_H__
#define __SPDTSTECPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsTECParSet                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsTECParSet : public SpdParSet {
  
public:
  
    SpdTsTECParSet(const char* name = "SpdTsTECParSet",
                   const char* title = "TsTEC parameters",
                   const char* context = "TestDefaultContext");
   
   ~SpdTsTECParSet(); 
 
private:
  
    ClassDef(SpdTsTECParSet,1)
};

#endif /* __SPDTSTECPARSET_H__ */
