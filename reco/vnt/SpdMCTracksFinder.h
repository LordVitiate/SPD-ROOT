// $Id$
// Author: artur   2021/02/03

#ifndef __SPDMCTRACKSFINDER_H__
#define __SPDMCTRACKSFINDER_H__

#include "SpdTask.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCTracksFinder                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTrackFitterGF;
class SpdMCEvent;
class SpdMCParticle;
class SpdTrackMC;
class SpdHit;
class SpdTrackFitPar;

class SpdMCTracksFinder: public SpdTask {

public:

    SpdMCTracksFinder();
    virtual ~SpdMCTracksFinder();
   
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    // get (or create) track fitter
    virtual SpdTrackFitterGF* Fitter();
    
    // Setters
    
    void  FitTracks(Bool_t fit = true)        { fFitTracks = fit;       }
    void  SaveTracks(Bool_t save = true)      { fSaveTracks = save;     }
    void  RemoveTrackPoints(Bool_t r = false) { fRemoveTrackPoints = r; }  

    void  SetVerboseLevel(Int_t level)        { fVerboseLevel = level; }
  
    //--------------------------------------
    // ATTENTION Acceptors (cuts) ATTENTION
    
    // particle
    void CheckMaxPartGeneration(Bool_t check = true, Int_t maxg = 2); 
    void CheckMinPartPt(Bool_t check, Double_t pt /*GeV*/);
    void CheckMinPartMomentum(Bool_t check = true, Double_t mom = 4.e-3 /*GeV*/); // WARNING! abs. min. value = 4 MeV
    void CheckMinPartBetaGamma(Bool_t check = true, Double_t bg = 0.05); // WARNING! abs. min. value = 0.05 
    void CheckMinPartTheta(Bool_t check = false, Double_t theta = 0. /*deg*/);
    
    // track (hits)
    void CheckMinItsHits(Bool_t check = true, Int_t min_value = 1); 
    void CheckMinTsHits(Bool_t check = true, Int_t min_value = 0); 
    void CheckMinHits(Bool_t check = true, Int_t min_value = 3); 
    
    // Getters
    
    SpdTrackFitterGF* GetFitter() const { return fFitter; }
    
    // Print
    void PrintCuts() const;
    
protected:
  
    /* Methods */

    virtual void SetDefaultCuts();

    virtual SpdTrackMC* AddTrack();
    virtual Bool_t      AcceptParticle(SpdMCParticle* part);
    virtual void        PrepareTrack(SpdTrackMC* mctrack, std::vector<SpdHit*>& track, std::vector<Int_t>& ids) const; 
    virtual void        CheckTrack(std::vector<SpdHit*>& track, SpdTrackFitPar* pars);          

    /* Data memebers */ 
    
    // Track fitter
    SpdTrackFitterGF*   fFitter;       //! track fitter

    // input data
    SpdMCEvent*         fEvent;        //! Main mc-data object
    TClonesArray*       fParticles;    //! List of mc-particles 
    TClonesArray*       fVertices;     //! List of mc-vertices 
    TClonesArray*       fItsHits;      //! List of ITS mc-hits
    TClonesArray*       fTsHits;       //! List of TS mc-hits
    
    // output data
    TClonesArray*       fTracks;       //! List of TS mc-hits

    // settings
    Bool_t              fFitTracks;         //! specifies whether to fit tracks or not; default = true
    Bool_t              fSaveTracks;        //! specifies whether to save tracks into an output file; default = true
    Bool_t              fRemoveTrackPoints; //! specifies whether to remove points from track after fitting; default = false
    
    // --- Acceptors (particle)
    
    Bool_t   fCheckMaxPartGeneration;  //! apply MaxPartGeneration-cut (yes/no); default = true
    Int_t    fMaxPartGeneration;       //! max. generation value; default = 2
    
    Bool_t   fCheckMinPartPt;          //! apply MaxPartGeneration-cut (yes/no); default = true
    Double_t fMinPartPt;               //! min. transverse momentum value [GeV/c] (!abs. min. value = 0 MeV); default = 0 MeV 
    
