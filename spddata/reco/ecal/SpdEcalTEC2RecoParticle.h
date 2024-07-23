// $Id$
// Author: andre   2021/03/04

#ifndef __SPDECALTEC2RECOPARTICLE_H__
#define __SPDECALTEC2RECOPARTICLE_H__

#include <TObject.h>
#include <TVector3.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTEC2RecoParticle                                                    //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalTEC2RecoParticle : public TObject {

public:

    SpdEcalTEC2RecoParticle();
    SpdEcalTEC2RecoParticle(Double_t energy, TVector3 pos);
    
    virtual ~SpdEcalTEC2RecoParticle();

    Double_t GetEnergy()   const { return fEnergy; }
    TVector3 GetPosition() const { return fPos;    }
    
    void Print(const Option_t* opt = 0) const;
    
private:

    Double_t fEnergy;
    TVector3 fPos;

    ClassDef(SpdEcalTEC2RecoParticle,1)
};

#endif  /* __SPDECALTEC2RECOPARTICLE_H__ */

