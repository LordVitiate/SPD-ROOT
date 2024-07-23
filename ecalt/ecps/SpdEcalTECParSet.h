// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTECPARSET_H__
#define __SPDECALTECPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTECParSet                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTECParSet : public SpdParSet {
  
public:
  
    SpdEcalTECParSet(const char* name = "SpdEcalTECParSet",
                     const char* title = "EcalTEC parameters",
                     const char* context = "TestDefaultContext");
   
   ~SpdEcalTECParSet();
    
private:
  
    ClassDef(SpdEcalTECParSet,1)
};

#endif /* __SPDECALTECPARSET_H__ */
