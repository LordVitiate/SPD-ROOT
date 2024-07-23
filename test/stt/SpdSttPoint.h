

#ifndef __SPDSTTPOINT_H__
#define __SPDSTTPOINT_H__

#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"

class SpdSttPoint : public FairMCPoint 
{

public:

 
  /** Constructors with arguments:
   * 
   *@param trackID       Index of MCTrack (FairMCPoint)
   *@param detID         Detector ID (FairMCPoint)
   *@param secID         Section ID 
   *@param pos           Coordinates of MC point [cm] (FairMCPoint)
   *@param posInLocal    Local coordinates at entrance to active volume [cm]
   *@param posOutLocal   Local coordinates at exit of active volume [cm]
   *@param momIn         Momentum of track at entrance [GeV]
   *@param momOut        Momentum of track at exit [GeV]
   *@param mass          Particle mass [GeV]
   *@param tof           Time since event start [ns] (FairMCPoint)
   *@param length        Track length since creation [cm] (FairMCPoint)
   *@param eLoss         Energy deposit [GeV] (FairMCPoint)
   * 
   **/
  
  SpdSttPoint();
  
  SpdSttPoint(Int_t trackID, Int_t detID, 
              TVector3 pos, 
              TVector3 posInLocal, TVector3 posOutLocal, 
              TVector3 momIn, TVector3 momOut, 
              Double_t tof, Double_t length, Double_t eLoss, Double_t mass);
  
  SpdSttPoint(Int_t trackID, Int_t detID, Int_t secID,
              TVector3 pos, 
              TVector3 posInLocal, TVector3 posOutLocal, 
              TVector3 momIn, TVector3 momOut, 
              Double_t tof, Double_t length, Double_t eLoss, Double_t mass);
 
  SpdSttPoint(const SpdSttPoint& point);
  
  virtual ~SpdSttPoint() {}
 
  /* Set methods */
  
  inline void SetPositionInLocal(TVector3 pos);
  inline void SetPositionOutLocal(TVector3 pos); 
  inline void SetMomentumIn(TVector3 mom);  
  inline void SetMomentumOut(TVector3 mom);     
  
  /* Get methods */
  
  inline Int_t GetSectionID() { return fSectionID; }
  
  inline void PositionInLocal(TVector3& pos);
  inline void PositionOutLocal(TVector3& pos); 
  inline void MomentumIn(TVector3& mom);
  inline void MomentumOut(TVector3& mom); 
  
  inline Double_t GetXOutLocal()  { return fXoutLoc; }
  inline Double_t GetYOutLocal()  { return fYoutLoc; }
  inline Double_t GetZOutLocal()  { return fZoutLoc; }
  
  inline Double_t GetXInLocal()   { return fXinLoc;  }
  inline Double_t GetYInLocal()   { return fYinLoc;  }
  inline Double_t GetZInLocal()   { return fZinLoc;  }
 
  inline Double_t GetMass() { return fMass; }
  
  Double_t GetRadialDistance() const;

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

protected:
  
  //additional ID info
  Int_t fSectionID; // tube section number
   
  // exit coordinates in straw frame
  Double_t fXoutLoc;
  Double_t fYoutLoc; 
  Double_t fZoutLoc;
  
  // entry coordinates in straw frame
  Double_t fXinLoc;
  Double_t fYinLoc;
  Double_t fZinLoc;
  
  // entry momentum components [GeV]
  Double_t fPx_in; 
  Double_t fPy_in; 
  Double_t fPz_in; 

  // exit momentum components [GeV]
  Double_t fPx_out; 
  Double_t fPy_out; 
  Double_t fPz_out; 

  // particle mass [GeV]
  Double_t fMass; 
  
  ClassDef(SpdSttPoint,1)
};

//_____________________________________________________________________
inline void SpdSttPoint::SetPositionInLocal(TVector3 pos) 
{
  fXinLoc = pos.X();
  fYinLoc = pos.Y();
  fZinLoc = pos.Z();
}

//_____________________________________________________________________
inline void SpdSttPoint::SetPositionOutLocal(TVector3 pos) 
{
  fXoutLoc = pos.X();
  fYoutLoc = pos.Y();
  fZoutLoc = pos.Z();
}

//_____________________________________________________________________
inline void SpdSttPoint::SetMomentumIn(TVector3 mom) 
{
  fPx_in = mom.X();
  fPy_in = mom.Y();
  fPz_in = mom.Z();
}

//_____________________________________________________________________
inline void SpdSttPoint::SetMomentumOut(TVector3 mom) 
{
  fPx_out = mom.X(); 
  fPy_out = mom.X(); 
  fPz_out = mom.X(); 
}
  
//_____________________________________________________________________
inline void SpdSttPoint::PositionOutLocal(TVector3& pos) 
{ 
  pos.SetXYZ(fXoutLoc,fYoutLoc,fZoutLoc); 
} 

//_____________________________________________________________________
inline void SpdSttPoint::PositionInLocal(TVector3& pos)  
{ 
  pos.SetXYZ(fXinLoc,fYinLoc,fZinLoc); 
}

//_____________________________________________________________________
inline void SpdSttPoint::MomentumIn(TVector3& mom)
{ 
  mom.SetXYZ(fPx_in,fPy_in,fPz_in); 
} 

//_____________________________________________________________________
inline void SpdSttPoint::MomentumOut(TVector3& mom)
{
  mom.SetXYZ(fPx_out,fPy_out,fPz_out); 
} 




#endif
