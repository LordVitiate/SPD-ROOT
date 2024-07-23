// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTECHITPRODUCER_H__
#define __SPDECALTECHITPRODUCER_H__

#include "FairTask.h"

#include "SpdEcalTECHit.h"
#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTECHitProducer                                                      //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdEcalTECHitProducer: public FairTask {

public:

    SpdEcalTECHitProducer(TString params = "EcalTECParSet");
    virtual ~SpdEcalTECHitProducer();
    
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
        
    ClassDef(SpdEcalTECHitProducer,1)
};

#endif  /* __SPDECALTECHITPRODUCER_H__ */

