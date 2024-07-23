// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsSECHit
//_____________________________________________________________________________

#include "SpdRsSECHit.h"
//#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdRsSECHit)

//_____________________________________________________________________________
SpdRsSECHit::SpdRsSECHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdRsSECHit::~SpdRsSECHit() 
{
 
}

//_____________________________________________________________________________
void SpdRsSECHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdRsSECHit::Print(const Option_t* opt) const
{
   cout << "<SpdRsSECHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


