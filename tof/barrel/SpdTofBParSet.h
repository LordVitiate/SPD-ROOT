// $Id$
// Author: artur   2021/08/17

#ifndef __SPDTOFBPARSET_H__
#define __SPDTOFBPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTofBParSet                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTofBParSet: public SpdParSet {

public:
    
    SpdTofBParSet(const char* name = "SpdTofBParSet",
                  const char* title = "TofB parameters",
                  const char* context = "DefaultContext");
    
    virtual ~SpdTofBParSet();

private:

    ClassDef(SpdTofBParSet,1)
};

#endif  /* __SPDTOFBPARSET_H__ */

