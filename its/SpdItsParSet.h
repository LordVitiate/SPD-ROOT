// $Id$
// Author: artur   2019/02/20

#ifndef __SPDITSPARSET_H__
#define __SPDITSPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsParSet                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdItsParSet : public SpdParSet {
  
public:
  
    SpdItsParSet(const char* name = "SpdItsParSet",
                 const char* title = "Its parameters",
                 const char* context = "TestDefaultContext");
   
   ~SpdItsParSet();
    
private:
  
    ClassDef(SpdItsParSet,1)
};

#endif /* __SPDITSPARSET_H__ */
