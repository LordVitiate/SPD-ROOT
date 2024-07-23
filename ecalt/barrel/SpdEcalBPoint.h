// $Id$
// Author: andre   2020/07/31

#ifndef __SPDECALBPOINT_H__
#define __SPDECALBPOINT_H__ 

#include <TVector3.h>
#include "FairMCPoint.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalBGeoPoint                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalBPoint : public FairMCPoint {

public:
  
   /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param cellID    Cell ID 
   *@param submodID SubModule ID 
   *@param pos      Coordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param ELoss    Energy deposit [GeV]
   *@param nodePath Node Path (TString)
   **/
  
  SpdEcalBPoint();
  
  SpdEcalBPoint(Int_t fTrackID, Int_t fDetID, 
                  Int_t fBasketID, Int_t fModuleZID, Int_t fModulePhiID, Int_t fCellID, Int_t fLayerID, 
                  TString fMatID,
                  TVector3 pos, TVector3 mom, 
                  Double_t tof, Double_t length, Double_t ELoss, 
                  TString nodePath);

  SpdEcalBPoint(const SpdEcalBPoint& point) { *this = point; };

  virtual ~SpdEcalBPoint();
  
  inline Int_t GetBasketID()        const { return fBasketID;    }
  inline Int_t GetModuleZID()       const { return fModuleZID;   }
  inline Int_t GetModulePhiID()     const { return fModulePhiID; }
  inline Int_t GetCellID()          const { return fCellID;      }
  inline Int_t GetLayerID()         const { return fLayerID;     }
  
  inline TString GetMaterialID()    const { return fMatID; }
  
  inline Double_t GetEloss()        const { return fELoss;  } //->FairMCPoint
  inline Double_t GetTime()         const { return fTime;   } //->FairMCPoint
  inline Double_t GetLength()       const { return fLength; } //->FairMCPoint
  
  TVector3 GetPosition()            const { return TVector3(fPx,fPy,fPz); }
  TVector3 GetMomentum()            const { return TVector3(fX,fY,fZ); }
  
  TString  GetNodePath()            const { return fNodePath; }
  TString  GetNodePathToCell(TString & absVol, TString & scintVol);

  virtual void Print(const Option_t* opt) const;

  
private:

  Int_t   fBasketID; 
  Int_t   fModuleZID; 
  Int_t   fModulePhiID; 
  Int_t   fCellID;
  Int_t   fLayerID;

  TString fMatID;
  
  TString fNodePath;

  ClassDef(SpdEcalBPoint,1)
};

#endif /* __SPDECALBPOINT_H__ */
