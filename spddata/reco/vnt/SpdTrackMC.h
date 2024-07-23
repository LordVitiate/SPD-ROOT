// $Id$
// Author: artur   2020/10/01

#ifndef __SPDTRACKMC_H__
#define __SPDTRACKMC_H__

#include <TObject.h>
#include <vector>
#include "SpdTrackFitPar.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTrackMC                                                                 //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

typedef std::pair<Int_t,Int_t> spd_mctrack_point_; 

class SpdTrackMC: public TObject {

public:

    SpdTrackMC();
    virtual ~SpdTrackMC();
  
    virtual void DeleteFitPars();
    virtual void Clear(Option_t* option = "");
    virtual void ClearAll();

    void  RemovePoints() { fPoints.clear(); }
    
    void  AddPoint(Int_t hitId, Int_t dataId) { fPoints.push_back(spd_mctrack_point_(hitId,dataId)); } 
    void  AddPoint(spd_mctrack_point_ point)  { fPoints.push_back(point); }
        
    // get (or create) track fit paramaters
    SpdTrackFitPar* FitParameters();
    
    /* Set methods */

    void  SetId(Int_t id)           { fId = id; }
    
    void  SetParticleId(Int_t id)   { fParticleId = id; }
    void  SetVertexId(Int_t id)     { fVertexId = id;   }
    
    void  SetParticlePdg(Int_t pdg) { fPdgCode = pdg; }
    
    void  SetNHitsIts(Int_t n)      { fNHits[0] = n; } 
    void  SetNHitsTsB(Int_t n)      { fNHits[1] = n; } 
    void  SetNHitsTsEC(Int_t n)     { fNHits[2] = n; }
    
    /* Get methods */
       
    Int_t GetId()          const { return fId; }
    
    Int_t GetParticleId()  const { return fParticleId; }
    Int_t GetVertexId()    const { return fVertexId;   }
    
    Int_t GetParticlePdg() const { return fPdgCode;  }

    Int_t GetNHitsIts()    const { return fNHits[0]; } 
    Int_t GetNHitsTsB()    const { return fNHits[1]; } 
    Int_t GetNHitsTsEC()   const { return fNHits[2]; }

    Int_t GetNPoints()     const { return fPoints.size(); }
    Int_t GetNHits()       const { return (fNHits[0] + fNHits[1] + fNHits[2]); }
    
    spd_mctrack_point_ GetTrackPoint(const Int_t i) const { return fPoints[i]; }
    
    Int_t GetTrackPoint(const Int_t i, Int_t& data_id) const;
    Int_t GetTrackPointId(const Int_t i, Int_t& data_id) const { return fPoints[i].first; }
    Int_t GetTrackPointDataType(const Int_t i, Int_t& data_id) const { return fPoints[i].second; }
    
    const std::vector<spd_mctrack_point_>& GetTrackPoints() const { return fPoints; }
    
    SpdTrackFitPar* GetFitPars() const { return fFitPars; }
    Bool_t GetIsFitted() const { return (fFitPars && fFitPars->GetIsFittedOk()); }
    
    // shortcuts to fit data
    SpdTrackState*  GetInitialState() const { return (fFitPars) ? fFitPars->GetInitialState() : 0; }
    SpdTrackState*  GetFirstState()   const { return (fFitPars) ? fFitPars->GetFirstState()   : 0; }
    SpdTrackState*  GetLastState()    const { return (fFitPars) ? fFitPars->GetLastState()    : 0; }
    SpdTrackState*  GetFinalState()   const { return (fFitPars) ? fFitPars->GetFinalState()   : 0; } 
    
    virtual void Print(Option_t* option = "") const; //option = "points","fitpars","points+fitpars"

private:
 
    Int_t  fId;          // track own unique ID (index in the corresponding array) 
    Int_t  fParticleId;  // corresponding mc-particle id
    Int_t  fVertexId;    // corresponding mc-vertex id
    Int_t  fPdgCode;     // particle pdg code
 
    //--------------------------------------------------------------------------
    // array of "track points" <hit unique id, data branch type>, sorted by time
    // for "data branch type" see SpdDetectorList.h
    
    std::vector<spd_mctrack_point_> fPoints;
    
    //-------------------------
    
    Int_t fNHits[3];     // number of hits in detector subsystems [0]:ITS, [1]:TS(B), [2]:TS(EC)        
    
    SpdTrackFitPar* fFitPars;

    ClassDef(SpdTrackMC,1)
};

//------------------------------------------------------------------------
inline Int_t SpdTrackMC::GetTrackPoint(const Int_t i, Int_t& data_id) const 
{ 
    if (i < 0 || i >= fPoints.size()) {
        data_id = -1;
        return -1;
    }
    data_id = fPoints[i].second;
    return fPoints[i].first;
}

#endif  /* __SPDTRACKMC_H__ */

