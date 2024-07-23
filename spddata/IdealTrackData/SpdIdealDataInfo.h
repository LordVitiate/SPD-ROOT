// $Id$
// Author: artur   2019/09/06

#ifndef __SPDIDEALDATAINFO_H__
#define __SPDIDEALDATAINFO_H__

#include <TNamed.h>
#include <vector>

#include "SpdDetectorList.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdIdealDataInfo                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdIdealDataInfo: public TNamed {

public:

    SpdIdealDataInfo();
    SpdIdealDataInfo(const Char_t* DataBranch, const Char_t* ParsType);
    SpdIdealDataInfo(const Char_t* DataBranch, const Char_t* ParsType, Int_t DetId);
    
    virtual ~SpdIdealDataInfo() {}
    
    virtual void ClearData() { fHits.clear(); } 
    
    void    SetHitType(Int_t htype) { fHitType = htype; }
    void    SetDetectorId(Int_t id) { fDetId = id; }
    void    SetNTracks(Int_t n)     { fHits.assign(n,0); }
    
    void    SetTrackHits(Int_t ntrack, Int_t nhits);
    void    AddTrackHits(Int_t ntrack, Int_t nhits = 1);
    
    Int_t   GetHitType()     const { return fHitType; }
    Int_t   GetDetectorId()  const { return fDetId; }
    Int_t   GetNTracks()     const { return fHits.size(); }
    
    Int_t   GetTrackHits(Int_t ntrack) const;
    
    Int_t   GetNHits() const;
    const std::vector<Int_t>& GetHits() const { return fHits; }
    
    TString GetDataBranch() const { return fName;  }
    TString GetParsType()   const { return fTitle; }
    
    virtual void PrintInfo(TString opt = "") const;

private:
    
    Int_t fDetId;    // geo. module id (see SpdDetectorList)
    Int_t fHitType;  // hit type (SpdHitType) for the geo. module (see SpdIdealHit)
    
    std::vector<Int_t> fHits;  // Number of hits in the geo. module [track number in SpdIdealTrackColl]
    
    ClassDef(SpdIdealDataInfo,1)
};

//_____________________________________________________________________________
inline void SpdIdealDataInfo::SetTrackHits(Int_t ntrack, Int_t nhits)
{
    if (ntrack < 0 || ntrack >= fHits.size()) return;
    fHits[ntrack] = nhits;
}

//_____________________________________________________________________________
inline void SpdIdealDataInfo::AddTrackHits(Int_t ntrack, Int_t nhits)
{
    if (ntrack < 0 || ntrack >= fHits.size()) return;
    fHits[ntrack] += nhits;
}
    
//_____________________________________________________________________________
inline Int_t SpdIdealDataInfo::GetTrackHits(Int_t ntrack) const
{
    if (ntrack < 0 || ntrack >= fHits.size()) return 0;
    return fHits[ntrack];
}

#endif  /* __SPDIDEALDATAINFO_H__ */

