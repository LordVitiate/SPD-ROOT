// $Id$
// Author: artur   2018/02/01

#ifndef __SPDRSSBHIT_H__
#define __SPDRSSBHIT_H__

#include "FairHit.h"

#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsSBHit                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsSBHit : public FairHit {

public:

    SpdRsSBHit(Int_t uid = -1);
    virtual ~SpdRsSBHit();
    
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
   
    ClassDef(SpdRsSBHit,1)
};

#endif  /* __SPDRSSBHIT_H__ */

