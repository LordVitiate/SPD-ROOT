// $Id$
// Author: andre   2021/03/04

//_____________________________________________________________________________
//
// SpdEcalTEC2RecoParticle
//_____________________________________________________________________________

#include "SpdEcalTEC2RecoParticle.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalTEC2RecoParticle)

//_____________________________________________________________________________
SpdEcalTEC2RecoParticle::SpdEcalTEC2RecoParticle():fEnergy(0)
{
}

//_____________________________________________________________________________
SpdEcalTEC2RecoParticle::SpdEcalTEC2RecoParticle(Double_t energy, TVector3 pos):
fEnergy(energy),fPos(pos)
{
}

//_____________________________________________________________________________
SpdEcalTEC2RecoParticle::~SpdEcalTEC2RecoParticle()
{
}

//_____________________________________________________________________________
void SpdEcalTEC2RecoParticle::Print(const Option_t* opt) const
{
    cout << "-I- <SpdEcalTEC2RecoParticle::Print> " << endl;
    cout << " E = " << fEnergy 
    << " | X/Y/Z = " << fPos.X() << "/" << fPos.Y() << "/" << fPos.Z() << endl;
    cout << endl;
}


