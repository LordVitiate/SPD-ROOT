// $Id$
// Author: artur   2021/02/03

#ifndef __SPDMCSTRAWHIT2D_H__
#define __SPDMCSTRAWHIT2D_H__

#include "SpdMCTrackHit.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCStrawHit2D                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCStrawHit2D: public SpdMCTrackHit {

public:

    SpdMCStrawHit2D();
    virtual ~SpdMCStrawHit2D() {}
    
    virtual void Reset();
  
    // ======= Setters =======
    
    //---------------------------------------------------------
    // for the hit
    
    // >>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<
    // set R- Z- drift from mc-hit
    // WARNING The DETECTOR parameters must be set 
    // >>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<
    
    void SetRZdrift(TVector3 hit);
    void SetRdrift(TVector3 hit);
    void SetZdrift(TVector3 hit);
    
    // set R-, Z- drift directly
    void SetRZ(Double_t r, Double_t z) { fR = r; fZ = z; }
    void SetR(Double_t r) { fR = r; }
    void SetZ(Double_t z) { fZ = z; }
    
    // set R-, Z- resolution directly
    void SetResolutionRZ(Double_t dr, Double_t dz) { fResR = dr; fResZ = dz; }
    void SetResolutionR(Double_t dr) { fResR = dr; }
    void SetResolutionZ(Double_t dz) { fResZ = dz; }
    
    //---------------------------------------------------------
    // for the point
    
    void SetDirInPoint(Double_t lx, Double_t ly, Double_t lz) 
         { fDirInPoint.SetXYZ(lx,ly,lz); }
    void SetDirInPoint(Double_t l, Double_t nx, Double_t ny, Double_t nz) 
         { fDirInPoint.SetXYZ(nx,ny,nz); fDirInPoint.SetMag(l); }
       
    void SetDirInPoint(TVector3 dir, Double_t s) { fDirInPoint = dir; dir.SetMag(1); fSegLength = s; }
    void SetDirInPoint(TVector3 dir) { fDirInPoint = dir; dir.SetMag(1); }
    void SetSegLength(Double_t s) { fSegLength = s; }
    
    //---------------------------------------------------------
    // for the detector
    
    void SetMaxRadius(Double_t R) { fMaxRadius = fabs(R); }
    void SetWire(TVector3& point1, TVector3& point2) { fWirePoint1 = point1; fWirePoint2 = point2; }
    void SetWire(TVector3& point, TVector3& dir, Double_t halfl) 
         { fWirePoint1 = point-halfl*dir; fWirePoint2 = point+halfl*dir; }
   
    // ======= Getters =======
    
    //---------------------------------------------------------
    // for the hit
    
    Double_t  GetRdrift() const { return fR; } // hit drift radius
    Double_t  GetZwire()  const { return fZ; } // hit coordinate along the wire
    
    TVector3  GetPointOnWire() const; // hit projection on wire (global CS)
    TVector3  GetWireDirZ()    const; // hit z component in the wire CS (along the wire)
    
    Double_t  GetResolutionR() const { return fResR; }
    Double_t  GetResolutionZ() const { return fResZ; }
    
    //---------------------------------------------------------
    // for the point
    
    void      GetDirInPoint(TVector3& dir) const { dir = fDirInPoint; }
    TVector3  GetDirInPoint() const { return fDirInPoint; }
    Double_t  GetSegLen()     const { return fSegLength;  } 
    
    Double_t  GetPointRdrift() const;  // point drift radius
    Double_t  GetPointZwire()  const;  // point coordinate along the wire
    Int_t     GetPointLRtype() const;  // point hit type: -1(left), +1(right), 0(undefined) (to resolove left/right ambiguity)
     
    TVector3  GetPointPointOnWire() const; // point projection on wire
    TVector3  GetPointDriftDirR()   const; // point radial component in the wire CS (orthohonal to the wire)
    TVector3  GetPointWireDirZ()    const; // point z component in the wire CS (along the wire)
    
    //---------------------------------------------------------
    // for the detector

    Double_t  GetMaxRadius()      const { return fMaxRadius; }
    TVector3  GetWirePoint1()     const { return fWirePoint1; }
    TVector3  GetWirePoint2()     const { return fWirePoint2; }
    
    Double_t  GetWireTheta()      const { return GetWireDirection(false).Theta()*TMath::RadToDeg(); }  // deg
    Double_t  GetWirePhi()        const { return GetWireDirection(false).Phi()*TMath::RadToDeg();   }  // deg
    
    Double_t  GetWireLength()     const { return (fWirePoint2-fWirePoint1).Mag();     }
    Double_t  GetWireHalfLength() const { return 0.5*(fWirePoint2-fWirePoint1).Mag(); }
    TVector3  GetWirePosition()   const { return 0.5*(fWirePoint1+fWirePoint2);       }
    
    TVector3  GetWireDirection(Bool_t norm = true) const 
             { return (norm) ? (fWirePoint2-fWirePoint1).Unit() : (fWirePoint2-fWirePoint1); } 
     
    // point-hit distance, [cm]
    Double_t  GetDeltaR() const { return fabs(GetPointRdrift()-fR); } 
    Double_t  GetDeltaZ() const { return fabs(GetPointZwire()-fZ);  } 
    
    // Print
    virtual void Print(Option_t* option = "") const;

private:
    
    // hit
    Double_t  fR;             // hit radial coordinate [cm]
    Double_t  fZ;             // hit longitudinal (z) coordinate [cm] 
    Double_t  fResR;          // hit radial resolution [cm]
    Double_t  fResZ;          // hit longitudinal (z) resolution [cm]
    
    // additional mc-point info 
    TVector3  fDirInPoint;    // track direction in mc-point
    Double_t  fSegLength;     // track segment length  [cm]
    
    // detector
    Double_t  fMaxRadius;     // max. drift radius
    TVector3  fWirePoint1;    // first end of the wire
    TVector3  fWirePoint2;    // second end of the wire
    
    ClassDef(SpdMCStrawHit2D,1)
};

#endif  /* __SPDMCSTRAWHIT2D_H__ */

