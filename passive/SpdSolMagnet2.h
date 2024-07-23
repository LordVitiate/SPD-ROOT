// $Id$
// Author: artur   2018/01/25

#ifndef __SPDSOLMAGNET2_H__
#define __SPDSOLMAGNET2_H__

#include "SpdPassiveModule.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdSolMagnet2                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdSolMagnet2: public SpdPassiveModule {

public:

    SpdSolMagnet2();
    SpdSolMagnet2(const char* Name, const char* Prefix = "SPDSOLMAGNET2");
    
    virtual ~SpdSolMagnet2();
    
    virtual void ConstructGeometry();
    
    /* ----- setters ------ */
    
    virtual void SetGeometryType(Int_t type);
    
    void  SetWidth(Double_t width /*cm*/);  // width >= 1 cm 
    void  SetBaseMaterial(TString material);
   
    virtual void UnsetMaterials(TString media = "vacuum2");
    virtual void ResetMaterials(/*set dafault materials*/);
    
    /* ----- getters ------ */
    
    virtual Bool_t   IsGeometryLocked() const { return fMagnetSection;  }
    
    virtual Double_t GetCapacity() const;  // cm^3
    virtual Double_t GetMass()     const;  // g
            
    Double_t GetMagnetCapacity() const; // cm^3
    Double_t GetMagnetMass()     const; // g
    Double_t GetCryoCapacity()   const; // cm^3
    Double_t GetCryoMass()       const; // g
    
    inline TString GetBaseMaterial() const { return fBaseMaterial; }
    
    inline Double_t GetTotalLength()    const { return fTotalLength;   }
    inline Double_t GetTotalHalfSize()  const { return fTotalHalfSize; }
    inline Double_t GetTotalWidth()     const { return fTotalWidth;    }
    
    inline const TGeoVolume* GetSection() const { return fMagnetSection; }
       
    virtual void Print(Option_t*) const;
   
    virtual Bool_t FillParsIn(SpdPassiveGeoParSet* params);
    virtual Bool_t LoadParsFrom(SpdPassiveGeoParSet* params);

    FairModule* CloneModule() const { return new SpdSolMagnet2(*this); }

private:
  
    TString     fBaseMaterial; 
    TString     fCryoMaterial; 
    
    Int_t       fNSections;
    
    Double_t    fTotalHalfSize;   // total half size, cm
    Double_t    fTotalWidth;      // radial thickness, cm
    Double_t    fTotalLength;     // total Z-size, cm
    
    Double_t    fHalfSize;        // magnet half size, cm (magnet)
    Double_t    fWidth;           // magnet radial thickness, cm (magnet)
    Double_t    fModuleLength;    // magnet Z-size, cm (magnet)
    
    TGeoVolume* fMagnetSection;   //! magnet
    
    TGeoVolume* fInnerCryo;       //! inner cryostat
    TGeoVolume* fOuterCryo;       //! outer cryostat
    TGeoVolume* fEndfCryo;        //! endface cryostat
    
    Double_t    fInnerCryoWidth;  // cryostat radial width, cm (inner central barrel, geotype = 1)
    Double_t    fOuterCryoWidth;  // cryostat radial width, cm (outer central barrel, geotype = 1)
    Double_t    fEndfCryoWidth;   // cryostat radial width, cm (end face part, geotype = 1)
    
    Int_t       fFillColor;
    Int_t       fLineColor;
    Int_t       fTransparency;
    
    void   SetGeoPars(Int_t geotype);
    void   SetDefaultMaterials(Int_t geotype);
    void   ConstructGeometry_1();
    
    Bool_t BuildSection1();
    Bool_t BuildCryostat1();
   
    ClassDef(SpdSolMagnet2,1)
};

#endif  /* __SPDSOLMAGNET2_H__ */

