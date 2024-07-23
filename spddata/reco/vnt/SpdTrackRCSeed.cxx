// $Id$
// Author: vladimir   2021/11/28


//_____________________________________________________________________________
//
// SpdTrackSeed
//_____________________________________________________________________________

#include "SpdTrackRCSeed.h"
#include "SpdDetectorList.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTrackRCSeed)

//_____________________________________________________________________________
SpdTrackRCSeed::SpdTrackRCSeed():
fId(-1), fSeedPars(0) 
{}

//_____________________________________________________________________________
SpdTrackRCSeed::~SpdTrackRCSeed() 
{
    if (fSeedPars) delete fSeedPars;
}

//_____________________________________________________________________________
void SpdTrackRCSeed::DeleteSeedPars() 
{ 
    if (fSeedPars) {
        delete fSeedPars; 
        fSeedPars = 0; 
    }
}

//_____________________________________________________________________________
void SpdTrackRCSeed::Clear(Option_t* option)
{
    DeleteSeedPars();
}

//_____________________________________________________________________________
SpdTrackRCSeedPar* SpdTrackRCSeed::SeedPars() 
{ 
    if (fSeedPars) return fSeedPars; 
    
    fSeedPars = new SpdTrackRCSeedPar();

    return fSeedPars; 
}

//_____________________________________________________________________________
//Int_t SpdTrackRCSeed::GetSeed(Int_t i) const
//{
//    if (i == 0) return *fSeeds.begin();
//    if (i < 0 || i > fSeeds.size()) return -1;
//    std::set<Int_t>::const_iterator it = fTracks.begin();
//    Int_t n(0);
//    while (true) { if (i == n++) return *it; else it++; }
//    return -1;
//}

//_____________________________________________________________________________
void SpdTrackRCSeed::Print(Option_t* option) const
{
    cout << "\n<SpdTrackRCSeed::Print> ----------------------------------\n\n";
    
    cout << "Unique Id:                " << fId << endl;
    cout << "Seeds (id):              "; 

//    for (const int& i : fHits) cout << Form("%-4d ",i);
//    cout << endl;
    
    if (fSeedPars && TString(option).Contains("seedpars")) {
        cout << endl;
        fSeedPars->Print();
    }

    cout << endl;

}




