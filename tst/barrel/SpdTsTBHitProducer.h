// $Id$
// Author: artur   2018/02/01

#ifndef __SPDTSTBHITPRODUCER_H__
#define __SPDTSTBHITPRODUCER_H__

#include "FairTask.h"
#include "SpdField.h"
#include "SpdTsTBHit.h"
#include "SpdGeoBuilder.h"
#include "SpdMCEventHeader.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsTBHitProducer                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdTsTBHitProducer: public FairTask {

public:

    SpdTsTBHitProducer(TString params = "TsTBParSet");
    virtual ~SpdTsTBHitProducer();
    
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
        
    ClassDef(SpdTsTBHitProducer,1)
};

#endif  /* __SPDTSTBHITPRODUCER_H__ */

