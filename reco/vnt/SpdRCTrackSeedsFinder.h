// $Id$
// Author: vladimir   2022/07/01

#ifndef __SPDRCTRACKSEEDSFINDER_H__
#define __SPDRCTRACKSEEDSFINDER_H__

#include "SpdTask.h"

#include <TVector3.h>
#include <TVectorD.h>
#include <TMatrixDSym.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRCTrackSeedsFinder                                                      //
//                                                                            //
// Find track seed candidates using only Its hits:                            //
//                                                                            //
// 1) combine 2 hits in different layers (starting from 1-st)                 //  
// 2) add new hit layer by layer using theta and phi conditions               //
// 3) estimate track seed parameters                                          //
// 4) merge track seed candidates                                             //
// 5) clean track seed candidates                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCEvent;
class SpdHit;
class SpdTrackRCSeed;

class SpdRCTrackSeedsFinder: public SpdTask {

public:

    SpdRCTrackSeedsFinder();
    virtual ~SpdRCTrackSeedsFinder();
   
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    // Setters
    
    void  SetVerboseLevel(Int_t level)        { fVerboseLevel = level; }
    
    // Save data (track seeds) into the output file 
    void  SaveTrackSeeds(Bool_t save = true)   { fSaveSeeds = save;   }
    
    //--------------------------------------
    // ATTENTION Acceptors (cuts) ATTENTION
    
    void CheckMinItsHits(Bool_t check = true, Int_t min_value = 2); 
    void CheckMinPtSeed(Bool_t  check = true, Double_t min_value = 0.025);
    void CheckMinMomSeed(Bool_t check = true, Double_t min_value = 0.075);

    void SetMaxDthetaOfHits(Double_t theta)  { fMaxDtheta = theta; } 
    void SetMaxDradiusOfHits(Double_t rad)   { fMaxDrad   = rad;   } 
    void SetMaxResOfLineFit(Double_t res)    { fMaxRes    = res; } 
    void SetUsedSameTypeHits(Bool_t same)    { fSameHits  = same;  } 

    void SetMaxPVZvtxRange(Double_t zvtx)    { fMaxPVZvtx = zvtx;  } 
    void SetMaxDistPVCluster(Double_t dist)  { fMaxPVDist = dist;  } 
    void SetMaxIterPVZvtx(Int_t iter)        { fMaxPVIter = iter;  } 

    // Getters
    
    // Print
    void PrintCuts() const;

    /* Track seed */

    struct SeedCand
    {
       SeedCand(): charge(0.), radius(0.), theta(-99.), phi(-99.),
                   zvtx(0.), res(0.)
       {
          seedPoint.clear();
          seedLay.clear();
       }

       std::vector<Int_t> seedPoint;     // hit's number in fXYZhits
       std::vector<Int_t> seedLay;       // detector layer for hits 

       TVector3 seedRad;                 // x, y, radius on XY-plane 

       Double_t charge;                  // charge of seed (+, -, 0)
       Double_t radius;                  // radius of seed
       Double_t theta;                   // theta of seed
       Double_t phi;                     // phi of seed 
       Double_t zvtx;                    // z-coordinate of vertex 
       Double_t res;                     // residual of n-point line fit
//       Double_t chi2;                    // chi2 of n-point line fit
//       Double_t ndf;                     // ndf of n-point line fit

       void Reset()
       {
          seedPoint.clear();
          seedLay.clear();

          seedRad.SetXYZ(0.,0.,0.);

          charge =   0.;
          radius =   0.;
          theta  = -99.;
          phi    = -99.;
          zvtx   =   0.;
          res    =   0.;
//          chi2   = -99.;
//          ndf    =    0;
       }

    };

    struct zPrimVtxCand
    {
       zPrimVtxCand(): np(0), zVtx(0.), sigVtx(0.1)
       { } 

       Int_t np;                       // used number of seeds
       Double_t zVtx;                  // z of PV vertex
       Double_t sigVtx;                // sigma 
    };

protected:
  
    /* Methods */

    virtual void SetDefaultCuts();

    virtual   SpdTrackRCSeed* AddSeed();

    void      ClearAll();                            // clear all vectors         

    void      Create2pointsSeeds();                  // 2-points seeds         
    void      Create2pointsSeed(Int_t, Int_t);       // 2-points seed         
    void      CreateNpointsSeeds();                  // n-points seed

