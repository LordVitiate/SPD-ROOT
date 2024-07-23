// $Id$
// Author: artur   2021/11/28


//_____________________________________________________________________________
//
// SpdTrackRCSeedPar
//_____________________________________________________________________________

#include "SpdTrackRCSeedPar.h"
#include <TMath.h>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTrackRCSeedPar)

//_____________________________________________________________________________
SpdTrackRCSeedPar::SpdTrackRCSeedPar():
fNHits(0), fCharge(0.), fRadius(0.), fTheta(-99.),
fPhi(-99.), fZvtx(0.), fRes(-99.)
//fChi2(0), fNDF(0)
{

}
 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                          SpdTrackRCSeedPar
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdTrackRCSeedPar::Print(Option_t* option) const
{
    cout << "\n\t<SpdTrackRCSeedPar::Print> " << "\n\n";

    cout << "\tCharge of seed particle:         " << fCharge << endl;
    cout << "\tRadius of seed particle:         " << fRadius << endl;
    cout << "\tTheta of seed particle:          " << fTheta  << endl;
    cout << "\tPhi of seed particle:            " << fPhi    << endl;
    
    cout << "\tNumber of hits in seed particle: " << fNHits  << endl;
    cout << "\tZVertex [cm] of seed particle:   " << fZvtx   << endl;
    cout << "\tRes [cm] of line fit:            " << fRes    << endl;
//    cout << "\tChi2 of LSM for seed particle:   " << fChi2    << endl;
//    cout << "\tNDF:                             " << fNDF     << endl;

    printf("\n");
}

