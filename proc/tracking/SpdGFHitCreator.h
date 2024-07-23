// $Id$
// Author: artur   2021/02/08

#ifndef __SPDGFHITCREATOR_H__
#define __SPDGFHITCREATOR_H__

#include <TObject.h>
#include <TVector3.h>

#include "AbsMeasurement.h"
#include "SpdGFBase.h"
#include "SpdMCSiliconHit.h"
#include "SpdMCStrawHit1D.h"
#include "SpdMCStrawHit2D.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGFHitCreator                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdGFHitCreator: public TObject {

public:

    typedef std::vector<genfit::AbsMeasurement*> gf_measurements_;
    
    SpdGFHitCreator();
    virtual ~SpdGFHitCreator();
    
    void  ResetHitCounter() { fHitCounter = 0; fLastHitId = -1; }
    
    gf_measurements_ CreateSpacepointHit(const SpdHit* hit, TVector3 res);
    gf_measurements_ CreateHit(const SpdHit* hit);
    
    void CreatePlanarHit(const SpdMCSiliconHit* hit, gf_measurements_& meas);
    void CreateWireHit1D(const SpdMCStrawHit1D* hit, gf_measurements_& meas);
    void CreateWireHit2D(const SpdMCStrawHit2D* hit, gf_measurements_& meas);
    
    gf_measurements_ CreateSpacepointHit(const SpdMCTrackHit* hit, TVector3 res);
    gf_measurements_ CreateSpacepointHit(TVector3 point, TVector3 res);
    
    Int_t GetNHits()     const { return fHitCounter; }
    Int_t GetLastHitId() const { return fLastHitId;  }
    
    void  ClearCounter();
    void  PrintCounter();
    
private:

    // count hits: [0]=total, [1]=silicon, [2]=staraw1D, [3]=straw2D, [4]=pointlike
    Int_t   fMeasCounter[5];
    Int_t   fMeasCounterErrors[5];
 
    Int_t   fHitCounter;
    Int_t   fLastHitId;
 
    ClassDef(SpdGFHitCreator,1)
};

#endif  /* __SPDGFHITCREATOR_H__ */