    Bool_t    RadiusNPointsSeed(SeedCand&, TVector3& ); // N-points radius (LSM)      
    Double_t  ChargeOfSeedCand(SeedCand& );             // (+,-,0) clockwise, anticlockwise, undefined
    Bool_t    ZvtxThetaNpointsSeed(SeedCand&, Double_t&, Double_t&, Double_t&); // z, theta and residual
    Bool_t    CompareSeedCand(SeedCand&, SeedCand&, Int_t); // compare 2 seeds           

    void      MergeSeedCands();                      // mearge seeds
    void      CheckNpointsSeeds();                   // check n-points seeds
    void      CleanNpointsSeeds();                   // clean n-points seeds 
    void      ZofPrimaryVertexCand();                // provide z of PV

    void      PrintSeed(Int_t);                      // print seed
    void      PrintSeeds();                          // print seeds
    void      PrintItsHits();                        // print Its hits

    /* Data members */ 
    
    // input data
    SpdMCEvent*         fEvent;        // Main mc-data object
    TClonesArray*       fItsHits;      // List of ITS digi mc-hits

    // output data
    TClonesArray*       fSeeds;        // List of TS mc-hits

    // settings
    Bool_t              fSaveSeeds;    //! save tracks into an output file; default = true
    
    // --- Acceptors (seeds)

    Bool_t   fCheckMinItsHits;         //! apply MinItsHits-cut (yes/no); default = true
    Int_t    fMinItsHits;              //! min. number of hits in vertex detector (ITS); default = 2

    Bool_t   fCheckMinPtSeed;          //! apply MinPt-cut (yes/no); default = true
    Double_t fMinPtSeed;               //! min. value of Pt-cut; default = 25 MeV

    Bool_t   fCheckMinMomSeed;         //! apply MinMomentum-cut (yes/no); default = true
    Double_t fMinMomSeed;              //! min. value of P-cut; default = 50 MeV

    //--- counters & checking flags 

    Int_t    fNHitsAccepted;           //! number of accepted Itshits     
    Int_t    fNHitsUsed;               //! number of used Its hits     
    Int_t    fNSeedsTotal;             //! total number of seeds have been found    

    //--------------------------
    Double_t fMaxItsLayers;            //! max number of Its layers
    Bool_t   fSameHits;                //! use hits from the same part (barrel or endcap) 

    Double_t fMinLine;                 //! min value for radius estimation 
    Double_t fMaxDtheta;               //! max delta theta for adding new point 
    Double_t fMaxPVZvtx;               //! max z-range of primary vertex
    Double_t fMaxRes;                  //! max residual of line fit 
    Double_t fMaxDrad;                 //! max delta radius for 3-d points

    Double_t fMaxPVIter;               //! max number iteration for PV estimation
    Double_t fMaxPVDist;               //! max distance for PV clustering 

//    Double_t fChi2Max;                 //! max chi2/ndf 

    std::vector<Int_t>        fTypeHits;               //! Its hits barrel (0), endcap (1)
    std::vector<Int_t>        fUsedHits;               //! Used Its hits 
    std::vector<TVector3>     fXYZhits;                //! Its hits in global CS
    std::vector<zPrimVtxCand> fZprimVtx;               //! vector with PV 

    std::map<Int_t, std::vector<Int_t>>     fIts_map;  //! Its hits [layer, hits]  
    std::map<Int_t, std::vector<SeedCand>> fSeed_map;  //! seed candidates 
    
    //----------------------
    Int_t    fVerboseLevel;            //! verbose level 
    
    ClassDef(SpdRCTrackSeedsFinder,1)
};

//__________________________________________________________________________
inline void SpdRCTrackSeedsFinder::CheckMinItsHits(Bool_t check, Int_t min_value)
{
    fCheckMinItsHits = check;
    fMinItsHits = min_value;
}

//__________________________________________________________________________
inline void SpdRCTrackSeedsFinder::CheckMinPtSeed(Bool_t check, Double_t min_value)
{
    fCheckMinPtSeed = check;
    fMinPtSeed      = min_value;
}

//__________________________________________________________________________
inline void SpdRCTrackSeedsFinder::CheckMinMomSeed(Bool_t check, Double_t min_value)
{
    fCheckMinMomSeed = check;
    fMinMomSeed      = min_value;
}

#endif  /* __SPDRCTRACKSEEDSFINDER_H__ */

