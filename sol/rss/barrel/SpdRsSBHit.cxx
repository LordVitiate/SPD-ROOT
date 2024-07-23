// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdRsSBHit
//_____________________________________________________________________________

#include "SpdRsSBHit.h"
//#include <TMath.h>

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdRsSBHit)

//_____________________________________________________________________________
SpdRsSBHit::SpdRsSBHit(Int_t uid):FairHit(),
fFirstID(-1),fSecondID(-1),
fDeposit(0)
{
    fDetectorID = uid;
}

//_____________________________________________________________________________
SpdRsSBHit::~SpdRsSBHit() 
{
 
}

//_____________________________________________________________________________
void SpdRsSBHit::AddPoint(Int_t num) 
{
   fPoints.push_back(num);
}

//_____________________________________________________________________________
void SpdRsSBHit::Print(const Option_t* opt) const
{
   cout << "<SpdRsSBHit::Print> "
        << " Uid: " << GetUid()
        << " ID: (" << fFirstID << "," << fSecondID << ") " 
        << " Points: " << GetNPoints() 
        << " Deposit: " << GetDeposit() << endl;
}


