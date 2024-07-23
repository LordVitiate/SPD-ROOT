// $Id$
// Author: artur   2018/02/01

#ifndef __SPDECALTPOINTAN_H__
#define __SPDECALTPOINTAN_H__

#include "FairTask.h"

#include "SpdField.h"
#include "SpdParSet.h"
#include "SpdGeoMapper.h"
#include "SpdMCEventHeader.h"
#include "SpdEcalTBHit.h"
#include "SpdEcalTECHit.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdEcalTPointAn                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TClonesArray;

class SpdEcalTPointAn: public FairTask {

public:

    SpdEcalTPointAn();
    virtual ~SpdEcalTPointAn();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    void PrintInput();
    
    inline void SetParsNameB(TString name)  { fParsNameB = name;  }
    inline void SetParsNameEC(TString name) { fParsNameEC = name; }
    
    inline TString GetParsNameB()  const { return fParsNameB;  }
    inline TString GetParsNameEC() const { return fParsNameEC; }
    
private:
  
    SpdGeoMapper*  fGeoMapperB;  // barrel geometry mapper
    SpdGeoMapper*  fGeoMapperEC; // endcaps geometry mapper
    
    SpdField*      fField;       // magnetic field
   
    /* methods */
    
    void SetParameters();
    void SetField();
        
    /* data memebers */
   
    SpdMCEventHeader* fEventHeader;   //! Input event header
    
    TString       fParsNameB;         //! Input parameters object name (barrel)
    TString       fParsNameEC;        //! Input parameters object name (endcaps)
    
    SpdParSet*    fParametersB;       //! Input parameters (barrel)
    SpdParSet*    fParametersEC;      //! Input parameters (endcaps)
    
    Int_t         fSaveDetIdOptionB;  //! Input parameters detector's id saving option (barrel)
    Int_t         fSaveDetIdOptionEC; //! Input parameters detector's id saving option (endcaps)
   
    TClonesArray* fTracksArray;       //! Input  array of tracks
    
    TClonesArray* fPointsArrayB;      //! Input  array of points (barrel)
    TClonesArray* fDigitsArrayB;      //! Output array of digits (barrel)
    
    TClonesArray* fPointsArrayEC;     //! Input  array of points (endcaps) 
    TClonesArray* fDigitsArrayEC;     //! Output array of digits (endcaps)
           
    ClassDef(SpdEcalTPointAn,1)
};

#endif  /* __SPDECALTPOINTAN_H__ */

