// $Id$
// Author: artur   2021/03/02

//_____________________________________________________________________________
//
// SpdZdcMCHit
//_____________________________________________________________________________

#include "SpdZdcMCHit.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdZdcMCHit)

//_____________________________________________________________________________
SpdZdcMCHit::SpdZdcMCHit():fDetId(0),
fTrackLen(0),fTrackSegLen(0),
fResp(0),fMCTruth(0)
{
 
}

//_____________________________________________________________________________
SpdZdcMCHit::~SpdZdcMCHit() 
{
   RemoveMCTruth();  
}

//_____________________________________________________________________________
void SpdZdcMCHit::RemoveMCTruth()
{
   if (fMCTruth) {
       delete fMCTruth;
       fMCTruth = 0;
   }
}

//_____________________________________________________________________________
void SpdZdcMCHit::Reset()
{
    SpdHit::Reset();
    
    fDetId = 0;
    fResp = 0; 
    fHitPos.SetXYZ(0,0,0); 
    fTrackMom.SetXYZ(0,0,0);
    fTrackLen = 0;
    fTrackSegLen = 0;
    
    if (fMCTruth) fMCTruth->clear();
}

//_____________________________________________________________________________
Int_t SpdZdcMCHit::AddMCTruth(Int_t MotherId)
{
    if (!fMCTruth) fMCTruth = new SpdHitMCTruth();
    return fMCTruth->Add(MotherId);
}

//_____________________________________________________________________________
void SpdZdcMCHit::Print(Option_t* option) const
{    
    cout << "\t<" << this->ClassName() << "::Print>" << "\n\n";
    cout << "\tHit unique ID:             " << fId << endl;
    cout << "\tModule id:                 " << fModId << endl;
    cout << "\tDetector id:               " << fDetId << endl;
    cout << "\tWeight:                    " << fWeight << endl;
    cout << "\tParticle id (MC):          " << GetParticleId() << endl;
    cout << "\tHit time (error) [ns]:     " << GetHitTime() << " ( " << GetTimeStampError() << ") " << endl;
    cout << "\tHit position [cm]:         " << fHitPos.X() << ", " << fHitPos.Y() << ", " << fHitPos.Z() << endl;
    cout << "\tTrack momentum [GeV/c]:    " << fTrackMom.X() << ", " << fTrackMom.Y() << ", " << fTrackMom.Z() << endl;
    cout << "\tTrack length [cm]:         " << fTrackLen << endl;
    cout << "\tTrack segment length [cm]: " << fTrackSegLen << endl;
    cout << "\tResponse [keV]:            " << fResp*1e6 << endl;
     
    if (fMCTruth) {
        cout << endl;
        fMCTruth->print();
    }
    
    cout << "\n";
}
