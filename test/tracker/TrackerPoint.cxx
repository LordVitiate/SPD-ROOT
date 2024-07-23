
#include "TrackerPoint.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(TrackerPoint)

//_____________________________________________________________________________
TrackerPoint::TrackerPoint():FairMCPoint(),
fFirstNodeId(0),fSecondNodeId(0),fSegmentLength(0)
{
}

//_____________________________________________________________________________
TrackerPoint::TrackerPoint(Int_t trackID, Int_t detID,
                             TVector3 pos, TVector3 mom,
                             Double_t tof, Double_t length,
                             Double_t eLoss):
FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss),
fFirstNodeId(0),fSecondNodeId(0),fSegmentLength(0)
{
}

//_____________________________________________________________________________________
void TrackerPoint::Print(const Option_t* opt) const
{
  cout << "<TrackerPoint::Print> " << endl; 
  cout << "\n";
  cout << " EventID:                 " << fEventId << endl;
  cout << " Track/DetectorID:        " << fTrackID << "/" << fDetectorID << endl;
  cout << " Length(total,segment):   " << fLength << ", " << fSegmentLength << " [cm] " << endl;
  cout << " Node:                    " << fNode << endl;
  cout << " Node(ID):                " << fFirstNodeId << "/" << fSecondNodeId << endl;
  cout << " Position, Time (in):     " << fX << ", " << fY << ", " << fZ << " [cm]  " 
                                       << fTime << " [ns] " << endl;
  cout << " Position, Time (out):    " << fXOut << ", " << fYOut << ", " << fZOut << " [cm]  " 
                                       << fTimeOut << " [ns] " << endl;                                                                          
  cout << " Momentum (in):           " << fPx << ", " << fPy << ", " << fPz << " [GeV]" << endl;
  cout << " Momentum (out):          " << fPxOut << ", " << fPyOut << ", " << fPzOut << " [GeV]" << endl;
  cout << " Energy loss:             " << fELoss*1.0e06 << " [keV] " << endl;
  cout << "\n";
}





