// $Id$
// Author: alver   2020/07/19

#ifndef __SPDRSTEC2HITPRODUCER_H__
#define __SPDRSTEC2HITPRODUCER_H__

#include "SpdTask.h"

#include "SpdRsTEC2Hit.h"
#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"
#include <TGeoNavigator.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRsTEC2HitProducer                                                       //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdRsTEC2HitProducer: public SpdTask {
    
public:

    SpdRsTEC2HitProducer(TString params = "RsTECParSet");
    virtual ~SpdRsTEC2HitProducer();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    inline TString GetParsName() const { return fParsName; }

    SpdRsTEC2Hit* AddHit(Int_t detID, TVector3& pos, std::vector<Int_t> pointList);
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
        
    ClassDef(SpdRsTEC2HitProducer,1)
};

#endif  /* __SPDRSTEC2HITPRODUCER_H__ */

