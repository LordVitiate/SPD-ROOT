// $Id$
// Author: artur   2021/11/26

#ifndef __SPDAEGPARSET_H__
#define __SPDAEGPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdAegParSet                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdAegParSet : public SpdParSet {
  
public:
  
    SpdAegParSet(const char* name = "SpdAegParSet",
                 const char* title = "AEG parameters",
                 const char* context = "TestDefaultContext");
   
   ~SpdAegParSet(); 
 
private:
  
    ClassDef(SpdAegParSet,1)
};

#endif /* __SPDAEGPARSET_H__ */
