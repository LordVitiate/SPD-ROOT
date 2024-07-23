// $Id$
// Author: artur   2018/02/01

#ifndef __SPDRSTBPARSET_H__
#define __SPDRSTBPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTBParSet                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsTBParSet : public SpdParSet {
  
public:
  
    SpdRsTBParSet(const char* name = "SpdRsTBParSet",
                  const char* title = "RsTB parameters",
                  const char* context = "TestDefaultContext");
   
   ~SpdRsTBParSet();
    
private:
  
    ClassDef(SpdRsTBParSet,1)
};

#endif /* __SPDRSTBPARSET_H__ */
