// $Id$
// Author: artur   2020/07/04

#ifndef __SPDAXIALFIELDMAPDATA_H__
#define __SPDAXIALFIELDMAPDATA_H__

#include <TString.h>
#include <TArrayD.h>
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

class SpdAxialFieldMapData {

public:

    SpdAxialFieldMapData();
    SpdAxialFieldMapData(const Char_t* name, const Char_t* type = "");
     
    virtual ~SpdAxialFieldMapData();
    
    virtual void Clear();
    virtual void ClearData();
    
    virtual Bool_t InitData(TString fileName, TString path = "");
    virtual Bool_t InitData(const SpdAxialFieldMapData* data);
    virtual Bool_t InitData(Double_t rmin, Double_t rmax, Int_t nr,
                            Double_t zmin, Double_t zmax, Int_t nz);
  
    void SetPathToData(TString path);
    
    /* ----- transform field data ----- */
  
    virtual void MultiplyField(Double_t v);
    virtual void MultiplyField(Double_t vr, Double_t vz);
    
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
    
    inline  Int_t GetNr() const { return fNr; }
    virtual Int_t GetNz() const { return fNz; }
      
    inline Int_t GetNTotal() const { return fNr*fNz; } 
    
    inline const SpdRegion* GetMapRegion() const { return fMapRegion; }
    
    inline  Double_t GetRmin() const { return (fMapRegion) ? fMapRegion->GetRmin() : 0; }
    inline  Double_t GetRmax() const { return (fMapRegion) ? fMapRegion->GetRmax() : 0; }
    
    virtual Double_t GetZmin() const { return (fMapRegion) ? fMapRegion->GetZmin() : 0; }
    virtual Double_t GetZmax() const { return (fMapRegion) ? fMapRegion->GetZmax() : 0; }
    
    Double_t GetRstep() const;
    Double_t GetZstep() const;
      
    inline TArrayD* GetBr() const { return fBr; }
    inline TArrayD* GetBz() const { return fBz; }
    
    inline Double_t GetBr(Int_t n) const { return (fBr && (n > -1) && (n < GetNTotal()) ) ? fBr->At(n) : 0; }
    inline Double_t GetBz(Int_t n) const { return (fBz && (n > -1) && (n < GetNTotal()) ) ? fBz->At(n) : 0; }
    
    virtual Bool_t GetMinMaxStat(Double_t* B /* array of size 6: (B,Bz,Br)[min,max]*/ ) const;
   
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
    
    SpdTubeRegion* fMapRegion; // Field region
    
    Int_t      fNr;            // Grid points in radial direction
    Int_t      fNz;            // Grid points along Z-axis

    /* Arrays with the field values */
    TArrayD*   fBr;
    TArrayD*   fBz;
      
    virtual Bool_t ReadAsciiFile();   // Read a field map from ASCII file  
    virtual Bool_t ReadBinaryFile();  // Read a field map from BINARY file  
    
    friend class SpdField;

    ClassDef(SpdAxialFieldMapData,1)
};

#endif  /* __SPDAXIALFIELDMAPDATA_H__ */

