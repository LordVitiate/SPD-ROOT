// $Id$
// Author: andre   2021/03/04

//_____________________________________________________________________________
//
// SpdEcalTB2Cluster
//_____________________________________________________________________________

#include "SpdEcalTB2Cluster.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalTB2Cluster)

//_____________________________________________________________________________
SpdEcalTB2Cluster::SpdEcalTB2Cluster():fEnergy(0)
{
}

//_____________________________________________________________________________
SpdEcalTB2Cluster::~SpdEcalTB2Cluster() 
{
}

//_____________________________________________________________________________
void SpdEcalTB2Cluster::Print(const Option_t* opt) const
{
   cout << "-I- <SpdEcalTB2Cluster::Print> " << endl;
}


