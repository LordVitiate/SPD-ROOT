// $Id$
// Author: artur   2018/12/25

#ifndef __SPDFTFGENERATOR_H__
#define __SPDFTFGENERATOR_H__

#include "SpdGenerator.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdFtfGenerator                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TFile;
class TTree;
class TClonesArray;

class SpdFtfGenerator: public SpdGenerator {

public:

    SpdFtfGenerator();
    SpdFtfGenerator(const char* Name);
    SpdFtfGenerator(Int_t mode);
    SpdFtfGenerator(const char* Name, Int_t mode);
    
    virtual ~SpdFtfGenerator();
    
    virtual void   Reset();
    virtual void   RemoveDataFile();
   
    virtual Bool_t Init();
    
    virtual void   Initialize(Int_t nevents = -1, Double_t epar = -1.);
     
    virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen); 
    
    /* _____________ settings _____________ */
    
    virtual void   SetSeed(UInt_t seed = 0, UInt_t ncycle = 1); 
    virtual void   SetConfig(TString config = "");
    virtual void   SetFilePath(TString fpath = "");
    
    virtual void   SetSeeds(const std::vector<UInt_t>& seeds);
    virtual void   SetRemoveDataFile(Bool_t rmfile) { fRemoveDataFile = rmfile; }
    
    /* _____________ gettings _____________ */
    
    virtual void   GetSeeds(std::vector<UInt_t>& seeds);
   
    inline  Bool_t IsInit()      const { return fInit; }
    inline  Bool_t IsInitSeeds() const { return fInitSeeds; }
    
    /* ____________________________________ */
    
    virtual Bool_t FillParsIn(SpdPrimGenParSet* params);
    virtual Bool_t LoadParsFrom(SpdPrimGenParSet* params);
    
    virtual void   Print(Option_t* opt) const;
    
protected:
  
    virtual void   Build(Int_t mode); 
    
    virtual Bool_t OpenInput();
    virtual void   CloseInput();
    
    virtual void   InitSeeds();
  
    Int_t          fMode;
    
    Int_t          fInitEvents;
    Double_t       fInitEpar;
    TString        fConfig;
    TString        fFilePath;
    
    UInt_t         fSeed;
    UInt_t         fSeedCycle;
    
    TString        fGenPath;
     
    Bool_t         fInit;
    Bool_t         fInitSeeds;  
    
    TFile*         fInputFile; 
    TTree*         fInputTree; 
    TClonesArray*  fParticles;
    Int_t          fNEvents;
    
    Bool_t         fRemoveDataFile;
    
    ClassDef(SpdFtfGenerator,1)
};

#endif  /* __SPDFTFGENERATOR_H__ */

