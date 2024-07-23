// $Id$
// Author: artur   2018/02/01

#ifndef __SPDRSTECHIT_H__
#define __SPDRSTECHIT_H__

#include "FairHit.h"

#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTECHit                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsTECHit : public FairHit {

public:

    SpdRsTECHit(Int_t uid = -1);
    virtual ~SpdRsTECHit();
    
    void AddDeposit(Float_t dep) { fDeposit += dep; } 
    void AddPoint(Int_t num);
    
    void SetUid(Int_t id)      { fDetectorID = id; }
    void SetFirstID(Int_t id)  { fFirstID  = id;   } 
    void SetSecondID(Int_t id) { fSecondID = id;   } 
    
    Int_t    GetUid()      const { return fDetectorID; }
    Int_t    GetFirstID()  const { return fFirstID;    }
    Int_t    GetSecondID() const { return fSecondID;   }
    
    Float_t  GetDeposit()  const { return fDeposit; }
    Int_t    GetNPoints()  const { return fPoints.size(); }
  
    const std::vector<Int_t>& GetPoints() { return fPoints; }
    
    void Print(const Option_t* opt = 0) const;
    
private:
 
    Int_t         fFirstID;
    Int_t         fSecondID;
    
    std::vector<Int_t> fPoints;  // MC-points (numbers), which have a contribution to this hit
    Float_t            fDeposit; // total deposit    
   
    ClassDef(SpdRsTECHit,1)
};

#endif  /* __SPDRSTECHIT_H__ */

