// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalTECHit
//_____________________________________________________________________________

#include "SpdEcalTECHit.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdEcalTECHit)

//_____________________________________________________________________________
SpdEcalTECHit::SpdEcalTECHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdEcalTECHit::~SpdEcalTECHit() 
{
 
}

//_____________________________________________________________________________
void SpdEcalTECHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdEcalTECHit::Print(const Option_t* opt) const
{
   cout << "<SpdEcalTECHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


