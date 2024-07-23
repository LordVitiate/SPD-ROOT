
#ifndef __TRACKER_H__
#define __TRACKER_H__

#include <TLorentzVector.h>
#include <TString.h>
#include "SpdDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Tracker                                                                    //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TrackerPoint;

class FairVolume;
class TClonesArray;
class TGeoMedium;
class TParticle;

class Tracker: public SpdDetector {

public:

    Tracker(const char* Name, Bool_t Active);
    Tracker();
  
    virtual ~Tracker();
  
    virtual void  Initialize();
    virtual void  Reset();
    virtual void  Register();
    
    virtual Bool_t ProcessHits(FairVolume* v = 0);
    
    virtual TString GetDataOut(Int_t n) const;
    virtual TClonesArray* GetCollection(Int_t iColl) const;
    
    virtual void ConstructGeometry();
     
    virtual void  BeginEvent();
    virtual void  EndOfEvent();
    virtual void  FinishRun(); 
    
    virtual Bool_t FillParsIn(SpdParSet* params);
    virtual Bool_t LoadParsFrom(SpdParSet* params);
    FairModule* CloneModule() const { return new Tracker(*this); }
 
    virtual void  Print(Option_t*) const;
    
    void SetBaseMaterial(TString material)   { fBaseMaterial   = material; }
    void SetActiveMaterial(TString material) { fActiveMaterial = material; }
    
    void SetMaxSimuStep(Double_t step) { fMaxSimuStep = step; }
    
    void SetSlicing(Int_t N) { fNslices = (N < 1) ? 1 : N; }
    
    // geotype = 1,2,3
    void SetGeoParameters(Double_t Zmin, Double_t Zmax, Double_t Rmin, Double_t Rmax, Int_t N);
    void SetGeoEndcapsParameters(Double_t Zmin, Double_t Zmax, Double_t Rmin, Double_t Rmax, Int_t N);
    void SetGeoBarrelParameters(Double_t Zmin, Double_t Zmax, Double_t Rmin, Double_t Rmax, Int_t N);
    
    void SetDefaultGeometryParameters(Int_t g = 1); // geotype = 1
     
    TString    GetBaseMaterial()   const { return fBaseMaterial;   }
    TString    GetActiveMaterial() const { return fActiveMaterial; }
    Double_t   GetMaxSimuStep()    const { return fMaxSimuStep;    }
    Int_t      GetNSlices()        const { return fNslices;        }
  
private:
  
    void AddHit();
    
    /* Track information to be stored until the track leaves the active volume. */
    
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID1;         //!  volume id1
    Int_t          fVolumeID2;         //!  volume id2
    TLorentzVector fPos;               //!  position at entrance
    TLorentzVector fMom;               //!  momentum at entrance
    Double32_t     fTime;              //!  entrance time
    TLorentzVector fPosOut;            //!  position at exit
    TLorentzVector fMomOut;            //!  momentum at exit
    Double32_t     fTimeOut;           //!  exit time
    Double32_t     fLength;            //!  length
    Double32_t     fSegmentLength;     //!  length of the trajectory segment inside
    Double32_t     fELoss;             //!  energy loss
    Int_t          fNSteps;            //!  number of steps in the current volume
    
    /* simulation parameters & data */
    
    Double_t       fMaxSimuStep;  

    /* containers for data */

    TString        fOutDataPointObject; //!
    TClonesArray*  fPointCollection;    //!
    Int_t          fEnum;               //!
    
    Tracker(const Tracker&);
    Tracker& operator=(const Tracker&);
    
    /* GEOMETRY */
    
    Int_t    fGeoType;
    
    TString  fBaseMaterial;
    TString  fActiveMaterial;
  
    Double_t fRmin;
    Double_t fRmax;
    Double_t fZmin;
    Double_t fZmax;
    Int_t    fNslices;
         
    void ConstructGeometry_1();
    void ConstructGeometry_2();
    void ConstructGeometry_3();
    
    //TGeoVolume* BuildTarget();
    
    ClassDef(Tracker,1)
};

#endif /* __TRACKER_H__ */
