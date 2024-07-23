// $Id$
// Author: alver   2020/08/25

//_____________________________________________________________________________
//
// SpdRsTB2Hit
//_____________________________________________________________________________

#include "SpdRsTB2Hit.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdRsTB2Hit)

//_____________________________________________________________________________
SpdRsTB2Hit::SpdRsTB2Hit():SpdHit(),
fDetectorID(-1),
fPos(0.,0.,0.),
fPoints()
{
}

//_____________________________________________________________________________
SpdRsTB2Hit::SpdRsTB2Hit(Int_t detID, TVector3& pos, std::vector<Int_t> pointList):SpdHit(),
fDetectorID(detID),
fPos(pos),
fPoints(pointList)
{
}

//_____________________________________________________________________________
SpdRsTB2Hit::~SpdRsTB2Hit() 
{
 
}

//_____________________________________________________________________________
void SpdRsTB2Hit::Print(const Option_t* opt) const
{
   cout << "<SpdRsTB2Hit::Print> "
        << " DetId: " << GetUid()
        << " Points: " << GetNPoints() 
        << " Position (X,Y,Z): (" << GetPosition().X() << ", " << GetPosition().Y() << ", " 
        << GetPosition().Z() << ")" << endl;
}


