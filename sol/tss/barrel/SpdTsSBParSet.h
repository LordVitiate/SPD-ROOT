// $Id$
// Author: artur   2018/02/01

#ifndef __SPDTSSBPARSET_H__
#define __SPDTSSBPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsSBParSet                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsSBParSet: public SpdParSet {

public:
    
    SpdTsSBParSet(const char* name = "SpdTsSBParSet",
                  const char* title = "TsSB parameters",
                  const char* context = "DefaultContext");
    
    virtual ~SpdTsSBParSet();

private:

    ClassDef(SpdTsSBParSet,1)
};

#endif /* __SPDTSSBPARSET_H__ */
