// $Id$ // Author: vladimir 2022/07/01

#ifndef __SPDRCTRACKSFINDER_H__
#define __SPDRCTRACKSFINDER_H__

#include "SpdTask.h"
#include "SpdTrackRCSeed.h"
#include <TVector3.h>

#include "SpdMCStrawHit1D.h"
#include "SpdTrackFitPar.h"
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRCTracksFinder                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCEvent;
class SpdTrackFitterGF;
class SpdRCTrackFitterGF;
class SpdTrackPropagatorGF;
class SpdHit;
class SpdTrackMC;
//class SpdTrackFitPar;
class SpdTrackRC;

typedef std::pair<Int_t,Int_t> spd_ts_point;

class SpdRCTracksFinder: public SpdTask {

public:

    SpdRCTracksFinder();
    virtual ~SpdRCTracksFinder();
   
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    // get (or create) track fitter
    virtual SpdTrackFitterGF* Fitter();
    virtual SpdRCTrackFitterGF* RCFitter();

    // track cand structure
    //----------------------
    struct trkcand 
    {
       trkcand(): itrk(-1), iseed(-1), iq(0), imoth(-1), igen(0), ifit(0), 
                  imod(0), ilay(0), chi2(0.), nits(0), nts_b(0), nts_ec(0) 
       {
          pMom.SetXYZ(0.,0.,1.);  
          pVtx.SetXYZ(0.,0.,0.);
          itsHits.clear();  
           tsHits.clear();  
       }

       Int_t  itrk;                          //! primary seed number
       Int_t  iseed;                         //! primary seed number
       Int_t  iq;                            //! charge (0 - undefined)
       Int_t  imoth;                         //! mother particle
       Int_t  igen;                          //! generation level
       Bool_t ifit;                          //! 1 - fit, 0 - no fit
       Int_t  imod;                          //! Ts module (1-8 barrel, 9,10 endcap)
       Int_t  ilay;                          //! last Ts layer used in fit
       Double_t chi2;                        //! chi2 of fit

       TVector3 pMom;                        //! primary momentum
       TVector3 pVtx;                        //! primary vertex

       std::vector<spd_its_point> itsHits;   //! Its hits of track cand
       std::vector<spd_ts_point>   tsHits;   //!  Ts hits of track cand

       Int_t nits;                           //! Its hits 
       Int_t nts_b;                          //! Ts hits in barrel  
       Int_t nts_ec;                         //! Ts hits in endcap
       Int_t nts() { return nts_b+nts_ec; }       //! Total Ts hits 
       Int_t n()   { return nits+nts_b+nts_ec; }  //! Total Its+Ts hits
    
       void Update(Bool_t fit, Int_t lay, SpdTrackFitPar* pars, SpdMCTrackHit* hit, Bool_t full)
       {
          ifit = fit;
          ilay = lay;

          if (pars != nullptr) { 
             if (full) {
                 chi2 = pars->GetChi2();
                 pMom = pars->GetFirstState()->GetMomentum();
             }
             else {
                 chi2 += pars->GetChi2();
                 pMom  = pars->GetLastState()->GetMomentum();
             } 
          }

          if (hit != nullptr) {
              tsHits.push_back( spd_ts_point(hit->GetId(),kMCTsHits) );

              if (     hit->GetModId() == kSpdTsTB)  nts_b++;
              else if (hit->GetModId() == kSpdTsTEC) nts_ec++;
          }
       }
    };
    
    // Setters
    
    void  SaveTracks(Bool_t save = true)      { fSaveTracks = save;     }

    void  SetVerboseLevel(Int_t level)        { fVerboseLevel = level;  }
  
    //--------------------------------------
    // ATTENTION Acceptors (cuts) ATTENTION

    void CheckMinItsHits      (Bool_t check = true,  Int_t min_value = 2); 
    void CheckMinTsHits       (Bool_t check = true,  Int_t min_value = 0); 
    void CheckMinHits         (Bool_t check = true,  Int_t min_value = 3); 
    void CheckFakeTracks      (Bool_t check = false, Int_t min_value = 7); 
    void CheckMaxTrkGeneration(Bool_t check = true,  Int_t max_value = 2); 
    void CheckEfficiencyTsHits(Bool_t check = false, Double_t max_value = 0.98); 
 
