// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALSECHITPRODUCER_H__
#define __SPDECALSECHITPRODUCER_H__

#include "FairTask.h"
#include "SpdEcalSECHit.h"
#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalSECHitProducer                                                      //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdEcalSECHitProducer: public FairTask {

public:

    SpdEcalSECHitProducer(TString params = "EcalSECParSet");
    virtual ~SpdEcalSECHitProducer();
    
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
        
    ClassDef(SpdEcalSECHitProducer,1)
};

#endif  /* __SPDECALSECHITPRODUCER_H__ */

