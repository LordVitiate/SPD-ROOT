// $Id$
// Author: artur   2021/04/22

#ifndef __SPDRCKFPARTV0FINDER_H__
#define __SPDRCKFPARTV0FINDER_H__

#include "SpdTask.h"

#include <TClonesArray.h>
#include <TVector3.h>
#include <vector>
#include <set>

#include "KFParticle.h"
#include "SpdVertexCombiFinder.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRCKFpartV0Finder                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCEvent;
class SpdTrackFitPar;
class SpdVertexRC;
class SpdVertexFitPar;
class SpdTrackPropagatorGF;

class SpdRCKFpartV0Finder: public SpdTask {

public:

    SpdRCKFpartV0Finder();
    virtual ~SpdRCKFpartV0Finder();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    // Setters
    void    SetMinItsHits(Int_t nmin)          { fMinItsHits = nmin;    }      
    void    SetTrackSelOption(Int_t opt)       { fTrackSelOption = opt; } 
    void    SetUsedTypeOfPV(Int_t p)           { fUsedTypeOfPV = p;     } 
    void    SetConstrainToPV(Int_t p)          { fConstrainToPV = p;    } 
    void    SetMinPVTrackChi2(Double_t chi2)   { fMinChi2PV = chi2;     } 
    void    SetMaxTwoTracksChi2(Double_t chi2) { fMaxChi2Part = chi2;   } 
    void    SetMinimumMass(Double_t m)         { fMinMass = m;          } 
    void    SetMaximumMass(Double_t m)         { fMaxMass = m;          } 
    
    void    AddVertexCandidate(Int_t pdg1, Int_t pdg2);
    void    AddVertexCandidate(Int_t pdg1, Int_t pdg2, Int_t pdg3);
    void    AddVertexCandidate(Int_t pdg1, Int_t pdg2, Int_t pdg3, Int_t pdg4);
    void    AddVertexCandidate(const std::vector<Int_t>& pdg);
    void    AddVertexCandidate(Int_t n, const Int_t* pdg);

    void    SetVerboseLevel(Int_t level)       { fVerboseLevel = level; }

private:
    
    SpdTrackPropagatorGF*   fPropagator;    //! field propagator
    SpdVertexCombiFinder*   fVertexFinder;  //! vertex finder
    
         /* auxiliary data members */
    KFParticle              fPVertex;       //! primary vertex
    std::vector<KFParticle> fKFparticles;   //! list of selected particles
    std::vector<Int_t>      fTracksId;      //! list of selected tracks
      
    /* Methods */
    
    SpdVertexRC*    GetPrimaryVertex();
    Bool_t          GetPrimMCVertexPos(TVector3& v);
    
    Int_t           FindSecondaryVertices();
    
    Int_t           ConstructVertex(const std::vector<Int_t>& vertex);
    Bool_t          CheckVertex(const KFParticle** pp, Int_t np);
    Bool_t          CheckDecay(const KFParticle& p0, const KFParticle** pp, Int_t np);
    Bool_t          CheckTrack(SpdTrackFitPar* pars);
     
    SpdVertexRC*    AddVertex();
        
    /* Data memebers */
    
    // input data
    
    SpdMCEvent*     fEvent;             //! Main mc-data object
    TClonesArray*   fParticles;         //! List of mc-particles 
    TClonesArray*   fTracks;            //! List of mc-tracks

    // input/output data     
    
    TClonesArray*   fVerticesRC;        //! List of reconstructed vertices
    
    // settings: options and cuts for finder
    
    Int_t           fMinItsHits;        //! min. number of hits in ITS for each vertex track 
    Int_t           fTrackSelOption;    //! if option: > 0, then hard cut for track selection
    Int_t           fUsedTypeOfPV;      //! type of used primary vertex position (0: simu, 1: reco)
    Int_t           fConstrainToPV;     //! constrain to primary vertex
    Double_t        fMinChi2PV;         //! minimum chi2 track to primary vertex
    Double_t        fMaxChi2Part;       //! maximum chi2 deviation between any 2 daughter particles
    Double_t        fMinMass;           //! minimum mass for mother particle, [GeV]
    Double_t        fMaxMass;           //! maximum mass for mother particle, [GeV]

    std::set<Int_t> fDaughters;                   //! list of unique daughters (pdg)
    std::vector< std::vector<Int_t> > fVertices;  //! list of vertex candidates (pdg)
    
    // counters
    Int_t           fNTotalSecFittedVertices;  //! total (run) number of secondary fitted vertices

    //-------------------------------
    Int_t           fVerboseLevel;             //! verbose level 
     
    ClassDef(SpdRCKFpartV0Finder,1)
};



#endif  /* __SPDRCKFPARTV0FINDER_H__ */

