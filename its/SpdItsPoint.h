// $Id$
// Author: artur   2019/02/20

#ifndef __SPDITSPOINT_H__
#define __SPDITSPOINT_H__ 

#include "FairMCPoint.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsPoint                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdItsPoint : public FairMCPoint {

public:
 
     SpdItsPoint();
     
    /** 
     *@param trackID  Index of MCTrack
     *@param detID    Detector ID
     *@param pos      Coordinates at entrance [cm]
     *@param mom      Momentum of track at entrance [GeV/c]
     *@param tof      Time at entrance of detector [ns]
     *@param length   Track length since creation [cm]
     *@param eLoss    Energy deposit [GeV]
     **/
       
    SpdItsPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
                 Double_t tof, Double_t length, Double_t eLoss);
    
    virtual ~SpdItsPoint() {}
    
    void     ClearVData();

    void     SetNodeId(Int_t nid) { fNodeId  = nid; }
 
    void     SetPositionOut(Double_t x, Double_t y, Double_t z)    { fXOut  = x;  fYOut  = y;  fZOut  = z;  }
    void     SetMomentumOut(Double_t px, Double_t py, Double_t pz) { fPxOut = px; fPyOut = py; fPzOut = pz; }
    
    void     SetVData(const std::vector<Int_t>& vid, const std::vector<Double_t>& ved);
              
    Int_t    GetNodeId()   const { return fNodeId;  }
    
    Double_t GetXOut()     const { return fXOut;    }
    Double_t GetYOut()     const { return fYOut;    }
    Double_t GetZOut()     const { return fZOut;    } 
    Double_t GetPxOut()    const { return fPxOut;   }
    Double_t GetPyOut()    const { return fPyOut;   }
    Double_t GetPzOut()    const { return fPzOut;   }
    
    Int_t    GetVDataSize()  const { return fVid.size(); }
    
    std::vector<Int_t>    GetVid() const { return fVid; }
    std::vector<Double_t> GetVed() const { return fVed; }
    
    void GetVid(std::vector<Int_t>& vid)    const { vid = fVid; }
    void GetVed(std::vector<Double_t>& ved) const { ved = fVed; }
    
    Int_t    GetVid(Int_t i) const { return (i > -1 && i < Int_t(fVid.size())) ? fVid[i] : -1; }
    Double_t GetVed(Int_t i) const { return (i > -1 && i < Int_t(fVed.size())) ? fVed[i] :  0; }
 
    virtual void     GetPosTime(TVector3& pos, Double_t& t) const;
    virtual Double_t GetSegLength() const;
    
    virtual Bool_t   IsSpecialPoint(Int_t& vid, Double_t& dep) const;
    
    virtual void Print(const Option_t* opt) const;

    void PrintVData() const;

private:
    
    SpdItsPoint(const SpdItsPoint& point);
    SpdItsPoint operator=(const SpdItsPoint& point);

    Int_t    fNodeId;
    
    Double_t fXOut,  fYOut,  fZOut;  // cm
    Double_t fPxOut, fPyOut, fPzOut; // cm 
    
    std::vector<Int_t>    fVid;        // set of v-detectors
    std::vector<Double_t> fVed;        // set of v-deposits
     
    ClassDef(SpdItsPoint,1)

};

#endif  /* __SPDITSPOINT_H__ */
