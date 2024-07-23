// $Id$
// Author: artur   2018/02/01

#include "SpdTsTBPoint.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdTsTBPoint)

//_____________________________________________________________________________________
SpdTsTBPoint::SpdTsTBPoint():FairMCPoint(),
fSegmentLength(0),fDetectorTID(-1)
{
  
}

//_____________________________________________________________________________________
SpdTsTBPoint::SpdTsTBPoint(Int_t trackID, Int_t detID,
                           TVector3 posIn, TVector3 momIn, Double_t tofIn, 
			   Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, posIn, momIn, tofIn, length, eLoss), 
fSegmentLength(0),fDetectorTID(-1)
{
  
}

//_____________________________________________________________________________________
SpdTsTBPoint::SpdTsTBPoint(Int_t trackID, Int_t detID, Long_t detTID,
                           TVector3 posIn, TVector3 momIn, Double_t tofIn, 
			   Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, posIn, momIn, tofIn, length, eLoss), 
fSegmentLength(0),fDetectorTID(detTID)
{
  
}

//_____________________________________________________________________________________
SpdTsTBPoint::SpdTsTBPoint(Int_t trackID, Int_t detID,
                           TVector3 posIn, TVector3 momIn, Double_t tofIn,
			   TVector3 posOut, TVector3 momOut, Double_t tofOut,
			   Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, posIn, momIn, tofIn, length, eLoss), 
fXOut(posOut.X()), fYOut(posOut.Y()), fZOut(posOut.Z()), fTimeOut(tofOut),
fPxOut(momOut.Px()),fPyOut(momOut.Py()),fPzOut(momOut.Pz()),fSegmentLength(0),
fDetectorTID(-1)
{
  
}

//_____________________________________________________________________________________
SpdTsTBPoint::SpdTsTBPoint(Int_t trackID, Int_t detID, Long_t detTID,
                           TVector3 posIn, TVector3 momIn, Double_t tofIn, 
			   TVector3 posOut, TVector3 momOut, Double_t tofOut,
			   Double_t length, Double_t eLoss)
:FairMCPoint(trackID, detID, posIn, momIn, tofIn, length, eLoss), 
fXOut(posOut.X()), fYOut(posOut.Y()), fZOut(posOut.Z()), fTimeOut(tofOut),
fPxOut(momOut.Px()),fPyOut(momOut.Py()),fPzOut(momOut.Pz()),fSegmentLength(0),
fDetectorTID(detTID)
{
  
}


//_____________________________________________________________________________________
SpdTsTBPoint::~SpdTsTBPoint() 
{ 
  
}

//_____________________________________________________________________________________
Double_t SpdTsTBPoint::GetPointDir(TVector3& dir) const 
{ 
    dir.SetXYZ(fXOut-fX,fYOut-fY,fZOut-fZ);
    Double_t mag = dir.Mag();
    if (mag < 1.e-6) {
        mag = 0;
        dir.SetXYZ(0.5*(fPxOut+fPx),0.5*(fPyOut+fPy),0.5*(fPzOut+fPz));
    }
    dir.SetMag(1);
    return mag;
}

//_____________________________________________________________________________
void SpdTsTBPoint::GetPosTime(TVector3& pos, Double_t& t) const
{
    pos.SetXYZ(0.5*(fX+fXOut),0.5*(fY+fYOut),0.5*(fZ+fZOut));
    t = 0.5*(fTime+fTimeOut); 
}

//_____________________________________________________________________________________
void SpdTsTBPoint::Print(const Option_t* opt) const
{
  cout << "<SpdTsTBPoint::Print> " << endl; 
  cout << "\n";
  cout << " EventID:                 " << fEventId << endl;
  cout << " Track/DetectorID:        " << fTrackID << "/" << fDetectorID << endl;
  cout << " DetectorTID (table id):  " << fDetectorTID << endl;
  cout << " Length(total,segment):   " << fLength << ", " << fSegmentLength << " [cm] " << endl;
  cout << " Position, Time (IN):     " << fX << ", " << fY << ", " << fZ << " [cm]  " 
                                       << fTime << " [ns] " << endl;
  cout << " Momentum (IN)            " << fPx << ", " << fPy << ", " << fPz << " [GeV]" << endl;
  cout << " Position, Time (OUT):    " << fXOut << ", " << fYOut << ", " << fZOut << " [cm]  " 
                                       << fTimeOut << " [ns] " << endl;
  cout << " Momentum (OUT)           " << fPxOut << ", " << fPyOut << ", " << fPzOut << " [GeV]" << endl;
  cout << " Energy loss:             " << fELoss*1.0e06 << " [keV] " << endl;
  cout << "\n";
}





