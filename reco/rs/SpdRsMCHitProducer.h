// $Id$
// Author: artur   2021/03/02

#ifndef __SPDRSMCHITPRODUCER_H__
#define __SPDRSMCHITPRODUCER_H__

#include <TObject.h>

#include "SpdTask.h"

#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>

/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
// SpdRsMCHitProducer                                                          //
//                                                                             //
// The task produces hits in RS.                                               //                                                                        //                                                                             //
// Main settings:                                                              //
//                                                                             //
// CellMinTrackLen - min. track length inside the cell                         //
//                   to produce a signal, [cm]                                 //
// CellMinTrackDep - min. track deposit inside the cell                        //
//                   to produce a signal, [keV]                                //
// MakeStripHits   - specifies whether to create STRIP hits (default = false)  // 
//                   There are two options for hit production (use             //
//                   method SaveStripHits): = true (only wires),               //
//                   = false (hits from wires and strips; strip hits           //
//                   are “ideal” i.e. every wire-hit has one strip-hit.        //
//                                                                             //
// SaveHits        - specifies whether to save hits into an output file        //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

class SpdRsTB2Point;
class SpdRsTEC2Point;
class SpdMCEvent;
class SpdRsMCHit;
class SpdGeoMapper;
class SpdParSet;

class SpdRsMCHitProducer: public SpdTask {

public:

    SpdRsMCHitProducer();
    virtual ~SpdRsMCHitProducer();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    void SetCellMinTrackLen(Double_t len /*cm*/)  { fCellMinTrackLen = len; }
    void SetCellMinTrackDep(Double_t dep /*keV*/) { fCellMinTrackDep = dep; }
    
    // Calculate the strip hits 
    void MakeStripHits(Bool_t make = true) { fMakeStripHits = make; }

    // Save the hits into the output file 
    void SaveHits(Bool_t save = true)  { fSaveHits = save; }
    
    void SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }

protected:
    
    /* Methods */
    
    virtual void   LoadGeometry();
    virtual void   AddParameters();
    
    template <class T> Bool_t AcceptPoint(const T* p) const;
    
    virtual void   AddMCTruth(Int_t itrack, SpdRsMCHit* hit);
    
    virtual SpdRsMCHit* AddHit();
    
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
    Double_t        fCellMinTrackLen;  //! min. track length inside the cell to produce a signal, [cm] 
    Double_t        fCellMinTrackDep;  //! min. track deposit inside the cell to produce a signal, [keV]
 
    Bool_t          fSaveHits;         //! specifies whether to save hits into an output file (default: true)
    Bool_t          fMakeStripHits;    //! specifies whether to create STRIP hits  (default: false)

    // counters
    Int_t           fNFailedHits;      //! total number of failed hits  
    Int_t           fNTotalHits;       //! total number of processed hits
    
    //-----------------------
    Int_t           fVerboseLevel;     //! verbose level 
  
    ClassDef(SpdRsMCHitProducer,1)
};

#endif  /* __SPDRSMCHITPRODUCER_H__ */

