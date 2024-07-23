// $Id$
// Author: artur   2021/11/19


//_____________________________________________________________________________
//
// SpdTsParticle
//_____________________________________________________________________________

#include "SpdTsParticle.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTsParticle)

//_____________________________________________________________________________
SpdTsParticle::SpdTsParticle():fId(-1),fTrackId(-1), 
fMomentum(-1.), fTruncMeandEdx(-1.), 
fLikelihoods(), fDistances()
{
 
}

//_____________________________________________________________________________
SpdTsParticle::~SpdTsParticle() 
{
 
}

//_____________________________________________________________________________
void SpdTsParticle::Print(Option_t* option) const
{
    cout << "\n<SpdTsParticle::Print>\n\n";
    cout << "Id:                " << fId << endl;
    cout << "Track (id):        " << fTrackId << endl;
    cout << "Momentum(average): " << fMomentum << endl;
    cout << "Trunc. mean dE/dx: " << fTruncMeandEdx << endl;
    cout << "Likelihoods:       ";
    for(Double_t lh: fLikelihoods) cout << lh << "\t";
    cout << endl;
    cout << "Distances:         ";
    for(Double_t dist: fDistances) cout << dist << "\t";
    cout << endl;
    cout << endl;
}


