
#ifndef __SPDBARREL_H__
#define __SPDBARREL_H__

#include "FairDetector.h"

#include <TString.h>
#include <TLorentzVector.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdBarrel                                                                  //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairVolume;
class TClonesArray;
class TGeoVolume;
class TGeoMedium;

class SpdBarrel: public FairDetector {
  
public: 

    SpdBarrel();
    SpdBarrel(const char* Name, Bool_t Active);
    
    virtual ~SpdBarrel();

    virtual void  Initialize();
    virtual void  Reset();
    virtual void  Register();
    
    // Create geometry
    virtual void ConstructGeometry();
 
    // Simulations
    virtual Bool_t ProcessHits(FairVolume* v = 0);
    
    virtual void EndOfEvent();
  
    // Access methods
    virtual TClonesArray* GetCollection(Int_t iColl) const 
            { return (iColl == 0) ? fSpdBarrelPointCollection : NULL; }
            
    virtual void SetBuildGeoOption(TString option);        
   
    // --- NOT USED ---
    virtual void   CopyClones( TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {}
    virtual void   SetSpecialPhysicsCuts() {}
   
    virtual void   FinishPrimary() {}
    virtual void   FinishRun()     {}
    virtual void   BeginPrimary()  {}
    virtual void   PostTrack()     {}
    virtual void   PreTrack()      {}
    virtual void   BeginEvent()    {}

protected:
    
    virtual void ConstructToroidalGeo();
    virtual void ConstructSolenoidalGeo();
  
    TString FName(TString name);

    Int_t          fTrackID;           //  track index
    Int_t          fVolumeID;          //  volume id
    TLorentzVector fPos;               //  position at entrance
    TLorentzVector fMom;               //  momentum at entrance
    Double32_t     fTime;              //  time
    Double32_t     fLength;            //  length
    Double32_t     fELoss;             //  energy loss

    TClonesArray*  fSpdBarrelPointCollection;  //!
    
    TString        fBuildGeoOption;   // "toroid", "solenoid"
    TGeoVolume*    fMasterVol;
    TGeoMedium*    fMasterMedium;
    
    TGeoVolume* MakeArb(const char* name, 
                        double dz, double angle, double hmin, double hmax, 
                        double cutoff);

    void MakeBarrel_1(const char* name, 
                      int nsec, double dz, double hmin, double hmax, 
                      double cutoff, int col);

    void MakeBarrel_2(const char* name_c, const char* name_s,
                      int nsec, double dz, double hmin_c, double hmin_s, double hmax, 
                      double cutoff , int col);

    SpdBarrel(const SpdBarrel&);
    SpdBarrel& operator=(const SpdBarrel&);

    ClassDef(SpdBarrel,1)
};

//_______________________________________________________________________________________
inline TString SpdBarrel::FName(TString name) 
{   
  TString mname = GetName();
  mname += "1_" + name;
  return mname; 
}

#endif /*__SPDBARREL_H__ */
