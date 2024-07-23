// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTBPOINT_H__
#define __SPDECALTBPOINT_H__ 

#include "FairMCPoint.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTBPoint                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTBPoint : public FairMCPoint {

public:
  
   /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param modID    Module ID 
   *@param submodID SubModule ID 
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param ELoss    Energy deposit [GeV]
   **/
  
  SpdEcalTBPoint();
  
  SpdEcalTBPoint(Int_t trackID, Int_t detID, 
                 TVector3 pos, TVector3 mom, 
                 Double_t tof, Double_t length, Double_t ELoss);
  
  SpdEcalTBPoint(Int_t trackID, Int_t detID, Int_t modID,
                 TVector3 pos, TVector3 mom, 
                 Double_t tof, Double_t length, Double_t ELoss);
  
  SpdEcalTBPoint(Int_t trackID, Int_t detID, Int_t modID, Int_t submodID,
                 TVector3 pos, TVector3 mom, 
                 Double_t tof, Double_t length, Double_t ELoss);

  SpdEcalTBPoint(const SpdEcalTBPoint& point) { *this = point; };

  virtual ~SpdEcalTBPoint();
  
  inline void SetModuleID(Int_t id)    { fModuleID = id;    }
  inline void SetSubModuleID(Int_t id) { fSubModuleID = id; }
  
  inline Int_t GetModuleID()    const  { return fModuleID;    }
  inline Int_t GetSubModuleID() const  { return fSubModuleID; }

  virtual void Print(const Option_t* opt) const;
  
private:

  Int_t fModuleID; 
  Int_t fSubModuleID; 

  ClassDef(SpdEcalTBPoint,1)

}; /* __SPDECALTBPOINT_H__ */

#endif
