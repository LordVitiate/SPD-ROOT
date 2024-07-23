// $Id$
// Author: andre   2020/07/31

#include "SpdEcalECPoint.h"
#include "SpdEcalECGeoMapper.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdEcalECPoint)


//_____________________________________________________________________________________
SpdEcalECPoint::SpdEcalECPoint():FairMCPoint(),
fEndcapID(0),fModuleID(0),fCellID(0),fLayerID(0),
fMatID(""),fNodePath("")

{
}

//_____________________________________________________________________________________
SpdEcalECPoint::SpdEcalECPoint(Int_t trackID, Int_t detID, Int_t endcapID, Int_t moduleID, Int_t cellID, Int_t layerID, 
                                   TString matID,
                                   TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, 
                                   TString nodePath)
:FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss), 
fEndcapID(endcapID),fModuleID(moduleID),fCellID(cellID),fLayerID(layerID),
fMatID(matID),fNodePath(nodePath)
{
}

//_____________________________________________________________________________________
SpdEcalECPoint::~SpdEcalECPoint() 
{ 
  
}

//_____________________________________________________________________________________
TString SpdEcalECPoint::GetNodePathToCell(TString & absVol, TString & scintVol) 
{

     TString tok;
     Ssiz_t from = 0;
     TString pathToCell = "";
     while (fNodePath.Tokenize(tok, from, "[/]")) {
         if (tok.Contains(absVol) || tok.Contains(scintVol)) break;
         if (tok != "") pathToCell += "/" + tok;
     }

     return pathToCell;
}

//_____________________________________________________________________________________
void SpdEcalECPoint::Print(const Option_t* opt) const
{
  cout << "<SpdEcalECPoint::Print> " << endl; 
  cout << "\n";
  cout << " EventID:                  " << fEventId << endl;
  cout << " Track/DetectorID:         " << fTrackID  << "/" << fDetectorID << endl;
  cout << " Endcap/Module:            " << fEndcapID << "/" << fModuleID << endl;
  cout << " Cell/Layer:               " << fCellID  << "/" << fLayerID  << endl;
  cout << " Material:                 " << fMatID << endl;
  cout << " Node path:                " << fNodePath << endl;
  cout << " Position, Time:           " << fX << ", " << fY << ", " << fZ << " [cm]  " << fTime << " [ns] " << endl;
  cout << " Momentum:                 " << fPx << ", " << fPy << ", " << fPz << " [GeV]" << endl;
  cout << " Energy loss:              " << fELoss*1.0e06 << " [keV] " << endl;
  cout << "\n";
}

