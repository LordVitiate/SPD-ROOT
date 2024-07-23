// $Id$
// Author: artur   2021/03/02

#ifndef __SPDECALMCHITPRODUCER_H__
#define __SPDECALMCHITPRODUCER_H__

#include "SpdTask.h"

#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalMCHitProducer                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdEcalBPoint;
class SpdEcalECPoint;
class SpdMCEvent;
class SpdEcalMCHit;
class SpdGeoMapper;
class SpdParSet;

class SpdEcalMCHitProducer: public SpdTask {

public:

    SpdEcalMCHitProducer();
    virtual ~SpdEcalMCHitProducer();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    // Save the hits into the output file 
    void SaveHits(Bool_t save = true)  { fSaveHits = save; }

    void SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }

protected:
    
    /* Methods */
    
    virtual void   LoadGeometry();
    virtual void   AddParameters();
    
    virtual Bool_t AcceptPointB(const SpdEcalBPoint* point) const;  // barrel
    virtual Bool_t AcceptPointE(const SpdEcalECPoint* point) const; // endcaps
         
    virtual void   AddMCTruth(Int_t itrack, SpdEcalMCHit* hit, Double_t edep);
    
    virtual SpdEcalMCHit* AddHit();
    
    Double_t CalibrateEnergy(Double_t e) const;
    
    /* Data memebers */ 
    
    // geometry
    SpdGeoMapper*  fGeoMapper[2];         //! geometry mappers
 
    // input parameters 
    TString        fParsName[2];          //! Input parameters object name
    SpdParSet*     fParameters[2];        //! Input parameters
    Int_t          fSaveDetIdOption[2];   //! Input detector's id saving option
    Bool_t         fSaveEmptyHits[2];     //! Input mark to save empty hits 
    TString        fDataType[2];          //! Input "mc-points" data type  
    TString        fAbsorberVolName[2];   //! Absorber volume name (passive cell part) 
    TString        fScintVolName[2];      //! Scintillator volume name (active cell part) 
    
    // input data 
    SpdMCEvent*                fEvent;           //! Main mc-data object
    TClonesArray*              fParticles;       //! List of mc-particles 
    const std::vector<Int_t>*  fIdTable;         //! old mc-track id -> new particle id 
    TClonesArray*              fPointsArray[2];  //! List mc-points
 
    // output data
    TClonesArray*   fHits;            //! List of hits 
    
    // settings 
    Bool_t          fSaveHits;        //! specifies whether to save hits into an output file 

    // counters
    Int_t           fNFailedHits;     //! total number of failed hits  
    Int_t           fNTotalHits;      //! total number of processed hits
    
    //-------------------------------
    Int_t           fVerboseLevel;    //! verbose level 
  
    ClassDef(SpdEcalMCHitProducer,1)
};

#endif  /* __SPDECALMCHITPRODUCER_H__ */

