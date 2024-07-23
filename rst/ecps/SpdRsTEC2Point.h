// $Id$
// Author: alver   2020/07/16

#ifndef __SPDRSTEC2POINT_H__
#define __SPDRSTEC2POINT_H__ 

#include "FairMCPoint.h"
#include <TString.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTEC2Point                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsTEC2Point : public FairMCPoint {

public:
  
   /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param modID    Module ID 
   *@param submodID SubModule ID 
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param posOut   Coordinates at exit to active volume [cm]
   *@param momOut   Momentum of track at exit [GeV]   
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param ELoss    Energy deposit [GeV]
   **/
  
  SpdRsTEC2Point();
  
  SpdRsTEC2Point(Int_t trackID, Int_t detID, 
                 TVector3 pos, TVector3 mom, 
                 Double_t tof, Double_t length, Double_t ELoss);
  
  SpdRsTEC2Point(Int_t trackID, Int_t detID, TString hpath,
                 TVector3 pos, TVector3 mom, 
                 Double_t tof, Double_t length, Double_t ELoss);

  SpdRsTEC2Point(Int_t trackID, Int_t detID,  TString hpath,
                 TVector3 pos, TVector3 mom, TVector3 posOut, TVector3 momOut,
                 Double_t tof, Double_t tofOut, 
                 Double_t length, Double_t segmentlength, Double_t ELoss);
  
  SpdRsTEC2Point(const SpdRsTEC2Point& point) { *this = point; };

  virtual ~SpdRsTEC2Point();
  
  inline void SetHitGeoPath(TString hpath) { fHitGeoPath = hpath; }

  /** Accessors **/
    
  inline TVector3 GetPosIn()         const { return TVector3(fX, fY, fZ);    }
  inline TVector3 GetMomIn()         const { return TVector3(fPx, fPy, fPz); }
  inline TVector3 GetPosOut()        const { return fPosOut;                 }
  inline TVector3 GetMomOut()        const { return fMomOut;                 }
  inline Double_t GetSegmentLength() const { return fSegmentLength;          }
  inline Double_t GetTimeIn()        const { return fTime;                   }   
  inline Double_t GetTimeOut()       const { return fTimeOut;                } 
  
  inline TString GetHitGeoPath()     const { return fHitGeoPath;  }

  virtual void Print(const Option_t* opt) const;
  
protected:

  TVector3      fPosOut;
  TVector3      fMomOut;
  Double32_t    fTimeOut;          // Time since event start at exit of detector [ns]
  Double_t      fSegmentLength;    // Trajectory segment length inside the detector, [cm]

private:

  TString  fHitGeoPath;

  ClassDef(SpdRsTEC2Point,1)

}; /* __SPDRSTEC2POINT_H__ */

#endif
