// $Id$
// Author: artur   2019/09/16

#ifndef __SPDIDEALTRACKFINDER_H__
#define __SPDIDEALTRACKFINDER_H__

#include <TString.h>
#include <TMath.h>
#include <vector>
#include "FairTask.h"
#include "SpdGeoLoader.h"
#include "SpdIdealHit.h"
#include "SpdBaseParSet.h"

class SpdMCEventHeader;
class SpdGeoLoader;
class SpdGeoBuilder;
class SpdGeoMapper;
class SpdIdealTrackColl;
class SpdIdealTrack;
class SpdMCTrack;
class FairMCPoint;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdIdealTrackFinder                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdIdealTrackFinder: public FairTask {

public:

    SpdIdealTrackFinder();
    virtual ~SpdIdealTrackFinder();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    void DeleteGeoLoader();
    
    void SetTrackingGeoModules(Bool_t add_passives = kFALSE);
    
    void AddModule(TString module) { fGeoModules.push_back(module);  }
    void AddModule(Int_t module)   { fGeoModules.push_back(Form("%d",module)); }
    void ClearListOfGeoModules()   { fGeoModules.clear(); }
    
    inline Double_t GetMinMomentum()     const { return fMinMomentum[1];     }
    inline Double_t GetMinGammaBeta()    const { return fMinGammaBeta[1];    }
    inline Int_t    GetMinVertexNHits()  const { return fMinVertexNHits[1];  }
    inline Int_t    GetMinTrackerNHits() const { return fMinTrackerNHits[1]; }
           Bool_t   FindSelectedTrack(Int_t eid, Int_t tid) const;
     
    inline void AppendPrimaryTracksOnly(Bool_t prim = true)  { fPrimVertexOnly = prim; }      
    inline void AppendSecondaryTracks(Bool_t sec = true)     { fPrimVertexOnly = !sec; }        
           
    inline void SetMinMomentum(Double_t pmin);
    inline void SetMinGammaBeta(Double_t gbmin);
    inline void SetMinVertexNHits(Int_t nhits);
    inline void SetMinTrackerNHits(Int_t nhits);
    
    virtual void     SetHitResolution(Int_t det, Char_t spec, Double_t res1, Double_t res2 = -1, Double_t res3 = -1);
    virtual TVector3 GetHitResolution(Int_t det, Char_t spec);
  
    virtual void AddTrackAsSelected(Int_t event /* >=1 */, Int_t track /*>= 0*/); 
           
    virtual void PrintInput() const;
    
    struct IdealTrackFinderInfo 
    {
        IdealTrackFinderInfo():mapper(0),builder(0),points(0),datainfo(-1),htype(kSpacepoint) { }
        
        SpdGeoMapper*  mapper;
        SpdGeoBuilder* builder;
        TClonesArray*  points;
        Int_t          datainfo;
        SpdHitType     htype;
    };
    
    typedef IdealTrackFinderInfo ITF_info;
    
    void FillParametersIn(SpdBaseParSet* params);
    void LoadParametersFrom(SpdBaseParSet* params);
    
    inline Int_t GetNErrors()     const { return fNErrors; }
    inline Int_t GetNFailedHits() const { return fNFailedHits; }
    
    inline void SetAbsMinMomentum(Double_t pmin);
    inline void SetAbsMinGammaBeta(Double_t gbmin);
    inline void SetAbsMinVertexNHits(Int_t nhits);
    inline void SetAbsMinTrackerNHits(Int_t nhits);
    
protected:

    /* methods */
    
    void LoadGeometry();
    
    virtual SpdIdealTrack* CreateTrack(Int_t id, SpdMCTrack* mctrack, Bool_t& accept_track, Int_t& selmark);
    virtual SpdIdealHit*   CreateHit(FairMCPoint* point, Int_t DetId, ITF_info* info);
    
    ITF_info* FindInfo(Int_t id);
    
