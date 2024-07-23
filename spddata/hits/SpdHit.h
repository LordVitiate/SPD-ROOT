// $Id$
// Author: artur   2020/09/20

#ifndef __SPDHIT_H__
#define __SPDHIT_H__

#include "FairTimeStamp.h" 

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdHit                                                                     //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdHitMCTruth;

class SpdHit: public FairTimeStamp {

public:

    SpdHit():fId(-1),fModId(-1),fWeight(1) {}
    virtual ~SpdHit() {}

    virtual void Reset();
    
    virtual void SetId(Int_t id)     { fId  = id;    }
    virtual void SetModId(Int_t id)  { fModId = id;  }
    virtual void SetWeight(Int_t w)  { fWeight = w;  }

    Int_t GetId()     const { return fId;     }
    Int_t GetModId()  const { return fModId;  }
    Int_t GetWeight() const { return fWeight; }
    
    //-----------------------------------------
    // Inherited methods (from FairTimeStamp):
    // 
    // void  SetTimeStamp(Double_t t);
    // void  SetTimeStampError(Double_t t);
    //
    // Double_t  GetTimeStamp()       const;
    // Double_t  GetTimeStampError()  const;
    //-----------------------------------------
    
    void     SetHitTime(Double_t t)       { SetTimeStamp(t);       } // ns !
    void     SetHitTimeError(Double_t te) { SetTimeStampError(te); } // ns !
    
    Double_t GetHitTime()      const { return GetTimeStamp();      } // ns !
    Double_t GetHitTimeError() const { return GetTimeStampError(); } // ns !
    
    /* MC data info */
    virtual SpdHitMCTruth* GetMCTruth() const { return 0; }
    
    /* Print */
    virtual void Print(Option_t* option = "") const;
    
protected:
    
    Int_t  fId;      // hit own unique ID (index in the corresponding array) 
    Int_t  fModId;   // hit supermodule (detector) ID 
    Int_t  fWeight;  // weight/status code (for example, "mixed"(2),"signal"(1),"disable"(0),"background"(-1),"fake"(-2),...) 
    
    ClassDef(SpdHit,1)
};

#endif  /* __SPDHIT_H__ */

