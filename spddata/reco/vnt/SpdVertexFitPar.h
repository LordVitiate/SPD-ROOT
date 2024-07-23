// $Id$
// Author: artur   2020/10/01

#ifndef __SPDVERTEXFITPAR_H__
#define __SPDVERTEXFITPAR_H__

#include <TObject.h>
#include <TVector3.h>
#include <TMatrixDSym.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdVteFitPar                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdVtxFitPar: public TObject {

public:

    SpdVtxFitPar() {}
    virtual ~SpdVtxFitPar() {}
    
    virtual void Print(Option_t* option = "") const {}

private:

    ClassDef(SpdVtxFitPar,1)
};

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPrimVertexFitPar                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdPrimVertexFitPar: public SpdVtxFitPar {

public:

    SpdPrimVertexFitPar():SpdVtxFitPar(),fNTracks(0),fChi2(0),fNDF(1) {}
    virtual ~SpdPrimVertexFitPar() {}
    
    // Setters
    void       SetNTracks(Int_t n)     { fNTracks = n; }
    void       SetVertex(TVector3 v)   { fVertex = v;  }
    void       SetChi2(Double_t chi2)  { fChi2 = chi2; }      
    void       SetNDF(Int_t ndf)       { fNDF = ndf;   }  
   
    void       SetCovariance(const TMatrixDSym& c /*3x3*/); 
   
    void       SetVertex(Double_t x, Double_t y, Double_t z) { fVertex.SetXYZ(x,y,z); }
   
    // Getters
    Int_t      GetNTracks()      const { return fNTracks;    }
    TVector3   GetVertex()       const { return fVertex;     }
    Double_t   GetVx()           const { return fVertex.X(); }
    Double_t   GetVy()           const { return fVertex.Y(); }
    Double_t   GetVz()           const { return fVertex.Z(); }
    Double_t   GetChi2()         const { return fChi2;       }
    Int_t      GetNDF()          const { return fNDF;        } 
    Double_t   GetChi2overNDF()  const { return (fNDF > 0) ? fChi2/fNDF : 0; }
    
    Bool_t     IsCovDefined()    const { return (fVertexCov.GetMatrixArray()) ? true : false;  } 

    const TMatrixDSym& GetCov()  const { return fVertexCov; }
    Bool_t GetCovariance(TMatrixDSym& c /*3x3*/) const;
   
    virtual void Print(Option_t* option = "") const;

private:
    
    Int_t       fNTracks;    // number of fitted tracks
    TVector3    fVertex;     // vertex coordinates [cm]
    TMatrixDSym fVertexCov;  // vertex covariation matrix 3x3
    Double_t    fChi2;       // fit chisqure
    Int_t       fNDF;        // fit degrees of freedom number 
    
    ClassDef(SpdPrimVertexFitPar,1)
};

//------------------------------------------------------------------------
inline void SpdPrimVertexFitPar::SetCovariance(const TMatrixDSym& c /*3x3*/) 
{ 
    fVertexCov.ResizeTo(c); 
    fVertexCov = c;
} 
    
