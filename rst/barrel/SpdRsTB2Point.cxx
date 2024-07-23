// $Id$
// Author: artur   2018/02/01

#include "SpdRsTB2Point.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRsTB2Point)

//_____________________________________________________________________________________
SpdRsTB2Point::SpdRsTB2Point():FairMCPoint(),fHitGeoPath("") 
{
  
}

//_____________________________________________________________________________________
SpdRsTB2Point::SpdRsTB2Point(Int_t trackID, Int_t detID,
                             TVector3 pos, TVector3 mom, Double_t tof, 
                             Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss), 
fHitGeoPath("") 
{
  
}

//_____________________________________________________________________________________
SpdRsTB2Point::SpdRsTB2Point(Int_t trackID, Int_t detID, TString hpath,
                             TVector3 pos, TVector3 mom, Double_t tof, 
                             Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss), 
fHitGeoPath(hpath) 
{
  
}

//_____________________________________________________________________________________
SpdRsTB2Point::SpdRsTB2Point(Int_t trackID, Int_t detID, TString hpath,
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
SpdRsTB2Point::~SpdRsTB2Point() 
{ 
  
}

//_____________________________________________________________________________________
void SpdRsTB2Point::Print(const Option_t* opt) const
{
  cout << "<SpdRsTB2Point::Print> " << endl; 
  cout << "\n";
  cout << " Track/DetectorID:  " << fTrackID << "/" << fDetectorID << endl;
  cout << " Hit node path:     " << fHitGeoPath << endl;
  cout << " Position In, Time: " << fX << ", " << fY << ", " << fZ << " [cm]  " 
                                 << fTime << " [ns] " << endl;
  cout << " Position Out:      " << fPosOut.X() << ", " << fPosOut.Y() << ", " << fPosOut.Z() << " [cm]  " << endl;
  cout << " Momentum In:       " << fPx << ", " << fPy << ", " << fPz << " [GeV]" << endl;
  cout << " Momentum Out:      " << fMomOut.X() << ", " << fMomOut.Y() << ", " << fMomOut.Z() << " [GeV]" << endl;
  cout << " Energy loss:       " << fELoss*1.0e06 << " keV " << endl;
  cout << "\n";
}




