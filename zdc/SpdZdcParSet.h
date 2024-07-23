// $Id$
// Author: artur   2021/09/21

#ifndef __SPDZDCPARSET_H__
#define __SPDZDCPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdZdcParSet                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdZdcParSet : public SpdParSet {
  
public:
  
    SpdZdcParSet(const char* name = "SpdZdcParSet",
                 const char* title = "ZDC parameters",
                 const char* context = "TestDefaultContext");
   
   ~SpdZdcParSet(); 
 
private:
  
    ClassDef(SpdZdcParSet,1)
};

#endif /* __SPDZDCPARSET_H__ */
