// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdTsSECHit
//_____________________________________________________________________________

#include "SpdTsSECHit.h"
//#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdTsSECHit)

//_____________________________________________________________________________
SpdTsSECHit::SpdTsSECHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdTsSECHit::~SpdTsSECHit() 
{
 
}

//_____________________________________________________________________________
void SpdTsSECHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdTsSECHit::Print(const Option_t* opt) const
{
   cout << "<SpdTsSECHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


