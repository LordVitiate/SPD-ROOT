// $Id$
// Author: artur   2021/11/12

#ifndef __SPDMVDPARSET_H__
#define __SPDMVDPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMvdParSet                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMvdParSet : public SpdParSet {
  
public:
  
    SpdMvdParSet(const char* name = "SpdMvdParSet",
                 const char* title = "Mvd parameters",
                 const char* context = "TestDefaultContext");
   
   ~SpdMvdParSet();
    
private:
  
    ClassDef(SpdMvdParSet,1)
};

#endif /* __SPDMVDPARSET_H__ */
