// $Id$
// Author: andre   2021/03/04

#ifndef __SPDECALTB2HITPRODUCER_H__
#define __SPDECALTB2HITPRODUCER_H__

#include "FairTask.h"

#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTB2HitProducer                                                      //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdEcalTB2HitProducer: public FairTask {

public:

    SpdEcalTB2HitProducer(TString params = "EcalTBParSet");
    virtual ~SpdEcalTB2HitProducer();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    inline TString GetParsName() const { return fParsName; }
    
private:
  
    SpdGeoMapper*  fGeoMapper;  // geometry mapper
    
    void SetParameters();
    Double_t CellEnergyCalibration(Double_t e);
    void WriteFilteredTracks();
    
    SpdMCEventHeader* fEventHeader; // Input event header
  
    TString       fParsName;        // Input parameters object name
    TString       fAbsorberVolName;      
    TString       fScintVolName;       
    SpdParSet*    fParameters;      // Input parameters
    Int_t         fSaveDetIdOption; // Input detector's id saving option
    
    TClonesArray* fPointsArray;    //! Input  array of points 
    TClonesArray* fTracksArray;    //! Input  array of tracks
    TClonesArray* fTracksFilteredArray;    //! Output (filtered)  array of tracks
    TClonesArray* fDigitsArray;    //! Output array of digits
        
    ClassDef(SpdEcalTB2HitProducer,1)
};

#endif  /* __SPDECALTB2HITPRODUCER_H__ */

