// $Id$
// Author: artur   2018/02/01

#ifndef __SPDTSTECPARSET_H__
#define __SPDTSTECPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsSECParSEt                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsSECParSet : public SpdParSet {
  
public:
  
    SpdTsSECParSet(const char* name = "SpdTsSECParSet",
                   const char* title = "TsSEC parameters",
                   const char* context = "TestDefaultContext");
   
   ~SpdTsSECParSet(); 
 
private:
  
    ClassDef(SpdTsSECParSet,1)
};

#endif /* __SPDTSTECPARSET_H__ */
