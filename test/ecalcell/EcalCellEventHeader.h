// $Id$
// Author: artur   2015/10/27

#ifndef __ECALCELLEVENTHEADER_H__
#define __ECALCELLEVENTHEADER_H__

#include <FairMCEventHeader.h>

#include <TVector3.h>
#include <TString.h>
//#include "NuConstants.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// EcalCellEventHeader                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class EcalCellEventHeader: public FairMCEventHeader {

public:

    EcalCellEventHeader();
    virtual ~EcalCellEventHeader();
    
    virtual void Clear();
    virtual void Clear(Option_t* option) { TNamed::Clear(option); }
    
    /* ---------- PARTICLE ---------- */
      
    void     SetParticleName(const char* name); 
    
    void     SetPdgNum(Int_t pdg)   { PdgNum_ = pdg; }
    void     SetMass(Double_t mass) { Mass_ = mass; }
   
    void     SetStartPoint(Double_t x, Double_t y, Double_t z);
    void     SetMomentum(Double_t px, Double_t py, Double_t pz);
             
    TString  GetParticleName() const { return fTitle; }
    
    Int_t    GetPdgNum() const { return PdgNum_; }
    Double_t GetPx()     const { return fRotX; }
    Double_t GetPy()     const { return fRotY; }
    Double_t GetPz()     const { return fRotZ; }
    Double_t GetMass()   const { return Mass_; }
    
    Double_t GetP() const;       // << momentum 
    Double_t GetEnergy() const;  // << Etot
    Double_t GetEkin() const;    // << Etot-m
    
    Double_t Theta() const;  // -> rad
    Double_t Phi() const;    // -> rad
    
    void     GetStartPoint(TVector3& v) const;
    void     GetMomentum(TVector3& v) const;
        
    /* ----------  Primary last process  ---------- */
    void     SetPrimLastProcessID(Int_t pid)  { PrimLastProcessID_ = pid; }
    void     SetPrimLastND(Int_t nd)          { PrimLastNDaughters_ = nd; }
    void     SetPrimLastVolPath(TString path) { PrimLastVolPath_ = path;  }
    void     SetPrimLastPoint(Double_t x, Double_t y, Double_t z);
    void     SetPrimLastIntLen(Double_t intlen) { PrimLastIntLen_ = intlen; }
    void     SetPrimLastRadLen(Double_t radlen) { PrimLastRadLen_ = radlen; }
    void     SetPrimLastDepth(Double_t depth) { PrimLastDepth_ = depth; }
    void     SetPrimLastDist(Double_t length) { PrimLastDist_ = length; }

    Int_t    GetPrimLastProcessID() const { return PrimLastProcessID_;  }
    Int_t    GetPrimLastND()        const { return PrimLastNDaughters_; }
    TString  GetPrimLastVolPath()   const { return PrimLastVolPath_;    }
    void     GetPrimLastPoint(TVector3& v) const;
    Double_t GetPrimLastIntLen()    const { return PrimLastIntLen_; }
    Double_t GetPrimLastRadLen()    const { return PrimLastRadLen_; }
    Double_t GetPrimLastDepth()     const { return PrimLastDepth_;  }
    Double_t GetPrimLastDist()      const { return PrimLastDist_;   }
    
    Double_t GetLPX() const { return LPX_; }
    Double_t GetLPY() const { return LPY_; }
    Double_t GetLPZ() const { return LPZ_; }
    
    void     SetTotalEloss(Double_t eloss)   { TotalEloss_ = eloss; }
    void     AddTotalEloss(Double_t eloss)   { TotalEloss_ += eloss; }
    
    Double_t GetTotalEloss() const { return TotalEloss_;   }
      
    Double_t GetP(Double_t ekin) const;     
    
    /* Register the class as data branch to the output */
    virtual void Register();
    
    virtual void Print(int opt = 0) const;
    virtual void Print(Option_t* option) const { TNamed::Print(option); }
    
protected:
    
    // particle
    Int_t    PdgNum_;
    Double_t Mass_;
        
    // last primary paricle info
    Int_t    PrimLastProcessID_;
    Int_t    PrimLastNDaughters_;
    TString  PrimLastVolPath_;
    Double_t LPX_,LPY_,LPZ_;
    
    Double_t PrimLastIntLen_;
    Double_t PrimLastRadLen_;
    Double_t PrimLastDepth_;
    Double_t PrimLastDist_;
    
    // trigger
    Double_t TotalEloss_;
    
    ClassDef(EcalCellEventHeader,1)
};

//_____________________________________________________________________________
inline void EcalCellEventHeader::SetParticleName(const char* name) 
{
   fTitle = name;  
}

//_____________________________________________________________________________
inline void EcalCellEventHeader::SetStartPoint(Double_t x, Double_t y, Double_t z)
{ 
  SetVertex(x,y,z);     
}

//_____________________________________________________________________________
inline void EcalCellEventHeader::SetMomentum(Double_t px, Double_t py, Double_t pz)     
{ 
  fRotX = px; fRotY = py; fRotZ = pz;
}

//_____________________________________________________________________________
inline void EcalCellEventHeader::GetStartPoint(TVector3& v) const
{
  v.SetXYZ(GetX(),GetY(),GetZ());
}

//_____________________________________________________________________________
inline void EcalCellEventHeader::GetMomentum(TVector3& v) const
{
  v.SetXYZ(fRotX,fRotY,fRotZ);
}

//_____________________________________________________________________________
inline void EcalCellEventHeader::SetPrimLastPoint(Double_t x, Double_t y, Double_t z)
{
  LPX_ = x; LPY_ = y; LPZ_ = z;
}

//____________________________________________________________________________   
inline void EcalCellEventHeader::GetPrimLastPoint(TVector3& v) const
{
  v.SetXYZ(LPX_,LPY_,LPZ_);
}
    
    
#endif  /* __ECALCELLEVENTHEADER_H__ */

