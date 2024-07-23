// $Id$
// Author: artur   2021/02/01

#ifndef __SPDTSMCHITPRODUCER_H__
#define __SPDTSMCHITPRODUCER_H__

#include "SpdTask.h"

#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>
#include <vector>
#include <map>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsMCHitProducer                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsTBPoint;
class SpdTsTECPoint;
class SpdMCEvent;
class SpdMCTrackHit;
class SpdMCStrawHit1D;
class SpdMCStrawHit2D;
class SpdGeoMapper;
class SpdGeoBuilder;
class SpdParSet;

class SpdTsMCHitProducer: public SpdTask {

public:

    SpdTsMCHitProducer();
    virtual ~SpdTsMCHitProducer();
  
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    // Set resolution [mkm]; detPart = 0(barrel), 1(endcap); spec = 'v', 'w', 'p', 'n'
    void SetHitResolution(Int_t detPart, Char_t spec, Double_t resR, Double_t resZ = 1.e5);
    
    // Set hit type ('v'(1D) or 'w'(2D)) for the both, barrel and endcaps
    void SetHitType(Char_t spec);
    
    // Save the hits into the output file 
    void SaveHits(Bool_t save = true)  { fSaveHits  = save; }

    void SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }

protected:
    
    /* Methods */
    
    virtual void LoadGeometry();
    virtual void AddParameters();
     
    virtual Bool_t AcceptPointB(const SpdTsTBPoint* point) const;  // barrel
    virtual Bool_t AcceptPointE(const SpdTsTECPoint* point) const; // endcaps
    
    template <class T> SpdMCStrawHit1D* CreateTsHit1D(T* p, Int_t detPart);
    template <class T> SpdMCStrawHit2D* CreateTsHit2D(T* p, Int_t detPart);
    
    virtual void AddMCTruth(Int_t itrack, SpdMCTrackHit* hit);
    
    virtual Double_t GetHitResolutionR(Int_t detPart, Char_t spec); 
    virtual Double_t GetHitResolutionZ(Int_t detPart, Char_t spec);
    
    virtual SpdMCStrawHit1D* AddHit1D();
    virtual SpdMCStrawHit2D* AddHit2D();
    
    /* Data memebers */ 
    
    // geometry
    SpdGeoMapper*   fGeoMapper[2];        //! geometry mappers
    SpdGeoBuilder*  fGeoBuilder[2];       //! geometry builders
    
    // input parameters 
    TString         fParsName[2];         //! Input parameters object name
    SpdParSet*      fParameters[2];       //! Input parameters
    Int_t           fSaveDetIdOption[2];  //! Input detector's id saving option
    Bool_t          fSaveEmptyHits[2];    //! Input mark to save empty hits 
    TString         fDataType[2];         //! Input "mc-points" data type  
    
    // input data
    SpdMCEvent*     fEvent;               //! Main mc-data object
    TClonesArray*   fParticles;           //! List of mc-particles 
    TClonesArray*   fPointsArray[2];      //! list mc-points 
    
    // output data
    TClonesArray*   fHits;                //! list of hits 
    
    // settings
    std::map<TString,TVector3> fResolution;  //! hit resolution table < hit type, value: (u,v,n) or (x,y,z) >
    Char_t                     fHitType;     //! hit type: 'v'(1D=R) or 'w'(2D=R,Z)
    
    Bool_t          fSaveHits;            //! specifies whether to save hits into an output file 
    
    // counters
    Int_t           fNFailedHits;     //! total number of failed hits  
    Int_t           fNTotalHits;      //! total number of processed hits
    
    //-----------------
    Int_t           fVerboseLevel;    //! verbose level 
    
    TString DetResKey(Int_t d, Char_t s) { return Form("dp%d_s%c",d,s); }
    
    ClassDef(SpdTsMCHitProducer,1)
};

#endif  /* __SPDTSMCHITPRODUCER_H__ */

