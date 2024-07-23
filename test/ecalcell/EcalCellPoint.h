
#ifndef __ECALCELLPOINT_H__
#define __ECALCELLPOINT_H__ 

#include <TVector3.h>
#include "FairMCPoint.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// EcalCellGeoPoint                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class EcalCellPoint : public FairMCPoint {

public:
  
    EcalCellPoint();
    
    /** 
     *@param trackID  Index of MCTrack
     *@param detID    Target ID
     *@param pos      Ccoordinates at entrance to active volume [cm]
     *@param mom      Momentum of track at entrance [GeV]
     *@param tof      Time since event start [ns]
     *@param length   Track length since creation [cm]
     *@param eLoss    Energy deposit [GeV]
     **/
    EcalCellPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
                  Double_t tof, Double_t length, Double_t eLoss);
    
    virtual ~EcalCellPoint() {}

    void    SetNode(TString node) { fNode = node; }
    void    SetPdg(Int_t pdg)     { fPdg = pdg;   }

    TString GetNode() const { return fNode; }
    Int_t   GetPdg()  const { return fPdg;  }
    
    virtual void Print(const Option_t* opt) const;

private:
    
    EcalCellPoint(const EcalCellPoint& point);
    EcalCellPoint operator=(const EcalCellPoint& point);

    TString fNode;
    Int_t   fPdg;
    
    ClassDef(EcalCellPoint,1)

};

#endif /* __ECALCELLPOINT_H__ */