//------------------------------------------------------------------------
inline Bool_t SpdPrimVertexFitPar::GetCovariance(TMatrixDSym& c /*3x3*/) const 
{
    if (!IsCovDefined()) return false; 
    c.ResizeTo(fVertexCov); 
    c = fVertexCov;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdVertexFitPar                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdVertexFitPar: public SpdVtxFitPar {

public:

    SpdVertexFitPar():fMass(-99.),fMassErr(-99.),fP(-99.),fEnergy(-99.),
                      fEta(-99.),fTheta(-99.),fPhi(-99.),fLength(-99.) {}
    virtual ~SpdVertexFitPar() {}
    
    // Setters
    
    void  SetMass(Float_t mass)     { fMass = mass;     }     
    void  SetMassErr(Float_t err)   { fMassErr = err;   }  
    void  SetP(Float_t p)           { fP = p;           }
    void  SetEnergy(Float_t energy) { fEnergy = energy; }  
    void  SetEta(Float_t eta)       { fEta = eta;       }
    void  SetTheta(Float_t theta)   { fTheta = theta;   } 
    void  SetPhi(Float_t phi)       { fPhi = phi;       }
    void  SetLength(Float_t length) { fLength = length; }   
    
    // Getters
    
    Float_t  GetMass()    const { return fMass;    }     
    Float_t  GetMassErr() const { return fMassErr; }  
    Float_t  GetP()       const { return fP;       }
    Float_t  GetEnergy()  const { return fEnergy;  }  
    Float_t  GetEta()     const { return fEta;     }
    Float_t  GetTheta()   const { return fTheta;   } 
    Float_t  GetPhi()     const { return fPhi;     }
    Float_t  GetLength()  const { return fLength;  }   
    
    // Print
    
    virtual void Print(Option_t* option = "") const;

private:
    
    Float_t   fMass;        // mass of decay particle
    Float_t   fMassErr;     // error of mass decay particle
    Float_t   fP;           // momentum of decay particle
    Float_t   fEnergy;      // energy of decay particle
    Float_t   fEta;         // eta of decay particle
    Float_t   fTheta;       // theta of decay particle
    Float_t   fPhi;         // phi of decay particle
    Float_t   fLength;      // decay length
    
    ClassDef(SpdVertexFitPar,1)
};

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// 1) SpdVertexFitParDaughterTrack                                            //
// 2) SpdVertexFitPar2                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdVertexFitParDaughterTrack : public TObject {
 
public:   
    
    SpdVertexFitParDaughterTrack() {}
   ~SpdVertexFitParDaughterTrack() {}

    Float_t  Trk_mass_   = -99.;   // mass of daughter particle, [GeV]
    Float_t  Trk_mom_    = -99.;   // momentum of daughter particle, [GeV]
    Float_t  Trk_momErr_ = -99.;   // error of momentum daughter particle, [GeV]
    Float_t  Trk_pt_     = -99.;   // pt of daughter particle, [GeV]
    Float_t  Trk_ptErr_  = -99.;   // error of pt daughter particle, [GeV]
    Float_t  Trk_eta_    = -99.;   // eta of daughter particle
    Float_t  Trk_etaErr_ = -99.;   // error of eta daughter particle
    Float_t  Trk_phi_    = -99.;   // phi of daughter particle, [rad]
    Float_t  Trk_phiErr_ = -99.;   // error of phi daughter particle, [rad]
    Float_t  Trk_theta_  = -99.;   // theta of daughter particle, [rad]
    Float_t  Trk_chi2PV_ = -99.;   // chi2 of daughter particle to PV
    
    // Print
    void print(Int_t n) const;
    
    ClassDef(SpdVertexFitParDaughterTrack,1)
};

class SpdVertexFitPar2: public SpdVtxFitPar {

public:

    SpdVertexFitPar2();
    virtual ~SpdVertexFitPar2() {}
    
    void Reset();
    
    // setters
    
    inline Int_t AddDaughter(const SpdVertexFitParDaughterTrack& daughter); 
    
    void SetMass(Double_t mass, Double_t err)   { fMass   = mass;  fMassErr   = err; }
    void SetMom(Double_t mom, Double_t err)     { fMom    = mom;   fMomErr    = err; }
    void SetPt(Double_t pt, Double_t err)       { fPt     = pt;    fPtErr     = err; }
    void SetTheta(Double_t theta, Double_t err) { fTheta  = theta; fThetaErr  = err; }
    void SetPhi(Double_t phi, Double_t err)     { fPhi    = phi;   fPhiErr    = err; }
    void SetEta(Double_t eta, Double_t err)     { fEta    = eta;   fEtaErr    = err; }
    void SetLength(Double_t len, Double_t err)  { fLength = len;   fLengthErr = err; }
    void SetLifetime(Double_t ct, Double_t err) { fCtau   = ct;    fCtauErr   = err; }
    
