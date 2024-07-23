// $Id$
// Author: alver   2020/07/19

#ifndef __SPDRSTEC2HIT_H__
#define __SPDRSTEC2HIT_H__

#include "SpdHit.h"
#include "SpdRsTID.h"
#include <vector>
#include <TVector3.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTEC2Hit                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRsTEC2Hit : public SpdHit {

public:

    SpdRsTEC2Hit();
    SpdRsTEC2Hit(Int_t detID, TVector3& pos, std::vector<Int_t> pointList);

    virtual ~SpdRsTEC2Hit();

    void     SetUid(Int_t detID) { fDetectorID = detID; } 

    Int_t    GetUid()      const { return fDetectorID; }    
    Int_t    GetNPoints()  const { return fPoints.size(); }
    TVector3 GetPosition() const { return fPos; } 
    
    const std::vector<Int_t>& GetPoints() const { return fPoints; }

    Int_t GetSector() const { return SpdRsTID::Sector(fDetectorID); }  // EC = 10 and 20
    Int_t GetLayer()  const { return SpdRsTID::Layer(fDetectorID);  }  // Layers = 1..20
    Int_t GetMDT()    const { return SpdRsTID::MDT(fDetectorID);    }  // MDT = 1..?
    Int_t GetCell()   const { return SpdRsTID::Cell(fDetectorID);   }  // Cell = 1..8

    void Print(const Option_t* opt = 0) const;
    
private:
 
    std::vector<Int_t> fPoints;     // MC-points (numbers), which have a contribution to this hit
    TVector3           fPos;        // Position in the frame 
    Int_t              fDetectorID;   
   
    ClassDef(SpdRsTEC2Hit,1)
};

#endif  /* __SPDRSTEC2HIT_H__ */

