// $Id$
// Author: artur   2018/12/05

#ifndef __SPDPRIMGENDATA_H__
#define __SPDPRIMGENDATA_H__

#include <TNamed.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPrimGenData                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;
class TParticle;
class SpdBaseParSet;

class SpdPrimGenData: public TNamed {

public:

    SpdPrimGenData();
    SpdPrimGenData(const Char_t* generator);
     
    virtual ~SpdPrimGenData();
    
    virtual void Clear();
    
    virtual TParticle* AddParticle(const TParticle* part);
    virtual TParticle* AddNewParticle();
    
    void SetGeneratorType(TString generator) { fName = generator; }
    void SetGeneratorIndex(Int_t index)      { fGenIndex = index; }
   
    TString    GetGenType()    const   { return fName;            }
    Int_t      GetGenIndex()   const   { return fGenIndex;        }
    Int_t      GetNGenSeeds()  const   { return fGenSeeds.size(); }
    Int_t      GetNParticles() const;
    
    Int_t      GetSeed(Int_t i);
    TParticle* GetParticle(Int_t i);
    
    std::vector<UInt_t>& GetGenSeeds()  { return fGenSeeds;  }
    TClonesArray*        GetParticles() { return fParticles; }
    SpdBaseParSet*       GetParameters(); 
      
    virtual void Print(Option_t* option = "");

protected:
  
    Int_t                fGenIndex;   // unique id
    std::vector<UInt_t>  fGenSeeds;   // set of seeds 
    TClonesArray*        fParticles;  // vertex particles list
    SpdBaseParSet*       fParameters; // parameters and triggers

    ClassDef(SpdPrimGenData,1)
};

#endif  /* __SPDPRIMGENDATA_H__ */

