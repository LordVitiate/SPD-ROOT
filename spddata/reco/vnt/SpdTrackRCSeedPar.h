// $Id$
// Author: vladimir   2021/11/28

#ifndef __SPDTRACKRCSEEDPAR_H__
#define __SPDTRACKRCSEEDPAR_H__

#include <TObject.h>
#include <TVector3.h>
#include <TMatrixDSym.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTrackRCSeedPar                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCSiliconHit;

class SpdTrackRCSeedPar: public TObject {

public:

    SpdTrackRCSeedPar();
    virtual ~SpdTrackRCSeedPar() {}
    
    // Setters
    void       SetNHits(Int_t n)     { fNHits = n;   }
  
    void       SetCharge(Double_t p) { fCharge = p;  }  
    void       SetRadius(Double_t p) { fRadius = p;  }  
    void       SetTheta(Double_t p)  { fTheta  = p;  }  
    void       SetPhi(Double_t p)    { fPhi    = p;  }  
    void       SetZvtx(Double_t p)   { fZvtx   = p;  }  
    void       SetRes(Double_t p)    { fRes    = p;  }  

//    void       SetChi2(Double_t chi2)  { fChi2 = chi2; }      
//    void       SetNDF(Int_t ndf)       { fNDF = ndf;   }

    // Getters
    Int_t      GetNHits()      const { return fNHits;  }

    Double_t   GetCharge()     const { return fCharge; } 
    Double_t   GetRadius()     const { return fRadius; } 
    Double_t   GetTheta()      const { return fTheta;  } 
    Double_t   GetPhi()        const { return fPhi;    } 
    Double_t   GetZvtx()       const { return fZvtx;   } 
    Double_t   GetRes()        const { return fRes;    } 

//    Double_t   GetChi2()       const { return fChi2;   }
//    Int_t      GetNDF()        const { return fNDF;    } 

    virtual void Print(Option_t* option = "") const;

private:
    
    Int_t       fNHits;      // number of hits in seed

    Double_t    fCharge;     // estimated charge   [0, 1, -1]
    Double_t    fRadius;     // estimated radius   [cm]
    Double_t    fTheta;      // estimated theta    [rad]
    Double_t    fPhi;        // estimated phi      [rad]
    Double_t    fZvtx;       // estimated z vertex [cm]
    Double_t    fRes;        // residual of line fit [cm]

//    Double_t    fChi2;       // chi2 of parabola fit
//    Int_t       fNDF;        // degrees of freedom number 

    ClassDef(SpdTrackRCSeedPar,1)
};

#endif  /* __SPDTRACKRCSEEDPAR_H__ */

