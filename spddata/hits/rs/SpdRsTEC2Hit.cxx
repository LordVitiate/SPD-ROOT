// $Id$
// Author: alver   2020/07/19

//_____________________________________________________________________________
//
// SpdRsTEC2Hit
//_____________________________________________________________________________

#include "SpdRsTEC2Hit.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdRsTEC2Hit)

//_____________________________________________________________________________
SpdRsTEC2Hit::SpdRsTEC2Hit():SpdHit(),
fDetectorID(-1),
fPos(0.,0.,0.),
fPoints()
{
}

//_____________________________________________________________________________
SpdRsTEC2Hit::SpdRsTEC2Hit(Int_t detID, TVector3& pos, std::vector<Int_t> pointList):
SpdHit(),
fDetectorID(detID),
fPos(pos),
fPoints(pointList)
{
}

//_____________________________________________________________________________
SpdRsTEC2Hit::~SpdRsTEC2Hit() 
{
 
}

//_____________________________________________________________________________
void SpdRsTEC2Hit::Print(const Option_t* opt) const
{
   cout << "-I- <SpdRsTEC2Hit::Print> "
        << " DetId: " << GetUid() 
        << " Points: " << GetNPoints() 
        << " Position (X,Y,Z): (" << GetPosition().X() << ", " 
                                  << GetPosition().Y() << ", " 
                                  << GetPosition().Z() << ")" << endl;
}


