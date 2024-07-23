// $Id$
// Author: artur   2019/02/04

#ifndef __SPDEVTBASEGENERATOR_H__
#define __SPDEVTBASEGENERATOR_H__

#include <TObject.h>
#include <fstream>
#include "SpdGenerator.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEvtBaseGenerator                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TFile;
class TTree;
class TClonesArray;

class SpdEvtBaseGenerator: public SpdGenerator {

public:

    SpdEvtBaseGenerator();  
    SpdEvtBaseGenerator(const char* Name);
    
    virtual ~SpdEvtBaseGenerator();
    
    virtual void   Reset();
    
    virtual Bool_t Init();
    virtual void   Initialize(Int_t nevents, Int_t nskipevents = 0);
    
    virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);
    
    /* _____________ settings _____________ */
    
    virtual Bool_t EnableExternalDecayer();
  
    void SetFile(TString filename, TString datapath = ""); 
     
    /* _____________ gettings _____________ */
   
    TString GetFullDataPath() const;
     
    inline  TString GetDataPath() const    { return fPathToData; }
    inline  TString GetFileName() const    { return fFileName;   }
    inline  Char_t  GetFileFormat() const  { return fFileFormat; }
    
    inline  Int_t   GetNEvents() const     { return fNEvents; }
    inline  Int_t   GetNSkipEvents() const { return fNSkipEvents; }
    
    inline  TString GetDataType() const    { return fDataType;   }
    inline  TString GetGenerator() const   { return fGenerator;  }
    inline  Int_t   GetFileEvents() const  { return fFileEvents; }
    
    inline  Bool_t  IsInit() const { return fInit; }
    
    virtual Bool_t  IsDecayerEnable() const { return fEnableDecayer; } 
  
    /* ____________________________________ */
     
    virtual Bool_t FillParsIn(SpdPrimGenParSet* params); 
    virtual Bool_t LoadParsFrom(SpdPrimGenParSet* params);
    
    virtual void   Print(Option_t* opt) const;
      
    /* methods to define ions */
    
    static Bool_t  RegisterIon(Int_t pdg); 
    static Bool_t  RegisterIon(Int_t Z, Int_t A); 
    static Bool_t  RegisterIon(Int_t Z, Int_t A, Int_t Q /*e-charge*/, Double_t EE = 0. /*excitation energy*/); 
    
    static Bool_t  IsIonDefined(Int_t pdg);

    static Bool_t  IsIon(Int_t pdg);
    static Int_t   IonPdg(Int_t Z, Int_t A);
    static void    IonZA(Int_t pdg, Int_t& Z, Int_t& A);
    static TString IonName(Int_t pdg);
    
protected:
  
    virtual Bool_t InitFile();
    virtual void   CloseFile();
  
    TString        fPathToData;    //  Path to the file location folder
    TString        fFileName;      //  File name
    Char_t         fFileFormat;    //  'a'(ascii), 'r'(root) or 'n'(undefined)  
    
    Int_t          fNEvents;       //  Events to generate
    Int_t          fNSkipEvents;   //  Events to skip
   
    Bool_t         fInit;          //  Init mark
      
    TString        fDataType;      //  FORMAT_TYPE (default = "std")
    TString        fGenerator;     //  Type of events generator (class name)
    Int_t          fFileEvents;    //  Total number of events in the file
    Bool_t         fEnableDecayer; //  Indicates that external decayer is enable
      
    TFile*         fRootFile;      //! Input root-file (*.root)
    TTree*         fTree;          //! Input tree from the file
    TClonesArray*  fData;          //! Input array of particles (TParticle's)
    
    std::ifstream  fAsciiFile;     //! Input ascii-file (*.txt)
    
    ClassDef(SpdEvtBaseGenerator,1)
};

#endif  /* __SPDEVTBASEGENERATOR_H__ */

