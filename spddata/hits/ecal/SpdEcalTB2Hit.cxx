// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalTB2Hit
//_____________________________________________________________________________

#include "SpdEcalTB2Hit.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalTB2Hit)

//_____________________________________________________________________________
SpdEcalTB2Hit::SpdEcalTB2Hit():FairHit() 
{
    SetDetectorID(-1);
    
    fRawEnergy = 0.;
    fMCEnergy = 0.;
    fRecoEnergy = 0.;
    fBasketID = -1;
    fModuleZID = -1;
    fModulePhiID = -1;
    fCellID = -1;
    fNodePath = "";
    fPos = TVector3(0., 0., 0.);
}

//_____________________________________________________________________________
SpdEcalTB2Hit::SpdEcalTB2Hit(
        Int_t detID, 
        Int_t basketID, Int_t moduleZID, 
        Int_t modulePhiID, Int_t cellID, 
        TString nodePath)
    :FairHit(), 
    fBasketID(basketID), fModuleZID(moduleZID), 
    fModulePhiID(modulePhiID), fCellID(cellID), 
    fNodePath(nodePath)
{
    SetDetectorID(detID);
    fRawEnergy = 0.;
    fMCEnergy = 0.;
    fRecoEnergy = 0.;
}

//_____________________________________________________________________________
SpdEcalTB2Hit::~SpdEcalTB2Hit() 
{
 
}


//_____________________________________________________________________________
void SpdEcalTB2Hit::Print(const Option_t* opt) const
{
    cout << "<SpdEcalTB2Hit::Print> " << endl;
    cout << endl;
    cout << " DetectorID: " << fDetectorID << endl;
    cout << " Basket: " << fBasketID << endl;
    cout << " Module Z/φ ID: " << fModuleZID << "/" << fModulePhiID << endl;
    cout << " Cell: " << fCellID << endl;
    cout << " Node path: " << fNodePath << endl;
    cout << " Position: (" << fPos.X() << ", " << fPos.Y() << ", " << fPos.Z() << ") [cm] " << endl;
    cout << " Deposited energy (raw, in scintillator): " << fRawEnergy << endl;
    cout << " Deposited energy (reconstructed): " << fRecoEnergy << endl;
    cout << " Deposited energy (MC truth): " << fMCEnergy << endl;
    cout << endl;
    
}


