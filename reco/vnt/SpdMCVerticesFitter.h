// $Id$
// Author: artur   2021/02/08

#ifndef __SPDMCVERTICESFITTER_H__
#define __SPDMCVERTICESFITTER_H__

#include "SpdTask.h"
#include <TClonesArray.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCVerticesFitter                                                        //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMCEvent;
class SpdVertexMC;
class SpdVertexFitPar;
class SpdTrackPropagatorGF;

class SpdMCVerticesFitter: public SpdTask {

public:

    SpdMCVerticesFitter();
    virtual ~SpdMCVerticesFitter();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    // Setters 
    void  SetFitSecondaries(Bool_t fit) { fFitSecondaries = fit;  }
     
    void  SetVerboseLevel(Int_t level)  { fVerboseLevel = level; }

protected:
    
    SpdTrackPropagatorGF* fPropagator;
    
    Bool_t  FitPrimaryVertex(SpdVertexMC* vertex);
    Bool_t  FitSecondaryVertex(SpdVertexMC* vertex);
   
    void    SearchForFinalState(SpdVertexMC* vertex);
     
    SpdVertexMC*  GetPrimaryVertex();
    
    // input data
    SpdMCEvent*    fEvent;      //! Main mc-data object
    TClonesArray*  fParticles;  //! List of mc-particles 
    TClonesArray*  fVertices;   //! List of mc-vertices
    TClonesArray*  fTracks;     //! List of mc-tracks
    
    // settings 
    Bool_t         fFitSecondaries;          //! specifies whether to fit secondary vertices
    Double_t       fVertexFitAngleCut;       //! reject primary vertex tracks with theta = 90+-cut [deg]
    
    // counters
    Int_t          fNFittedVertices;          //! current event fitted vertices
    Int_t          fNTotalFittedVertices;     //! total (run) number of fitted vertices
    Int_t          fNTotalPrimFittedVertices; //! total (run) number of primary fitted vertices
    Int_t          fNTotalSecFittedVertices;  //! total (run) number of secondary fitted vertices
    
    //-------------------------------
    Int_t          fVerboseLevel;             // ! verbose level 
    
    ClassDef(SpdMCVerticesFitter,1)
};

#endif  /* __SPDMCVERTICESFITTER_H__ */

