// $Id$
// Author: artur   2021/11/24


//_____________________________________________________________________________
//
// SpdMCStrawHit1D
//_____________________________________________________________________________

#include "SpdMCStrawHit1D.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMCStrawHit1D)

//_____________________________________________________________________________
SpdMCStrawHit1D::SpdMCStrawHit1D():SpdMCTrackHit(),
fR(0),fResR(0),fDetLayer(-1),
fDirInPoint(1,0,0),fSegLength(0),
fMaxRadius(0.5),fWirePoint1(0,0,0),fWirePoint2(0,0,1)
{
 
}

//_____________________________________________________________________________
void SpdMCStrawHit1D::Reset()
{
    SpdMCTrackHit::Reset();
        
    fR = 0.;   
    fResR = 0.;
    
    fDirInPoint.SetXYZ(1,0,0);
    fSegLength = 0.;
    fDetLayer  =-1;
    
    fMaxRadius = 0.5; 
    fWirePoint1.SetXYZ(0,0,0);    
    fWirePoint2.SetXYZ(0,0,1);     
}

//=============================================================================
// Hit
//=============================================================================

//___________________________________________________________________________    
void SpdMCStrawHit1D::SetRdrift(TVector3 hit)
{
   TVector3 WireDirection = GetWireDirection();
   TVector3 r = (hit-fWirePoint1).Cross(WireDirection);
   fR = r.Mag();  
}
    
//=============================================================================
// Point
//=============================================================================

//___________________________________________________________________________
Double_t SpdMCStrawHit1D::GetPointRdrift() const
{
    TVector3 u = GetWireDirection();
    TVector3 v = fDirInPoint.Unit();
    TVector3 deltaPoint = fMCPoint - fWirePoint1;
    
    // distance along the track line between fMCPoint and the point of the closest approach to the tube axis
    Double_t lca = deltaPoint.Dot(u*u.Dot(v) - v) / (1. - pow(u.Dot(v),2));    // in normal case lca == 0
    // segment length
    Double_t s = GetSegLen();
    
    if (fabs(lca) <= s/2.) {
        // the point of the closest approach lies inside the track segment
        // [fMCPoint is assumed to be the mid-point of the track segment]
        Double_t dca = fabs(deltaPoint.Dot(u.Cross(v).Unit()));
        return dca;
    }
    else {
        // can happen sometimes when the track enters or exits the tube through its end 
        // or when the particle stops inside the tube
        // minimal distance to the tube axis is achieved at one of the track segment's ends
        TVector3 in  = fMCPoint - s/2.*v;
        TVector3 out = fMCPoint + s/2.*v;
        Double_t r_in  = (in  - fWirePoint1).Cross(u).Mag();
        Double_t r_out = (out - fWirePoint1).Cross(u).Mag();
        return std::min(r_in, r_out);
    }
}

//___________________________________________________________________________
Double_t SpdMCStrawHit1D::GetPointZwire()  const
{
   TVector3 WireDirection = GetWireDirection();
   return fabs(WireDirection.Dot(fMCPoint-fWirePoint1));
}

//___________________________________________________________________________
Int_t SpdMCStrawHit1D::GetPointLRtype() const
{ 
   TVector3 WireDirection = GetWireDirection(false);
   TVector3 U = fDirInPoint.Cross(WireDirection);
   TVector3 l = GetPointPointOnWire();
   Double_t v = U.Dot(l-fMCPoint);
   if (fabs(v) < 1.e-6) return 0;
   return (v > 0) ? 1 : -1;
}

//___________________________________________________________________________    
TVector3 SpdMCStrawHit1D::GetPointPointOnWire() const
{
   TVector3 WireDirection = GetWireDirection();
   return fWirePoint1 + WireDirection.Dot(fMCPoint-fWirePoint1)*WireDirection;
}

//___________________________________________________________________________    
TVector3 SpdMCStrawHit1D::GetPointDriftDirR() const
{
   TVector3 p = GetPointPointOnWire();
   return fMCPoint - p;
}

//___________________________________________________________________________    
TVector3 SpdMCStrawHit1D::GetPointWireDirZ() const
{
   TVector3 WireDirection = GetWireDirection();
   return WireDirection.Dot(fMCPoint-fWirePoint1)*WireDirection;
}

//_____________________________________________________________________________    
void SpdMCStrawHit1D::Print(Option_t* option) const
{
   TVector3 wpos = GetWirePosition();
   
   cout << "\t<" << this->ClassName() << "::Print> ---------------------------------------- " << "\n\n";
   cout << "\tHit unique ID:         " << fId << endl;
   cout << "\tModule (detector) id:  " << fModId  << endl;
   cout << "\tDetector id:           "; for (Int_t i(0); i<fDetId.size(); i++) { cout << fDetId[i] << "  "; } cout << endl;
   cout << "\tHit time (error):      " << GetHitTime() << " (" << GetTimeStampError() << ") [ns] " << endl;
   cout << "\tResponse:              " << fResp*1e6 << " [keV] " << endl;
   cout << "\tWeight:                " << fWeight << endl;
   cout << "\tSpecifity:             " << fSpecifity << endl;
   cout << "\tMCPoint(id):           " << fMCPointId << endl;
   cout << "\tMCPoint(x,y,z):        " << fMCPoint.X() << ", " << fMCPoint.Y() << ", " << fMCPoint.Z() 
                                       << " (D0 = " << fMCPoint.Mag() << ") [cm] " << endl;
   cout << "\tPoint(r,z):            " << GetPointRdrift() << ", " << GetPointZwire() << " [cm] " << endl;
   cout << "\tMCDirPoint(nx,ny,nz):  " << fDirInPoint.X() << ", " << fDirInPoint.Y() << ", " << fDirInPoint.Z() << endl;
   cout << "\tSegment length:        " << GetSegLen() << " [cm] " << endl;
   cout << "\tHit(r):                " << fR << " [cm] " << endl;
   cout << "\tResolution(r):         " << fResR*1e4 << " [mkm] " << endl;
   cout << "\tDelta(h-p):            " << GetDeltaR()*1e4 << " [mkm] " << endl;

   cout << "\tDetLayer:              " << fDetLayer   << endl;
   
   if (fMCTruth) { cout << endl; fMCTruth->print(); }
   else cout << "\tPARTICLE: unknown  " << "\n\n"; 
   
   printf("\n\tDetector (position, local axes & dimensions): \n\n");
   
   cout << "\tWire (length,R):        " << GetWireLength() << ", " << GetMaxRadius() << " [cm] " << endl; 
   cout << "\tWire (center) position: " << wpos.X() << ", " << wpos.Y() << ", " << wpos.Z() << " [cm] " << endl; 
   cout << "\tWire dir.(theta,phi):   " << GetWireTheta() << ", " << GetWirePhi() << " [deg] " << endl;
   cout << endl;
}


