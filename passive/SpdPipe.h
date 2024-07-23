
#ifndef __SPDPIPE_H__
#define __SPDPIPE_H__

#include "SpdPassiveModule.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPipe                                                                    //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdPipe : public SpdPassiveModule {
  
public:
  
  SpdPipe();
  SpdPipe(const char* Name, const char* Prefix = "SPDPIPE");
  
  virtual ~SpdPipe() {}
  
  virtual void ConstructGeometry();
  
  /* ----- setters ------ */
  
  virtual void SetGeometryType(Int_t type) {}
   
  void SetPipeType(TString type);
  
  void SetR(Double_t r /*cm*/);
  void SetThickness(Double_t dr /*cm*/);
  void SetHalfLength(Double_t dl /*cm*/);
  
  void SetPipeMaterial(TString material, Int_t n = 0);
  void SetCenterMaterial(TString MaterialCenter );

  virtual void UnsetMaterials(TString media = "vacuum2");
  virtual void ResetMaterials(/*set dafault materials*/);
  
  /* ----- getters ------ */
  
  virtual Bool_t  IsGeometryLocked() const { return fPipe; }
   
  virtual Double_t GetCapacity() const;  // cm^3
  virtual Double_t GetMass()     const;  // g
  
  Double_t GetDensity() const;  // g/(cm^3)
  
  TString  GetPipeType()   const { return fPipeType; }
  TString  GetPipeName()   const { return fPipeName;  }
 
  Double_t GetRmin()       const { return fR; }
  Double_t GetRmax()       const { return fR - fThickness; }
  Double_t GetThickness()  const { return fThickness; }
  Double_t GetHalfLength() const { return fHalfLength; }
  
  const TGeoVolume* GetPipe() const { return fPipe; } 
  
  const TGeoVolume* GetHybPipeC()       const { return fHybPipeC;       }
  const TGeoVolume* GetHybPipeLZminus() const { return fHybPipeLZminus; }
  const TGeoVolume* GetHybPipeLZPlus()  const { return fHybPipeLZplus;  }
  const TGeoVolume* GetHybPipeISpace()  const { return fHybPipeISpace;  }
 
  virtual void Print(Option_t*) const;
  
  virtual Bool_t FillParsIn(SpdPassiveGeoParSet* params);
  virtual Bool_t LoadParsFrom(SpdPassiveGeoParSet* params);

  FairModule* CloneModule() const { return new SpdPipe(*this); }
  
protected:
    
  void SetDefault(Int_t geotype);  
  void SetDefaultMaterials(Int_t geotype);  
  void BuildHybridPipe();
  void BuildPipe();

  TString  fPipeType;     // pipe type
  TString  fPipeName;     // pipe volume name

  Double_t fR;            // outer radius of the pipe [cm]
  Double_t fThickness;    // pipe thicknes [cm]
  Double_t fHalfLength;   // pipe half length [cm] 
  
  TString  fBaseMaterial; // base pipe material
  TString  fMaterial[2];  // pipe material

  TGeoVolume* fPipe;      //! main pipe volume
  
  /* For "hybrid" geometry */
  
  TGeoVolume* fHybPipeC;        //! central part of the "hybrid" pipe 
  TGeoVolume* fHybPipeLZminus;  //! lateral(z-) part of the "hybrid" pipe 
  TGeoVolume* fHybPipeLZplus;   //! lateral(z+) part of the "hybrid" pipe 
  TGeoVolume* fHybPipeISpace;   //! inner space of the "hybrid" pipe 
  


  ClassDef(SpdPipe,1)

};

#endif /* __SPDPIPE_H__ */

