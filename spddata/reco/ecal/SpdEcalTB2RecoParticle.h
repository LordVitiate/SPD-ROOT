// $Id$
// Author: andre   2021/03/04

#ifndef __SPDECALTB2RECOPARTICLE_H__
#define __SPDECALTB2RECOPARTICLE_H__

#include <TObject.h>
#include <TVector3.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTB2RecoParticle                                                     //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTB2RecoParticle : public TObject {

public:

    SpdEcalTB2RecoParticle();
    SpdEcalTB2RecoParticle(Double_t energy, TVector3 pos);
    
    virtual ~SpdEcalTB2RecoParticle();

    Double_t GetEnergy()   const { return fEnergy; }
    TVector3 GetPosition() const { return fPos;    }

    void Print(const Option_t* opt = 0) const;

private:

    Double_t fEnergy;
    TVector3 fPos;

    ClassDef(SpdEcalTB2RecoParticle,1)
};

#endif  /* __SPDECALTB2RECOPARTICLE_H__ */

