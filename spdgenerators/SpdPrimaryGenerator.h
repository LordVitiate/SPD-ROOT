// $Id$
// Author: artur   2017/11/15

#ifndef __SPDPRIMARYGENERATOR_H__
#define __SPDPRIMARYGENERATOR_H__

#include <TRandom.h>
#include <TMCProcess.h>
#include "FairPrimaryGenerator.h"
#include "SpdGlobals.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPrimaryGenerator                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdStack;
class SpdPrimGenParSet;
class SpdPrimGenData;
class SpdGenerator;
class SpdDecayer; 

class SpdPrimaryGenerator: public FairPrimaryGenerator {

public:

    SpdPrimaryGenerator();
    SpdPrimaryGenerator(const char *name, const char *title);
    
    virtual ~SpdPrimaryGenerator();
    
    virtual void   Reset();
   
    virtual Bool_t Init();
   
    static void    SetPrimParSet(TString parset);
    static TString GetPrimParSet();
     
    virtual Bool_t GenerateEvent(FairGenericStack* pStack);
    
#if OLD_FAIRROOT_PRIMGEN_VERSION
    virtual void   AddTrack(Int_t pdgid, 
                            Double_t px, Double_t py, Double_t pz, /* momentum */
                            Double_t vx, Double_t vy, Double_t vz, /* vertex */
                            Int_t secondParent = -1, /* second mother */
                            Bool_t wanttracking = kTRUE,
                            Double_t e = -9e9, Double_t tof = 0., 
                            Double_t weight = 0.);  
#else
    virtual void   AddTrack(Int_t pdgid, 
                            Double_t px, Double_t py, Double_t pz, /* momentum */
                            Double_t vx, Double_t vy, Double_t vz, /* vertex */
                            Int_t secondParent = -1, /* second mother */
                            Bool_t wanttracking = kTRUE,
                            Double_t e = -9e9, Double_t tof = 0., 
                            Double_t weight = 0., TMCProcess proc = kPPrimary);
#endif  
   
    virtual void   AddPrimGenDataParticle(TParticle* part);
    
    virtual void   SetVerbose(Int_t level) { fVerboseLevel = level; }
    virtual void   SetVerboseLevel(Int_t level);
    virtual void   SetFairMode(Bool_t mode = kTRUE) { fFairMode = mode; }
    
    Bool_t  IsInit() const { return fInit; }
    
    Bool_t  GetFairMode()       const { return fFairMode; }
    Int_t   GetCurrentTrackId() const { return fCurrentTrackId; }
    
    virtual SpdGenerator* Generator(Int_t index) const;
     
    virtual Bool_t FillParsIn(SpdPrimGenParSet* params);
    virtual Bool_t LoadParsFrom(SpdPrimGenParSet* params);
    
    virtual void LoadPythia6Parameters(SpdPrimGenParSet* params);
    virtual void LoadPythia8Parameters(SpdPrimGenParSet* params);
    
    virtual void Print(Option_t* opt = "") const;
    
protected:
  
    virtual void FillPrimGenData(); 
   
    SpdPrimGenParSet* GetParameters();
    
    // parameters
  
    static TString      fPrimGenParSet;   //!
    
    Int_t               fVerboseLevel;    //!
    Bool_t              fFairMode;        //!
    Int_t               fNGenerators;     //!
    
    TRandom*            fGen;             //!
    SpdDecayer*         fExternalDecayer; //!
    
    // auxiliary members
  
    Bool_t              fInit;             //!
    SpdPrimGenData*     fCurrentGenData;   //!
    Int_t               fCurrentTrackId;   //!
     
    virtual void AddTrackX(Int_t pdgid, 
                           Double_t px, Double_t py, Double_t pz, /* momentum */
                           Double_t vx, Double_t vy, Double_t vz, /* vertex */
                           Int_t secondParent = -1, /* second mother */
                           Bool_t wanttracking = kTRUE,
                           Double_t e = -9e9, 
                           Double_t tof = 0., 
                           Double_t weight = 0., TMCProcess proc = kPPrimary);
    
    ClassDef(SpdPrimaryGenerator,1)
};

#endif  /* __SPDPRIMARYGENERATOR_H__ */

