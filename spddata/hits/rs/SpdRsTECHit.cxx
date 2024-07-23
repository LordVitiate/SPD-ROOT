// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsTECHit
//_____________________________________________________________________________

#include "SpdRsTECHit.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdRsTECHit)

//_____________________________________________________________________________
SpdRsTECHit::SpdRsTECHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdRsTECHit::~SpdRsTECHit() 
{
 
}

//_____________________________________________________________________________
void SpdRsTECHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdRsTECHit::Print(const Option_t* opt) const
{
   cout << "<SpdRsTECHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


