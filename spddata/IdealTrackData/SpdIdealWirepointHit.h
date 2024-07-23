// $Id$
// Author: artur   2019/09/16

#ifndef __SPDIDEALWIREPOINTHIT_H__
#define __SPDIDEALWIREPOINTHIT_H__

#include <TMath.h>
#include "SpdIdealHit.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdIdealWirepointHit                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdIdealWirepointHit: public SpdIdealHit  {

public:

    SpdIdealWirepointHit();
    virtual ~SpdIdealWirepointHit();
  
    /* ========== SETTERS ========== */

    // hit
    virtual void SetHit(Double_t x, Double_t y, Double_t z) { fHit.SetXYZ(x,y,z); }
    virtual void SetHit(TVector3 hit) { fHit = hit; }

    void SetResolutionRZ(Double_t rr, Double_t zz) { fResolution.SetXYZ(rr,zz,-1); }
    void SetResolutionR(Double_t rr)  { fResolution.SetX(rr); }
    void SetResolutionZ(Double_t zz)  { fResolution.SetY(zz); }

    // point additional (hit)
    void SetDirInPoint(Double_t lx, Double_t ly, Double_t lz) { fDirInPoint.SetXYZ(lx,ly,lz); }
    void SetDirInPoint(Double_t l, Double_t nx, Double_t ny, Double_t nz) { fDirInPoint.SetXYZ(nx,ny,nz); fDirInPoint.SetMag(l); }
    void SetDirInPoint(TVector3 dir, Double_t s) { fDirInPoint = dir; dir.SetMag(1); fSegLength = s; }
    void SetDirInPoint(TVector3 dir) { fDirInPoint = dir; dir.SetMag(1); }
    void SetSegLength(Double_t s) { fSegLength = s; }
    
    // detector
    void SetWireMaxRadius(Double_t R) { fWireMaxRadius = fabs(R); }
    void SetWire(TVector3& point1, TVector3& point2) { fWirePoint1 = point1; fWirePoint2 = point2; }
    void SetWire(TVector3& point, TVector3& dir, Double_t halfl) { fWirePoint1 = point-halfl*dir; fWirePoint2 = point+halfl*dir; }
    
    /* ========== GETTERS ========== */
    
    void      GetDirInPoint(TVector3& dir) const { dir = fDirInPoint; }
    TVector3  GetDirInPoint() const { return fDirInPoint; }
    Double_t  GetSegLen() const { return fSegLength; } // track segment length  
    
    Double_t  GetResolutionR() const { return fResolution.X(); }
    Double_t  GetResolutionZ() const { return fResolution.Y(); }
    
    // hit
    virtual Bool_t   GetHit(TVector3& hit) const { hit = fHit; return true; }
            TVector3 GetHit() const { return fHit; }
            
    Double_t  GetRdrift() const;  // hit drift radius
    Double_t  GetZwire()  const;  // hit coordinate along the wire
    Int_t     GetLRtype() const;  // hit type: -1(left), +1(right), 0(undefined) (to resolove left/right ambiguity)
     
    TVector3  GetPointOnWire() const; // hit projection on wire
    TVector3  GetDriftDirR()   const; // hit radial component in the wire CS (orthohonal to the wire)
    TVector3  GetWireDirZ()    const; // hit z componentin in the wire CS (along the wire)
    
    // point
    Double_t  GetPointRdrift() const;  // point drift radius
    Double_t  GetPointZwire()  const;  // point coordinate along the wire
    Int_t     GetPointLRtype() const;  // point hit type: -1(left), +1(right), 0(undefined) (to resolove left/right ambiguity)
     
    TVector3  GetPointPointOnWire() const; // point projection on wire
    TVector3  GetPointDriftDirR()   const; // point radial component in the wire CS (orthohonal to the wire)
    TVector3  GetPointWireDirZ()    const; // point z component in the wire CS (along the wire)
    
    // detector
    Double_t  GetWireMaxRadius()  const { return fWireMaxRadius; }
    TVector3  GetWirePoint1()     const { return fWirePoint1; }
    TVector3  GetWirePoint2()     const { return fWirePoint2; }
    
    Double_t  GetWireTheta()      const { return GetWireDirection(false).Theta()*TMath::RadToDeg(); }  // deg
    Double_t  GetWirePhi()        const { return GetWireDirection(false).Phi()*TMath::RadToDeg();   }  // deg
    
    Double_t  GetWireLength()     const { return (fWirePoint2-fWirePoint1).Mag();     }
    Double_t  GetWireHalfLength() const { return 0.5*(fWirePoint2-fWirePoint1).Mag(); }
    TVector3  GetWirePosition()   const { return 0.5*(fWirePoint1+fWirePoint2);       }
    
    TVector3  GetWireDirection(Bool_t norm = true) const { return (norm) ? (fWirePoint2-fWirePoint1).Unit() : (fWirePoint2-fWirePoint1); } 
   
    virtual void PrintHit(Bool_t print_point = true, Bool_t print_res = true) const;
    
private:
    
    // hit
    TVector3 fHit;           // hit coordinates (global S.C.)
    
    // additional point chars 
    TVector3 fDirInPoint;    // track direction in mc-point
    Double_t fSegLength;     // track segment length  
    
    // detector
    Double_t fWireMaxRadius; // max. drift radius
    TVector3 fWirePoint1;    // first end of the wire
    TVector3 fWirePoint2;    // second end of the wire
    
    ClassDef(SpdIdealWirepointHit,1)
};

#endif  /* __SPDIDEALWIREPOINTHIT_H__ */

