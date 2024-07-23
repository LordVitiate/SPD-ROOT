// $Id$
// Author: artur   2021/02/03


//_____________________________________________________________________________
//
// SpdMCStrawHit2D
//_____________________________________________________________________________

#include "SpdMCStrawHit2D.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMCStrawHit2D)

//_____________________________________________________________________________
SpdMCStrawHit2D::SpdMCStrawHit2D():SpdMCTrackHit(),
fR(0),fZ(0),fResR(0),fResZ(0),
fDirInPoint(1,0,0),fSegLength(0),
fMaxRadius(0.5),fWirePoint1(0,0,0),fWirePoint2(0,0,1)
{
}

//_____________________________________________________________________________
void SpdMCStrawHit2D::Reset()
{
    SpdMCTrackHit::Reset();
        
    fR = 0.;   
    fZ = 0.;   
    fResR = 0.;
    fResZ = 0.;
    
    fDirInPoint.SetXYZ(1,0,0);
    fSegLength = 0.;
    
    fMaxRadius = 0.5; 
    fWirePoint1.SetXYZ(0,0,0);    
    fWirePoint2.SetXYZ(0,0,1);  
}

//=============================================================================
// Hit
//=============================================================================

//___________________________________________________________________________    
void SpdMCStrawHit2D::SetRZdrift(TVector3 hit)
{
   TVector3 WireDirection = GetWireDirection();
   TVector3 r = (hit-fWirePoint1).Cross(WireDirection);
   fR = r.Mag();  
   fZ = fabs(WireDirection.Dot(hit-fWirePoint1)); 
}

//___________________________________________________________________________    
void SpdMCStrawHit2D::SetRdrift(TVector3 hit)
{
   TVector3 WireDirection = GetWireDirection();
   TVector3 r = (hit-fWirePoint1).Cross(WireDirection);
   fR = r.Mag();  
}

//___________________________________________________________________________
void SpdMCStrawHit2D::SetZdrift(TVector3 hit)
{
   TVector3 WireDirection = GetWireDirection();
   fZ = fabs(WireDirection.Dot(hit-fWirePoint1)); 
}

//___________________________________________________________________________    
TVector3 SpdMCStrawHit2D::GetPointOnWire() const
{
   TVector3 WireDirection = GetWireDirection(); 
   return fWirePoint1 + fZ*WireDirection;
}

//___________________________________________________________________________    
TVector3 SpdMCStrawHit2D::GetWireDirZ() const
{
   return fZ*GetWireDirection();
}

//=============================================================================
// Point
//=============================================================================

//___________________________________________________________________________
Double_t SpdMCStrawHit2D::GetPointRdrift() const
{
   TVector3 WireDirection = GetWireDirection();
   TVector3 r = (fMCPoint-fWirePoint1).Cross(WireDirection);
   return r.Mag();
}

//___________________________________________________________________________
Double_t SpdMCStrawHit2D::GetPointZwire()  const
{
   TVector3 WireDirection = GetWireDirection();
   return fabs(WireDirection.Dot(fMCPoint-fWirePoint1));
}

//___________________________________________________________________________
Int_t SpdMCStrawHit2D::GetPointLRtype() const
{ 
   TVector3 WireDirection = GetWireDirection(false);
   TVector3 U = fDirInPoint.Cross(WireDirection);
   TVector3 l = GetPointPointOnWire();
   Double_t v = U.Dot(l-fMCPoint);
   if (fabs(v) < 1.e-6) return 0;
   return (v > 0) ? 1 : -1;
}

//___________________________________________________________________________    
TVector3 SpdMCStrawHit2D::GetPointPointOnWire() const
{
   TVector3 WireDirection = GetWireDirection();
   return fWirePoint1 + WireDirection.Dot(fMCPoint-fWirePoint1)*WireDirection;
}

//___________________________________________________________________________    
TVector3 SpdMCStrawHit2D::GetPointDriftDirR() const
{
   TVector3 p = GetPointPointOnWire();
   return fMCPoint - p;
}

//___________________________________________________________________________    
TVector3 SpdMCStrawHit2D::GetPointWireDirZ() const
{
   TVector3 WireDirection = GetWireDirection();
   return WireDirection.Dot(fMCPoint-fWirePoint1)*WireDirection;
}

//_____________________________________________________________________________    
void SpdMCStrawHit2D::Print(Option_t* option) const
{
   TVector3 wpos = GetWirePosition();
   
   cout << "\t<" << this->ClassName() << "::Print> ---------------------------------------- " << "\n\n";
   cout << "\tHit unique ID:          " << fId << endl;
   cout << "\tModule (detector) id:   " << fModId  << endl;
   cout << "\tDetector id:            "; for (Int_t i(0); i<fDetId.size(); i++) { cout << fDetId[i] << "  "; } cout << endl;
   cout << "\tHit time (error):       " << GetHitTime() << " (" << GetTimeStampError() << ") [ns] " << endl;
   cout << "\tResponse:               " << fResp*1e6 << " [keV] " << endl;
   cout << "\tWeight:                 " << fWeight << endl;
   cout << "\tSpecifity:              " << fSpecifity << endl;
   cout << "\tMCPoint(id):            " << fMCPointId << endl;
   cout << "\tMCPoint(x,y,z):         " << fMCPoint.X() << ", " << fMCPoint.Y() << ", " << fMCPoint.Z() 
                                        << " (D0 = " << fMCPoint.Mag() << ") [cm] " << endl;
   cout << "\tPoint(r,z):             " << GetPointRdrift() << ", " << GetPointZwire() << " [cm] " << endl;
   cout << "\tMCDirPoint(nx,ny,nz):   " << fDirInPoint.X() << ", " << fDirInPoint.Y() << ", " << fDirInPoint.Z() << endl;
   cout << "\tSegment length:         " << GetSegLen() << " [cm] " << endl;
   cout << "\tHit(r,z):               " << fR << ", " << fZ << " [cm] " << endl;
   cout << "\tResolution(r,z):        " << fResR*1e4 << " [mkm], " << fResZ << " [cm] " << endl;
   cout << "\tDelta(h-p)(r,z):        " << GetDeltaR()*1e4 << " [mkm], " << GetDeltaZ() << " [cm] " << endl;
  
   if (fMCTruth) { cout << endl; fMCTruth->print(); }
   else cout << "\tPARTICLE: unknown  " << "\n\n"; 
   
   printf("\n\tDetector (position, local axes & dimensions): \n\n");
   
   cout << "\tWire (length,R):        " << GetWireLength() << ", " << GetMaxRadius() << " [cm] " << endl; 
   cout << "\tWire (center) position: " << wpos.X() << ", " << wpos.Y() << ", " << wpos.Z() << " [cm] " << endl; 
   cout << "\tWire dir.(theta,phi):   " << GetWireTheta() << ", " << GetWirePhi() << " [deg] " << endl;
   cout << endl;
   
}

