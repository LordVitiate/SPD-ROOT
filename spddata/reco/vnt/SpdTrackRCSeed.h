// $Id$
// Author: vladimir   2021/11/28

#ifndef __SPDTRACKRCSEED_H__
#define __SPDTRACKRCSEED_H__

#include <TObject.h>
#include <TVector3.h>
#include <set>

#include "SpdTrackRCSeedPar.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTrackRCSeed                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCSiliconHit;

typedef std::pair<Int_t,Int_t> spd_its_point;

class SpdTrackRCSeed: public TObject {

public:
    
    SpdTrackRCSeed();
    virtual ~SpdTrackRCSeed();
    
    virtual void DeleteSeedPars();
    virtual void Clear(Option_t* option = "");

    //------------ Setters ------------
    void  SetId(Int_t id)  { fId = id; }
    void  AddHit(spd_its_point hit)  { fHits.push_back(hit); } 

    // get (or create) track seed paramaters
    SpdTrackRCSeedPar* SeedPars();
    
    //------------ Getters ------------
    Int_t  GetId()             const { return fId; }

    const std::vector<spd_its_point>& GetSeedHits() const { return fHits; }

    // WARNING Slow method (complicity ~O(i))
//    Int_t  GetSeed(Int_t i) const;
    
    SpdTrackRCSeedPar* GetSeedPars() const { return fSeedPars; }
 
    virtual void Print(Option_t* option = "") const;
 
private:
    
    Int_t            fId;         // seed unique id (index in the corresponding array)

    std::vector<spd_its_point>  fHits;   //

    SpdTrackRCSeedPar* fSeedPars;   // seed parameters

    ClassDef(SpdTrackRCSeed,1)
};

#endif  /* __SPDTRACKRCSEED_H__ */

