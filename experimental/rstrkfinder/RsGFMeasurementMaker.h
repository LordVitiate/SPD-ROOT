#ifndef RsGFMeasurementMaker_h
#define RsGFMeasurementMaker_h

#include <TObject.h>
#include <TVector3.h>

#include "AbsMeasurement.h"
#include "SpdHit.h"
#include "SpdRsMCHit.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// RsGFMeasurementMaker                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class RsGFMeasurementMaker: public TObject {

public:

    typedef std::vector<genfit::AbsMeasurement*> gf_measurements_;
    
    RsGFMeasurementMaker();
    
    void  ResetCounter() { fLastHitId = -1; }
    gf_measurements_ ProcessHit(const SpdRsMCHit* hit);
    
    Int_t GetLastHitId() const { return fLastHitId;  }
    
private:

    Int_t   fLastHitId;
 
    ClassDef(RsGFMeasurementMaker,1)
};

#endif 

