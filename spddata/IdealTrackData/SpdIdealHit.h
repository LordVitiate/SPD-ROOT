// $Id$
// Author: artur   2019/09/06

#ifndef __SPDIDEALHIT_H__
#define __SPDIDEALHIT_H__

#include <TVector3.h>
#include <vector>

#include "FairTimeStamp.h"
#include "SpdDetectorList.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdIdealHit                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

enum SpdHitType 
{ 
    kUndefined = -1,
    kSpacepoint,   // 0
    kWirepoint,    // 1      
    kNHitTypes
};

class SpdIdealHit: public FairTimeStamp {

public:

    SpdIdealHit();
    virtual ~SpdIdealHit();

    void Clear();

    // unable/disable methods (set/get)
    virtual void   DisableHit(Bool_t disable) { fSpecifity = 'd'; }
    virtual Bool_t IsHitDisabled() { return (fSpecifity == 'd'); }
    
    /* setters */
    
    virtual void  SetDetId(Int_t i, Long_t id);
    virtual void  SetSpecifity(Char_t spec) { fSpecifity = spec; }
    
    void  SetEdep(Double_t edep)     { fEdep = edep; }
    void  SetId(Int_t id)            { fId = id;     } 
    void  SetInfoNum(Int_t n)        { fInfoNum = n; }
    void  SetMotherPointNum(Int_t n) { fMotherPointNum = n; }
    
    void  AddTrackId(Int_t id) { fTrackId.push_back(id); }
    
    virtual void SetPoint(Double_t x, Double_t y, Double_t z) { fPoint.SetXYZ(x,y,z); }
    virtual void SetPoint(TVector3 point) { fPoint = point; }
    
    virtual void SetHit(Double_t /*x*/, Double_t /*y*/, Double_t /*z*/) { }
    virtual void SetHit(TVector3 /*hit*/) { }

    virtual void SetResolution(Double_t r1, Double_t r2, Double_t r3) { fResolution.SetXYZ(r1,r2,r3); }
    virtual void SetResolution(TVector3 res) { fResolution = res; }
    
    /* getters */
    
    virtual Int_t  GetDetId() const { return Int_t(fDetId[0]); }
    virtual Long_t GetDetId(Int_t i) const;
    virtual Char_t GetSpecifity() const { return fSpecifity; }
     
    Double_t  GetEdep()           const { return fEdep; }
    Int_t     GetId()             const { return fId; }
    Int_t     GetInfoNum()        const { return fInfoNum; }
    Int_t     GetMotherPointNum() const { return fMotherPointNum; }
   
    virtual Bool_t    GetPoint(TVector3& point) const { point = fPoint;  return true; }
    virtual TVector3  GetPoint()   const { return fPoint; }
    
    Int_t  GetNTrakcs() const { return fTrackId.size(); }
    Int_t  GetTrackId(Int_t i = 0) const { return (fTrackId.empty()) ? -1 : fTrackId[i]; }
    
    Double_t GetPointX() const { return fPoint.X(); }
    Double_t GetPointY() const { return fPoint.Y(); }
    Double_t GetPointZ() const { return fPoint.Z(); }
   
    virtual Bool_t    GetHit(TVector3& /*hit*/) const { return false; }
    
    virtual TVector3  GetResolution()  const { return fResolution; }
    
    virtual Double_t  GetResolution1() const { return fResolution.X(); }
    virtual Double_t  GetResolution2() const { return fResolution.Y(); }
    virtual Double_t  GetResolution3() const { return fResolution.Z(); }
    
    virtual void PrintHit(Bool_t print_point = true, Bool_t print_res = false) const;
    
protected:
  
    std::vector<Long_t> fDetId;  // mc-digit: geo. module id, detector id, [subdectector id, submodule id, ... ]

    Double_t   fEdep;            // energy deposit, GeV
    
    Int_t      fId;              // hit unique id
    Int_t      fInfoNum;         // SpdIdealDataInfo number (see SpdIdealTrackColl or SpdIEvent)
    Int_t      fMotherPointNum;  // mother mc-point number
    
    std::vector<Int_t> fTrackId; // tracks (id's) to which the hit belong  
  
    // Additional parameter "specifity", for example: 
    // 'f'(failed), 'd'(disabled), 'n'(undefined)
    // spacepoint-like: 'p'(simple point),'m'(MAPS),'s'(DSSD), ...
    //  wirepoint-like: 'p'(simple point),'w'(wire)
    // ...
    Char_t     fSpecifity; 
    
    TVector3   fPoint;       // mc-point coordinates
    TVector3   fResolution;  // hit resolution
    
    ClassDef(SpdIdealHit,1)
};

#endif  /* __SPDIDEALHIT_H__ */