    void SetProductionVertex(TVector3 v)        { fProductionVertex = v;     }
    void SetDecayVertex(TVector3 v)        { fDecayVertex = v;     }
    void SetMass(Double_t mass)       { fMass   = mass;   }
    void SetMom(Double_t mom)         { fMom    = mom;    }
    void SetPt(Double_t pt)           { fPt     = pt;     }
    void SetTheta(Double_t theta)     { fTheta  = theta;  }
    void SetPhi(Double_t phi)         { fPhi    = phi;    }
    void SetEta(Double_t eta)         { fEta    = eta;    }
    void SetLength(Double_t len)      { fLength = len;    }
    void SetLifetime(Double_t ct)     { fCtau   = ct;     }
    
    void SetMassErr(Double_t err)     { fMassErr   = err; }
    void SetMomErr(Double_t err)      { fMomErr    = err; }
    void SetPtErr(Double_t err)       { fPtErr     = err; }
    void SetThetaErr(Double_t err)    { fThetaErr  = err; }
    void SetPhiErr(Double_t err)      { fPhiErr    = err; }
    void SetEtaErr(Double_t err)      { fEtaErr    = err; }
    void SetLengthErr(Double_t err)   { fLengthErr = err; }
    void SetLifetimeErr(Double_t err) { fCtauErr   = err; }
    
    void SetChi2PV(Double_t chi2)     { fChi2PV = chi2;   }
    void SetChi2SV(Double_t chi2)     { fChi2SV = chi2;   }
                   
    void SetQtAlfa1(Double_t val)     { fQtAlfa1 = val;   }
    void SetQtAlfa2(Double_t val)     { fQtAlfa2 = val;   }
      
    void SetProductionVertex(Double_t x, Double_t y, Double_t z) { fProductionVertex.SetXYZ(x,y,z); }
    void SetDecayVertex(Double_t x, Double_t y, Double_t z) { fDecayVertex.SetXYZ(x,y,z); }

    // getters
    
    TVector3 GetProductionVertex()        const { return fProductionVertex;    }
    Double_t GetProductionVx()            const { return fProductionVertex.X();}
    Double_t GetProductionVy()            const { return fProductionVertex.Y();}
    Double_t GetProductionVz()            const { return fProductionVertex.Z();}
    TVector3 GetDecayVertex()        const { return fDecayVertex;    }
    Double_t GetDecayVx()            const { return fDecayVertex.X();}
    Double_t GetDecayVy()            const { return fDecayVertex.Y();}
    Double_t GetDecayVz()            const { return fDecayVertex.Z();}
    Double_t GetMass()          const { return fMass;      }
    Double_t GetMom()           const { return fMom;       }
    Double_t GetPt()            const { return fPt;        }
    Double_t GetTheta()         const { return fTheta;     }
    Double_t GetPhi()           const { return fPhi;       }
    Double_t GetEta()           const { return fEta;       }
    Double_t GetLength()        const { return fLength;    }
    Double_t GetLifetime()      const { return fCtau;      }
    
    Double_t GetMassErr()       const { return fMassErr;   }
    Double_t GetMomErr()        const { return fMomErr;    }
    Double_t GetPtErr()         const { return fPtErr;     }
    Double_t GetThetaErr()      const { return fThetaErr;  }
    Double_t GetPhiErr()        const { return fPhiErr;    }
    Double_t GetEtaErr()        const { return fEtaErr;    }
    Double_t GetLengthErr()     const { return fLengthErr; }
    Double_t GetLifetimeErr()   const { return fCtauErr;   }
    
    Double_t GetChi2PV()        const { return fChi2PV;    }
    Double_t GetChi2SV()        const { return fChi2SV;    }
    
    Double_t GetQtAlfa1()       const { return fQtAlfa1;   }
    Double_t GetQtAlfa2()       const { return fQtAlfa2;   }
        
