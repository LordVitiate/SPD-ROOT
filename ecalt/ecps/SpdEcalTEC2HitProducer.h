// $Id$
// Author: andre   2021/03/04

#ifndef __SPDECALTEC2HITPRODUCER_H__
#define __SPDECALTEC2HITPRODUCER_H__

#include "FairTask.h"

#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTEC2HitProducer                                                     //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdEcalTEC2HitProducer: public FairTask {

public:

    SpdEcalTEC2HitProducer(TString params = "EcalTECParSet");
    virtual ~SpdEcalTEC2HitProducer();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    inline TString GetParsName() const { return fParsName; }
   
private:
  
    SpdGeoMapper*  fGeoMapper;  // geometry mapper
 
    /* methods */
    
    void SetParameters();
    Double_t CellEnergyCalibration(Double_t e);
    
    /* data memebers */ 
    
    SpdMCEventHeader* fEventHeader; // Input event header
  
    TString       fParsName;        // Input parameters object name
    TString       fAbsorberVolName;      
    TString       fScintVolName;       
    SpdParSet*    fParameters;      // Input parameters
    Int_t         fSaveDetIdOption; // Input detector's id saving option
    
    TClonesArray* fPointsArray;    //! Input  array of points 
    TClonesArray* fTracksArray;    //! Input  array of tracks
    TClonesArray* fDigitsArray;    //! Output array of digits
        
    ClassDef(SpdEcalTEC2HitProducer,1)
};

#endif  /* __SPDECALTEC2HITPRODUCER_H__ */

