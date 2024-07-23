// $Id$
// Author: artur   2021/03/04

//_____________________________________________________________________________
//
// SpdEcalMCHit
//_____________________________________________________________________________

#include "SpdEcalMCHit.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalMCHit)
ClassImp(SpdEcalHitMCTruth)

//_____________________________________________________________________________
void SpdEcalHitMCTruth::clear() 
{ 
    SpdHitMCTruth::clear();
    fEdep.clear();
}

//_____________________________________________________________________________
Int_t SpdEcalHitMCTruth::Add(Int_t MotherId)
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
    
    fEdep.push_back(0.0);
    
    return fTruth.size()-1;
}

//_____________________________________________________________________________
Int_t SpdEcalHitMCTruth::Add(Int_t MotherId, Double_t edep)
{
    Int_t i(0);
    while (i < fTruth.size()) {
       if (fTruth[i] == MotherId)
       {
           fTruth[i+1] += 1;
           fEdep[i/2] += edep;
           return i;
       }
       i += 2;
    }
    
    fTruth.push_back(MotherId);
    fTruth.push_back(1);
    
    fEdep.push_back(edep);
    
    return fTruth.size()-1;
}

//_____________________________________________________________________________
Int_t SpdEcalHitMCTruth::Add(Int_t MotherId, Bool_t& is_new)
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
    
    fEdep.push_back(0);
     
    is_new = true;
    return fTruth.size()-1;
}

//_____________________________________________________________________________
Int_t SpdEcalHitMCTruth::Add(Int_t MotherId, Double_t edep, Bool_t& is_new)
{
    Int_t i(0);
    while (i < fTruth.size()) {
       if (fTruth[i] == MotherId)
       {
           fTruth[i+1] += 1;
           fEdep[i/2] += edep;
           is_new = false;
           return i;
       }
       i += 2;
    }
    
    fTruth.push_back(MotherId);
    fTruth.push_back(1);
    
    fEdep.push_back(edep);
     
    is_new = true;
    return fTruth.size()-1;
}

//_____________________________________________________________________________
void SpdEcalHitMCTruth::print() const
{
    if (fTruth.empty()) return;
    
    for (Int_t i(0); i < fTruth.size(); i +=2 ) {
         printf("\tPARTICLE:  [ I | ID | ENTRIES | EDEP, MeV ] =" ); 
         printf(" [ %5d | %9d | %4d | %12.3e ] \n", i/2, fTruth[i], fTruth[i+1], fEdep[i/2]*1.e3); 
    }
}
    
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

//_____________________________________________________________________________
SpdEcalMCHit::SpdEcalMCHit():fMCEnergy(0),fRCEnergy(0),fNPoints(0),fMCTruth(0)
{

}

//_____________________________________________________________________________
SpdEcalMCHit::~SpdEcalMCHit() 
{
    RemoveMCTruth();
}

//_____________________________________________________________________________
void SpdEcalMCHit::RemoveMCTruth()
{
    if (fMCTruth) {
        delete fMCTruth;
        fMCTruth = 0;
    }
}

//_____________________________________________________________________________
void SpdEcalMCHit::Reset()
{
    SpdHit::Reset();
    
    Int_t size = fDetId.size();   
    if (size > 0) {
        fDetId.clear(); 
        fDetId.assign(size,0);
    }
    
    fMCEnergy = 0; 
    fRCEnergy = 0;
    fNPoints  = 0;
    
    fPosition.SetXYZ(0,0,0); 
         
    if (fMCTruth) fMCTruth->clear();
}

//_____________________________________________________________________________
void SpdEcalMCHit::SetModId(Int_t id)
{
    fDetId.clear(); 
    fModId = id;
    if (id == kSpdEcalTB) fDetId.assign(4,0);
    else if (id == kSpdEcalTEC) fDetId.assign(3,0);
    else fModId = -1;
}

//_____________________________________________________________________________
Int_t SpdEcalMCHit::AddMCTruth(Int_t MotherId, Double_t edep)
{
    if (!fMCTruth) fMCTruth = new SpdEcalHitMCTruth();
    return fMCTruth->Add(MotherId,edep);
}

//_____________________________________________________________________________
void SpdEcalMCHit::Print(Option_t* option) const
{    
    cout << "\t<" << this->ClassName() << "::Print>" << "\n\n";
    cout << "\tHit unique ID:         " << fId << endl;
    cout << "\tModule id:             " << fModId  << endl;
    cout << "\tDetector id:           "; for (auto id : fDetId) cout << id << "  "; cout << endl;
    cout << "\tNumber of points (mc): " << fNPoints << endl;
    cout << "\tHit time (error) [ns]: " << GetHitTime() << " ( " << GetTimeStampError() << ") " << endl;
    cout << "\tEnergy (mc) [MeV]:     " << fMCEnergy*1e3 << endl;
    cout << "\tEnergy [MeV]:          " << fRCEnergy*1e3 << endl;
    cout << "\tWeight:                " << fWeight << endl;
    cout << "\tPosition:              " << fPosition.X() << ", " << fPosition.Y() << ", " << fPosition.Z() << " [cm] " << endl;
    
    if (fMCTruth) {
        cout << endl;
        fMCTruth->print();
    }
    
    cout << "\n";
}
