// $Id$
// Author: artur   2021/11/19


//_____________________________________________________________________________
//
// SpdTofParticle
//_____________________________________________________________________________

#include "SpdTofParticle.h"
#include "TMath.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTofParticle)

//_____________________________________________________________________________
SpdTofParticle::SpdTofParticle():fId(-1),fHitId(-1),fTrackId(-1)
{
 
}

//_____________________________________________________________________________
SpdTofParticle::~SpdTofParticle() 
{
 
}

//_____________________________________________________________________________
void SpdTofParticle::Print(Option_t* option) const
{
    cout << "\n<SpdTofParticle::Print>\n\n";
    cout << "Id:                " << fId << endl;
    cout << "Track (id):        " << fTrackId << endl;
    cout << "Hit (id):          " << fHitId << endl;
    cout << endl;
}


