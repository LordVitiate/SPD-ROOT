// $Id$
// Author: artur   2021/02/16

#ifndef __SPDRCVERTICESFINDER_H__
#define __SPDRCVERTICESFINDER_H__

#include "SpdTask.h"
#include <TClonesArray.h>
#include <TVector3.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRCVerticesFinder                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCEvent;
class SpdTrackFitPar;
class SpdVertexRC;
class SpdVertexFitPar;
class SpdTrackPropagatorGF;

class SpdRCVerticesFinder: public SpdTask {

public:

    SpdRCVerticesFinder();
    virtual ~SpdRCVerticesFinder();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    // Setters
    
         /* primary and secondary vertices track selection cut */
    void    SetMinItsHits(Int_t nmin)             { fMinItsHits = nmin;     }      
 
         /* secondary vertex */ 
    void    SetFitSecondaries(Bool_t fit)         { fFitSecondaries = fit;  }
    void    SetTrackSelOption(Int_t opt)          { fTrackSelOption = opt;  } 
    
    void    SetUsedTypeOfPV(Int_t p)              { fUsedTypeOfPV = p;      } 
    void    SetConstrainToPV(Int_t p)             { fConstrainToPV = p;     } 
    void    SetMinTrackPVchi2(Double_t chi2)      { fMinChi2PV = chi2;      } 
    void    SetMaxDistDaughter(Double_t dist)     { fMaxDist = dist;        } // cm
    void    SetMinMass(Double_t m)                { fMinMass = m;           } // GeV
    void    SetMaxMass(Double_t m)                { fMaxMass = m;           } // GeV
    
    void    SetFirstDaughter(Int_t pdg)           { fFirstDaughter = pdg;   } 
    void    SetSecondDaughter(Int_t pdg)          { fSecondDaughter = pdg;  } 
    
        /* primary vertex estimation */
    void    SetBeamLinePosition(TVector3 x)       { fBeamLinePos    = x;    }    
    void    SetBeamLineDirection(TVector3 x)      { fBeamLineDir    = x;    }    
    void    SetBeamSigmaX(Double_t sigX)          { fBeamSigmaX     = sigX; }    
    void    SetBeamSigmaY(Double_t sigY)          { fBeamSigmaY     = sigY; }    
    void    SetMinTrackClusterDz(Double_t dz)     { fMinDzCluster   = dz;   }    
    void    SetMinTrackClusterChi2(Double_t chi2) { fMinChi2Cluster = chi2; }    
    
    // Save data (vertices) into the output file 
    void    SaveVertices(Bool_t save = true)      { fSaveVertices = save;   }

    void    SetVerboseLevel(Int_t level)          { fVerboseLevel = level;  }
    
protected:
    
    SpdTrackPropagatorGF* fPropagator;
    
    /* Methods */
    
    Bool_t          FindPrimaryVertex();
    Int_t           FindSecondaryVertices();
    Double_t        FindPrimVertexCand();
    
    SpdVertexRC*    AddVertex(Int_t vtype);
    
    SpdVertexRC*    GetPrimaryVertex();
    Bool_t          GetPrimMCVertexPos(TVector3& v);
    
    Bool_t          CheckTrack(SpdTrackFitPar* pars, Int_t opt);
    
    /* Data memebers */
    
    // input data
    
    SpdMCEvent*     fEvent;             //! Main mc-data object
    TClonesArray*   fParticles;         //! List of mc-particles 
    TClonesArray*   fTracks;            //! List of mc-tracks
    
    // output data     
    
    TClonesArray*   fVertices;          //! List of reconstructed vertices
    
    // settings: options and cuts
    
    Int_t           fMinItsHits;        //! min. number of hits in ITS for each vertex track 
     
          /* for primary vertices (PV) */
    TVector3        fBeamLinePos;       //! beam-line position  
    TVector3        fBeamLineDir;       //! beam-line direction
    Double_t        fBeamSigmaX;        //! sigma X of beam
    Double_t        fBeamSigmaY;        //! sigma Y of beam
    Double_t        fMinDzCluster;      //! minimum dz track to PV seed, [cm]
    Double_t        fMinChi2Cluster;    //! minimum chi2 track to PV seed
    Double_t        fVertexFitAngleCut; //! reject primary vertex tracks with theta = 90+-cut [deg]
    
          /* for secondary vertices (SV) */
    Bool_t          fFitSecondaries;    //! specifies whether to fit secondary vertices
    Int_t           fTrackSelOption;    //! if option: > 0, then hard cut for track selection
    
    Int_t           fUsedTypeOfPV;      //! type of used PV position (0: simu, 1: reco)
    Int_t           fConstrainToPV;     //! constrain to PV
    Double_t        fMinChi2PV;         //! minimum chi2 track to PV
    Double_t        fMaxDist;           //! maximum distance between 2 daughter particles, [cm]
    Double_t        fMinMass;           //! minimum mass for mother particle, [GeV]
    Double_t        fMaxMass;           //! maximum mass for mother particle, [GeV]
    
    Int_t           fFirstDaughter;     //! first daugther (pdg)
    Int_t           fSecondDaughter;    //! second daugther (pdg)
    
    // to save data
    Bool_t          fSaveVertices;      //! specifies whether to save reconstructed vertices into an output file
    
    // counters
    Int_t           fNFittedVertices;          //! current event fitted vertices
    Int_t           fNTotalFittedVertices;     //! total (run) number of fitted vertices
    Int_t           fNTotalPrimFittedVertices; //! total (run) number of primary fitted vertices
    Int_t           fNTotalSecFittedVertices;  //! total (run) number of secondary fitted vertices
    
    //-------------------------------
    Int_t           fVerboseLevel;      // ! verbose level 
    
    ClassDef(SpdRCVerticesFinder,1)
};

#endif  /* __SPDRCVERTICESFINDER_H__ */

