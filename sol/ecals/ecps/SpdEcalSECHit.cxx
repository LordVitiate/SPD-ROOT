// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalSECHit
//_____________________________________________________________________________

#include "SpdEcalSECHit.h"
//#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdEcalSECHit)

//_____________________________________________________________________________
SpdEcalSECHit::SpdEcalSECHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdEcalSECHit::~SpdEcalSECHit() 
{
 
}

//_____________________________________________________________________________
void SpdEcalSECHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdEcalSECHit::Print(const Option_t* opt) const
{
   cout << "<SpdEcalSECHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


