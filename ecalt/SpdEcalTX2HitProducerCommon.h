// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTX2HITPRODUCERCOMMON_H__
#define __SPDECALTX2HITPRODUCERCOMMON_H__

#include "SpdEcalTB2Hit.h"
#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"

namespace SpdEcalTX2HitProducerCommon {
    bool VerifyVertices(Double_t* vtx);
    TVector3 GetCellPosition(TString nodepath);
}

#endif  /* __SPDECALTX2HITPRODUCERCOMMON_H__ */

