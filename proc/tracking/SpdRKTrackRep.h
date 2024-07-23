// $Id$
// Author: artur   2021/02/18

#ifndef __SPDRKTRACKREP_H__
#define __SPDRKTRACKREP_H__

#include <TObject.h>

#include "RKTrackRep.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRKTrackRep                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRKTrackRep: public genfit::RKTrackRep {

public:

    SpdRKTrackRep();
    SpdRKTrackRep(Int_t pdg, Char_t prop_dir = 0);
    
    virtual ~SpdRKTrackRep();

private:

    ClassDef(SpdRKTrackRep,1)
};

#endif  /* __SPDRKTRACKREP_H__ */

