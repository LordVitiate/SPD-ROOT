// $Id$
// Author: artur   2021/11/19


//_____________________________________________________________________________
//
// SpdAegParticle
//_____________________________________________________________________________

#include "SpdAegParticle.h"
#include "TMath.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdAegParticle)

//_____________________________________________________________________________
SpdAegParticle::SpdAegParticle():fId(-1),fHitId(-1),fTrackId(-1)
{
 
}

//_____________________________________________________________________________
SpdAegParticle::~SpdAegParticle() 
{
 
}

//_____________________________________________________________________________
void SpdAegParticle::Print(Option_t* option) const
{
    cout << "\n<SpdAegParticle::Print>\n\n";
    cout << "Id:                " << fId << endl;
    cout << "Track (id):        " << fTrackId << endl;
    cout << "Hit (id):          " << fHitId << endl;
    cout << endl;
}


