// $Id$
// Author: artur   2021/09/20

#ifndef __SPDHITMCTRUTH_H__
#define __SPDHITMCTRUTH_H__

#include <TObject.h> 
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdHitMCTruth                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdHitMCTruth: public TObject {
    
public:

    SpdHitMCTruth() {}
    virtual ~SpdHitMCTruth() {}
    
    virtual void clear() { fTruth.clear(); }
    virtual void print() const;
   
    virtual Int_t Add(Int_t MotherId);                   // return particle local number
    virtual Int_t Add(Int_t MotherId, Bool_t& is_new);   // return particle local number
    
    Int_t  GetN()                  const { return fTruth.size()/2; }
    Int_t  GetMother(Int_t i = 0)  const { return (i < fTruth.size()/2) ? fTruth[2*i]   : -1; } 
    Int_t  GetEntries(Int_t i)     const { return (i < fTruth.size()/2) ? fTruth[2*i+1] :  0; }
    Int_t  GetEntries()            const;
    
protected:
    
    std::vector<Int_t> fTruth;
    
    ClassDef(SpdHitMCTruth,1)
};


#endif  /* __SPDHITMCTRUTH_H__ */

