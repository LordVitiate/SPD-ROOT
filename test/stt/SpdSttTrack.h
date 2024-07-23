
#ifndef __SPDSTTTRACK_H__
#define __SPDSTTTRACK_H__ 

#include <map>
#include "TArrayI.h"
#include "TObject.h"
#include "FairTrackParam.h"
#include "FairMultiLinkedData_Interface.h"
#include "TClonesArray.h"

class SpdSttHit;

class SpdSttTrack : public FairMultiLinkedData_Interface {
  
public:

  SpdSttTrack();
  virtual ~SpdSttTrack() {}

  void Print();

  /** Native Hits attached via SpdTrackCand **/
  Int_t GetTrackCandIndex() { return fTrackCandIndex; }

  /** Reconstructed "Helix" Hits **/
  inline Int_t GetNofHelixHits()            const { return fHelixHits.GetSize(); }
  inline Int_t GetHelixHitIndex(Int_t iHit) const { return fHelixHits.At(iHit); }
  inline Int_t GetFlag()                    const { return fFlag; }

  /** parameters of the helix: d0, phi0, Rad, tanlambda, z0 **/
  inline Double_t GetDist()   { return fDist; }
  inline Double_t GetPhi()    { return fPhi;  }
  inline Double_t GetRad()    { return fRad;  }
  inline Double_t GetTanL()   { return fTanL; }
  inline Double_t GetZ()      { return fZ;    } 
  inline Int_t    GetCharge() { return fH;    }

  inline void SetTrackCandIndex(Int_t trackCandID){ fTrackCandIndex = trackCandID; }

  inline void SetParameters(Double_t d, Double_t phi, Double_t r, Double_t tanl, Double_t z); 

  inline void SetDist(Double_t dist)  { fDist = dist; }
  inline void SetPhi(Double_t phi)    { fPhi = phi; }
  inline void SetRad(Double_t r)      { fRad = r; }
  inline void SetTanL(Double_t tanl)  { fTanL = tanl; }
  inline void SetZ(Double_t z)        { fZ = z; }
  inline void SetCharge(Int_t charge) { fH = charge; }
  inline void SetFlag(Int_t flag)     { fFlag = flag; }

  void AddHelixHit(Int_t size, Int_t index, Int_t helixhitindex);
  
  // track length calculation
  Double_t CalculateScosl(Double_t x, Double_t y);
  
  // find the tri-momentum in the PCA to a point
  TVector3* MomentumAtPoint(TVector3 *point);
  
  // find the PCA to a point
  TVector3* PCAToPoint(TVector3 *point);

private:
   
  TArrayI fHelixHits; // indices of the helixhits attached to the track
  
  /* track parameters */
  Double_t fDist;
  Double_t fPhi;
  Double_t fRad;
  Double_t fTanL;
  Double_t fZ;
  
  Int_t fH; // charge
  
  Int_t fFlag;           // quality flag
  Int_t fTrackCandIndex; // track candidate

  ClassDef(SpdSttTrack,1);
};

//_____________________________________________________________________________________
inline void SpdSttTrack::SetParameters(Double_t d, Double_t phi, Double_t r, Double_t tanl, Double_t z) 
{
   fDist = d;
   fPhi = phi;
   fRad = r;
   fTanL = tanl;
   fZ = z; 
}



#endif
