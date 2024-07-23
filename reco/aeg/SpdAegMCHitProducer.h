// $Id$
// Author: artur   2021/29/11

#ifndef __SPDAEGMCHITPRODUCER_H__
#define __SPDAEGMCHITPRODUCER_H__

#include <TObject.h>

#include "SpdTask.h"

#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdAegMCHitProducer                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdAegPoint;
class SpdMCEvent;
class SpdAegMCHit;
class SpdGeoMapper;
class SpdParSet;

class SpdAegMCHitProducer: public SpdTask {

public:

    SpdAegMCHitProducer();
    virtual ~SpdAegMCHitProducer();
    
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
    
    virtual void   AddMCTruth(Int_t itrack, SpdAegMCHit* hit);
    
    virtual SpdAegMCHit* AddHit();
    
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
  
    ClassDef(SpdAegMCHitProducer,1)
};

#endif  /* __SPDAEGMCHITPRODUCER_H__ */

