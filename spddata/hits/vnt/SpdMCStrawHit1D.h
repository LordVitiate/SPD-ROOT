// $Id$
// Author: artur   2021/11/24
// Author: vladimir   2022/11/13

#ifndef __SPDMCSTRAWHIT1D_H__
#define __SPDMCSTRAWHIT1D_H__

#include "SpdMCTrackHit.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCStrawHit1D                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCStrawHit1D: public SpdMCTrackHit {

public:

    SpdMCStrawHit1D();
    virtual ~SpdMCStrawHit1D() {}
    
    virtual void Reset();

    // ======= Setters =======
    
    //---------------------------------------------------------
    // for the hit
    
    // >>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<
    // set R-drift from mc-hit
    // WARNING The DETECTOR parameters must be set 
    // >>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<
    
    void SetRdrift(TVector3 hit);
    
    // set R-drift directly
    void SetR(Double_t r) { fR = r; }
     
    // set R-resolution directly
    void SetResolutionR(Double_t dr) { fResR = dr; }
        
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

    void SetDetModule(Int_t mod) { fDetModule = mod; }
    void SetDetLayer(Int_t lay)  { fDetLayer  = lay; }

    // ======= Getters =======
    
    //---------------------------------------------------------
    // for the hit
    
    Double_t  GetRdrift()      const { return fR;    }  // hit drift radius
    Double_t  GetResolutionR() const { return fResR; }
    
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
    
    Int_t     GetDetModule() const { return fDetModule; }
    Int_t     GetDetLayer()  const { return fDetLayer;  }

    // Print
    virtual void Print(Option_t* option = "") const;

private:
    
    // hit
    Double_t  fR;             // hit radial coordinate [cm]
    Double_t  fResR;          // hit radial resolution [cm]
    
    // additional mc-point info 
    TVector3  fDirInPoint;    // track direction in mc-point
    Double_t  fSegLength;     // track segment length  [cm]
    
    // detector
    Double_t  fMaxRadius;     // max. drift radius
    TVector3  fWirePoint1;    // first end of the wire
    TVector3  fWirePoint2;    // second end of the wire

    Int_t      fDetModule;    // detector module
    Int_t      fDetLayer;     // detector layer
    
    ClassDef(SpdMCStrawHit1D,1)
};

#endif  /* __SPDMCSTRAWHIT1D_H__ */