    Int_t    GetNDaughters()    const { return fDaughters.size(); }
    
    Bool_t   CheckTrackId(Int_t id) const { return (id > -1 && id < fDaughters.size()); }
    
    Float_t  GetTrkMass(Int_t id)    const { return CheckTrackId(id) ? fDaughters[id].Trk_mass_   : -99.; }
    Float_t  GetTrkMom(Int_t id)     const { return CheckTrackId(id) ? fDaughters[id].Trk_mom_    : -99.; }
    Float_t  GetTrkMomErr(Int_t id)  const { return CheckTrackId(id) ? fDaughters[id].Trk_momErr_ : -99.; }
    Float_t  GetTrkPt(Int_t id)      const { return CheckTrackId(id) ? fDaughters[id].Trk_pt_     : -99.; }
    Float_t  GetTrkPtErr(Int_t id)   const { return CheckTrackId(id) ? fDaughters[id].Trk_ptErr_  : -99.; }
    Float_t  GetTrkEta(Int_t id)     const { return CheckTrackId(id) ? fDaughters[id].Trk_eta_    : -99.; }
    Float_t  GetTrkEtaErr(Int_t id)  const { return CheckTrackId(id) ? fDaughters[id].Trk_etaErr_ : -99.; }  
    Float_t  GetTrkPhi(Int_t id)     const { return CheckTrackId(id) ? fDaughters[id].Trk_phi_    : -99.; }
    Float_t  GetTrkTheta(Int_t id)   const { return CheckTrackId(id) ? fDaughters[id].Trk_theta_  : -99.; }
    Float_t  GetTrkChi2PV(Int_t id)  const { return CheckTrackId(id) ? fDaughters[id].Trk_chi2PV_ : -99.; }
    
    SpdVertexFitParDaughterTrack* GetDaughter(Int_t i) { return &fDaughters[i]; } 
    
    // Print
    virtual void Print(Option_t* option = "") const;
    
private:
    
    TVector3  fProductionVertex;     // vertex coordinates [cm]
    TVector3  fDecayVertex;     // vertex coordinates [cm]
    Float_t   fMass;       // mass of mother particle, [GeV]
    Float_t   fMassErr;    // error of mass mother particle, [GeV]
    Float_t   fMom;        // momentum of mother particle, [GeV]
    Float_t   fMomErr;     // error of nomentum mother particle, [GeV]
    Float_t   fPt;         // pt of mother particle, [GeV]
    Float_t   fPtErr;      // error of pt mother particle, [GeV]
    Float_t   fTheta;      // theta of mother particle, [rad]
    Float_t   fThetaErr;   // error of theta mother particle, [rad]
    Float_t   fPhi;        // phi of mother particle, [rad]
    Float_t   fPhiErr;     // error of phi mother particle, [rad]
    Float_t   fEta;        // eta of mother particle
    Float_t   fEtaErr;     // error of eta mother particle
    Float_t   fLength;     // decay length of mother particle, [cm]
    Float_t   fLengthErr;  // error of decay length mother particle, [cm]
    Float_t   fCtau;       // life time of mother particle
    Float_t   fCtauErr;    // error of life time mother particle
    Float_t   fChi2PV;     // chi2 of mother particle to PV
    Float_t   fChi2SV;     // chi2 of secondary vertex
    Float_t   fQtAlfa1;    // 
    Float_t   fQtAlfa2;    //
    
    std::vector<SpdVertexFitParDaughterTrack> fDaughters;  // list of daughter tracks
    
    ClassDef(SpdVertexFitPar2,1)
};

//---------------------------------------------------------------------------------
inline Int_t SpdVertexFitPar2::AddDaughter(const SpdVertexFitParDaughterTrack& daughter) 
{ 
    fDaughters.push_back(daughter); 
    return fDaughters.size()-1; 
}

#endif  /* __SPDVERTEXFITPAR_H__ */

