/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#ifndef __SPDSTT_H__
#define __SPDSTT_H__

#include <Rtypes.h>
#include "FairDetector.h"
#include "TVector3.h"
#include "TLorentzVector.h"

class SpdSttGeoPar;
class SpdSttPoint;
class FairVolume;
class TClonesArray;

class SpdStt: public FairDetector
{
public:

    SpdStt();
    SpdStt(const char* Name, Bool_t Active);
    
    virtual ~SpdStt();

    virtual void Initialize();
    virtual void Reset(); 
    virtual void Register();
    
    // Create detector geometry
    virtual void ConstructGeometry();
    
    // Simulations
    virtual Bool_t ProcessHits(FairVolume* v = 0);
      
    SpdSttPoint* AddHit();  
    
    virtual void EndOfEvent();
    
    // Access methods
    virtual TClonesArray* GetCollection(Int_t iColl) const 
            { return (iColl == 0) ? fSpdSttPointCollection : NULL; }

    
    // --- NOT USED ---
    virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2 , Int_t offset) {}
    virtual void SetSpecialPhysicsCuts() {}
    
    virtual void BeginEvent()    {} 
    virtual void BeginPrimary()  {}
    virtual void PreTrack()      {}
    virtual void PostTrack()     {}
    virtual void FinishPrimary() {} 
    virtual void FinishRun()     {}
      
private:
    
    TString       FName(TString name);
    SpdSttGeoPar* GetParameters();
    void          FillGeometryParameters();

    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fEventNum;          //  event number
    Int_t          fTrackID;           //  track index
    Int_t          fVolumeID;          //  volume id
    Int_t          fSectionID;         //  section id
    TLorentzVector fPos;               //  position 
    TLorentzVector fPosIn;             //  position at entrance
    TLorentzVector fPosOut;            //  position at exit
    TLorentzVector fMomIn;             //  momentum at entrance
    TLorentzVector fMomOut;            //  momentum at exit
    Double32_t     fMass;              //  particle mass
    Double32_t     fTime;              //  time
    Double32_t     fLength;            //  track length
    Double32_t     fELoss;             //  energy loss
   
    TString        fTubeVolName;
    TString        fTubeSecName;
    
    /* container for data points */

    TClonesArray*  fSpdSttPointCollection;  //!

    SpdStt(const SpdStt&);
    SpdStt& operator=(const SpdStt&);

    ClassDef(SpdStt,1)
};

#endif //NEWDETECTOR_H


