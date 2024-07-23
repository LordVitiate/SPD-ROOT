// $Id$
// Author: artur   2018/01/25

#ifndef __SPDTORMAGNET_H__
#define __SPDTORMAGNET_H__

#include "SpdPassiveModule.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTorMagnet                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define SPD_TORMAGNET_NDCOILS  3

class SpdTorMagnet: public SpdPassiveModule {

public:

    SpdTorMagnet();
    SpdTorMagnet(const char* Name, const char* Prefix = "SPDTORMAGNET");
    
    virtual ~SpdTorMagnet();
    
    virtual void ConstructGeometry();
    
    /* ----- setters ------ */
  
    virtual void SetGeometryType(Int_t type);
    
    void  SetAxialDistance(Double_t d /*cm*/);          // DISTANSE TO CENTRAL AXIS: d >= 10 cm 
    void  SetCoilsInnerRounding(Double_t r /*cm*/);     // ROUNDING: r <= size(X,Y)/2 - width  && r > 0 
    void  SetCoilsWidth(Double_t width /*cm*/);         // width >= 1 cm && (width >= size(X,Y)/2 - ROUNDING)
    void  SetCoilsThickness(Double_t thickness/*cm*/);  // thickness >= 0.1 cm 
    
    void  SetCoilsMaterial(TString material);
    void  SetBaseMaterial(TString material);  
    
    void  StretchCoilsAlongR(Double_t dr /*cm*/);       // dr: > or < 0; new R_size >= width + ROUNDING
    void  PurseCoilsAlongZ(Double_t dz /*cm*/);         // dz -> |dz|;   new Z_size >= width + ROUNDING
    
    virtual void UnsetMaterials(TString media = "vacuum2");
    virtual void ResetMaterials(/*set dafault materials*/);
    
    /* ----- getters ------ */
    
    virtual Bool_t   IsGeometryLocked() const { return fLockGeom; }
    
    inline Double_t  GetAxialDistance() const { return fAxialDistance; }
    
    virtual Double_t GetCapacity()   const;  // cm^3
    virtual Double_t GetMass()       const;  // g
    
    inline TString   GetCoilMaterial(Int_t ncoil = 0) const;
    inline Double_t  GetCoilWidth(Int_t ncoil = 0) const;
    inline Double_t  GetCoilInnerRounding(Int_t ncoil = 0) const;
    inline Double_t  GetCoilOuterRounding(Int_t ncoil = 0) const;
    inline Double_t  GetCoilCapacity(Int_t ncoil = 0) const;
    inline Double_t  GetCoilMass(Int_t ncoil = 0) const;
    inline Double_t  GetCoilDensity(Int_t ncoil = 0) const;
    inline Double_t  GetCoilHXsize(Int_t ncoil = 0) const;
    inline Double_t  GetCoilHYsize(Int_t ncoil = 0) const;
    inline Double_t  GetCoilHZsize(Int_t ncoil = 0) const;
     
    inline const TGeoVolume* GetCoil(Int_t ncoil) const;
    
    inline Int_t GetCoilCounts(Int_t ncoil) const; 
   
    virtual void Print(Option_t*) const; 
    
    virtual Bool_t FillParsIn(SpdPassiveGeoParSet* params);
    virtual Bool_t LoadParsFrom(SpdPassiveGeoParSet* params);

    FairModule* CloneModule() const { return new SpdTorMagnet(*this); }
    
    //______________________________________________________________//
    struct tmcoil_ : public TObject {
      
           tmcoil_(); 
          ~tmcoil_() {}
          
           void SetFillPars(Int_t line, Int_t fill, Int_t transp);
           void SetFillPars();
           
           TGeoVolume* box_;
           
           TGeoVolume* aparts_[4]; // yokes
           TGeoVolume* bparts_[2]; // sides
          
           TString  base_material_;
           Double_t hxbox_, hybox_, hzbox_;  // cm
           
           TString  coil_material_;
           Double_t hxcoil_, hycoil_, hzcoil_;  // cm
           Double_t coil_width_; // cm
           Double_t inner_rounding_; // inner rounding radius, cm
           
           Double_t capacity_;   // cm^3
           Double_t mass_;       // g
           Double_t density_;    // g/(cm^3)       
           
           Int_t    fill_color_;
           Int_t    line_color_;
           Char_t   transparency_;  
           
           Int_t    counts_;
           
           ClassDef(SpdTorMagnet::tmcoil_,1)
    };
    //______________________________________________________________//
    
private:
  
    Bool_t   fLockGeom; //! if true, prevent geometry modifications
    
    Double_t fAxialDistance;   // mm
   
    tmcoil_ fCoil[SPD_TORMAGNET_NDCOILS];

    void SetGeoPars(Int_t geotype);
    void SetDefaultMaterials(Int_t geotype);
    
    Bool_t ConstructGeometry_1();
    Bool_t ConstructGeometry_2();
    
    Bool_t BuildCoil(Int_t ncoil);
    
    ClassDef(SpdTorMagnet,1)
};

//_____________________________________________________________________________
inline TString SpdTorMagnet::GetCoilMaterial(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? "" : fCoil[ncoil].coil_material_;
}

//_____________________________________________________________________________
inline Double_t SpdTorMagnet::GetCoilWidth(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? 0 : fCoil[ncoil].coil_width_;
}

//_____________________________________________________________________________
inline Double_t SpdTorMagnet::GetCoilInnerRounding(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? 0 : fCoil[ncoil].inner_rounding_;
}

//_____________________________________________________________________________
inline Double_t SpdTorMagnet::GetCoilOuterRounding(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? 0 
                     : fCoil[ncoil].inner_rounding_ + fCoil[ncoil].coil_width_;
}

//_____________________________________________________________________________
inline Double_t SpdTorMagnet::GetCoilCapacity(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? 0 : fCoil[ncoil].capacity_;
}

//_____________________________________________________________________________
inline Double_t SpdTorMagnet::GetCoilMass(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? 0 : fCoil[ncoil].mass_;
}

//_____________________________________________________________________________
inline Double_t SpdTorMagnet::GetCoilDensity(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? 0 : fCoil[ncoil].density_;
}

//_____________________________________________________________________________
inline Double_t SpdTorMagnet::GetCoilHXsize(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? 0 : fCoil[ncoil].hxcoil_;
}

//_____________________________________________________________________________
inline Double_t SpdTorMagnet::GetCoilHYsize(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? 0 : fCoil[ncoil].hycoil_;
}

//_____________________________________________________________________________
inline Double_t SpdTorMagnet::GetCoilHZsize(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? 0 : fCoil[ncoil].hzcoil_;
}

//_____________________________________________________________________________ 
inline const TGeoVolume* SpdTorMagnet::GetCoil(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? 0 : fCoil[ncoil].box_;
}

//_____________________________________________________________________________
inline Int_t SpdTorMagnet::GetCoilCounts(Int_t ncoil) const
{
   return (ncoil < 0 || ncoil >= SPD_TORMAGNET_NDCOILS) ? 0 : fCoil[ncoil].counts_;
}

#endif  /* __SPDTORMAGNET_H__ */

