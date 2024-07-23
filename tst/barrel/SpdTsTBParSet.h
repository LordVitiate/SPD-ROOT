// $Id$
// Author: artur   2018/10/29

#ifndef __SPDTSTBPARSET_H__
#define __SPDTSTBPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsTBParSet                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsTBParSet: public SpdParSet {

public:
    
    SpdTsTBParSet(const char* name = "SpdTsTBParSet",
                  const char* title = "TsTB parameters",
                  const char* context = "DefaultContext");
    
    virtual ~SpdTsTBParSet();

private:

    ClassDef(SpdTsTBParSet,1)
};

#endif  /* __SPDTSTBPARSET_H__ */

