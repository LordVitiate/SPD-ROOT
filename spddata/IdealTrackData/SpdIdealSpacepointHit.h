// $Id$
// Author: artur   2019/09/06

#ifndef __SPDIDEALSPACEPOINTHIT_H__
#define __SPDIDEALSPACEPOINTHIT_H__

#include "SpdIdealHit.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdIdealSpacepointHit                                                      //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdIdealSpacepointHit: public SpdIdealHit {

public:

    SpdIdealSpacepointHit();
    virtual ~SpdIdealSpacepointHit() {}
    
    /* ========== SETTERS ========== */
    
    // hit
    virtual void SetHit(Double_t x, Double_t y, Double_t z) { fHit.SetXYZ(x,y,z); }
    virtual void SetHit(TVector3 hit) { fHit = hit; }

    void SetResolutionUVN(Double_t ru, Double_t rv, Double_t rn) { fResolution.SetXYZ(ru,rv,rn); }
    void SetResolutionUV(Double_t ru, Double_t rv) { fResolution.SetXYZ(ru,rv,-1); }
    void SetResolutionU(Double_t ru)  { fResolution.SetX(ru); }
    void SetResolutionV(Double_t rv)  { fResolution.SetY(rv); }
    void SetResolutionN(Double_t rn)  { fResolution.SetZ(rn); }
       
    // detector   
    void SetU(TVector3 x, Double_t mag = -1);
    void SetV(TVector3 x, Double_t mag = -1);
    
    void SetUdir(TVector3 x);
    void SetVdir(TVector3 x);
    
    void SetL(Double_t lu, Double_t lv, Double_t ln) { fLu.SetMag(lu); fLv.SetMag(lv); fLn = ln; }
    void SetLu(Double_t l) { fLu.SetMag(l); }
    void SetLv(Double_t l) { fLv.SetMag(l); }
    void SetLn(Double_t l) { fLn = l; }
    
    void SetDetPos(Double_t x, Double_t y, Double_t z) { fDetPos.SetXYZ(x,y,z); }
    void SetDetPos(TVector3 pos) { fDetPos = pos; }
    
    /* ========== GETTERS ========== */
    
    Double_t GetResolutionU() const { return fResolution.X(); }
    Double_t GetResolutionV() const { return fResolution.Y(); }
    Double_t GetResolutionN() const { return fResolution.Z(); }   
    
    // hit
    virtual Bool_t   GetHit(TVector3& hit) const { hit = fHit; return true; }
            TVector3 GetHit() const { return fHit; }
                
    Double_t GetHitU() const;  // u coordinate (local CS)
    Double_t GetHitV() const;  // v coordinate (local CS)
    Double_t GetHitN() const;  // n coordinate (local CS)
    
    void GetHitUV(Double_t& xu, Double_t& xv) const;
    void GetHitUVN(Double_t& xu, Double_t& xv, Double_t& xn) const;
    
    // point
    Double_t GetPointU() const;  // u coordinate (local СS)
    Double_t GetPointV() const;  // v coordinate (local СS)
    Double_t GetPointN() const;  // n coordinate (local СS)
    
    void GetPointUV(Double_t& xu, Double_t& xv) const;
    void GetPointUVN(Double_t& xu, Double_t& xv, Double_t& xn) const;
    
    // detector  
    TVector3  GetU(Bool_t unit = kTRUE) const { return (unit) ? fLu.Unit() : fLu; }
    TVector3  GetV(Bool_t unit = kTRUE) const { return (unit) ? fLv.Unit() : fLv; }
    TVector3  GetN(Bool_t unit = kTRUE) const;
    
    Double_t  GetLu() const { return fLu.Mag(); }
    Double_t  GetLv() const { return fLv.Mag(); }
    Double_t  GetLn() const { return fLn; }
    
    TVector3  GetDetPos() const { return fDetPos; }
    
    Double_t  GetDetPosX() const { return fDetPos.X(); }
    Double_t  GetDetPosY() const { return fDetPos.Y(); }
    Double_t  GetDetPosZ() const { return fDetPos.Z(); }
        
    virtual void PrintHit(Bool_t print_point = true, Bool_t print_res = true) const;

private:

    // hit
    TVector3 fHit;    // hit coordinates (global S.C.)
    
    // detector 
    Double_t fLn;     // n = [u x v] 
    TVector3 fLu;     // u -direction
    TVector3 fLv;     // v- direction
    TVector3 fDetPos; // detector position
    
    ClassDef(SpdIdealSpacepointHit,1)
};

//_____________________________________________________________________________
inline void SpdIdealSpacepointHit::SetU(TVector3 x, Double_t mag) 
{   
    if (mag > 0) { fLu = x; fLu.SetMag(mag); }
    else { fLu = x.Unit(); }
}

//_____________________________________________________________________________
inline void SpdIdealSpacepointHit::SetV(TVector3 x, Double_t mag) 
{   
    if (mag > 0) { fLv = x; fLv.SetMag(mag); }
    else { fLv = x.Unit(); }
}

//_____________________________________________________________________________
inline void SpdIdealSpacepointHit::SetUdir(TVector3 x) 
{
    Double_t mag = fLu.Mag();
    fLu = x.Unit();
    fLu.SetMag(mag);
}

//_____________________________________________________________________________
inline void SpdIdealSpacepointHit::SetVdir(TVector3 x) 
{
    Double_t mag = fLv.Mag();
    fLv = x.Unit();
    fLv.SetMag(mag);
}


#endif  /* __SPDIDEALSPACEPOINTHIT_H__ */

