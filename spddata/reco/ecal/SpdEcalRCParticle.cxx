// $Id$
// Author: artur   2021/03/15


//_____________________________________________________________________________
//
// SpdEcalRCParticle
//_____________________________________________________________________________

#include "SpdEcalRCParticle.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalRCParticle)

//_____________________________________________________________________________
SpdEcalRCParticle::SpdEcalRCParticle():fId(-1),fClusterId(-1),fEnergy(0) 
{

}

//_____________________________________________________________________________
SpdEcalRCParticle::~SpdEcalRCParticle() 
{
 
}

//_____________________________________________________________________________
void SpdEcalRCParticle::Print(Option_t* option) const
{
    cout << "\n<SpdEcalRCParticle::Print> " << "\n\n";
    cout << "Unique particle ID:        " << fId << endl;
    cout << "Cluster ID:                " << fClusterId << endl;
    cout << "Energy [GeV]:              " << fEnergy << endl;
    cout << "Position(x,y,z) [cm]:      " << fPosition.X() << " " << fPosition.Y() << " " << fPosition.Z() << endl;
    cout << "PID:                       " << fPID << endl;
    cout << endl;
}



