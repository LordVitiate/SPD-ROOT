// $Id$
// Author: artur   2021/11/26

#include "SpdAegPoint.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdAegPoint)

//_____________________________________________________________________________________
SpdAegPoint::SpdAegPoint():FairMCPoint(),fModuleId(0),fTimeOut(0),fSegmentLength(0)
{
  fELoss = -1.;
}

//_____________________________________________________________________________________
SpdAegPoint::SpdAegPoint(Int_t trackID, Int_t detID, Int_t modID,
                            TVector3 pos, TVector3 mom, TVector3 posOut, TVector3 momOut,
                            Double_t tof, Double_t tofOut,
                            Double_t length, Double_t segmentlength, Double_t ELoss)
:FairMCPoint(trackID, detID, pos, mom, tof, length, ELoss),
fModuleId(modID),
fPosOut(posOut),fMomOut(momOut),fTimeOut(tofOut),
fSegmentLength(segmentlength)
{

}

//_____________________________________________________________________________________
SpdAegPoint::~SpdAegPoint() 
{ 
  
}

//_____________________________________________________________________________________
void SpdAegPoint::Print(const Option_t* opt) const
{
  cout << "<SpdAegPoint::Print> " << endl; 
  cout << "\n";
  cout << " Track:               " << fTrackID << endl;
  cout << " DetectorID/ModuleID: " << fDetectorID << "/" << fModuleId << endl;
  cout << " Position (in,out):   " << "(" << fX << ", " << fY << ", " << fZ << "); " 
                                   << "(" << fPosOut.X() << ", " << fPosOut.Y() << ", " << fPosOut.Z() << ") " 
                                   << "[cm]" << endl;
  cout << " Momentum (in,out):   " << "(" << fPx << ", " << fPy << ", " << fPz << "); "
                                   << "(" << fMomOut.X() << ", " << fMomOut.Y() << ", " << fMomOut.Z() << ") "
                                   << "[GeV/c]" << endl;
  cout << " Time (in,out):       " << fTime << ", " << fTimeOut << " [ns] " << endl;
  cout << " Length:              " << fLength << " [cm] " << endl;
  cout << " Segment length:      " << fSegmentLength << " [cm] " << endl;
  cout << " Energy loss:         " << fELoss*1.0e06 << " [keV] " << endl;
  cout << "\n";
}


