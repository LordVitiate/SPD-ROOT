// $Id$
// Author: artur   2018/02/01

#ifndef __SPDRSSBPARSET_H__
#define __SPDRSSBPARSET_H__

#include "SpdParSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsSBGeoPar                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsSBParSet : public SpdParSet {
  
public:
  
    SpdRsSBParSet(const char* name = "SpdRsSBParSet",
                  const char* title = "RsSB parameters",
                  const char* context = "TestDefaultContext");
   
   ~SpdRsSBParSet();
    
private:
    
    ClassDef(SpdRsSBParSet,1)
};

#endif /* __SPDRSSBPARSET_H__ */