    SpdIdealHit* CreateItsHit(FairMCPoint* point, ITF_info* info);
    SpdIdealHit* CreateTsTBHit(FairMCPoint* point, ITF_info* info);
    SpdIdealHit* CreateTsTECHit(FairMCPoint* point, ITF_info* info);
    
    /* Geometry */
    
    Bool_t               fOwnGeoLoader;   //! indicates geometry loader owning
    SpdGeoLoader*        fGeoLoader;      //! SPD geometry
    std::vector<TString> fGeoModules;     //! geometry modules are to be built
  
    /* data memebers */ 
     
    SpdMCEventHeader*    fEventHeader;    //! Input event header
    TClonesArray*        fMCTracksArray;  //! Input array of mc-tracks
    
    std::map<Int_t,IdealTrackFinderInfo*> fInputData; //! Input points < detector id, points array >
    
    SpdIdealTrackColl* fEvent;            //! Output data 
    SpdBaseParSet*     fParameters;       //! List of settings (will be saved into output)
    
    // cuts [abs. min. value (default value), actual value >= abs. min. value]
    
    Double_t fMinMomentum[2];     // min. momentum (p), GeV
    Double_t fMinGammaBeta[2];    // min. p/m, GeV
    Int_t    fMinVertexNHits[2];  // number of hits in ITS
    Int_t    fMinTrackerNHits[2]; // total number of hits in TSTB+TSTEC

    Bool_t   fPrimVertexOnly;     // if true, select tracks which belong to primary vertex only (default = true)

    std::map< Int_t, std::set<Int_t> > fTracksSelected; // list of selected tracks (id's) <event, set of tracks>
    
    Int_t    fNErrors;
    Int_t    fNFailedHits;
    
    TString DetResKey(Int_t d, Char_t s) { return Form("Det%d_%c",d,s); }
    std::map< TString, TVector3 > fResolution; // hit's resolution table
    
    ClassDef(SpdIdealTrackFinder,1)
};

//_____________________________________________________________________________
void SpdIdealTrackFinder::SetMinMomentum(Double_t pmin)
{
    fMinMomentum[1] = TMath::Max(fMinMomentum[0],pmin);
}

//_____________________________________________________________________________
void SpdIdealTrackFinder::SetMinGammaBeta(Double_t gbmin)
{
    fMinGammaBeta[1] = TMath::Max(fMinGammaBeta[0],gbmin);
}

//_____________________________________________________________________________
void SpdIdealTrackFinder::SetMinVertexNHits(Int_t nhits)
{
    fMinVertexNHits[1] = TMath::Max(fMinVertexNHits[0],nhits);
}

//_____________________________________________________________________________
void SpdIdealTrackFinder::SetMinTrackerNHits(Int_t nhits)
{
    fMinTrackerNHits[1] = TMath::Max(fMinTrackerNHits[0],nhits);
}

//--------------------------------------------
// !!! Absolute min. cut value setters !!!
//--------------------------------------------


//_____________________________________________________________________________
void SpdIdealTrackFinder::SetAbsMinMomentum(Double_t pmin)
{
    !(pmin > 0) ? fMinMomentum[0] = pmin :  fMinMomentum[0] = 0;
}

//_____________________________________________________________________________
void SpdIdealTrackFinder::SetAbsMinGammaBeta(Double_t gbmin)
{
    !(gbmin > 0) ? fMinGammaBeta[0] = gbmin : fMinGammaBeta[0] = 0;
}

//_____________________________________________________________________________
void SpdIdealTrackFinder::SetAbsMinVertexNHits(Int_t nhits)
{    
    !(nhits < 0) ? fMinVertexNHits[0] = nhits : fMinVertexNHits[0] = 0;
}

//_____________________________________________________________________________
void SpdIdealTrackFinder::SetAbsMinTrackerNHits(Int_t nhits)
{
    !(nhits < 0) ? fMinTrackerNHits[0] = nhits :  fMinTrackerNHits[0] = 0;
}

#endif  /* __SPDIDEALTRACKFINDER_H__ */

