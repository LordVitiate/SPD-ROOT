// $Id$
// Author: andre   2021/03/04

#ifndef __SPDECALCOMMON_H__
#define __SPDECALCOMMON_H__

#include <TVector3.h>
#include <TString.h>

enum SpdClusterRecoType 
{
    kSpdClRecoSingleSimpleLinear, // 1 cluster = 1 particle, energy = sum of energies, position = mean with linear weights
    kSpdClRecoSingleSimpleLog,    // 1 cluster = 1 particle, energy = sum of energies, position = mean with log weights
    kSpdClRecoSingleShower,       // 1 cluster = 1 particle, energy from shower fit
    kSpdClRecoMultiShower,        // fit with several shower shapes
    kSpdClRecoLast                          
};

namespace SpdEcalCommon 
{
    Bool_t  VerifyVertices(Double_t* vtx);
    Bool_t  GetCellPosition(TString nodepath, TVector3& nodepos);
}

#endif  /* __SPDECALCOMMON_H__ */

