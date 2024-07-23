// $Id$
// Author: artur   2018/02/01

#ifndef __SPDTSTBPOINT_H__
#define __SPDTSTBPOINT_H__ 

#include "FairMCPoint.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsTBPoint                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsTBPoint : public FairMCPoint {

public:
  
   /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param detTID   Detector Table ID 
   *@param posIn    Coordinates of track at entrance of detector [cm]
   *@param posOut   Coordinates of track at exit of detector  [cm]
   *@param momIn    Momentum of track at entrance of detector [GeV]
   *@param momOut   Momentum of track at exit of detector [GeV]
   *@param tofIn    Time since event start at exit of detector [ns]
   *@param tofOut   Time since event start at exit of detector [ns]
   *@param length   Track length since creation [cm]
   *@param ELoss    Energy deposit [GeV]
   **/
  
  SpdTsTBPoint();
  
  SpdTsTBPoint(Int_t trackID, Int_t detID, 
               TVector3 posIn, TVector3 momIn, Double_t tofIn, 
               Double_t length, Double_t ELoss);
  
  SpdTsTBPoint(Int_t trackID, Int_t detID, Long_t detTID,
               TVector3 posIn, TVector3 momIn, Double_t tofIn, 
               Double_t length, Double_t ELoss);
  
  SpdTsTBPoint(Int_t trackID, Int_t detID, 
               TVector3 posIn, TVector3 momIn, Double_t tofIn, 
               TVector3 posOut, TVector3 momOut, Double_t tofOut, 
               Double_t length, Double_t ELoss);
  
  SpdTsTBPoint(Int_t trackID, Int_t detID, Long_t detTID,
               TVector3 posIn, TVector3 momIn, Double_t tofIn, 
               TVector3 posOut, TVector3 momOut, Double_t tofOut, 
               Double_t length, Double_t ELoss);
  
  virtual ~SpdTsTBPoint();
 
  inline void SetDetectorTID(Long_t id) { fDetectorTID = id; }
   
  inline void SetXOut(Double_t x)   { fXOut = x; }
  inline void SetYOut(Double_t y)   { fYOut = y; }
  inline void SetZOut(Double_t z)   { fZOut = z; }
  
  inline void SetPxOut(Double_t px) { fPxOut = px; }
  inline void SetPyOut(Double_t py) { fPyOut = py; }
  inline void SetPzOut(Double_t pz) { fPzOut = pz; }
  
  inline void SetTimeOut(Double_t timeOut) { fTimeOut = timeOut; }
  inline void SetSegmentLength(Double_t l) { fSegmentLength = l; } 
  
  inline void SetPositionOut(Double_t x, Double_t y, Double_t z) 
              { fXOut = x; fYOut = y; fZOut = z; }
  inline void SetMomentumOut(Double_t px, Double_t py, Double_t pz)
              { fPxOut = px; fPyOut = py; fPzOut = pz; }
  
  inline void SetPositionOut(const TVector3& pos) 
              { fXOut = pos.X(); fYOut = pos.Y(); fZOut = pos.Z(); }
  inline void SetMomentumOut(const TVector3& mom)
              { fPxOut = mom.Px(); fPyOut = mom.Py(); fPzOut = mom.Pz(); } 

  inline Long_t GetDetectorTID() const { return fDetectorTID; }
  
  inline Double_t GetXIn()     const { return fX; }
  inline Double_t GetYIn()     const { return fY; }
  inline Double_t GetZIn()     const { return fZ; }
  inline Double_t GetXOut()    const { return fXOut; }
  inline Double_t GetYOut()    const { return fYOut; }
  inline Double_t GetZOut()    const { return fZOut; }

  inline Double_t GetPxIn()    const { return fPx; }
  inline Double_t GetPyIn()    const { return fPy; }
  inline Double_t GetPzIn()    const { return fPz; }
  inline Double_t GetPxOut()   const { return fPxOut; }
  inline Double_t GetPyOut()   const { return fPyOut; }
  inline Double_t GetPzOut()   const { return fPzOut; }
  
  inline Double_t GetTimeIn()  const { return fTime;    }
  inline Double_t GetTimeOut() const { return fTimeOut; }
  
  inline Double_t GetSegmentLength() const { return fSegmentLength; }
  
  inline void GetPositionIn(Double_t* pos)  const { pos[0] = fX; pos[1] = fY; pos[2] = fZ; }
  inline void GetPositionOut(Double_t* pos) const { pos[0] = fXOut; pos[1] = fYOut; pos[2] = fZOut; }
  
  inline void GetPositionIn(TVector3& pos)  const { pos.SetXYZ(fX, fY, fZ); }
  inline void GetPositionOut(TVector3& pos) const { pos.SetXYZ(fXOut, fYOut, fZOut); }
   
  inline void GetMomentumIn(TVector3& mom)  const { mom.SetXYZ(fPx, fPy, fPz); }
  inline void GetMomentumOut(TVector3& mom) const { mom.SetXYZ(fPxOut, fPyOut, fPzOut); }
  
  virtual Double_t GetPointDir(TVector3& dir) const;
  
  virtual void GetPosTime(TVector3& pos, Double_t& t) const;
  
  virtual void Print(const Option_t* opt) const;
  
protected:

  Long_t fDetectorTID; // detector table id
 
  Double32_t fXOut,  fYOut,  fZOut;     // Track position at exit of detector [cm] 
  Double32_t fPxOut, fPyOut, fPzOut;    // Momentum components at exit of detector [GeV]
  Double32_t fTimeOut;                  // Time since event start at exit of detector [ns]
  Double_t   fSegmentLength;            // Trajectory segment length inside the detector, [cm]

  ClassDef(SpdTsTBPoint,1)

}; /* __SPDTSTBPOINT_H__ */

#endif
