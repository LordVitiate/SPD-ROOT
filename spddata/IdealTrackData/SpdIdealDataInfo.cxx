// $Id$
// Author: artur   2019/09/06

//_____________________________________________________________________________
//
// SpdIdealDataInfo
//_____________________________________________________________________________

#include "SpdIdealDataInfo.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdIdealDataInfo)

//_____________________________________________________________________________
SpdIdealDataInfo::SpdIdealDataInfo():TNamed(),fDetId(kSpdUndefined),fHitType(-1)
{
 
}

//_____________________________________________________________________________
SpdIdealDataInfo::SpdIdealDataInfo(const Char_t* DataBranch, const Char_t* ParsType):
TNamed(DataBranch,ParsType),fDetId(kSpdUndefined),fHitType(-1)
{
    
}
//_____________________________________________________________________________
SpdIdealDataInfo::SpdIdealDataInfo(const Char_t* DataBranch, const Char_t* ParsType, Int_t DetId):
TNamed(DataBranch,ParsType),fDetId(DetId),fHitType(-1)
{
    
}

//_____________________________________________________________________________
Int_t SpdIdealDataInfo::GetNHits() const
{
    Int_t nhits = 0;
    for (Int_t i(0); i < fHits.size(); i++) nhits += fHits[i];
    return nhits;
}
  
//_____________________________________________________________________________
void SpdIdealDataInfo::PrintInfo(TString opt) const
{
    if (opt == "short") {
        cout << "-I- <SpdIdealDataInfo::PrintInfo> " << fName << ", " << fTitle 
             << ";  module: " << fDetId << ";  hit type: " << fHitType
             << ";  tracks/hits: " << GetNTracks() << "/" << GetNHits() 
             << endl; 
        return;
    }
        
    cout << "-I- <SpdIdealDataInfo::PrintInfo>" << endl;
    printf("Data branch name:         %s\n",fName.Data());
    printf("Data parameters type:     %s\n",fTitle.Data());
    printf("Detector id:              %d\n",fDetId);
    printf("Hit type:                 %d\n",fHitType);
    printf("Tracks/Hits:            %d/%d \n",GetNTracks(),GetNHits());
    printf("\n");
}



