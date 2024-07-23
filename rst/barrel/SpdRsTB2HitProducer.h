// $Id$
// Author: alver   2020/07/19

#ifndef __SPDRSTB2HITPRODUCER_H__
#define __SPDRSTB2HITPRODUCER_H__

#include "SpdTask.h"

#include "SpdRsTB2Hit.h"
#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"
#include <TGeoNavigator.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTB2HitProducer                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdRsTB2HitProducer: public SpdTask {
    
public:

    SpdRsTB2HitProducer(TString params = "RsTBParSet");
    virtual ~SpdRsTB2HitProducer();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    inline TString GetParsName() const { return fParsName; }

    SpdRsTB2Hit* AddHit(Int_t detID, TVector3& pos, std::vector<Int_t> pointList);
    Bool_t DetectorPosition(Double_t* p, TString node_path);
    
private:
  
    SpdGeoMapper*  fGeoMapper;      // geometry mapper
    TGeoNavigator* fGeoNavigator;   // geometry navigator
 
    /* methods */
    
    void SetParameters();
    
    /* data memebers */ 
    
    SpdMCEventHeader* fEventHeader; // Input event header
  
    TString       fParsName;        // Input parameters object name
    SpdParSet*    fParameters;      // Input parameters
    Int_t         fSaveDetIdOption; // Input detector's id saving option
    
    TClonesArray* fPointsArray;     //! Input  array of points 
    TClonesArray* fTracksArray;     //! Input  array of tracks
    TClonesArray* fHitsArray;       //! Output array of digits
        
    ClassDef(SpdRsTB2HitProducer,1)
};

#endif  /* __SPDRSTB2HITPRODUCER_H__ */

