// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdTsTBHit
//_____________________________________________________________________________

#include "SpdTsTBHit.h"
//#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdTsTBHit)

//_____________________________________________________________________________
SpdTsTBHit::SpdTsTBHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdTsTBHit::~SpdTsTBHit() 
{
 
}

//_____________________________________________________________________________
void SpdTsTBHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdTsTBHit::Print(const Option_t* opt) const
{
   cout << "<SpdTsTBHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


