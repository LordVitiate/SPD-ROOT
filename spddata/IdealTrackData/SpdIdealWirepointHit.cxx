// $Id$
// Author: artur   2019/09/16


//_____________________________________________________________________________
//
// SpdIdealWirepointHit
//_____________________________________________________________________________

#include "SpdIdealWirepointHit.h"

ClassImp(SpdIdealWirepointHit)

//_____________________________________________________________________________
SpdIdealWirepointHit::SpdIdealWirepointHit():SpdIdealHit(),
fDirInPoint(1,0,0),fSegLength(0),fWireMaxRadius(1),fWirePoint1(0,0,0),fWirePoint2(0,0,1)
{
}

//_____________________________________________________________________________
SpdIdealWirepointHit::~SpdIdealWirepointHit() 
{
}

//***************************************************************************/
//******************************** HIT **************************************/
//***************************************************************************/

//___________________________________________________________________________
Double_t SpdIdealWirepointHit::GetRdrift() const
{
   TVector3 WireDirection = GetWireDirection();
   TVector3 r = (fHit-fWirePoint1).Cross(WireDirection);
   return r.Mag();
}
//___________________________________________________________________________
Double_t SpdIdealWirepointHit::GetZwire()  const
{
   TVector3 WireDirection = GetWireDirection();
   return fabs(WireDirection.Dot(fHit-fWirePoint1));
}

//___________________________________________________________________________
Int_t SpdIdealWirepointHit::GetLRtype() const
{ 
   TVector3 WireDirection = GetWireDirection(false);
   TVector3 U = fDirInPoint.Cross(WireDirection);
   TVector3 l = GetPointOnWire();
   Double_t v = U.Dot(l-fHit);
   if (fabs(v) < 1.e-6) return 0;
   return (v > 0) ? 1 : -1;
}

//___________________________________________________________________________    
TVector3 SpdIdealWirepointHit::GetPointOnWire() const
{
   TVector3 WireDirection = GetWireDirection();
   return fWirePoint1 + WireDirection.Dot(fHit-fWirePoint1)*WireDirection;
}

//___________________________________________________________________________    
TVector3 SpdIdealWirepointHit::GetDriftDirR() const
{
   TVector3 p = GetPointOnWire();
   return fHit - p;
}

//___________________________________________________________________________    
TVector3 SpdIdealWirepointHit::GetWireDirZ() const
{
   TVector3 WireDirection = GetWireDirection();
   return WireDirection.Dot(fHit-fWirePoint1)*WireDirection;
}

//***************************************************************************/
//****************************** POINT **************************************/
//***************************************************************************/

//___________________________________________________________________________
Double_t SpdIdealWirepointHit::GetPointRdrift() const
{
   TVector3 WireDirection = GetWireDirection();
   TVector3 r = (fPoint-fWirePoint1).Cross(WireDirection);
   return r.Mag();
}
//___________________________________________________________________________
Double_t SpdIdealWirepointHit::GetPointZwire()  const
{
   TVector3 WireDirection = GetWireDirection();
   return fabs(WireDirection.Dot(fPoint-fWirePoint1));
}

//___________________________________________________________________________
Int_t SpdIdealWirepointHit::GetPointLRtype() const
{ 
   TVector3 WireDirection = GetWireDirection(false);
   TVector3 U = fDirInPoint.Cross(WireDirection);
   TVector3 l = GetPointPointOnWire();
   Double_t v = U.Dot(l-fPoint);
   if (fabs(v) < 1.e-6) return 0;
   return (v > 0) ? 1 : -1;
}

//___________________________________________________________________________    
TVector3 SpdIdealWirepointHit::GetPointPointOnWire() const
{
   TVector3 WireDirection = GetWireDirection();
   return fWirePoint1 + WireDirection.Dot(fPoint-fWirePoint1)*WireDirection;
}

//___________________________________________________________________________    
TVector3 SpdIdealWirepointHit::GetPointDriftDirR() const
{
   TVector3 p = GetPointPointOnWire();
   return fPoint - p;
}

//___________________________________________________________________________    
TVector3 SpdIdealWirepointHit::GetPointWireDirZ() const
{
   TVector3 WireDirection = GetWireDirection();
   return WireDirection.Dot(fPoint-fWirePoint1)*WireDirection;
}

//_____________________________________________________________________________
void SpdIdealWirepointHit::PrintHit(Bool_t print_point, Bool_t print_res) const
{
   SpdIdealHit::PrintHit(false,false);
   
   if (print_point) {
       printf("\tpoint:        [%8.3f, %8.3f, %8.3f]  [cm]\n",fPoint.X(),fPoint.Y(),fPoint.Z());
       printf("\thit:          [%8.3f, %8.3f, %8.3f]  [cm]\n",fHit.X(),fHit.Y(),fHit.Z());
       printf("\tdir(point):   [%8.3f, %8.3f, %8.3f]  [cm]\n",fDirInPoint.X(),fDirInPoint.Y(),fDirInPoint.Z());
   }
   if (print_res) {
       printf("\tresolution:   [%8.0f, %8.0f, %8.0f] [mkm]\n",fResolution.X()*1e4 ,fResolution.Y()*1e4 ,fResolution.Z()*1e4);
   }
   
   printf("\n");
   
   printf("\tPoint: r,z,s      [%8.3f, %8.3f, %8.3f]  [cm] lr = %-8d\n",GetPointRdrift(),GetPointZwire(),GetSegLen(),GetPointLRtype());
   printf("\tHit:   r,z,s      [%8.3f, %8.3f, %8.3f]  [cm] lr = %-8d\n",GetRdrift(),GetZwire(),GetSegLen(),GetLRtype());
   printf("\tWire: L,Theta,Phi [%8.3f, %8.3f, %8.3f]  [cm] r  = %8.3f [cm]\n",GetWireLength(),
          GetWireTheta(),GetWirePhi(),GetWireMaxRadius());
   
   TVector3 wpos = GetWirePosition();
   
   printf("\tWpos: R,Theta,Phi [%8.3f, %8.3f, %8.3f]  [cm] Dr = %8.3f [cm]\n",wpos.Mag(),wpos.Theta()*TMath::RadToDeg(),
                                                                        wpos.Phi()*TMath::RadToDeg(),wpos.Perp());
   printf("\n");
}

    
