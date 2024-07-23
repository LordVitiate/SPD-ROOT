// $Id$
// Author: artur   2018/02/01

//_____________________________________________________________________________
//
// SpdEcalTEC2Hit
//_____________________________________________________________________________

#include "SpdEcalTEC2Hit.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdEcalTEC2Hit)

//_____________________________________________________________________________
SpdEcalTEC2Hit::SpdEcalTEC2Hit():FairHit() 
{
    SetDetectorID(-1);
    
    fRecoEnergy = 0.;
    fRawEnergy = 0.;
    fMCEnergy = 0.;
    fEndcapID = -1;
    fModuleID = -1;
    fCellID = -1;
    fNodePath = "";
    fPos = TVector3(0., 0., 0.);
}

//_____________________________________________________________________________
SpdEcalTEC2Hit::SpdEcalTEC2Hit(
        Int_t detID, 
        Int_t endcapID, Int_t moduleID, 
        Int_t cellID, TString nodePath)
    :FairHit(), 
    fEndcapID(endcapID), 
    fModuleID(moduleID), fCellID(cellID), 
    fNodePath(nodePath)
{
    SetDetectorID(detID);
    fRawEnergy = 0.;
    fMCEnergy = 0.;
    fRecoEnergy = 0.;
}

//_____________________________________________________________________________
SpdEcalTEC2Hit::~SpdEcalTEC2Hit() 
{
 
}


//_____________________________________________________________________________
void SpdEcalTEC2Hit::Print(const Option_t* opt) const
{
    cout << "<SpdEcalTEC2Hit::Print> " << endl;
    cout << endl;
    cout << " DetectorID: " << fDetectorID << endl;
    cout << " Endcap: " << fEndcapID << endl;
    cout << " Module ID: " << fModuleID << endl;
    cout << " Cell: " << fCellID << endl;
    cout << " Node path: " << fNodePath << endl;
    cout << " Position: (" << fPos.X() << ", " << fPos.Y() << ", " << fPos.Z() << ") [cm] " << endl;
    cout << " Deposited energy (raw, in scintillator): " << fRawEnergy << endl;
    cout << " Deposited energy (reconstructed): " << fRecoEnergy << endl;
    cout << " Deposited energy (MC truth): " << fMCEnergy << endl;
    cout << endl;
}


