// $Id$
// Author: artur   2018/12/05

#ifndef __SPDPRIMGENFACTORY_H__
#define __SPDPRIMGENFACTORY_H__

#include "SpdGenerator.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPrimGenFactory                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdPrimGenFactory: public TObject {

public:

    SpdPrimGenFactory();
    virtual ~SpdPrimGenFactory();
    
    static SpdPrimGenFactory* Instance();
    
    virtual SpdGenerator* CreateGenerator(TString gentype);
    virtual SpdGenerator* CreateGenerator(TString gentype, TString genname);

private:
  
    static SpdPrimGenFactory* fInstance;

    ClassDef(SpdPrimGenFactory,1)
};

#endif  /* __SPDPRIMGENFACTORY_H__ */

