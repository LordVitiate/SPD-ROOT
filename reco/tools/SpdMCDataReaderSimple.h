// $Id$
// Author: artur   2021/02/01

#ifndef __SPDMCDATAREADERSIMPLE_H__
#define __SPDMCDATAREADERSIMPLE_H__

#include "SpdTask.h"
#include "SpdSetParSet.h"

#include <TString.h>
#include <TClonesArray.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMCDataReaderSimple                                                      //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdSetParSet;
class SpdMCEvent;

class SpdMCDataReaderSimple: public SpdTask {

public:

    SpdMCDataReaderSimple();
    virtual ~SpdMCDataReaderSimple();
    
    virtual InitStatus Init();   
    virtual void Exec(Option_t* opt);
    virtual void Finish();
    
    /* --------------- BASE MC-DATA ----------------*/
    
    // default = true
    void PrintEvent(Bool_t opt)     { fPrintEvent = opt; }     
    
    // opt  = 1, 2, (< 1 : nothing will be printed); default = 0
    void PrintParticles(Int_t opt)  { fPrintParticles = opt; }
    
    // opt = 1, 2, (< 1 : nothing will be printed); default = 0
    void PrintVertices(Int_t opt)   { fPrintVertices = opt; }
   
    /* ------------------- HITS --------------------*/
   
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintItsHits(Int_t opt)    { fPrintItsHits = opt; }
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintTsHits(Int_t opt)     { fPrintTsHits = opt; }  
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintTofHits(Int_t opt)    { fPrintTofHits = opt; }  
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintEcalHits(Int_t opt)   { fPrintEcalHits = opt; }  
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintRsHits(Int_t opt)     { fPrintRsHits = opt; } 
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintBbcHits(Int_t opt)    { fPrintBbcHits = opt; }   
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintAegHits(Int_t opt)    { fPrintAegHits = opt; }   
      
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintZdcHits(Int_t opt)    { fPrintZdcHits = opt; }   
    
    /* ----------------- OBJECTS -------------------*/
    
    // opt = 1, 2, (< 1 : nothing will be printed); default = 0
    void PrintTracks(Int_t opt)     { fPrintTracks = opt; }
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintVerticesRC(Int_t opt) { fPrintVerticesRC = opt; }
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintTracksRC(Int_t opt) { fPrintTracksRC = opt; }
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    //void PrintTrackSeedsRC(Int_t opt) { fPrintTrackSeedsRC = opt; }
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintEcalClustersRC(Int_t opt) { fPrintEcalClustersRC = opt; }  
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintEcalParticlesRC(Int_t opt) { fPrintEcalParticlesRC = opt; }  
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintEcalClustersMCInfo(Int_t opt) { fPrintEcalClustersMCInfo = opt; } 
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintEcalParticlesMC(Int_t opt) { fPrintEcalParticlesMC = opt; }  
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintRsClustersMC(Int_t opt) { fPrintRsClustersMC = opt; }  
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintRsParticlesMC(Int_t opt) { fPrintRsParticlesMC = opt; }  
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintTsParticles(Int_t opt) { fPrintTsParticles = opt; } 
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintTofParticles(Int_t opt) { fPrintTofParticles = opt; }  
    
    // opt = 2 (< 2 : nothing will be printed); default = 0
    void PrintAegParticles(Int_t opt) { fPrintAegParticles = opt; }  
    //------------------------------------------------------
    
    SpdSetParSet*   GetParameters() { return fParameters; }    
    
    virtual void PrintParameters(Int_t opt = 1) const 
                 {  if (fParameters) fParameters->PrintParameters(opt); }
    
protected:
    
    virtual void LoadGeometry();
    
    SpdSetParSet*    fParameters;              //! List of input parameters
    
    // Input
    SpdMCEvent*      fEvent;                   //! Main mc-data object
    TClonesArray*    fParticles;               //! List of mc-particles 
    TClonesArray*    fVertices;                //! List of mc-vertices

    TClonesArray*    fItsHits;                 //! List of ITS mc-hits
    TClonesArray*    fTsHits;                  //! List of TS mc-hits
    TClonesArray*    fTofHits;                 //! List of TOF mc-hits
    TClonesArray*    fEcalHits;                //! List of ECAL mc-hits
    TClonesArray*    fRsHits;                  //! List of RS mc-hits
    TClonesArray*    fBbcHits;                 //! List of BBC mc-hits
    TClonesArray*    fAegHits;                 //! List of AEG mc-hits
    TClonesArray*    fZdcHits;                 //! List of ZDC mc-hits

    TClonesArray*    fTracks;                  //! List of mc-tracks 
    TClonesArray*    fVerticesRC;              //! List of rc-vertices 
    
    TClonesArray*    fTracksRC;                //! List of reco-tracks 
    TClonesArray*    fTrackSeedsRC;            //! List of seeds for reco-tracks 
    
    TClonesArray*    fEcalClustersRC;          //! List of ECAL rc-clusters 
    TClonesArray*    fEcalParticlesRC;         //! List of ECAL rc-particles 
    TClonesArray*    fEcalClustersMCInfo;      //! List of ECAL rc-cluster infos
    TClonesArray*    fEcalParticlesMC;         //! List of ECAL mc-particles 
    
    TClonesArray*    fRsClustersMC;            //! List of RS mc-clusters 
    TClonesArray*    fRsParticlesMC;           //! List of RS mc-particles 
    
    TClonesArray*    fTsParticles;             //! List of TS particles
    TClonesArray*    fTofParticles;            //! List of TOF particles
    TClonesArray*    fAegParticles;            //! List of Aeg particles

    Bool_t           fPrintEvent;              // default = true
    Int_t            fPrintParticles;          // 1, 2, (< 1 : nothing will be printed); default = 0
    Int_t            fPrintVertices;           // 1, 2, (< 1 : nothing will be printed); default = 0
    
    Int_t            fPrintItsHits;            // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintTsHits;             // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintTofHits;            // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintEcalHits;           // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintRsHits;             // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintBbcHits;            // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintAegHits;            // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintZdcHits;            // 2 (< 2 : nothing will be printed); default = 0
    
    Int_t            fPrintTracks;             // 1, 2, (< 1 : nothing will be printed); default = 0
    Int_t            fPrintVerticesRC;         // 2 (< 2 : nothing will be printed); default = 0
    
    Int_t            fPrintTracksRC;           // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintTrackSeedsRC;       // 2 (< 2 : nothing will be printed); default = 0
        
    Int_t            fPrintEcalClustersRC;     // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintEcalParticlesRC;    // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintEcalClustersMCInfo; // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintEcalParticlesMC;    // 2 (< 2 : nothing will be printed); default = 0
    
    Int_t            fPrintRsClustersMC;       // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintRsParticlesMC;      // 2 (< 2 : nothing will be printed); default = 0
    
    Int_t            fPrintTsParticles;        // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintTofParticles;       // 2 (< 2 : nothing will be printed); default = 0
    Int_t            fPrintAegParticles;       // 2 (< 2 : nothing will be printed); default = 0

    ClassDef(SpdMCDataReaderSimple,1)
};

#endif  /* __SPDMCDATAREADERSIMPLE_H__ */

