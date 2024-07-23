// $Id$
// Author: andre   2021/03/04

//_____________________________________________________________________________
//
// SpdEcalTB2Particle
//_____________________________________________________________________________

#include "SpdEcalTB2RecoParticle.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalTB2RecoParticle)

//_____________________________________________________________________________
SpdEcalTB2RecoParticle::SpdEcalTB2RecoParticle():fEnergy(0)
{
}

//_____________________________________________________________________________
SpdEcalTB2RecoParticle::SpdEcalTB2RecoParticle(Double_t energy, TVector3 pos):
fEnergy(energy),fPos(pos)
{
}

//_____________________________________________________________________________
SpdEcalTB2RecoParticle::~SpdEcalTB2RecoParticle() 
{ 
}

//_____________________________________________________________________________
void SpdEcalTB2RecoParticle::Print(const Option_t* opt) const
{
    cout << "-I- <SpdEcalTB2RecoParticle::Print> " << endl;
    cout << " E = " << fEnergy 
    << " | X/Y/Z = " << fPos.X() << "/" << fPos.Y() << "/" << fPos.Z() << endl;
    cout << endl;
}


