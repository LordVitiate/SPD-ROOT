// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalSBHit
//_____________________________________________________________________________

#include "SpdEcalSBHit.h"
//#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdEcalSBHit)

//_____________________________________________________________________________
SpdEcalSBHit::SpdEcalSBHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdEcalSBHit::~SpdEcalSBHit() 
{
 
}

//_____________________________________________________________________________
void SpdEcalSBHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdEcalSBHit::Print(const Option_t* opt) const
{
   cout << "<SpdEcalSBHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


