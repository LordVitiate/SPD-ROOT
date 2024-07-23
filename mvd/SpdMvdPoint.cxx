// $Id$
// Author: artur   2021/11/12

#include "SpdMvdPoint.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMvdPoint)

//_____________________________________________________________________________________
SpdMvdPoint::SpdMvdPoint():FairMCPoint(),fSubdetID(0),fLayerID(0),fTimeOut(0),fSegmentLength(0)
{
  fELoss = -1.;
}

//_____________________________________________________________________________________
SpdMvdPoint::SpdMvdPoint(Int_t trackID, Int_t detID, Int_t subdetID, Int_t layerID,
                         TVector3 pos, TVector3 mom, TVector3 posOut, TVector3 momOut,
                         Double_t tof, Double_t tofOut,
                         Double_t length, Double_t segmentlength, Double_t ELoss)
:FairMCPoint(trackID, detID, pos, mom, tof, length, ELoss),
fSubdetID(subdetID),fLayerID(layerID),
fPosOut(posOut),fMomOut(momOut),fTimeOut(tofOut),
fSegmentLength(segmentlength)
{

}

//_____________________________________________________________________________________
SpdMvdPoint::~SpdMvdPoint() 
{ 
  
}

//_____________________________________________________________________________
void SpdMvdPoint::GetPosTime(TVector3& pos, Double_t& t) const
{
    pos.SetXYZ(0.5*(fX+fPosOut.X()),0.5*(fY+fPosOut.Y()),0.5*(fZ+fPosOut.Z()));
    t = fTime;
}

//_____________________________________________________________________________________
void SpdMvdPoint::Print(const Option_t* opt) const
{
  cout << "<SpdMvdPoint::Print> " << endl; 
  cout << "\n";
  cout << " Track:             " << fTrackID << endl;
  cout << " DetectorID:        " << fDetectorID << "; detector part: " << fSubdetID << endl;
  cout << " LayerID:           " << fLayerID << endl;
  cout << " Position (in,out): " << "(" << fX << ", " << fY << ", " << fZ << "); " 
                                 << "(" << fPosOut.X() << ", " << fPosOut.Y() << ", " << fPosOut.Z() << ") " 
                                << "[cm]" << endl;
  cout << " Momentum (in,out): " << "(" << fPx << ", " << fPy << ", " << fPz << "); "
                                 << "(" << fMomOut.X() << ", " << fMomOut.Y() << ", " << fMomOut.Z() << ") "
                                 << "[GeV/c]" << endl;
  cout << " Time (in,out):     " << fTime << ", " << fTimeOut << " [ns] " << endl;
  cout << " Length:            " << fLength << " [cm] " << endl;
  cout << " Segment length:    " << fSegmentLength << " [cm] " << endl;
  cout << " Energy loss:       " << fELoss*1.0e06 << " [keV] " << endl;
  cout << "\n";
}


