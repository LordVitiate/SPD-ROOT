// $Id$
// Author: artur   2021/08/18

#ifndef __SPDTOFECPOINT_H__
#define __SPDTOFECPOINT_H__ 

#include "FairMCPoint.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTofECPoint                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTofECPoint : public FairMCPoint {

public:
  
    /** Constructor with arguments
   *@param trackID   Index of MCTrack
   *@param detID     Detector ID
   *@param pos       Coordinates at entrance to active volume [cm]
   *@param mom       Momentum of track at entrance [GeV]
   *@param posOut    Coordinates at exit to active volume [cm]
   *@param momOut    Momentum of track at exit [GeV]   
   *@param time      Time since event start [ns]
   *@param timeOut   Track time at exit to active volume [ns]
   *@param length    Track length since creation [cm]
   *@param seglength Track segment length in the volume [cm]
   *@param ELoss     Energy deposit [GeV]
   **/
    
  SpdTofECPoint();
  
  SpdTofECPoint(Int_t trackID, Int_t detID, Int_t modID,
               TVector3 pos, TVector3 mom, TVector3 posOut, TVector3 momOut,
               Double_t tof, Double_t tofOut,
               Double_t length, Double_t segmentlength, Double_t ELoss);

  SpdTofECPoint(const SpdTofECPoint& point) { *this = point; }
 
  virtual ~SpdTofECPoint();
  
  inline void     SetNodePath(TString path) { fNodePath = path; }
  
  inline TString  GetNodePath()       const { return fNodePath;             }
  inline Int_t    GetModuleId()       const { return fModuleId;             }  
  
  inline TVector3 GetPosIn()          const { return TVector3(fX,fY,fZ);    } // cm
  inline TVector3 GetMomIn()          const { return TVector3(fPx,fPy,fPz); } // GeV/c
  inline TVector3 GetPosOut()         const { return fPosOut;               } // cm
  inline TVector3 GetMomOut()         const { return fMomOut;               } // GeV/c
  inline Double_t GetSegmentLength()  const { return fSegmentLength;        } // cm
  inline Double_t GetTimeIn()         const { return fTime;                 } // ns
  inline Double_t GetTimeOut()        const { return fTimeOut;              } // ns

  virtual void Print(const Option_t* opt) const;
  
private:
    
  Int_t     fModuleId;         // -1 = endcap z-; +1 = endcap z+   

  TVector3  fPosOut;           // Track coordinates at exit to active volume, [cm]
  TVector3  fMomOut;           // Track coordinates at exit to active volume, [cm]
  Double_t  fTimeOut;          // Time since event start at exit of detector, [ns]
  Double_t  fSegmentLength;    // Trajectory segment length inside the detector, [cm]
  
  TString   fNodePath;         // Node full geopath
  
  ClassDef(SpdTofECPoint,1)
  
};

#endif  /* __SPDTOFECPOINT_H__ */
