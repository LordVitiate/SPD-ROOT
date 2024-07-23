// $Id$
// Author: artur   2021/02/03


//_____________________________________________________________________________
//
// SpdMCTrackHit
//_____________________________________________________________________________

#include "SpdMCTrackHit.h"

#include <iostream>

using std::cout;
using std::endl;

#define IDSIZE 2

ClassImp(SpdMCTrackHit)

//_____________________________________________________________________________
SpdMCTrackHit::SpdMCTrackHit():SpdHit(),
fResp(0),fMCPointId(-1),
fSpecifity('n'),fMCTruth(0) 
{
    fDetId.assign(IDSIZE,0);
}

//_____________________________________________________________________________
SpdMCTrackHit::~SpdMCTrackHit() 
{
    if (fMCTruth) delete fMCTruth;
}

//_____________________________________________________________________________
void SpdMCTrackHit::RemoveMCTruth()
{
    if (fMCTruth) {
        delete fMCTruth;
        fMCTruth = 0;
    }
}

//_____________________________________________________________________________
void SpdMCTrackHit::Reset()
{
    SpdHit::Reset();
    
    fDetId.assign(IDSIZE,0);
    fResp = 0; 
    
    fMCPoint.SetXYZ(0,0,0); 
    fMCPointId = -1;    
    
    fSpecifity = 'n';
    
    if (fMCTruth) fMCTruth->clear();
}

//_____________________________________________________________________________
Int_t SpdMCTrackHit::AddMCTruth(Int_t MotherId)
{
    if (!fMCTruth) fMCTruth = new SpdHitMCTruth();
    return fMCTruth->Add(MotherId);
}

//_____________________________________________________________________________
void SpdMCTrackHit::SetDetId(Int_t i, Long_t id)
{
    if (i < 0) return;
    if (i < IDSIZE) fDetId[i] = id;
    else {
        fDetId.resize(i+1,0);
        fDetId[i] = id;
    }
}

//_____________________________________________________________________________
Long_t SpdMCTrackHit::GetDetId(Int_t i) const
{
   if (i < 0) return Int_t(kSpdUndefined);
   if (i < IDSIZE) return fDetId[i];
   if (i < fDetId.size()) return fDetId[i];
   return Int_t(kSpdUndefined);
}

//_____________________________________________________________________________
void SpdMCTrackHit::Print(Option_t* option) const
{    
    cout << "\t<" << this->ClassName() << "::Print>" << "\n\n";
    cout << "\tHit unique ID:         " << fId << endl;
    cout << "\tModule id:             " << fModId  << endl;
    cout << "\tDetector id:           "; for (Int_t i(0); i<fDetId.size(); i++) { cout << fDetId[i] << " "; } cout << endl;
    cout << "\tHit time (error) [ns]: " << GetHitTime() << "( " << GetTimeStampError() << ") " << endl;
    cout << "\tResponse [keV]:        " << fResp*1e6 << endl;
    cout << "\tWeight:                " << fWeight << endl;
    cout << "\tSpecifity:             " << fSpecifity << endl;
    cout << "\tMCPoint(x,y,z):        " << fMCPoint.X() << ", " << fMCPoint.Y() << ", " << fMCPoint.Z() << " [cm] " << endl;
    
    if (fMCTruth) {
        cout << endl;
        fMCTruth->print();
    }
    
    cout << "\n";
}

