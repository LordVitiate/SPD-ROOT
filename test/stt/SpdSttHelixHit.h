
#ifndef __SPDSTTHELIXHIT_H__
#define __SPDSTTHELIXHIT_H__ 

#include "TVector3.h"
#include "FairHit.h"

#include "TArrayI.h"

class SpdSttHit;

class SpdSttHelixHit : public TObject {

public:    

  SpdSttHelixHit();
 
  /** Constructor with arguments:
   * 
  *@param detID      Tube ID
  *@param tubeID     Tube sector ID
  *@param hitID      Array of corresponding hits
  *@param pos        Position coordinates of the tube [cm]
  *@param dpos       Error in position coordinates of the tube [cm]
  *@param radal      The radial measurement of deposit (drift radius)
  *@param radialErr  The error on on the radial measurement
  *@param Deposit    Deposit value
  **/
  
  SpdSttHelixHit(Int_t hitID, SpdSttHit* hit = 0, Bool_t addRadials = kFALSE);

  SpdSttHelixHit(Int_t detID, Int_t secID, Int_t hitID, 
                 TVector3& pos, TVector3& dpos,   
                 Double_t radial, Double_t radialErr,
                 Double_t deposit);

  virtual ~SpdSttHelixHit() {}
  
  virtual void Clear();
  virtual void ResetDeposit() { fDeposit = 0.; }
  virtual void ResetRadials() { fRadial = 0.; fRadialErr = 0.; }
   
  /* Get-methods */
  inline Int_t        HitID(Int_t i = 0) { return fHitsID.At(i); }
  inline const Int_t* Hits()             { return (NHits()) ? fHitsID.fArray : 0; }
  
  inline Int_t    DetectorID()   const { return fDetectorID; }
  inline Int_t    SectionID()    const { return fSectionID;  }
  inline Double_t Radial()       const { return fRadial;     }
  inline Double_t RadialError()  const { return fRadialErr;  } 
  inline Double_t Deposit()      const { return fDeposit;    }
  
  inline void Position(TVector3& p)      { p = fDetPos;    }
  inline void PositionError(TVector3& p) { p = fDetPosErr; }
  
  inline Int_t NHits() const { return fHitsID.GetSize(); }
  
  /* Set-methods */
  inline void SetHitID(Int_t hitID, Int_t i = 0) { fHitsID.AddAt(hitID,i); }
 
  inline void SetDetectorID(Double_t detID) { fDetectorID = detID; }
  inline void SetSectionID(Double_t secID)  { fSectionID = secID;  }
  inline void SetRadial(Double_t r)         { fRadial = r;         }
  inline void SetRadialErr(Double_t re)     { fRadialErr = re;     }
  inline void SetDeposit(Double_t dep)      { fDeposit = dep;      }
  
  inline void SetPosition(const TVector3& p)      { fDetPos = p;    }
  inline void SetPositionError(const TVector3& p) { fDetPosErr = p; }
  
  inline void SetRadials(Double_t r, Double_t re) { fRadial = r; fRadialErr = re; }
  
  /* ----------- */
  virtual Bool_t FindHit(Int_t hitID);
  
  virtual Bool_t AddDeposit(Int_t hitID, SpdSttHit* hit); // return TRUE if hit added and FALSE otherwise
  virtual Bool_t AddDeposit(Int_t hitID, Double_t dep);   // return TRUE if new hit and FALSE otherwise
  virtual void   AddDeposit(Double_t dep) { fDeposit += dep; }
  
  virtual Bool_t Equal(SpdSttHit* hit) const;
  virtual Bool_t Equal(SpdSttHelixHit* hit) const { return *this == *hit; }
  
  virtual Bool_t operator==(const SpdSttHelixHit& hit) const 
  { 
    return (fDetectorID != hit.fDetectorID) ? kFALSE 
          :(fSectionID != hit.fSectionID) ? kFALSE 
          : kTRUE; 
  }
  
  virtual void Print(const Option_t* opt = 0) const;
  
  /* ----------- */
   
  inline void SetFake(Bool_t fake = kTRUE) { fIsFake = fake; }
  inline Bool_t IsFake() const { return fIsFake;  }
  
protected:
 
  TArrayI   fHitsID;     // array with merged hits 
  
  Int_t     fDetectorID; // tube id
  Int_t     fSectionID;  // tube section id
  TVector3  fDetPos;     // tube position
  TVector3  fDetPosErr;  // tube position error
  Double_t  fRadial;     // radial distance to the wire
  Double_t  fRadialErr;  // error on the radial distance to the wire 
  Double_t  fDeposit;    // deposit
  
  Bool_t    fIsFake;     // indicator for fake hit
  
  ClassDef(SpdSttHelixHit,1);
};


#endif
