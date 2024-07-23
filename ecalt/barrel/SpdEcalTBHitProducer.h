// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTBHITPRODUCER_H__
#define __SPDECALTBHITPRODUCER_H__

#include "FairTask.h"

#include "SpdEcalTBHit.h"
#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTBHitProducer                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdEcalTBHitProducer: public FairTask {

public:

    SpdEcalTBHitProducer(TString params = "EcalTBParSet");
    virtual ~SpdEcalTBHitProducer();
    
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
        
    ClassDef(SpdEcalTBHitProducer,1)
};

#endif  /* __SPDECALTBHITPRODUCER_H__ */

