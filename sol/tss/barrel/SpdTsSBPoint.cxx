// $Id$
// Author: artur   2018/02/01

#include "SpdTsSBPoint.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTsSBPoint)

//_____________________________________________________________________________________
SpdTsSBPoint::SpdTsSBPoint():FairMCPoint(),fDetectorTID(-1)
{
  
}

//_____________________________________________________________________________________
SpdTsSBPoint::SpdTsSBPoint(Int_t trackID, Int_t detID,
                           TVector3 posIn, TVector3 momIn, Double_t tofIn, 
                           Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, posIn, momIn, tofIn, length, eLoss), 
fDetectorTID(-1)
{
  
}

//_____________________________________________________________________________________
SpdTsSBPoint::SpdTsSBPoint(Int_t trackID, Int_t detID, Long_t detTID,
                           TVector3 posIn, TVector3 momIn, Double_t tofIn, 
                           Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, posIn, momIn, tofIn, length, eLoss), 
fDetectorTID(detTID)
{
  
}

//_____________________________________________________________________________________
SpdTsSBPoint::SpdTsSBPoint(Int_t trackID, Int_t detID,
                           TVector3 posIn, TVector3 momIn, Double_t tofIn,
                           TVector3 posOut, TVector3 momOut, Double_t tofOut,
                           Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, posIn, momIn, tofIn, length, eLoss), 
fXOut(posOut.X()), fYOut(posOut.Y()), fZOut(posOut.Z()), fTimeOut(tofOut),
fPxOut(momOut.Px()),fPyOut(momOut.Py()),fPzOut(momOut.Pz()),
fDetectorTID(-1)
{
  
}

//_____________________________________________________________________________________
SpdTsSBPoint::SpdTsSBPoint(Int_t trackID, Int_t detID, Long_t detTID,
                           TVector3 posIn, TVector3 momIn, Double_t tofIn, 
                           TVector3 posOut, TVector3 momOut, Double_t tofOut,
                           Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, posIn, momIn, tofIn, length, eLoss), 
fXOut(posOut.X()), fYOut(posOut.Y()), fZOut(posOut.Z()), fTimeOut(tofOut),
fPxOut(momOut.Px()),fPyOut(momOut.Py()),fPzOut(momOut.Pz()),
fDetectorTID(detTID)
{
  
}


//_____________________________________________________________________________________
SpdTsSBPoint::~SpdTsSBPoint() 
{ 
  
}

//_____________________________________________________________________________________
void SpdTsSBPoint::Print(const Option_t* opt) const
{
  cout << "<SpdTsSBPoint::Print> " << endl; 
  cout << "\n";
  cout << " EventID:                 " << fEventId << endl;
  cout << " Track/DetectorID:        " << fTrackID << "/" << fDetectorID << endl;
  cout << " DetectorTID (table id):  " << fDetectorTID << endl;
  cout << " Position, Time (IN):     " << fX << ", " << fY << ", " << fZ << " [cm]  " 
                                       << fTime << " [ns] " << endl;
  cout << " Momentum (IN)            " << fPx << ", " << fPy << ", " << fPz << " [GeV]" << endl;
  cout << " Position, Time (OUT):    " << fXOut << ", " << fYOut << ", " << fZOut << " [cm]  " 
                                       << fTimeOut << " [ns] " << endl;
  cout << " Momentum (OUT)           " << fPxOut << ", " << fPyOut << ", " << fPzOut << " [GeV]" << endl;
  cout << " Energy loss:             " << fELoss*1.0e06 << " [keV] " << endl;
  cout << "\n";
}




