
#include "EcalCellPoint.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(EcalCellPoint)

//_____________________________________________________________________________
EcalCellPoint::EcalCellPoint():FairMCPoint(),fPdg(0)
{
}

//_____________________________________________________________________________
EcalCellPoint::EcalCellPoint(Int_t trackID, Int_t detID,
                             TVector3 pos, TVector3 mom,
                             Double_t tof, Double_t length,
                             Double_t eLoss)
:FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss),fPdg(0)
{
}

//_____________________________________________________________________________________
void EcalCellPoint::Print(const Option_t* opt) const
{
  cout << "<EcalCellPoint::Print> " << endl; 
  cout << "\n";
  cout << " EventID:                 " << fEventId << endl;
  cout << " Track/DetectorID:        " << fTrackID << "/" << fDetectorID << endl;
  cout << " Particle (pdg):          " << fPdg << endl;
  cout << " Node:                    " << fNode << endl;
  cout << " Position, Time:          " << fX << ", " << fY << ", " << fZ << " [cm]  " 
                                       << fTime << " [ns] " << endl;
  cout << " Momentum                 " << fPx << ", " << fPy << ", " << fPz << " [GeV]" << endl;
  cout << " Energy loss:             " << fELoss*1.0e06 << " [keV] " << endl;
  cout << "\n";
}





