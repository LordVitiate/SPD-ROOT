// $Id$
// Author: artur   2021/11/26

#ifndef __SPDBBCPARSET_H__
#define __SPDBBCPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdBbcParSet                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdBbcParSet : public SpdParSet {
  
public:
  
    SpdBbcParSet(const char* name = "SpdBbcParSet",
                 const char* title = "BBC parameters",
                 const char* context = "TestDefaultContext");
   
   ~SpdBbcParSet(); 
 
private:
  
    ClassDef(SpdBbcParSet,1)
};

#endif /* __SPDBBCPARSET_H__ */
