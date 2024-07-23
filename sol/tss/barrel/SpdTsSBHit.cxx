// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdTsSBHit
//_____________________________________________________________________________

#include "SpdTsSBHit.h"
//#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdTsSBHit)

//_____________________________________________________________________________
SpdTsSBHit::SpdTsSBHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdTsSBHit::~SpdTsSBHit() 
{
 
}

//_____________________________________________________________________________
void SpdTsSBHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdTsSBHit::Print(const Option_t* opt) const
{
   cout << "<SpdTsSBHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


