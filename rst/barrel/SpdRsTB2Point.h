// $Id$
// Author: artur   2018/02/01
//
// added new variables for points 

#ifndef __SPDRSTB2POINT_H__
#define __SPDRSTB2POINT_H__ 

#include "FairMCPoint.h"
#include <TString.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTB2Point2                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsTB2Point : public FairMCPoint {

public:
  
   /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param hpath    Hit node full geopath 
   *@param pos      Coordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param posOut   Coordinates at exit to active volume [cm]
   *@param momOut   Momentum of track at exit [GeV]   
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param ELoss    Energy deposit [GeV]
   **/
  
  SpdRsTB2Point();
  
  SpdRsTB2Point(Int_t trackID, Int_t detID, 
                 TVector3 pos, TVector3 mom, 
                 Double_t tof, Double_t length, Double_t ELoss);
  
  SpdRsTB2Point(Int_t trackID, Int_t detID,  TString hpath,
                 TVector3 pos, TVector3 mom, 
                 Double_t tof, Double_t length, Double_t ELoss);

  SpdRsTB2Point(Int_t trackID, Int_t detID,  TString hpath,
                 TVector3 pos, TVector3 mom, TVector3 posOut, TVector3 momOut,
                 Double_t tof, Double_t tofOut,
                 Double_t length, Double_t segmentlength, Double_t ELoss);

  SpdRsTB2Point(const SpdRsTB2Point& point) { *this = point; }

  virtual ~SpdRsTB2Point();

  inline void  SetHitGeoPath(TString hpath) { fHitGeoPath = hpath; }
   
  /** Accessors **/
  
  inline TVector3 GetPosIn()         const { return TVector3(fX, fY, fZ);    }
  inline TVector3 GetMomIn()         const { return TVector3(fPx, fPy, fPz); }
  inline TVector3 GetPosOut()        const { return fPosOut;                 }
  inline TVector3 GetMomOut()        const { return fMomOut;                 }
  inline Double_t GetSegmentLength() const { return fSegmentLength;          }
  inline Double_t GetTimeIn()        const { return fTime;                   }
  inline Double_t GetTimeOut()       const { return fTimeOut;                }

  inline TString GetHitGeoPath() const { return fHitGeoPath;  }

  virtual void Print(const Option_t* opt) const;

protected:

  TVector3  fPosOut;
  TVector3  fMomOut;
  Double_t  fTimeOut;          // Time since event start at exit of detector [ns]
  Double_t  fSegmentLength;    // Trajectory segment length inside the detector, [cm]
  
private:

  TString  fHitGeoPath;

  ClassDef(SpdRsTB2Point,1)

};

#endif  /* __SPDRSTB2POINT_H__ */
