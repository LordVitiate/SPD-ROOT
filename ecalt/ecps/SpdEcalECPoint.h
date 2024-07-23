// $Id$
// Author: andre   2020/07/31

#ifndef __SPDECALECECPOINT_H__
#define __SPDECALECECPOINT_H__ 

#include <TVector3.h>
#include "FairMCPoint.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalECGeoPoint                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalECPoint : public FairMCPoint {

public:
  
   /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param endcapID Endcap ID
   *@param moduleID Module ID 
   *@param cellID   Cell ID
   *@param layerID  Layer ID
   *@param matID    Node material 
   *@param pos      Coordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param ELoss    Energy deposit [GeV]
   *@param nodePath Full node geopath 
   **/
  
  SpdEcalECPoint();
  
  SpdEcalECPoint(Int_t trackID, Int_t detID, Int_t endcapID, Int_t moduleID, Int_t cellID, Int_t layerID, 
                   TString matID,
                   TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t ELoss, 
                   TString nodePath);

  SpdEcalECPoint(const SpdEcalECPoint& point) { *this = point; };

  virtual ~SpdEcalECPoint();
 
  // Int_t FairMCPoint::GetDetectorID() const;
  // Int_t FairMCPoint::GetTrackID() const;
  
  inline Int_t    GetEndcapID()     const { return fEndcapID;   }
  inline Int_t    GetModuleID()     const { return fModuleID;   }
  inline Int_t    GetCellID()       const { return fCellID;     }
  inline Int_t    GetLayerID()      const { return fLayerID;    }
  
  inline TString  GetMaterialID()   const { return fMatID;    }
  
  inline Double_t GetEloss()        const { return fELoss;    } //->FairMCPoint
  inline Double_t GetTime()         const { return fTime;     } //->FairMCPoint
  inline Double_t GetLength()       const { return fLength;   } //->FairMCPoint
  
  TVector3 GetPosition() const { return TVector3(fPx,fPy,fPz); }
  TVector3 GetMomentum() const { return TVector3(fX,fY,fZ);    }
  
  TString  GetNodePath() const { return fNodePath; }
  TString  GetNodePathToCell(TString & absVol, TString & scintVol);

  virtual void Print(const Option_t* opt) const;

  
private:
    
  Int_t      fEndcapID; 
  Int_t      fModuleID; 
  Int_t      fCellID;
  Int_t      fLayerID;

  TString    fMatID;
  TString    fNodePath;
  
  ClassDef(SpdEcalECPoint,1)

};

#endif /* __SPDECALECECPOINT_H__ */
