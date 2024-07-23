// $Id$
// Author: andre   2021/03/04

//_____________________________________________________________________________
//
// SpdEcalTEC2Cluster
//_____________________________________________________________________________

#include "SpdEcalTEC2Cluster.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalTEC2Cluster)

//_____________________________________________________________________________
SpdEcalTEC2Cluster::SpdEcalTEC2Cluster():fEnergy(0)
{
}

//_____________________________________________________________________________
SpdEcalTEC2Cluster::~SpdEcalTEC2Cluster() 
{
}


//_____________________________________________________________________________
void SpdEcalTEC2Cluster::Print(const Option_t* opt) const
{
   cout << "-I- <SpdEcalTEC2Cluster::Print> " << endl;  
}


