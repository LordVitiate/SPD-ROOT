// $Id$
// Author: artur   2018/02/01

#include "SpdEcalTECPoint.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalTECPoint)

//_____________________________________________________________________________________
SpdEcalTECPoint::SpdEcalTECPoint():FairMCPoint(),fModuleID(-1),fSubModuleID(-1) 
{
  
}

//_____________________________________________________________________________________
SpdEcalTECPoint::SpdEcalTECPoint(Int_t trackID, Int_t detID,
                           TVector3 pos, TVector3 mom, Double_t tof, 
                           Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss), 
fModuleID(-1),fSubModuleID(-1) 
{
  
}

//_____________________________________________________________________________________
SpdEcalTECPoint::SpdEcalTECPoint(Int_t trackID, Int_t detID, Int_t modID,
                           TVector3 pos, TVector3 mom, Double_t tof, 
                           Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss), 
fModuleID(modID),fSubModuleID(-1) 
{
  
}


//_____________________________________________________________________________________
SpdEcalTECPoint::SpdEcalTECPoint(Int_t trackID, Int_t detID, Int_t modID, Int_t submodID,
                           TVector3 pos, TVector3 mom, Double_t tof, 
                           Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss), 
fModuleID(modID),fSubModuleID(submodID) 
{
  
}

//_____________________________________________________________________________________
SpdEcalTECPoint::~SpdEcalTECPoint() 
{ 
  
}

//_____________________________________________________________________________________
void SpdEcalTECPoint::Print(const Option_t* opt) const
{
  cout << "<SpdEcalTECPoint::Print> " << endl; 
  cout << "\n";
  cout << " Track/DetectorID: " << fTrackID << "/" << fDetectorID << endl;
  cout << " ModuleID:         " << fModuleID << " " << fSubModuleID << endl;
  cout << " Position, Time:   " << fX << ", " << fY << ", " << fZ << " [cm]  " 
                                << fTime << " [ns] " << endl;
  cout << " Momentum:         " << fPx << ", " << fPy << ", " << fPz << " [GeV]" << endl;
  cout << " Energy loss:      " << fELoss*1.0e06 << " keV " << endl;
  cout << "\n";
}




