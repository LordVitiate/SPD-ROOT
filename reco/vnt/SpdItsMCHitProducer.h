// $Id$
// Author: artur   2021/02/01

#ifndef __SPDITSMCHITPRODUCER_H__
#define __SPDITSMCHITPRODUCER_H__

#include "SpdTask.h"

#include <TString.h>
#include <TVector3.h>
#include <TClonesArray.h>
#include <vector>
#include <map>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsMCHitProducer                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCSiliconHit;
class SpdGeoMapper;
class SpdGeoBuilder;
class SpdParSet;
class SpdMCEvent;
class SpdItsPoint;

class SpdItsMCHitProducer: public SpdTask {

public:

    SpdItsMCHitProducer();
    virtual ~SpdItsMCHitProducer();
  
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    // Set hit production method: 0 (default), 1
    void SetHitProductionMetod(Int_t n);
    
    // Set resolution [mkm], spec = 's', 'm', 'n' (resU only), 'p' (resU only), 
    void SetHitResolution(Char_t spec, Double_t resU, Double_t resV);
    
    // Save the hits into the output file 
    void SaveHits(Bool_t save = true)  { fSaveHits  = save; }
 
    void SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }
 
protected:
 
    /* methods */
 
    virtual void LoadGeometry();
    virtual void AddParameters();
    
    virtual Bool_t AcceptPoint(const SpdItsPoint* point) const;
    
    virtual SpdMCSiliconHit* CreateItsHit(SpdItsPoint* p);
    virtual void AddMCTruth(SpdItsPoint* point, SpdMCSiliconHit* hit);
    
    virtual Double_t GetHitResolutionU(Char_t spec); 
    virtual Double_t GetHitResolutionV(Char_t spec);
    
    virtual SpdMCSiliconHit* AddHit();
    
    /* data memebers */

    // geometry
    SpdGeoMapper*  fGeoMapper;           //! geometry mapper
    SpdGeoBuilder* fGeoBuilder;          //! geometry builder
    
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
    std::map<Char_t,TVector3>  fResolution;          //! hit resolution table < hit type, value: (u,v,n) or (x,y,z) >
    Int_t                      fHitProductionMetod;  //! production method type
    
    Bool_t         fSaveHits;      //! specifies whether to save hits into an output file 
    
    // output data
    TClonesArray*  fHits;         //! list of hits 
    
    // counters
    Int_t          fNFailedHits;  //! total number of failed hits  
    Int_t          fNTotalHits;   //! total number of processed hits
     
    //----------------------
    Int_t          fVerboseLevel; //! verbose level 
     
    ClassDef(SpdItsMCHitProducer,1)
};

//______________________________________________________________________________
inline void SpdItsMCHitProducer::SetHitProductionMetod(Int_t n)
{
    if (n < 0) n = 0;
    else if (n > 1) n = 1;
    
    fHitProductionMetod = n;
}

#endif  /* __SPDITSMCHITPRODUCER_H__ */

