// $Id$
// Author: artur   2021/09/10

#ifndef __SPDZDCMCHITPRODUCER_H__
#define __SPDZDCMCHITPRODUCER_H__

#include <TObject.h>

#include "SpdTask.h"

#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdZdcMCHitProducer                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdZdcPoint;
class SpdMCEvent;
class SpdZdcMCHit;
class SpdGeoMapper;
class SpdParSet;

class SpdZdcMCHitProducer: public SpdTask {

public:

    SpdZdcMCHitProducer();
    virtual ~SpdZdcMCHitProducer();
    
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
    
    virtual void   AddMCTruth(Int_t itrack, SpdZdcMCHit* hit);
    
    virtual SpdZdcMCHit* AddHit();
    
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
  
    ClassDef(SpdZdcMCHitProducer,1)
};

#endif  /* __SPDZDCMCHITPRODUCER_H__ */

