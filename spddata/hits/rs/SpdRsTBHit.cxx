// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsTBHit
//_____________________________________________________________________________

#include "SpdRsTBHit.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdRsTBHit)

//_____________________________________________________________________________
SpdRsTBHit::SpdRsTBHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdRsTBHit::~SpdRsTBHit() 
{
 
}

//_____________________________________________________________________________
void SpdRsTBHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdRsTBHit::Print(const Option_t* opt) const
{
   cout << "<SpdRsTBHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


