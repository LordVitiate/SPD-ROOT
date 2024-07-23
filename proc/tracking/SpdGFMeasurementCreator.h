// $Id$
// Author: artur   2019/09/09

#ifndef __SPDGFMEASUREMENTCREATOR_H__
#define __SPDGFMEASUREMENTCREATOR_H__

#include <TObject.h>
#include <TVector3.h>

#include "AbsMeasurement.h"
#include "SpdGFBase.h"
#include "SpdIdealSpacepointHit.h"
#include "SpdIdealWirepointHit.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGFMeasurementCreator                                                    //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
        
class SpdGFMeasurementCreator: public TObject {

public:

    SpdGFMeasurementCreator();
    virtual ~SpdGFMeasurementCreator();
    
    std::vector<genfit::AbsMeasurement*> CreateHit(SpdIdealHit* hit, GFMeasurementType mtype = pkSpacepoint);
    std::vector<genfit::AbsMeasurement*> CreateHit(SpdIdealSpacepointHit* hit, GFMeasurementType mtype = pkSpacepoint);
    std::vector<genfit::AbsMeasurement*> CreateHit(SpdIdealWirepointHit* hit, GFMeasurementType mtype = pkSpacepoint);
    
    inline void SetPointHitResolutionXYZ(Double_t rx /*cm*/, Double_t ry /*cm*/, Double_t rz /*cm*/);
    inline void SetWireHitResolutionRZ(Double_t rr /*cm*/, Double_t rz = -1/*cm*/);
    
    inline void ForcePointHitResolution() { fForcePointHitResolution = true; }
    inline void ForceWireHitResolution()  { fForceWireHitResolution = true;  }
    
    inline void UseMCPointAsPointHit(Bool_t smear = false) { fUseMCPointAsPointHit = true; fSmearPointHitResolution = smear; }
    inline void UseMCPointAsWireHit(Bool_t smear = false)  { fUseMCPointAsWireHit = true; fSmearWireHitResolution = smear;  }
    
    void ResetOptions(); 

private:

    std::vector<genfit::AbsMeasurement*> CreateSpacepointHit(TVector3 pos, TVector3 res, Char_t spec = 'n');
    
    std::vector<genfit::AbsMeasurement*> CreateSpacepointHitP(SpdIdealSpacepointHit* hit);
    std::vector<genfit::AbsMeasurement*> CreateSpacepointHitW(SpdIdealWirepointHit* hit);
    
    std::vector<genfit::AbsMeasurement*> CreatePixelHit(SpdIdealSpacepointHit* hit);
    std::vector<genfit::AbsMeasurement*> CreateWireHit(SpdIdealWirepointHit* hit);
    std::vector<genfit::AbsMeasurement*> CreateWirePointHit(SpdIdealWirepointHit* hit);
  
    //std::vector<genfit::AbsMeasurement*> CreateWireHit2(SpdIdealWirepointHit* hit);
    
    Int_t    fMeasurementCounter_;
    
    TVector3 fPointHitResolution;
    TVector3 fWireHitResolution;
    
    Bool_t   fForcePointHitResolution;
    Bool_t   fForceWireHitResolution;
    
    Bool_t   fUseMCPointAsPointHit;
    Bool_t   fUseMCPointAsWireHit;
    
    Bool_t   fSmearPointHitResolution;
    Bool_t   fSmearWireHitResolution;
    
    ClassDef(SpdGFMeasurementCreator,1)
};

void SpdGFMeasurementCreator::SetPointHitResolutionXYZ(Double_t rx, Double_t ry, Double_t rz) 
{
    // set resolution for x(u), y(v), z: pkPixel -> (x=u,y=v); pkSpacepoint -> (x,y,z)
    // for example r(x,y,z) = 0.005 = 50*1e-4 = 50 mkm 
    
    fPointHitResolution.SetXYZ(rx,ry,rz);
}

void SpdGFMeasurementCreator::SetWireHitResolutionRZ(Double_t rr, Double_t rz) 
{
    // set resolution for r,z: pkWire -> (r); pkWirePoint -> (r,z) 
    // for example r(x,y,z) = 0.005 = 50*1e-4 = 50 mkm 
    
    if (rz < 0) rz = rr;
    fWireHitResolution.SetXYZ(rr,rz,0.);
}

#endif  /* __SPDGFMEASUREMENTCREATOR_H__ */

