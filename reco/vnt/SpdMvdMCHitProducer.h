// $Id$
// Author: artur   2021/12/26

#ifndef __SPDMVDMCHITPRODUCER_H__
#define __SPDMVDMCHITPRODUCER_H__

#include "SpdTask.h"

#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>
#include <vector>
#include <map>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMvdMCHitProducer                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCSiliconHit;
class SpdGeoMapper;
class SpdGeoBuilder;
class SpdParSet;
class SpdMCEvent;
class SpdMvdPoint;

class SpdMvdMCHitProducer: public SpdTask {

public:

    SpdMvdMCHitProducer();
    virtual ~SpdMvdMCHitProducer();
  
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    // Set resolution [mkm], layerID > 1
    void SetHitResolution(Int_t layerID, Double_t resU, Double_t resV);
    
    // Save the hits into the output file 
    void SaveHits(Bool_t save = true)  { fSaveHits  = save; }
 
    void SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }
 
protected:
 
    /* methods */
 
    virtual void LoadGeometry();
    virtual void AddParameters();
     
    virtual Bool_t AcceptPoint(const SpdMvdPoint* point) const;
     
    virtual SpdMCSiliconHit* CreateMvdHit(SpdMvdPoint* p);
    virtual void AddMCTruth(SpdMvdPoint* point, SpdMCSiliconHit* hit);
    
    virtual Double_t GetHitResolutionU(Int_t layerID); 
    virtual Double_t GetHitResolutionV(Int_t layerID);
    
    virtual SpdMCSiliconHit* AddHit();
    
    /* data memebers */

    // geometry
    SpdGeoMapper*  fGeoMapper;           //! geometry mapper
    
    // input parameters
    TString        fParsName;            //! Input parameters object name
    SpdParSet*     fParameters;          //! Input parameters
    Int_t          fSaveDetIdOption;     //! Input detector's id saving option
    Bool_t         fSaveEmptyHits;       //! Input mark to save empty hits 
    TString        fDataType;            //! Input "mc-points" data type  
    
    // input data
    SpdMCEvent*    fEvent;               //! Main mc-data object
    TClonesArray*  fParticles;           //! List of mc-particles 
    TClonesArray*  fPointsArray;         //! List of mc-points
    
    // settings
    std::map<Int_t,TVector3>  fResolution;  //! hit resolution table < layer ID, value: (u,v,n) or (x,y,z) >
    
    Bool_t         fSaveHits;      //! specifies whether to save hits into an output file 
    
    // output data
    TClonesArray*  fHits;         //! list of hits 
    
    // counters
    Int_t          fNFailedHits;  //! total number of failed hits  
    Int_t          fNTotalHits;   //! total number of processed hits
     
    //----------------------
    Int_t          fVerboseLevel; //! verbose level 
     
    ClassDef(SpdMvdMCHitProducer,1)
};


#endif  /* __SPDMVDMCHITPRODUCER_H__ */

