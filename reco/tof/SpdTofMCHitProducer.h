// $Id$
// Author: artur   2021/09/10

#ifndef __SPDTOFMCHITPRODUCER_H__
#define __SPDTOFMCHITPRODUCER_H__

#include "SpdTask.h"

#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTofMCHitProducer                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTofBPoint;
class SpdTofECPoint;
class SpdMCEvent;
class SpdTofMCHit;
class SpdGeoMapper;
class SpdParSet;

class SpdTofMCHitProducer: public SpdTask {

public:

    SpdTofMCHitProducer();
    virtual ~SpdTofMCHitProducer();
    
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
    
    virtual void   AddMCTruth(Int_t itrack, SpdTofMCHit* hit);
    
    virtual SpdTofMCHit* AddHit();
    
    /* Data memebers */ 
    
    // geometry
    SpdGeoMapper*  fGeoMapper[2];         //! geometry mappers
 
    // input parameters 
    TString        fParsName[2];          //! Input parameters object name
    SpdParSet*     fParameters[2];        //! Input parameters
    Int_t          fSaveDetIdOption[2];   //! Input detector's id saving option
    Bool_t         fSaveEmptyHits[2];     //! Input mark to save empty hits 
    TString        fDataType[2];          //! Input "mc-points" data type  
    
    // input data
    SpdMCEvent*                fEvent;           //! Main mc-data object
    TClonesArray*              fParticles;       //! List of mc-particles 
    const std::vector<Int_t>*  fIdTable;         //! old mc-track id -> new particle id
    TClonesArray*              fPointsArray[2];  //! List mc-points

    // output data
    TClonesArray*   fHits;             //! List of hits 
    
    // settings
    Bool_t          fSaveHits;         //! specifies whether to save hits into an output file 

    // counters
    Int_t           fNFailedHits;      //! total number of failed hits  
    Int_t           fNTotalHits;       //! total number of processed hits
    
    //-----------------------
    Int_t           fVerboseLevel;     //! verbose level 
  
    ClassDef(SpdTofMCHitProducer,1)
};

#endif  /* __SPDTOFMCHITPRODUCER_H__ */

