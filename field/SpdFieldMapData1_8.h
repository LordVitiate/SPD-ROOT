//
//  Created 29/09/17  by A. Ivanov
//
//  Redesign:
//          18/01/17 (A. Tkachenko)
//

#ifndef __SPDMAGFIELDMAPDATA1_8_H__
#define __SPDMAGFIELDMAPDATA1_8_H__

#include <TString.h>
#include <TArrayF.h>
#include "SpdRegion.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRadialField                                                             //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
//  Base units:                                                               //
//                                                                            //
//       - FIELD   [kG]                                                       //
//       - SPACING [cm]                                                       //
//       - ANGLES  [deg]                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdFieldMapData1_8 {

public:

  SpdFieldMapData1_8();
  SpdFieldMapData1_8(const Char_t* name, const Char_t* type = "");

  virtual ~SpdFieldMapData1_8();

  virtual void Clear();
  virtual void ClearData();

  virtual Bool_t InitData(TString fileName, TString path = "");
  virtual Bool_t InitData(const SpdFieldMapData1_8* data);
  virtual Bool_t InitData(Double_t xmin, Double_t xmax, Int_t nx,
                          Double_t ymin, Double_t ymax, Int_t ny,
                          Double_t zmin, Double_t zmax, Int_t nz);

  void SetPathToData(TString path);

  /* ----- transform field data ----- */

  virtual void MultiplyField(Double_t v);
  virtual void MultiplyField(Double_t vx, Double_t vy, Double_t vz);
  virtual void ShiftField(Double_t dx, Double_t dy, Double_t dz);

  virtual void RotateField(Double_t angle, Double_t theta, Double_t phi);

  /* ----- getters ----- */

  inline TString  GetPathToData() const { return fPathToData; }
  inline TString  GetFileName()   const { return fFileName;   }

  inline TString  GetMapName()    const { return fMapName; }
  inline TString  GetMapType()    const { return fMapType; }

  inline Double_t GetScaleUnit()  const { return fScaleUnit; }
  inline Int_t    GetFieldUnit()  const { return fFieldUnit; }

  TString GetFieldUnitAsString()  const;
  TString GetScaleUnitAsString()  const;

  inline Bool_t GetInitLevel() const { return fInitLevel; }

  Bool_t IsGridInit() const;
  Bool_t IsDataInit() const;

  Bool_t IsInsideMapRegion(Double_t x, Double_t y, Double_t z) const;
  Bool_t IsInsideMapRegion(Double_t r, Double_t z) const;

  inline Int_t GetNx() const { return fNx; }
  inline Int_t GetNy() const { return fNy; }
  inline Int_t GetNz() const { return fNz; }

  inline Int_t GetNTotal() const { return fNx*fNy*fNz; }

  inline const SpdBoxRegion* GetMapRegion() const { return fMapRegion; }

  inline Double_t GetXmin() const { return (fMapRegion) ? fMapRegion->GetXmin() : 0; }
  inline Double_t GetXmax() const { return (fMapRegion) ? fMapRegion->GetXmax() : 0; }
  inline Double_t GetYmin() const { return (fMapRegion) ? fMapRegion->GetYmin() : 0; }
  inline Double_t GetYmax() const { return (fMapRegion) ? fMapRegion->GetYmax() : 0; }
  inline Double_t GetZmin() const { return (fMapRegion) ? fMapRegion->GetZmin() : 0; }
  inline Double_t GetZmax() const { return (fMapRegion) ? fMapRegion->GetZmax() : 0; }

  Double_t GetXstep() const;
  Double_t GetYstep() const;
  Double_t GetZstep() const;

  inline TArrayF* GetBx() const { return fBx; }
  inline TArrayF* GetBy() const { return fBy; }
  inline TArrayF* GetBz() const { return fBz; }

  inline Double_t GetBx(Int_t n) const { return (fBx && (n > -1) && (n < GetNTotal()) ) ? fBx->At(n) : 0; }
  inline Double_t GetBy(Int_t n) const { return (fBy && (n > -1) && (n < GetNTotal()) ) ? fBy->At(n) : 0; }
  inline Double_t GetBz(Int_t n) const { return (fBz && (n > -1) && (n < GetNTotal()) ) ? fBz->At(n) : 0; }

  virtual Bool_t GetMinMaxStat(Double_t* B /* array of size 10: (B,BT,Bz,By,Bx)[min,max]*/ ) const;

  /* ----- write data to a file ----- */

  virtual Bool_t WriteAsciiFile(TString fileName, TString dirName = "") const;
  virtual Bool_t WriteBinaryFile(TString fileName, TString dirName = "") const;

  /* ----- print data ----- */

  virtual void PrintData() const;

protected:

  virtual Bool_t InitData();

  virtual void   SetMapType(TString type = "");
  virtual Bool_t SetScaleUnit(TString unit);
  virtual Bool_t SetFieldUnit(TString unit);

  TString    fMapName;       // Map name

  TString    fPathToData;    // File name
  TString    fFileName;      // File name

  TString    fMapType;       // Map type

  Double_t   fScaleUnit;     // Original grid scale factor (spatial); default = 1 (cm)
  Double_t   fFieldUnit;     // Original field units of the map; default = 1 (kG)

  Int_t      fInitLevel;     // Initialization level

  SpdBoxRegion* fMapRegion;  // Field region

  Int_t      fNx;            // Grid points along X-axis
  Int_t      fNy;            // Grid points along Y-axis
  Int_t      fNz;            // Grid points along Z-axis

  /* Arrays with the field values */
  TArrayF*   fBx;
  TArrayF*   fBy;
  TArrayF*   fBz;

  virtual Bool_t ReadAsciiFile();   // Read a field map from ASCII file
  virtual Bool_t ReadBinaryFile();  // Read a field map from BINARY file

  friend class SpdField;

  ClassDef(SpdFieldMapData1_8,1)
};

#endif /*__SPDMAGFIELDMAPDATA_H__*/
