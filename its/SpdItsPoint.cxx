// $Id$
// Author: artur   2019/02/20

#include "SpdItsPoint.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdItsPoint)

//_____________________________________________________________________________
SpdItsPoint::SpdItsPoint():FairMCPoint(),fNodeId(-1),
fXOut(0.),fYOut(0.), fZOut(0.),
fPxOut(0.),fPyOut(0.),fPzOut(0.)
{
}

//_____________________________________________________________________________
SpdItsPoint::SpdItsPoint(Int_t trackID, Int_t detID,
                        TVector3 pos, TVector3 mom,
                        Double_t tof, Double_t length,
                        Double_t eLoss):
FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss),fNodeId(-1),
fXOut(0.),fYOut(0.), fZOut(0.),
fPxOut(0.),fPyOut(0.),fPzOut(0.)
{
}

//_____________________________________________________________________________
void SpdItsPoint::ClearVData()
{
  fVid.clear();
  fVed.clear();
}

//_____________________________________________________________________________
void SpdItsPoint::SetVData(const std::vector<Int_t>& vid, const std::vector<Double_t>& ved)
{
  fVid = vid;
  fVed = ved;
}

//_____________________________________________________________________________
void SpdItsPoint::GetPosTime(TVector3& pos, Double_t& t) const
{
    pos.SetXYZ(0.5*(fX+fXOut),0.5*(fY+fYOut),0.5*(fZ+fZOut));
    t = fTime;
}

//_____________________________________________________________________________
Double_t SpdItsPoint::GetSegLength() const
{
    Double_t s2 = (fXOut-fX)*(fXOut-fX) + (fYOut-fY)*(fYOut-fY) + (fZOut-fZ)*(fZOut-fZ);
    return TMath::Sqrt(s2);
}

//_____________________________________________________________________________
Bool_t SpdItsPoint::IsSpecialPoint(Int_t& vid, Double_t& dep) const
{
    vid = -1; 
    dep = 0.;
    
    if (fVid.size() != 2) return kFALSE;
 
    vid = fVid[0];
    if (vid < 0) {
        if (fVid[1] < 0) return kFALSE;
        vid = fVid[1];
    }
    
    dep = fVed[0] + fVed[1];
    
    return kTRUE;
}

//_____________________________________________________________________________________
void SpdItsPoint::Print(const Option_t* opt) const
{
  cout << "<SpdItsPoint::Print> " << endl; 
  cout << "\n";
  cout << " EventID:                 " << fEventId << endl;
  cout << " Track/DetectorID:        " << fTrackID << "/" << fDetectorID << endl;
  cout << " Node(ID):                " << fNodeId << endl;
  cout << " Position, (in):          " << fX << ", " << fY << ", " << fZ << " [cm] " << endl;
  cout << " Position, (out):         " << fXOut << ", " << fYOut << ", " << fZOut << " [cm] " << endl;              
  cout << " Time, Length:            " << fTime << " [ns] " << fLength << " [s] " << endl;     
  cout << " Segment length:          " << GetSegLength()*10 << " [mm] " << endl;     
  cout << " Momentum (in):           " << fPx << ", " << fPy << ", " << fPz << " [GeV]" << endl;
  cout << " Momentum (out):          " << fPxOut << ", " << fPyOut << ", " << fPzOut << " [GeV]" << endl;
  cout << " Energy loss:             " << fELoss*1.0e06 << " [keV] " << endl;
  cout << " Energy loss table size:  " << fVid.size() << endl;
  cout << "\n";
  
  PrintVData();
}

//_____________________________________________________________________________________
void SpdItsPoint::PrintVData() const
{
  cout << "\t<SpdItsPoint::PrintVData> " << endl; 
  cout << "\n";
  
  if (fVid.empty()) return;
  
  printf("\t%5s  %12s  %12s \n\n","I","Node_ID","Edep [keV]");
   
  Double_t esum(0.);
  for (Int_t i(0); i < fVid.size(); i++) {
       printf("\t%5d  %12d  %12.6f \n",i,fVid[i],fVed[i]*1e6);
       esum += fVed[i];
  }
  
  cout << "\n\t    Energy loss:       " << esum*1.0e06 << " [keV] " << endl;
  cout << "\n";
}


