// $Id$
// Author: artur   2021/03/15

#ifndef __SPDECALRCPARTICLE_H__
#define __SPDECALRCPARTICLE_H__

#include <TObject.h>
#include <TVector3.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalRCParticle                                                          //
//                                                                            //
// Describes parameters of a particle reconstructed in ECAL, such as:         //
// - position of hit in ECAL                                                  //
// - reconstructed energy of the particle, in case it was e+/e-/photon        //
// - index of cluster used as input for the recosntruction algorithm          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


class SpdEcalRCParticle: public TObject {

public:

    SpdEcalRCParticle();
    virtual ~SpdEcalRCParticle();
   
    // ----- Setters -----
    void  SetId(Int_t id)           { fId = id;        }
    void  SetClusterId(Int_t id)    { fClusterId = id; }
    void  SetEnergy(Double_t e)     { fEnergy = e;     }
    void  SetPosition(TVector3 pos) { fPosition = pos; }
    void  SetPID(Int_t pid)         { fPID = pid;      }
    
    void  SetPosition(Double_t x, Double_t y, Double_t z) { fPosition.SetXYZ(x,y,z); }
    
    // ----- Getters -----
    Int_t     GetId()         const { return fId;           }
    Int_t     GetClusterId()  const { return fClusterId;    }
    Double_t  GetEnergy()     const { return fEnergy;       }
    TVector3  GetPosition()   const { return fPosition;     }
    Double_t  GetPosX()       const { return fPosition.X(); }
    Double_t  GetPosY()       const { return fPosition.Y(); }
    Double_t  GetPosZ()       const { return fPosition.Z(); }
    Int_t     GetPID()        const { return fPID;          }
    
    void GetPosition(TVector3& pos) const { pos = fPosition; }
    
    // Print
    virtual void Print(Option_t* option = "") const;
    
private:
    
    Int_t    fId;           // unique id
    Int_t    fClusterId;    // cluster (id) to which the particle belongs
    Double_t fEnergy;       // particle energy (reconstructed)
    TVector3 fPosition;     // point on the particle trajectory (reconstructed)
    Int_t    fPID;          // predicted particle type
    
    ClassDef(SpdEcalRCParticle,1)
};

#endif  /* __SPDECALRCPARTICLE_H__ */

