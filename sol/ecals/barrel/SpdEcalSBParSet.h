// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALSBPARSET_H__
#define __SPDECALSBPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalSBParSet                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalSBParSet : public SpdParSet {
  
public:
  
    SpdEcalSBParSet(const char* name = "SpdEcalSBParSet",
                    const char* title = "EcalSB parameters",
                    const char* context = "TestDefaultContext");
   
   ~SpdEcalSBParSet();
 
private:
    
    ClassDef(SpdEcalSBParSet,1)
};

#endif /* __SPDECALSBPARSET_H__ */
