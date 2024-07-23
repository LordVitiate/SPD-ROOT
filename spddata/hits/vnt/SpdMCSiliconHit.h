// $Id$
// Author: artur   2021/11/24

#ifndef __SPDMCSILICONHIT_H__
#define __SPDMCSILICONHIT_H__

#include "SpdMCTrackHit.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCSiliconHit                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCSiliconHit: public SpdMCTrackHit {

public:

    SpdMCSiliconHit();
    virtual ~SpdMCSiliconHit() {}
    
    virtual void Reset();
    
    // ======= Setters =======
    
    //---------------------------------------------------------
    // for the hit
    
    // >>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<
    // set U- V- coordinates
    // WARNING The DETECTOR parameters must be set 
    // >>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<
    
    void SetHitU(Double_t u) { fU = u; }
    void SetHitV(Double_t v) { fV = v; }
    
    void SetHitU(TVector3 hit);
    void SetHitV(TVector3 hit);
    void SetHitUV(TVector3 hit);

    void SetResolutionU(Double_t ru)  { fResU = ru; }
    void SetResolutionV(Double_t rv)  { fResV = rv; }
    void SetResolutionUV(Double_t ru, Double_t rv) { fResU = ru; fResV = rv; }
    
    //---------------------------------------------------------
    // for the detector
    
    void SetU(TVector3 x, Double_t mag = -1);  //inline
    void SetV(TVector3 x, Double_t mag = -1);  //inline
    
    void SetUdir(TVector3 x);  //inline
    void SetVdir(TVector3 x);  //inline
    
    void SetL(Double_t lu, Double_t lv, Double_t ln) { fLu.SetMag(lu); fLv.SetMag(lv); fLn = ln; }
    void SetLu(Double_t l) { fLu.SetMag(l); }
    void SetLv(Double_t l) { fLv.SetMag(l); }
    void SetLn(Double_t l) { fLn = l; }
    
    void SetDetPos(Double_t x, Double_t y, Double_t z) { fDetPos.SetXYZ(x,y,z); }
    void SetDetPos(TVector3 pos)  { fDetPos = pos;     }
    void SetDetLayer(Int_t layer) { fDetLayer = layer; }
    
    void SetDelta(Double_t delta) { fDelta = delta; } 
    
    //---------------------------------------------------------
    
    void SetSegLength(Double_t s) { fSegLength = s; }
    
    // ======= Getters =======

    //---------------------------------------------------------
    // for the hit

    Double_t GetHitU() const { return fU; }  // u coordinate (local CS)
    Double_t GetHitV() const { return fV; }  // v coordinate (local CS)
    
    Double_t GetResolutionU() const { return fResU; }
    Double_t GetResolutionV() const { return fResV; }
    
    TVector3 GetHit()  const;  // return hit position in global CS
    Double_t GetHitZ() const;  // return hit z-position in global CS
    
    //---------------------------------------------------------
    // for the point
    
    Double_t GetPointU() const;  // u coordinate (local СS)
    Double_t GetPointV() const;  // v coordinate (local СS)
    Double_t GetPointN() const;  // n coordinate (local СS)
    
    void GetPointUV(Double_t& xu, Double_t& xv) const;
    void GetPointUVN(Double_t& xu, Double_t& xv, Double_t& xn) const;
    
    //---------------------------------------------------------
    // for the detector  
    
    TVector3  GetU(Bool_t unit = kTRUE) const { return (unit) ? fLu.Unit() : fLu; }
    TVector3  GetV(Bool_t unit = kTRUE) const { return (unit) ? fLv.Unit() : fLv; }
    TVector3  GetN(Bool_t unit = kTRUE) const;
    
    Double_t  GetLu() const { return fLu.Mag(); }
    Double_t  GetLv() const { return fLv.Mag(); }
    Double_t  GetLn() const { return fLn; }
    
    TVector3  GetDetPos()   const { return fDetPos; }
    Int_t     GetDetLayer() const { return fDetLayer; }
    
    Double_t  GetDetPosX() const { return fDetPos.X(); }
    Double_t  GetDetPosY() const { return fDetPos.Y(); }
    Double_t  GetDetPosZ() const { return fDetPos.Z(); }
    
    Double_t  GetDelta()   const { return fDelta; } 
    Double_t  GetDeltaU()  const { return fabs(fU-GetPointU()); }
    Double_t  GetDeltaV()  const { return fabs(fV-GetPointV()); }

    //---------------------------------------------------------
    
    Double_t  GetSegLength() const { return fSegLength; }
    
    // Print
    virtual void Print(Option_t* option = "") const;
    
private:
   
    // hit
    Double_t   fU;        // hit U-coordinate (in global S.C.) [cm]
    Double_t   fV;        // hit V-coordinate (in global S.C.) [cm]
    Double_t   fResU;     // hit U-resolution [cm]
    Double_t   fResV;     // hit V-resolution [cm]
    
    // detector 
    Double_t   fLn;          // detector n-size/2 = |n|, n = [u x v] 
    TVector3   fLu;          // detector u - direction and size/2 = |fLu|
    TVector3   fLv;          // detector v - direction and size/2 = |fLv|
    TVector3   fDetPos;      // detector position
    Int_t      fDetLayer;    // detector layer
    
    // point-hit distance
    Double_t   fDelta;
    
    // additional mc-point info
    Double_t   fSegLength;
    
    ClassDef(SpdMCSiliconHit,1)
};

//_____________________________________________________________________________
inline void SpdMCSiliconHit::SetU(TVector3 x, Double_t mag) 
{   
    if (mag > 0) { fLu = x; fLu.SetMag(mag); }
    else { fLu = x.Unit(); }
}

//_____________________________________________________________________________
inline void SpdMCSiliconHit::SetV(TVector3 x, Double_t mag) 
{   
    if (mag > 0) { fLv = x; fLv.SetMag(mag); }
    else { fLv = x.Unit(); }
}

//_____________________________________________________________________________
inline void SpdMCSiliconHit::SetUdir(TVector3 x) 
{
    Double_t mag = fLu.Mag();
    fLu = x.Unit();
    fLu.SetMag(mag);
}

//_____________________________________________________________________________
inline void SpdMCSiliconHit::SetVdir(TVector3 x) 
{
    Double_t mag = fLv.Mag();
    fLv = x.Unit();
    fLv.SetMag(mag);
}

#endif  /* __SPDMCSILICONHIT_H__ */

