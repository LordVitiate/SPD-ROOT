// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTBPARSET_H__
#define __SPDECALTBPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTBParSet                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTBParSet : public SpdParSet {
  
public:
  
    SpdEcalTBParSet(const char* name = "SpdEcalTBParSet",
                    const char* title = "EcalTB parameters",
                    const char* context = "TestDefaultContext");
   
   ~SpdEcalTBParSet();
 
private:
    
    ClassDef(SpdEcalTBParSet,1)
};

#endif /* __SPDECALTBPARSET_H__ */