    // set cuts

    void SetTrackMaxLayers(Int_t   p)   { fMaxLayer     = p; }
    void SetMaxDistToWire(Double_t p)   { fMaxDist      = p; }
    void SetMaxChi2ToHits(Double_t p)   { fMaxChi2      = p; }
    void SetMaxDeltaChi2(Double_t  p)   { fMaxDchi2     = p; }
    void SetMaxFittedTsHits(Int_t  p)   { fMaxFitTsHits = p; }

    void SetMinTsHitsDiff(Double_t p)   { fMaxTsDiff = p; }
    void SetTrackQuality(Double_t  p)   { fQuality   = p; }

    void SetUseKalmanTree(Bool_t   p)   { fKalmanTree   = p; }
    void SetFullKalmanTrack(Bool_t p)   { fFullTracking = p; }
    void SetRemoveUsedTsHits(Bool_t p)  { fUsedTsHits   = p; }

    void SetTrackPDGcodeForFit(Int_t p) { fTrkCodeFit = p; }
    
    // Getters
    SpdTrackFitterGF*   GetFitter()   const { return fFitter; }
    SpdRCTrackFitterGF* GetRCFitter() const { return fRCFitter; }
    
    // Print
    void PrintCuts() const;
    
protected:

    SpdTrackPropagatorGF* fPropagator;
  
    /* Methods */

    virtual void SetDefaultCuts();

    virtual SpdTrackRC* AddTrack();
    virtual void  PrepareTrack(SpdTrackRC* trk, std::vector<SpdHit*>& track, std::vector<Int_t>& ids) const; 

    /* Data members */ 

    void    ClearAll();                                         // clear all 

    void    FillPrimaryTrackSeeds();                            // fill arrays with primary seeds 
    void    FillTsHitsArrays();                                 // fill arrays with Ts hits
 
    void    FindTrackFullKalmanFit();                           // full Kalman fit method        
    void    FindTrackFastKalmanFit();                           // fast Kalman fit method        
    void    FindTrackKalmanTree();                              // Kalman tree method        

    void    FillTrackWithSeed(trkcand&);                        // fill track candidates        
    void    RemoveLastHit(trkcand&);                            // remove last Ts point        
    void    AddCloneTrackCand(Int_t, Int_t, Int_t);             // add cand clone        

    Bool_t  FitTrackCand(SpdTrackRC* , SpdTrackFitPar* );       // track fit           
    Int_t   FindTrackerModule(SpdTrackFitPar* );                // find tracker module          

    Bool_t  CheckTrackerModule(SpdTrackFitPar* , Int_t, Int_t, Int_t&, TVector3&, TVector3&);
    Bool_t  ExtrapToPlane(SpdTrackFitPar* , Int_t, Int_t, TVector3&, TVector3&, Double_t&);
    Bool_t  CheckTsHitsInLayer(Int_t, Int_t, TVector3, TVector3, std::vector<Int_t>&); 

    void    CleanTrackCandidate();                              // clean track candidates        
    std::vector<Int_t> CompareTwoTracksCand(Bool_t);            // compare tracks

    TVector3 GetPlaneNormal(Int_t);                             // normal of straw plane
    TVector3 GetPlanePos(Int_t, Int_t);                         // position of straw plane

    void     PrintTrkCand(trkcand&);                            // print track candidate

    // Track fitter
    SpdTrackFitterGF*   fFitter;       //! track fitter
    SpdRCTrackFitterGF* fRCFitter;     //! track fitter

    // input data
    SpdMCEvent*         fEvent;        //! Main mc-data object
    TClonesArray*       fItsHits;      //! List of ITS mc-hits
    TClonesArray*       fTsHits;       //! List of TS mc-hits
    TClonesArray*       fSeeds;        //! List of track-seeds
    
    // output data
    TClonesArray*       fTracks;       //  List of reconstructed tracks

    // settings
    Bool_t              fSaveTracks;   //! specifies whether to save tracks into an output file; default = true

    // --- variablee

    SpdTrackRC*     rctrack;            //! reco track 
    SpdTrackFitPar* fitpars;            //! track fit parameters

