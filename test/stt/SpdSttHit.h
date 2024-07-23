
#ifndef __SPDSTTHIT_H__
#define __SPDSTTHIT_H__ 

#include "FairHit.h"

class SpdSttHit : public FairHit {

public:    

  SpdSttHit();

  /** Constructors with arguments:
   * 
  *@param detID      Tube ID
  *@param tubeID     Tube sector ID
  *@param mcindex    Index of corresponding MCPoint
  *@param pos        Position coordinates of the tube [cm]
  *@param dpos       Errors in position coordinates of the tube (or tube sizes)[cm]
  *@param radal      The radial measurement of deposit
  *@param radialErr  The error on on the radial measurement (drift radius)
  *@param htime      Hit time
  *@param htimeErr   Hit time error
  *@param Deposit    Deposit
  **/

  SpdSttHit(Int_t detID, Int_t secID, Int_t mcindex, 
            TVector3& pos, TVector3& dpos, 
            Double_t radial, Double_t radialErr, 
            Double_t htime, Double_t htimeErr,
            Double_t deposit);

  virtual ~SpdSttHit() {}
  
  virtual void Clear();
  
  virtual void Print(const Option_t* opt = 0) const;

  inline Int_t    DetectorID()   const { return GetDetectorID(); }
  inline Int_t    SectionID()    const { return fSectionID; }
  inline Int_t    PointID()      const { return GetRefIndex(); }
  inline Double_t Radial()       const { return fRadial; }
  inline Double_t RadialError()  const { return fRadialErr; } 
  inline Double_t HitTime()      const { return GetTimeStamp(); }
  inline Double_t HitTimeError() const { return GetTimeStampError(); }
  inline Double_t Deposit()      const { return fDeposit; }
 
  void SetSectionID(Double_t secID)  { fSectionID = secID;   }
  void SetPointID(Double_t mcindex)  { SetRefIndex(mcindex); }
  void SetRadial(Double_t r)         { fRadial = r;          }
  void SetRadialErr(Double_t re)     { fRadialErr = re;      }
  void SetHitTime(Double_t t)        { SetTimeStamp(t);      }
  void SetHitTimeErr(Double_t te)    { SetTimeStamp(te);     }
  void SetDeposit(Double_t dep)      { fDeposit = dep;       }

  virtual Bool_t Equal(FairHit* data) const;
  
  virtual Bool_t operator<(const SpdSttHit& hit) const 
  { return (DetectorID() < hit.DetectorID()) ? kTRUE : kFALSE; }
  

  friend std::ostream& operator<< (std::ostream& out, SpdSttHit& hit)
  {
     std::cout << "<SpdSttHit> Tube: " << hit.DetectorID() << " Section: " << hit.SectionID() 
               << " R: " << hit.Radial() << " +/- " << hit.RadialError()
               << " Time: " << hit.HitTime()  << " +/- " << hit.HitTimeError()
               << " Deposit: " << hit.Deposit() << std::endl;
     return out;
  }

protected:
  
  Int_t     fSectionID; // tube section id
  Double_t  fRadial;    // radial distance to the wire
  Double_t  fRadialErr; // error on the radial distance to the wire 
  Double_t  fDeposit;   // deposit

  ClassDef(SpdSttHit,1);
 
};


#endif