    Bool_t   fCheckMinPartMomentum;    //! apply MaxPartGeneration-cut (yes/no); default = true
    Double_t fMinPartMomentum;         //! min. momentum value [GeV/c] (WARNING!abs. min. value = 4 MeV); default = 4 MeV 
    
    Bool_t   fCheckMinPartBetaGamma;   //! apply MaxPartGeneration-cut (yes/no); default = true
    Double_t fMinPartBetaGamma;        //! min. (p/m)-value (WARNING! abs. min. value = 0.05); default = 0.05 
    
    Bool_t   fCheckMinPartTheta;       //! apply MinPartTheta-cut (yes/no); default = false
    Double_t fMinPartTheta;            //! min. theta value [deg]; default = 0.
    
    // --- Acceptors (track)
    
    Bool_t   fCheckMinItsHits;         //! apply MinItsHits-cut (yes/no); default = true
    Int_t    fMinItsHits;              //! min. number of hits in vertex detector (ITS); default = 1
    
    Bool_t   fCheckMinTsHits;          //! apply kMinTsHits-cut (yes/no); default = true
    Int_t    fMinTsHits;               //! min. number of hits in straw tracker detector (TS); default = 0
    
    Bool_t   fCheckMinHits;            //! apply kMinHits-cut (yes/no); default = true
    Int_t    fMinHits;                 //! min. number of hits in ITS+TS; default = 3
    
    // counters & checking flags 
    
    Bool_t   fHitsError;               //! hit error flags
    
    Int_t    fNHitsAccepted;           //! total number of accepted hits  
    Int_t    fNHitsNoTruth;            //! total number of hits without MC-truth 
    Int_t    fNHitsRejectedPart;       //! total number of hits produced by rejected particle 
    
    Int_t    fNTracksTotal;            //! total number of tracks have been found     
    Int_t    fNTracksAccepted;         //! total number of accepted tracks      
    
    //----------------------
    Int_t    fVerboseLevel;            // ! verbose level 
    
    ClassDef(SpdMCTracksFinder,1)
};

//__________________________________________________________________________
inline void SpdMCTracksFinder::CheckMaxPartGeneration(Bool_t check, Int_t maxg)
{
    fCheckMaxPartGeneration = check;
    fMaxPartGeneration = maxg;
}

//__________________________________________________________________________
inline void SpdMCTracksFinder::CheckMinPartPt(Bool_t check, Double_t pt /*GeV*/)
{
    fCheckMinPartPt = check;
    fMinPartPt = (pt > 0) ? pt : 0; 
}

//__________________________________________________________________________
inline void SpdMCTracksFinder::CheckMinPartMomentum(Bool_t check, Double_t mom /*GeV*/)
{
    fCheckMinPartMomentum = check;
    fMinPartMomentum = (mom > 4.e-3) ? mom : 4.e-3;  // -> 4 MeV
}

//__________________________________________________________________________
inline void SpdMCTracksFinder::CheckMinPartBetaGamma(Bool_t check, Double_t bg) 
{
    fCheckMinPartBetaGamma = check;
    fMinPartBetaGamma = (bg > 0.05) ? bg : 0.05;
}

//__________________________________________________________________________
inline void SpdMCTracksFinder::CheckMinPartTheta(Bool_t check, Double_t theta /*deg*/)
{
    fCheckMinPartTheta = check;
    fMinPartTheta = theta;
}

//__________________________________________________________________________
inline void SpdMCTracksFinder::CheckMinItsHits(Bool_t check, Int_t min_value)
{
    fCheckMinItsHits = check;
    fMinItsHits = min_value;
}

//__________________________________________________________________________
inline void SpdMCTracksFinder::CheckMinTsHits(Bool_t check, Int_t min_value)
{
    fCheckMinTsHits = check;
    fMinTsHits = min_value;  
}

//__________________________________________________________________________
inline void SpdMCTracksFinder::CheckMinHits(Bool_t check, Int_t min_value)
{
    fCheckMinHits = check;
    fMinHits = min_value;
}

#endif  /* __SPDMCTRACKSFINDER_H__ */

