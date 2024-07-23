// $Id$
// Author: artur   2018/12/05

#ifndef __SPDGENERATOR_H__
#define __SPDGENERATOR_H__

#include <vector>
#include <TString.h>
#include <TRandom.h>
#include "FairGenerator.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGenerator                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdPrimGenData;
class SpdPrimGenParSet;
class TClonesArray;

class SpdGenerator: public FairGenerator {

public:

    SpdGenerator();   
    SpdGenerator(const char* Name);
   
    virtual ~SpdGenerator();
    
    virtual void Reset();
    virtual void RemoveGenData();
    
    virtual Bool_t Init() { return kTRUE; } //-> /* !must be overloaded! */
    
    virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);  //-> 
    
    virtual Bool_t AddTrack(FairPrimaryGenerator* primGen, Bool_t AddToStack, 
                            Int_t pdg, Int_t num, Int_t parent_num, 
                            Double_t px, Double_t py, Double_t pz, /* momentum */
                            Double_t vx = 0., Double_t vy = 0., Double_t vz = 0., /* vertex */
                            Bool_t wanttracking = kTRUE,
                            Double_t e = -9e9, Double_t time = 0., Double_t weight = 0.);
    
    virtual void  SetSeeds(const std::vector<UInt_t>& /*seeds*/) {}        //-> 
    virtual void  GetSeeds(std::vector<UInt_t>& seeds) { seeds.clear(); }  //-> 
    
    virtual FairGenerator* CloneGenerator() const;
    
    virtual void  SetVerboseLevel(Int_t verb) { fVerboseLevel = verb; }
    
    virtual void  SetGenIndex(Int_t idx)           { fGenIndex = idx; }
    virtual void  SetGenData(SpdPrimGenData* data) { fGenData = data; }
    
    static  Int_t GetNTracks() { return fNTracks; }
    
    virtual Int_t GetVerboseLevel() const { return fVerboseLevel; }
    
    virtual Int_t           GetGenIndex() const { return fGenIndex; }
    virtual SpdPrimGenData* GetGenData()  const { return fGenData;  }
    
    virtual Bool_t IsDecayerEnable() const { return kFALSE; }
       
    virtual Bool_t FillParsIn(SpdPrimGenParSet* params);    //->
    virtual Bool_t LoadParsFrom(SpdPrimGenParSet* params);  //-> 
    
    virtual Bool_t CreateAsciiFile(Int_t nevents, TString filename, TString path = "", TString fmt = "std");
    virtual Bool_t CreateRootFile(Int_t nevents, TString filename, TString path = "", TString fmt = "std");
    
    static  void ResetTracksCounter() { fNTracks = 0; }
    
protected:

    Int_t            fGenIndex;      //! Generator id in the Primary Generator list
    SpdPrimGenData*  fGenData;       //! Generator data (outer)
    Int_t            fVerboseLevel;  //! Verbose Level
    
    TRandom*         fGen;           //! Simple inner generator
  
    TClonesArray*    fEvent;         //! Current set of tracks (for data "save" mode only)
    
private:
  
    static Int_t     fNTracks;  //!
    
    ClassDef(SpdGenerator,1)
};

#endif  /* __SPDGENERATOR_H__ */

