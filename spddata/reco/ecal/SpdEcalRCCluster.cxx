// $Id$
// Author: artur   2021/03/15


//_____________________________________________________________________________
//
// SpdEcalRCCluster
//_____________________________________________________________________________

#include "SpdEcalRCCluster.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalRCCluster)

//_____________________________________________________________________________
SpdEcalRCCluster::SpdEcalRCCluster():fId(-1),fModId(-1),fDetId(-10),
fEnergy(0),fInfoId(-1) 
{
   
}

//_____________________________________________________________________________
SpdEcalRCCluster::~SpdEcalRCCluster() 
{
 
}

//_____________________________________________________________________________
void SpdEcalRCCluster::Print(Option_t* option) const
{
    cout << "\n<SpdEcalRCCluster::Print> " << "\n\n";
    cout << "Unique cluster ID:            " << fId << endl;
    cout << "Detector (supermodule) ID:    " << fModId << endl;
    cout << "Detector (basket,endcap) ID:  " << fDetId << endl;
    cout << "Total deposited energy [GeV]: " << fEnergy << endl;
    cout << "Hits (cells):                 " << fCells.size() << endl;
    cout << "Particles (reconstructed):    " << fParticles.size() << endl;
    cout << "Cluster info ID:              " << fInfoId << endl;
    cout << endl;
}


