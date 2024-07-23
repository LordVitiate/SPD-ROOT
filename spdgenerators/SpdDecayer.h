// $Id$
// Author: artur   2018/12/19

#ifndef __SPDDECAYER_H__
#define __SPDDECAYER_H__

#include <TNamed.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdDecayer                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TLorentzVector;
class TClonesArray;
class TVirtualMCDecayer;
class SpdPrimGenParSet;
class FairMCEventHeader;
class SpdMCEventHeader;

class SpdDecayer: public TNamed {

public:

    SpdDecayer(Int_t storage_index = -1);
    virtual ~SpdDecayer();
    
    static SpdDecayer* Instance();
    static SpdDecayer* Decayer();
    
    virtual void  Reset();
    
    virtual void  Init();
    
    virtual void  FillDecay(const Int_t& idpart, const TLorentzVector* p);
    virtual void  FillParticles(const TClonesArray *particles);
    virtual void  FillEventData();
    
    virtual void  SetDecayer(TString decayer, Int_t option = 1);
    virtual void  SetEventHeader(FairMCEventHeader* header);
    
    Bool_t IsInit() const { return fIsInit; }
    Int_t  GetStorageIndex() const { return fStorageIndex; }
    
    TString            GetDecayerType() const { return fTitle;   }
    TVirtualMCDecayer* GetDecayer()     const { return fDecayer; }
    SpdMCEventHeader*  GetEventHeader() const { return fEHeader; }
    
    virtual Bool_t FillParsIn(SpdPrimGenParSet* params);
    virtual Bool_t LoadParsFrom(SpdPrimGenParSet* params);
    
    virtual void Print(Option_t* opt = "") const;
    
protected:
  
    virtual Bool_t Configure(TString decayer_type);
    virtual Bool_t ConfigureSpdPythia6Decayer();
    virtual Bool_t ConfigureSpdPythia8Decayer();
  
    static SpdDecayer* fInstance;        //!
    
    Int_t              fStorageIndex;    //!
   
    TVirtualMCDecayer* fDecayer;         //!
    Int_t              fDecayerOption;   //!
    SpdMCEventHeader*  fEHeader;         //!
    Bool_t             fIsInit;          //!

    ClassDef(SpdDecayer,1)
};

#endif  /* __SPDDECAYER_H__ */

