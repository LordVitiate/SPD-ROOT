// $Id$
// Author: artur   2021/03/02

//_____________________________________________________________________________
//
// SpdRsMCHit
//_____________________________________________________________________________

#include "SpdRsMCHit.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRsMCHit)
ClassImp(SpdRsHitMCTruth)

//_____________________________________________________________________________
void SpdRsHitMCTruth::clear() 
{ 
   SpdHitMCTruth::clear();
}

//_____________________________________________________________________________
void SpdRsHitMCTruth::print() const
{
   if (fTruth.empty()) return;
   for (Int_t i(0); i < fTruth.size(); i +=2 ) {
        printf("\tPARTICLE:  [ I | ID | ENTRIES ] =" ); 
        printf(" [ %5d | %9d | %4d ] \n", i/2, fTruth[i], fTruth[i+1]); 
   }
}
    
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

//_____________________________________________________________________________
SpdRsMCHit::SpdRsMCHit():fDetectorId(-1),fNPoints(0),fResp(0),
fHitType('n'),fAssociatedHit(-1),fMCTruth(0)
{
 
}

//_____________________________________________________________________________
SpdRsMCHit::~SpdRsMCHit() 
{
   RemoveMCTruth();  
}

//_____________________________________________________________________________
void SpdRsMCHit::RemoveMCTruth()
{
   if (fMCTruth) {
       delete fMCTruth;
       fMCTruth = 0;
   }
}

//_____________________________________________________________________________
void SpdRsMCHit::Reset()
{
    SpdHit::Reset();
    
    fDetectorId = 0;
    fNPoints = 0;
    fResp = 0; 
    fHitPos.SetXYZ(0,0,0);
    fHitType = '-';   
    fAssociatedHit = -1;  
     
    if (fMCTruth) fMCTruth->clear();
}

//_____________________________________________________________________________
Int_t SpdRsMCHit::AddMCTruth(Int_t MotherId)
{
    if (!fMCTruth) fMCTruth = new SpdRsHitMCTruth();
    return fMCTruth->Add(MotherId);
}

//_____________________________________________________________________________
void SpdRsMCHit::Print(Option_t* option) const
{    
    cout << "\t<" << this->ClassName() << "::Print>" << "\n\n";
    cout << "\tHit unique ID:              " << fId << endl;
    cout << "\tHit type:                   " << fHitType << endl;
    cout << "\tModule id:                  " << fModId  << endl;
    cout << "\tDetector (strip/wire) id:   " << fDetectorId;
    
    if (IsWireHit()) {
        cout << "  [Sector/Layer/MDT/Cell: " 
                    << SpdRsTID::Sector(fDetectorId)
             << "/" << SpdRsTID::Layer(fDetectorId)
             << "/" << SpdRsTID::MDT(fDetectorId)
             << "/" << SpdRsTID::Cell(fDetectorId) 
             << "] ";
    }
    cout << endl;
        
    cout << "\tNumber of points (mc):      " << fNPoints << endl;
    cout << "\tHit time (error) [ns]:      " << GetHitTime() << " ( " << GetTimeStampError() << ") " << endl;
    cout << "\tResponse [keV]:             " << fResp*1e6 << endl;
    cout << "\tWeight:                     " << fWeight << endl;
    cout << "\tHit position:               " << fHitPos.X() << ", " << fHitPos.Y() << ", " << fHitPos.Z() << " [cm] " << endl;
    cout << "\tAssociated hit:             " << fAssociatedHit << endl;
    
    if (fMCTruth) {
        cout << endl;
        fMCTruth->print();
    }
    
    cout << "\n";
}
