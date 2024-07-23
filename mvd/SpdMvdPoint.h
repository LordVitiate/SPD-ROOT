// $Id$
// Author: artur   2021/11/12

#ifndef __SPDMVDPOINT_H__
#define __SPDMVDPOINT_H__ 

#include "FairMCPoint.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMvdPoint                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMvdPoint : public FairMCPoint {

public:
  
    /** Constructor with arguments
   *@param trackID   Index of MCTrack
   *@param detID     Detector ID
   *@param subdetID  Subdetector (barrel, endcap) ID 
   *@param layerID   Layer ID
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
    
  SpdMvdPoint();
  
  SpdMvdPoint(Int_t trackID, Int_t detID, Int_t subdetID, Int_t layerID,
              TVector3 pos, TVector3 mom, TVector3 posOut, TVector3 momOut,
              Double_t tof, Double_t tofOut,
              Double_t length, Double_t segmentlength, Double_t ELoss);

  SpdMvdPoint(const SpdMvdPoint& point) { *this = point; }

  virtual ~SpdMvdPoint();
  
  inline Int_t    GetSubdetId()      const { return fSubdetID; }
  inline Int_t    GetLayerId()       const { return fLayerID;  }
  
  inline TVector3 GetPosIn()         const { return TVector3(fX,fY,fZ);    } // cm
  inline TVector3 GetMomIn()         const { return TVector3(fPx,fPy,fPz); } // GeV/c
  inline TVector3 GetPosOut()        const { return fPosOut;               } // cm
  inline TVector3 GetMomOut()        const { return fMomOut;               } // GeV/c
  inline Double_t GetSegmentLength() const { return fSegmentLength;        } // cm
  inline Double_t GetTimeIn()        const { return fTime;                 } // ns
  inline Double_t GetTimeOut()       const { return fTimeOut;              } // ns
  
  virtual void GetPosTime(TVector3& pos, Double_t& t) const;

  virtual Double_t GetPosInPerp()  const { return GetPosIn().Perp(); }
  virtual Double_t GetPosOutPerp() const { return fPosOut.Perp();    }

  virtual void Print(const Option_t* opt) const;
  
private:
    
  Int_t     fSubdetID;         // Point subdetector number (0: barrel; -1: endcapZ-; +1 endcapZ+)     
  Int_t     fLayerID;          // Point layer number   

  TVector3  fPosOut;           // Track coordinates at exit to active volume, [cm]
  TVector3  fMomOut;           // Track coordinates at exit to active volume, [cm]
  Double_t  fTimeOut;          // Time since event start at exit of detector, [ns]
  Double_t  fSegmentLength;    // Trajectory segment length inside the detector, [cm]
  
  ClassDef(SpdMvdPoint,1)
  
};

#endif  /* __SPDMVDPOINT_H__ */
