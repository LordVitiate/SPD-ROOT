
#ifndef __TRACKERPOINT_H__
#define __TRACKERPOINT_H__ 

#include <TVector3.h>
#include "FairMCPoint.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TrackerGeoPoint                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TrackerPoint : public FairMCPoint {

public:
 
     TrackerPoint();
     
    /** 
     *@param trackID  Index of MCTrack
     *@param detID    Target ID
     *@param pos      Coordinates at entrance  [cm]
     *@param mom      Momentum of track at entrance [GeV/c]
     *@param tof      Time at entrance of detector [ns]
     *@param length   Track length since creation [cm]
     *@param eLoss    Energy deposit [GeV]
     **/
       
    TrackerPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
                 Double_t tof, Double_t length, Double_t eLoss);
    
    virtual ~TrackerPoint() {}

    void     SetNode(TString node)        { fNode = node;        }  
    void     SetFirstNodeId(Int_t nid)    { fFirstNodeId  = nid; }
    void     SetSecondNodeId(Int_t nid)   { fSecondNodeId = nid; }
    
    void     SetSegmentLength(Double_t l) { fSegmentLength = l;  }   
    
    void     SetTimeOut(Double_t t)                                { fTimeOut = t; }
    void     SetPositionOut(Double_t x, Double_t y, Double_t z)    { fXOut  = x;  fYOut  = y;  fZOut  = z;  }
    void     SetMomentumOut(Double_t px, Double_t py, Double_t pz) { fPxOut = px; fPyOut = py; fPzOut = pz; }
              
    TString  GetNode()          const { return fNode;          }
    Int_t    GetFirstNodeId()   const { return fFirstNodeId;   }
    Int_t    GetSecondNodeId()  const { return fSecondNodeId;  }
    
    Double_t GetSegmentLength() const { return fSegmentLength; }
    
    Double_t GetTimeOut()       const { return fTimeOut; }
    Double_t GetXOut()          const { return fXOut;    }
    Double_t GetYOut()          const { return fYOut;    }
    Double_t GetZOut()          const { return fZOut;    } 
    Double_t GetPxOut()         const { return fPxOut;   }
    Double_t GetPyOut()         const { return fPyOut;   }
    Double_t GetPzOut()         const { return fPzOut;   }
 
    virtual void Print(const Option_t* opt) const;

private:
    
    TrackerPoint(const TrackerPoint& point);
    TrackerPoint operator=(const TrackerPoint& point);

    TString  fNode;
    Int_t    fFirstNodeId;
    Int_t    fSecondNodeId;
    Double_t fSegmentLength; // cm
    
    Double_t fXOut,  fYOut,  fZOut;  // cm
    Double_t fPxOut, fPyOut, fPzOut; // cm   
    Double_t fTimeOut;               // ns
     
    ClassDef(TrackerPoint,1)

};

#endif /* __TRACKERPOINT_H__ */
