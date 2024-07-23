// $Id$
// Author: andre   2020/07/31

#include "SpdEcalBPoint.h"
#include "SpdEcalBGeoMapper.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdEcalBPoint)

//_____________________________________________________________________________________
SpdEcalBPoint::SpdEcalBPoint():FairMCPoint(),
fBasketID(0),fModuleZID(0),fModulePhiID(0),fCellID(0),fLayerID(0),
fMatID(""),fNodePath("")
{
}

//_____________________________________________________________________________________
SpdEcalBPoint::SpdEcalBPoint(Int_t trackID, Int_t detID, 
                                 Int_t basketID, Int_t moduleZID, Int_t modulePhiID, Int_t cellID, Int_t layerID,
                                 TString matID,
                                 TVector3 pos, TVector3 mom, Double_t tof, 
                                 Double_t length, Double_t eLoss, TString nodePath)
:FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss), 
fBasketID(basketID), fModuleZID(moduleZID), fModulePhiID(modulePhiID), fCellID(cellID), fLayerID(layerID), 
fMatID(matID), fNodePath(nodePath)
{
    
}

//_____________________________________________________________________________________
SpdEcalBPoint::~SpdEcalBPoint() 
{ 
  
}

//_____________________________________________________________________________________
TString SpdEcalBPoint::GetNodePathToCell(TString & absVol, TString & scintVol) 
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
void SpdEcalBPoint::Print(const Option_t* opt) const
{
  cout << "<SpdEcalBPoint::Print> " << endl; 
  cout << "\n";
  cout << " Track/DetectorID: " << fTrackID << "/" << fDetectorID << endl;
  cout << " Basket:           " << fBasketID << endl;
  cout << " Module Z/φ ID:    " << fModuleZID << "/" << fModulePhiID << endl;
  cout << " Cell/Layer:       " << fCellID << "/" << fLayerID  << endl;
  cout << " Material:         " << fMatID << endl;
  cout << " Node path:        " << fNodePath << endl;
  cout << " Position, Time:   " << fX << ", " << fY << ", " << fZ << " [cm]  " 
                                << fTime << " [ns] " << endl;
  cout << " Momentum:         " << fPx << ", " << fPy << ", " << fPz << " [GeV]" << endl;
  cout << " Energy loss:      " << fELoss*1.0e06 << " [keV] " << endl;
  cout << "\n";
}


