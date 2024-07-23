// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalTBHit
//_____________________________________________________________________________

#include "SpdEcalTBHit.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdEcalTBHit)

//_____________________________________________________________________________
SpdEcalTBHit::SpdEcalTBHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdEcalTBHit::~SpdEcalTBHit() 
{
 
}

//_____________________________________________________________________________
void SpdEcalTBHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdEcalTBHit::Print(const Option_t* opt) const
{
   cout << "<SpdEcalTBHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