    std::vector<trkcand>     fTrkCand;  //! track-structure 

    std::map<std::pair<Int_t,Int_t>, std::vector<Int_t>> fTsHits_map; //! map with Ts hits
    std::map<Int_t,Int_t> fTsUsed_hits;                               //! used Tshits
   
    // --- Acceptors (track)
    
    Bool_t   fCheckMinItsHits;         //! apply MinItsHits-cut (yes/no); default = true
    Int_t    fMinItsHits;              //! min. number of hits in vertex detector (ITS); default = 1
    
    Bool_t   fCheckMinTsHits;          //! apply MinTsHits-cut (yes/no); default = true
    Int_t    fMinTsHits;               //! min. number of hits in straw tracker detector (TS); default = 0
    
    Bool_t   fCheckMinHits;            //! apply MinHits-cut (yes/no); default = true
    Int_t    fMinHits;                 //! min. number of hits in ITS+TS; default = 3
    
    Bool_t   fCheckMaxTrkGeneration;   //! apply MaxTrkGeneration-cut (yes/no); default = true
    Int_t    fMaxGen;                  //! max generation of track cand (Kalman tree); default = 2

    Bool_t   fCheckTsHitsEfficiency;   //! apply max efficiency to Ts hits (yes/no); default = false
    Double_t fEffTsHits;               //! value of Ts hits efficiency

    // counters & checking flags 
    
    Int_t    fNHitsAccepted;           //! total number of accepted Ts hits  
    Int_t    fNHitsUsed;               //! number of used Ts hits 

    Double_t fMinRadBar;               //! min radius of 1-s layer in Barrel
    Double_t fMinDistEC;               //! min distance of Endcap
    Double_t fMinRadEC;                //! min radius of Endcap
    Double_t fMaxRadEC;                //! max radius of Endcap

    Int_t    fMaxLayer;                //! max number layers in Barrel  
    Int_t    fMaxEcLayer;              //! max number layers in Endcap 
    Double_t fMaxDist;                 //! max distance hit to wire for selection 
    Double_t fMaxChi2;                 //! max chi2 for Ts hit selection 
    Double_t fMaxDchi2;                //! max delta chi2 increment for Ts hit  
    Int_t    fMaxFitTsHits;            //! max Ts hits for full fit method 

    Double_t fMaxTsDiff;               //! max difference of Ts hits for track cand 
    Double_t fMaxDtheta;               //! max delta theta between two track cand's 
    Double_t fQuality;                 //! coefficient for track quality  

    Bool_t   fKalmanTree;              //! use Kalman Tree method (default false) 
    Bool_t   fFullTracking;            //! use full Kalman fit method (default false)
    Bool_t   fUsedTsHits;              //! remove already used Ts hits (default false) 

    Double_t fTrkCodeFit;              //! particle code for track fit (default = 211)  

    //----------------------
    Int_t    fVerboseLevel;            // ! verbose level 
    
    ClassDef(SpdRCTracksFinder,1)
};

//__________________________________________________________________________
inline void SpdRCTracksFinder::CheckMinItsHits(Bool_t check, Int_t min_value)
{
    fCheckMinItsHits = check;
    fMinItsHits = min_value;
}

//__________________________________________________________________________
inline void SpdRCTracksFinder::CheckMinTsHits(Bool_t check, Int_t min_value)
{
    fCheckMinTsHits = check;
    fMinTsHits = min_value;  
}

//__________________________________________________________________________
inline void SpdRCTracksFinder::CheckMinHits(Bool_t check, Int_t min_value)
{
    fCheckMinHits = check;
    fMinHits = min_value;
}

//________________________-_________________________________________________________
inline void SpdRCTracksFinder::CheckMaxTrkGeneration(Bool_t check, Int_t max_value)
{
    fCheckMaxTrkGeneration = check;
    fMaxGen = max_value;
}

//_____________________________________________________________________________________
inline void SpdRCTracksFinder::CheckEfficiencyTsHits(Bool_t check, Double_t max_value)
{
    fCheckTsHitsEfficiency = check;
    fEffTsHits = max_value;
}


#endif  /* __SPDRCTRACKSFINDER_H__ */

