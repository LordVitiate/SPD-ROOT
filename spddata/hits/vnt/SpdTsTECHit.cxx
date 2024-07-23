// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdTsTECHit
//_____________________________________________________________________________

#include "SpdTsTECHit.h"
//#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdTsTECHit)

//_____________________________________________________________________________
SpdTsTECHit::SpdTsTECHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdTsTECHit::~SpdTsTECHit() 
{
 
}

//_____________________________________________________________________________
void SpdTsTECHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdTsTECHit::Print(const Option_t* opt) const
{
   cout << "<SpdTsTECHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


