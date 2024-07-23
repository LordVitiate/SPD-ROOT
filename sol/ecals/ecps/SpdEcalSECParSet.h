// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALSECPARSET_H__
#define __SPDECALSECPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalSECParSet                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalSECParSet : public SpdParSet {
  
public:
  
    SpdEcalSECParSet(const char* name = "SpdEcalSECParSet",
                     const char* title = "EcalSEC parameters",
                     const char* context = "TestDefaultContext");
   
   ~SpdEcalSECParSet();
    
private:
  
    ClassDef(SpdEcalSECParSet,1)
};
#endif /* __SPDECALSECPARSET_H__ */
