// $Id$
// Author: artur   2018/02/01

#ifndef __SPDRSSBHITPRODUCER_H__
#define __SPDRSSBHITPRODUCER_H__

#include "FairTask.h"

#include "SpdRsSBHit.h"
#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsSBHitProducer                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdRsSBHitProducer: public FairTask {

public:

    SpdRsSBHitProducer(TString params = "RsSBParSet");
    virtual ~SpdRsSBHitProducer();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    inline TString GetParsName() const { return fParsName; }
    
private:
  
    SpdGeoMapper*  fGeoMapper;  // geometry mapper
 
    /* methods */
    
    void SetParameters();
    
    /* data memebers */ 
    
    SpdMCEventHeader* fEventHeader; // Input event header
  
    TString       fParsName;        // Input parameters object name
    SpdParSet*    fParameters;      // Input parameters
    Int_t         fSaveDetIdOption; // Input detector's id saving option
    
    TClonesArray* fPointsArray;    //! Input  array of points 
    TClonesArray* fTracksArray;    //! Input  array of tracks
    TClonesArray* fDigitsArray;    //! Output array of digits    
        
    ClassDef(SpdRsSBHitProducer,1)
};

#endif  /* __SPDRSSBHITPRODUCER_H__ */

