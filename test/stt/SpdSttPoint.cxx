
#include "SpdSttPoint.h"
#include "SpdDetectorList.h"

#include <iostream>
#include <TMath.h>

using std::cout;
using std::endl;

//_____________________________________________________________________________________
SpdSttPoint::SpdSttPoint() : 

FairMCPoint(),

fSectionID(0),

fXinLoc(0),  fYinLoc(0),  fZinLoc(0),
fXoutLoc(0), fYoutLoc(0), fZoutLoc(0),
fPx_in(0),   fPy_in(0),   fPz_in(0),
fPx_out(0),  fPy_out(0),  fPz_out(0),

fMass(0)
{

}

//_____________________________________________________________________________________
SpdSttPoint::SpdSttPoint(Int_t trackID, Int_t detID, TVector3 pos, 
                         TVector3 posInLocal, TVector3 posOutLocal, 
                         TVector3 momIn, TVector3 momOut,
                         Double_t tof, Double_t length, Double_t eLoss, Double_t mass) : 
                         
FairMCPoint(trackID, detID, pos, momIn, tof, length, eLoss),

fSectionID(0),

fXoutLoc(posOutLocal.X()), fYoutLoc(posOutLocal.Y()), fZoutLoc(posOutLocal.Z()),
fXinLoc(posInLocal.X()),   fYinLoc(posInLocal.Y()),   fZinLoc(posInLocal.Z()),
fPx_in(momIn.Px()),        fPy_in(momIn.Py()),        fPz_in(momIn.Pz()),
fPx_out(momOut.Px()),      fPy_out(momOut.Py()),      fPz_out(momOut.Pz()),

fMass(mass)
{
  
  fPx = (momIn.Px() + momOut.Px()) / 2.;
  fPy = (momIn.Py() + momOut.Py()) / 2.;
  fPz = (momIn.Pz() + momOut.Pz()) / 2.;
  
  SetLink(FairLink("MCTrack", trackID));
  
  //Print("");
}

//_____________________________________________________________________________________
SpdSttPoint::SpdSttPoint(Int_t trackID, Int_t detID, Int_t secID, TVector3 pos, 
                         TVector3 posInLocal, TVector3 posOutLocal, 
                         TVector3 momIn, TVector3 momOut,
                         Double_t tof, Double_t length, Double_t eLoss, Double_t mass) : 
                         
FairMCPoint(trackID, detID, pos, momIn, tof, length, eLoss),

fSectionID(secID),

fXoutLoc(posOutLocal.X()), fYoutLoc(posOutLocal.Y()), fZoutLoc(posOutLocal.Z()),
fXinLoc(posInLocal.X()),   fYinLoc(posInLocal.Y()),   fZinLoc(posInLocal.Z()),
fPx_in(momIn.Px()),        fPy_in(momIn.Py()),        fPz_in(momIn.Pz()),
fPx_out(momOut.Px()),      fPy_out(momOut.Py()),      fPz_out(momOut.Pz()),

fMass(mass)
{
  
  fPx = (momIn.Px() + momOut.Px()) / 2.;
  fPy = (momIn.Py() + momOut.Py()) / 2.;
  fPz = (momIn.Pz() + momOut.Pz()) / 2.;
  
  SetLink(FairLink("SpdMCTrack",trackID));
  //Print("");
}

//_____________________________________________________________________________________
SpdSttPoint::SpdSttPoint(const SpdSttPoint& point) :

FairMCPoint(point.fTrackID, point.fDetectorID, 
            TVector3(point.fX, point.fY, point.fZ), 
            TVector3(point.fPx, point.fPy, point.fPz), 
            point.fTime, point.fLength, point.fELoss, point.fEventId),
            
fSectionID(point.fSectionID),
            
fXoutLoc(point.fXoutLoc), fYoutLoc(point.fYoutLoc), fZoutLoc(point.fZoutLoc),
fXinLoc(point.fXinLoc),   fYinLoc(point.fYinLoc),   fZinLoc(point.fZinLoc),
fPx_in(point.fPx_in),     fPy_in(point.fPy_in),     fPz_in(point.fPz_in),
fPx_out(point.fPx_out),   fPy_out(point.fPy_out),   fPz_out(point.fPz_out),

fMass(point.fMass)
{
  SetLink(FairLink("SpdMCTrack",point.fTrackID));
  //Print("");
}

//_____________________________________________________________________________________
Double_t SpdSttPoint::GetRadialDistance() const 
{
  Double_t v = (fXoutLoc+fXinLoc)*(fXoutLoc+fXinLoc) + (fYoutLoc+fYinLoc)*(fYoutLoc+fYinLoc);
  return 0.5*TMath::Sqrt(v);
}

//_____________________________________________________________________________________
void SpdSttPoint::Print(const Option_t* opt) const 
{
  cout << "<SpdSttPoint::Print> ";
  cout << "Track: " << fTrackID << " Tube: " << fDetectorID << " Section: " << fSectionID
       << "  R = " << GetRadialDistance() << "  Eloss = " << fELoss << endl;
}




ClassImp(SpdSttPoint)
