// $Id$
// Author: artur   2018/02/01

#ifndef __SPDRSTBHIT_H__
#define __SPDRSTBHIT_H__

#include "FairHit.h"

#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTBHit                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsTBHit : public FairHit {

public:

    SpdRsTBHit(Int_t uid = -1);
    virtual ~SpdRsTBHit();
    
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
   
    ClassDef(SpdRsTBHit,1)
};

#endif  /* __SPDRSTBHIT_H__ */

