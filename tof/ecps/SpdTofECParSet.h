// $Id$
// Author: artur   2021/08/17

#ifndef __SPDTOFECPARSET_H__
#define __SPDTOFECPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTofECParSet                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTofECParSet : public SpdParSet {
  
public:
  
    SpdTofECParSet(const char* name = "SpdTofECParSet",
                   const char* title = "TofEC parameters",
                   const char* context = "TestDefaultContext");
   
   ~SpdTofECParSet(); 
 
private:
  
    ClassDef(SpdTofECParSet,1)
};

#endif /* __SPDTOFECPARSET_H__ */
