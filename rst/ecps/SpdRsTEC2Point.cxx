// $Id$
// Author: alver   2020/07/16

#include "SpdRsTEC2Point.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRsTEC2Point)

//_____________________________________________________________________________________
SpdRsTEC2Point::SpdRsTEC2Point():FairMCPoint(),fHitGeoPath("") 
{
  
}

//_____________________________________________________________________________________
SpdRsTEC2Point::SpdRsTEC2Point(Int_t trackID, Int_t detID,
                           TVector3 pos, TVector3 mom, Double_t tof, 
                           Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss), 
fHitGeoPath("")
{
  
}

//_____________________________________________________________________________________
SpdRsTEC2Point::SpdRsTEC2Point(Int_t trackID, Int_t detID, TString hpath,
                           TVector3 pos, TVector3 mom, Double_t tof, 
                           Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss), 
fHitGeoPath("")
{
  
}

//_____________________________________________________________________________________
SpdRsTEC2Point::SpdRsTEC2Point(Int_t trackID, Int_t detID, TString hpath,
                             TVector3 pos, TVector3 mom, TVector3 posOut, TVector3 momOut,
                             Double_t tof, Double_t tofOut, 
                             Double_t length, Double_t segmentlength, Double_t eLoss)
:FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss), 
fHitGeoPath(hpath),
fPosOut(posOut),
fMomOut(momOut),
fTimeOut(tofOut),
fSegmentLength(segmentlength)
{
  
}

//_____________________________________________________________________________________
SpdRsTEC2Point::~SpdRsTEC2Point() 
{ 
  
}

//_____________________________________________________________________________________
void SpdRsTEC2Point::Print(const Option_t* opt) const
{
  cout << "<SpdRsTECPoint::Print> " << endl; 
  cout << "\n";
  cout << " Track/DetectorID:   " << fTrackID << "/" << fDetectorID << endl;
  cout << " Hit node path:      " << fHitGeoPath << endl;
  cout << " Position, Time In:  " << fX << ", " << fY << ", " << fZ << " [cm]  " 
                                  << fTime << " [ns] " << endl;
  cout << " Position, Time Out: " << fPosOut.X() << ", " << fPosOut.Y() << ", " << fPosOut.Z() << " [cm]  " 
                                  << fTimeOut << " [ns] " << endl;
  cout << " Momentum In:        " << fPx << ", " << fPy << ", " << fPz << " [GeV]" << endl;
  cout << " Momentum Out:       " << fMomOut.X() << ", " << fMomOut.Y() << ", " << fMomOut.Z() << " [GeV]" << endl;
  cout << " Energy loss:        " << fELoss*1.0e06 << " keV " << endl;
  cout << "\n";
}




