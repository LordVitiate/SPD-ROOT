
#ifndef  __SPDMCTRACK_H__
#define  __SPDMCTRACK_H__

#include "TObject.h"
#include "SpdDetectorList.h"
#include "Rtypes.h"    
#include "TLorentzVector.h"
#include "TMath.h"      
#include "TVector3.h" 

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCTrack                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


class TParticle;

class SpdMCTrack : public TObject {

public:

    SpdMCTrack();

    SpdMCTrack(Int_t pdgCode, Int_t motherID, Double_t px, Double_t py,
               Double_t pz, Double_t x, Double_t y, Double_t z,
               Double_t t, Int_t nPoints);
    
    SpdMCTrack(TParticle* particle);

    SpdMCTrack(const SpdMCTrack& track);
    
    virtual ~SpdMCTrack();
    
    void     SetMotherId(Int_t id)       { fMotherId = id;       }
    void     SetFirstMotherId(Int_t id)  { fMotherId = id;       }
    void     SetSecondMotherId(Int_t id) { fSecondMotherId = id; }
    void     SetVModule(Int_t module)    { fVModule = module;    }
    void     SetVNodeName(TString nname) { fVNodeName = nname;   }   
    
    void     SetNPoints(Int_t iDet, Int_t np);

    Int_t    GetPdgCode()        const { return fPdgCode;  }
    Int_t    GetMotherId()       const { return fMotherId; }
    Int_t    GetFirstMotherId()  const { return fMotherId; }
    Int_t    GetSecondMotherId() const { return fSecondMotherId; }
    
    Double_t GetPx()             const { return fPx; }
    Double_t GetPy()             const { return fPy; }
    Double_t GetPz()             const { return fPz; }
    TVector3 GetMomentum()       const { return TVector3(fPx, fPy, fPz); }
    
    Double_t GetStartX()         const { return fStartX; }
    Double_t GetStartY()         const { return fStartY; }
    Double_t GetStartZ()         const { return fStartZ; }
    Double_t GetStartT()         const { return fStartT; }
    TVector3 GetStartVertex()    const { return TVector3(fStartX, fStartY, fStartZ); }
    Int_t    GetStartProcess()   const { return fVProcess;  }
    Int_t    GetStartModule()    const { return fVModule;   }
    TString  GetStartNodeName()  const { return fVNodeName; }
    
    Double_t GetPt() const { return TMath::Sqrt(fPx*fPx+fPy*fPy); }
    Double_t GetP()  const { return TMath::Sqrt(fPx*fPx+fPy*fPy+fPz*fPz); }
    
    Double_t GetMass() const;
    
    inline Double_t GetEnergy() const; 
    inline Double_t GetRapidity() const;
    inline void     GetMomentum(TVector3& momentum) const;
    inline void     Get4Momentum(TLorentzVector& momentum) const;
    inline void     GetStartVertex(TVector3& vertex) const;
    
    Int_t GetNPoints(DetectorId detId)  const;
    Int_t GetTotalPoints() const { return fNPoints; }

    void Print(Int_t iTrack = 0) const;
  
private:

    Int_t     fPdgCode;                             // PDG particle code
    Int_t     fMotherId;                            // First mother track number (-1 for primaries).   
    Int_t     fSecondMotherId;                      // Track number in the MC-Header primary data list (-1 for secondaries). 
    Int_t     fVProcess;                            // Initial process
    Double_t  fPx, fPy, fPz;                        // Momentum components at start vertex [GeV]
    Double_t  fStartX, fStartY, fStartZ, fStartT;   // Coordinates of start vertex [cm, ns] 
    Int_t     fVModule;                             // Start vertex module ID
    TString   fVNodeName;                           // Start vertex node name
    
    Int_t     fNPoints;                  // Total number of stored points in the track
    Int_t     fPoints[NSPDDETECTORS];    // Number of stored points for detectors
 
    ClassDef(SpdMCTrack,1);

};

//_____________________________________________________________________
inline Double_t SpdMCTrack::GetEnergy() const 
{  
  Double_t mass = GetMass();
  return TMath::Sqrt(mass*mass + fPx*fPx + fPy*fPy + fPz*fPz );
}

//_____________________________________________________________________
inline Double_t SpdMCTrack::GetRapidity() const
{
  Double_t e = GetEnergy();
  Double_t y = 0.5 * TMath::Log( (e+fPz) / (e-fPz) );
  return y;
}

//_____________________________________________________________________
inline void SpdMCTrack::GetMomentum(TVector3& momentum) const
{ 
  momentum.SetXYZ(fPx,fPy,fPz); 
}

//_____________________________________________________________________
inline void SpdMCTrack::Get4Momentum(TLorentzVector& momentum)  const
{
  momentum.SetXYZT(fPx,fPy,fPz,GetEnergy());
}

//_____________________________________________________________________
inline void SpdMCTrack::GetStartVertex(TVector3& vertex) const
{
  vertex.SetXYZ(fStartX,fStartY,fStartZ);
}

#endif
