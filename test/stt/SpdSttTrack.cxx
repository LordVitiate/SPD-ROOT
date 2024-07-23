

#include "SpdSttTrack.h"
#include "SpdDetectorList.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;
using std::map;

//_____________________________________________________________________________________
SpdSttTrack::SpdSttTrack() :
   fDist(0.), fPhi(0.), fRad(0.), fTanL(0.), fZ(0.), 
   fH(0),  
   fFlag(0),
   fTrackCandIndex(0)
{
}

//_____________________________________________________________________________________
void SpdSttTrack::Print() 
{
}

//_____________________________________________________________________________________
void SpdSttTrack::AddHelixHit(Int_t size, Int_t index, Int_t HelixHitID) 
{
    fHelixHits.Set(size);
    fHelixHits[index] = HelixHitID;
    AddLink(FairLink("SttHelixHit",HelixHitID));
}

//_____________________________________________________________________________________
TVector3 *SpdSttTrack::PCAToPoint(TVector3 *point)
{
  // calculates the pca to (0, 0) in 2D (xy plane)
  
  // transverse  
  Double_t R = fRad;
  Double_t xc = (fDist + fRad) * TMath::Cos(fPhi);
  Double_t yc = (fDist + fRad) * TMath::Sin(fPhi);
 
  // transverse -> circle
  // 1. find the line joining the point and the centre
  Double_t m = (yc - point->Y()) / (xc - point->X());
  Double_t q = yc - m * xc;

  // 2. find the point on track closest to point
  // +
  Double_t x1 = (-(m*(q - yc) - xc) + sqrt((m*(q - yc) - xc)*(m*(q - yc) - xc) - (m*m + 1)*((q - yc)*(q - yc) + xc*xc - R*R))) / (m*m + 1);
  Double_t y1 = m*x1 + q;
  // - 
  Double_t x2 = (-(m*(q - yc) - xc) - sqrt((m*(q - yc) - xc)*(m*(q - yc) - xc) - (m*m + 1)*((q - yc)*(q - yc) + xc*xc - R*R))) / (m*m + 1);
  Double_t y2 = m*x2 + q;
  
  Double_t dist1 = sqrt((point->Y() - y1)*(point->Y() - y1) + (point->X() - x1)*(point->X() - x1));
  Double_t dist2 = sqrt((point->Y() - y2)*(point->Y() - y2) + (point->X() - x2)*(point->X() - x2));
 
  TVector3 *clsontrk; // close on track
  if(dist1 < dist2) clsontrk = new TVector3(x1, y1, 0.);
  else clsontrk = new TVector3(x2, y2, 0.);

  // longitudinal: find the z correspondent to the PCA in x, y (CHECK not in 3D!!!)
  Double_t clstrkln = CalculateScosl(clsontrk->X(), clsontrk->Y());
  Double_t z0 = fZ;
  Double_t zslope = fTanL;
  Double_t z = z0 + zslope * clstrkln;
  clsontrk->SetZ(z);

  return clsontrk;
}

//_____________________________________________________________________________________
TVector3 *SpdSttTrack::MomentumAtPoint(TVector3 *point)
{
  // momentum at a certain point of the helix track  
  
  // transverse momentum 
  Double_t pt = 0., px = 0., py = 0.;
  
  // tangent in point on track
  Double_t R = fRad;
  Double_t xc = (fDist + fRad) * TMath::Cos(fPhi);
  Double_t yc = (fDist + fRad) * TMath::Sin(fPhi);
  Double_t m = (yc - point->Y()) / (xc - point->X());
  Double_t mt = -1./m;
  Double_t alpha = TMath::ATan(mt);
  
  pt = 0.006 * R;
  px = pt * TMath::Cos(alpha);
  py = pt * TMath::Sin(alpha);
  
  
  // negative NO if point higher than center
  // positive NO if point lower than center
  if((fH < 0 &&  (yc - point->Y()) < 0) || (fH > 0 &&  (yc - point->Y()) > 0)) {
      px = -px;
      py = -py;
   }
   else if(fH == 0) cout << "NO CHARGE" << endl;
  
  // longitudinal momentum 
  Double_t pl = 0.;
  
  Double_t tanl = fTanL;
  pl = pt * tanl;
  
  TVector3 *ptotcl = new TVector3(px, py, pl);
 
  return ptotcl;
}

//_____________________________________________________________________________________
Double_t SpdSttTrack::CalculateScosl(Double_t x, Double_t y)
{
  // track length
  
  Double_t x_0 = (fDist + fRad) * TMath::Cos(fPhi);
  Double_t y_0 = (fDist + fRad) * TMath::Sin(fPhi);

  Double_t x0 = fDist * TMath::Cos(fPhi);
  Double_t y0 = fDist * TMath::Sin(fPhi);

  Double_t Phi0 = TMath::ATan2((y0 - y_0),(x0 - x_0));

  //  cout << "calculate Phi0: " << Phi0 << " v0: " << x0 << " " << y0 << endl;
  //  cout << "phi: " << fPhi << " Phi0: " << Phi0 << endl;

  Double_t scos = -fH * fRad * TMath::ATan2((y - y0) * TMath::Cos(Phi0) - (x - x0) * TMath::Sin(Phi0) , 
                                             fRad + (x - x0) * TMath::Cos(Phi0) + (y - y0) * TMath::Sin(Phi0));

  return scos;

}



ClassImp(SpdSttTrack)
