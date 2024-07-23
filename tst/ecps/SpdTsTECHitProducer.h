// $Id$
// Author: artur   2018/02/01

#ifndef __SPDTSTECHITPRODUCER_H__
#define __SPDTSTECHITPRODUCER_H__

#include "FairTask.h"
#include "SpdField.h"
#include "SpdTsTECHit.h"
#include "SpdGeoBuilder.h"
#include "SpdMCEventHeader.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsTECHitProducer                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdTsTECHitProducer: public FairTask {
public:

    SpdTsTECHitProducer(TString params = "TsTECParSet");
    virtual ~SpdTsTECHitProducer();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    void PrintInput();
    
    inline TString GetParsName() const { return fParsName; }
   
private:
  
    SpdGeoMapper*  fGeoMapper;  // geometry mapper
    SpdGeoBuilder* fGeoBuilder; // geometry builder
    
    SpdField*      fField;      // magnetic field
   
    /* methods */
    
    void SetParameters();
    void SetField();
    
    /* data memebers */ 
    
    SpdMCEventHeader* fEventHeader; // Input event header
  
    TString       fParsName;        // Input parameters object name
    SpdParSet*    fParameters;      // Input parameters
    Int_t         fSaveDetIdOption; // Input detector's id saving option
    
    TClonesArray* fPointsArray;    //! Input  array of points 
    TClonesArray* fTracksArray;    //! Input  array of tracks
    TClonesArray* fDigitsArray;    //! Output array of digits
        
    ClassDef(SpdTsTECHitProducer,1)
};

#endif  /* __SPDTSTECHITPRODUCER_H__ */

