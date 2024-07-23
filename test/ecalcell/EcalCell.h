
#ifndef __ECALCELL_H__
#define __ECALCELL_H__

#include <TLorentzVector.h>
#include <TString.h>
#include "FairDetector.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// EcalCell                                                                   //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class EcalCellPoint;
class EcalCellHits;
class EcalCellEventHeader;

class FairVolume;
class TClonesArray;
class TGeoMedium;
class TParticle;

class EcalCell: public FairDetector {

public:

    EcalCell(const char* Name, Bool_t Active);
    EcalCell();
  
    virtual ~EcalCell();
  
    virtual void   Initialize();
    virtual Bool_t ProcessHits(FairVolume* v = 0);
    virtual void   Register();
    virtual void   Reset();
    
    virtual TClonesArray* GetCollection(Int_t iColl) const ;

    virtual void ConstructGeometry();
         
    virtual void CopyClones(TClonesArray* cl1,  TClonesArray* cl2 , Int_t offset) {}
    
    virtual void FillParameters();
    
    inline void SetNlayers(Int_t nl)  { fNLayers = nl; }
    inline void SetNXYPads(Int_t np)  { fNXYPads = np; }
    inline void SetLayerSizeXY(Double_t size /*cm*/) { fLayerSizeXY = size; }
    inline void SetLayer1SizeZ(Double_t size /*cm*/) { fLayer1SizeZ = size; }
    inline void SetLayer2SizeZ(Double_t size /*cm*/) { fLayer2SizeZ = size; }
    
    void SetGeometryType(Int_t type);
    
    virtual void   SetSpecialPhysicsCuts() {}
    
    virtual void   BeginEvent();
    virtual void   FinishEvent();
    virtual void   EndOfEvent();
    virtual void   PostTrack();
    virtual void   FinishRun(); 
   
    virtual void   BeginPrimary()  {}
    virtual void   FinishPrimary() {}
    virtual void   PreTrack()      {}
    
    inline Int_t     GetNlayers()     const { return fNLayers;     }
    inline Int_t     GetNXYPads()     const { return fNXYPads;     }
    inline Double_t  GetLayerSizeXY() const { return fLayerSizeXY; }
    inline Double_t  GetLayer1SizeZ() const { return fLayer1SizeZ; }
    inline Double_t  GetLayer2SizeZ() const { return fLayer2SizeZ; }
    
    virtual TString GetDataOut(Int_t n) const;
    
    void  PrintMCProcesses();
    
private:
    
    void  AddHit();
    
    Int_t GetCurrentStepProcess();
    
    Int_t          fTrackID;            //!  track index
    Int_t          fVolumeID;           //!  volume id
    TLorentzVector fPos;                //!  position at entrance
    TLorentzVector fMom;                //!  momentum at entrance
    Double32_t     fTime;               //!  time
    Double32_t     fLength;             //!  length
    Double32_t     fELoss;              //!  energy loss

    EcalCellEventHeader* fEventHeader;  //!  event header
    
    Int_t          fNDataOut;           //!
    
    TClonesArray*  fPoints;             //!
    TString        fOutPointObject;     //!  
   
    EcalCellHits*  fHits;               //!  
    TString        fOutHitsObject;      //!  

    Int_t          fEnum;               //!
    Double_t       fTotalEloss;         //!
    
    // Geometry parameters
    Int_t     fGeoType;     //!
    
    Int_t     fNLayers;     //!
    Int_t     fNXYPads;     //!
    Double_t  fLayerSizeXY; //! cm
    Double_t  fLayer1SizeZ; //! cm
    Double_t  fLayer2SizeZ; //! cm
    
    EcalCell(const EcalCell&);
    EcalCell& operator=(const EcalCell&);
   
    Int_t       CreateMedium(const char* name);
    TGeoMedium* DefaultMedium();
    
    void GetIntRadLen(TParticle* part, Double_t x, Double_t y, Double_t z, 
                      Double_t& radlen, Double_t& intlen, Double_t& depth, Double_t& length);

    ClassDef(EcalCell,1)
};

#endif /* __ECALCELL_H__ */
