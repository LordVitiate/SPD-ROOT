// $Id$
// Author: artur   2021/29/11

#ifndef __SPDBBCMCHITPRODUCER_H__
#define __SPDBBCMCHITPRODUCER_H__

#include <TObject.h>

#include "SpdTask.h"

#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdBbcMCHitProducer                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdBbcPoint;
class SpdMCEvent;
class SpdBbcMCHit;
class SpdGeoMapper;
class SpdParSet;

class SpdBbcMCHitProducer: public SpdTask {

public:

    SpdBbcMCHitProducer();
    virtual ~SpdBbcMCHitProducer();
    
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
    
    template <class T> Bool_t AcceptPoint(const T* p) const;
    
    virtual void   AddMCTruth(Int_t itrack, SpdBbcMCHit* hit);
    
    virtual SpdBbcMCHit* AddHit();
    
    /* Data memebers */ 
    
    // geometry
    SpdGeoMapper*  fGeoMapper;         //! geometry mappers
 
    // input parameters 
    TString        fParsName;          //! Input parameters object name
    SpdParSet*     fParameters;        //! Input parameters
    Int_t          fSaveDetIdOption;   //! Input detector's id saving option
    Bool_t         fSaveEmptyHits;     //! Input mark to save empty hits 
    TString        fDataType;          //! Input "mc-points" data type  
    
    // input data
    SpdMCEvent*                fEvent;        //! Main mc-data object
    TClonesArray*              fParticles;    //! List of mc-particles 
    const std::vector<Int_t>*  fIdTable;      //! old mc-track id -> new particle id
    TClonesArray*              fPointsArray;  //! List mc-points

    // output data
    TClonesArray*   fHits;             //! List of hits 
    
    // settings
    Bool_t          fSaveHits;         //! specifies whether to save hits into an output file 

    // counters
    Int_t           fNFailedHits;      //! total number of failed hits  
    Int_t           fNTotalHits;       //! total number of processed hits
    
    //-----------------------
    Int_t           fVerboseLevel;     //! verbose level 
  
    ClassDef(SpdBbcMCHitProducer,1)
};

#endif  /* __SPDBBCMCHITPRODUCER_H__ */

