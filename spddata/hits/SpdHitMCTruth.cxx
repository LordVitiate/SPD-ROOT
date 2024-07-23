// $Id$
// Author: artur   2021/09/20

//_____________________________________________________________________________
//
// SpdHitMCTruth
//_____________________________________________________________________________

#include "SpdHitMCTruth.h"
#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdHitMCTruth)

//_____________________________________________________________________________
Int_t SpdHitMCTruth::Add(Int_t MotherId)
{
    Int_t i(0);
    while (i < fTruth.size()) {
       if (fTruth[i] == MotherId)
       {
           fTruth[i+1] += 1;
           return i;
       }
       i += 2;
    }
    
    fTruth.push_back(MotherId);
    fTruth.push_back(1);
    
    return fTruth.size()-1;
}

//_____________________________________________________________________________
Int_t SpdHitMCTruth::Add(Int_t MotherId, Bool_t& is_new)
{
    Int_t i(0);
    while (i < fTruth.size()) {
       if (fTruth[i] == MotherId)
       {
           fTruth[i+1] += 1;
           is_new = false;
           return i;
       }
       i += 2;
    }
    
    fTruth.push_back(MotherId);
    fTruth.push_back(1);
    
    is_new = true;
    return fTruth.size()-1;
}

//_____________________________________________________________________________
Int_t SpdHitMCTruth::GetEntries() const
{
    Int_t sum(0), i(0);
    while (i < fTruth.size()) { sum += fTruth[i]; i += 2; }
    return sum;
}

//_____________________________________________________________________________
void SpdHitMCTruth::print() const
{
    if (fTruth.empty()) return;
    for (Int_t i(0); i < fTruth.size(); i +=2 ) {
         printf("\tPARTICLE:  [ I | ID | ENTRIES ] =" ); 
         printf(" [ %5d | %9d | %4d ] \n", i/2, fTruth[i], fTruth[i+1]); 
    }
}

