// $Id$
// Author: artur   2018/02/01

#ifndef __SPDTSSBHITPRODUCER_H__
#define __SPDTSSBHITPRODUCER_H__

#include "FairTask.h"
#include "SpdField.h"
#include "SpdTsSBHit.h"
#include "SpdTsSBGeoMapper.h"
#include "SpdTsSBGeoBuilder.h"
#include "SpdMCEventHeader.h"

class TClonesArray;
class SpdMCTrack;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsSBHitProducer                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsSBHitProducer: public FairTask {

public:

    SpdTsSBHitProducer(TString params = "TsSBParSet");
    virtual ~SpdTsSBHitProducer();
    
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
        
    ClassDef(SpdTsSBHitProducer,1)
};

#endif  /* __SPDTSSBHITPRODUCER_H__ */

