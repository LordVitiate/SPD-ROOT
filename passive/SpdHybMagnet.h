// $Id$
// Author: artur   2018/01/25

#ifndef __SPDHYBMAGNET_H__
#define __SPDHYBMAGNET_H__

#include "SpdPassiveModule.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdHybMagnet                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdHybMagnet: public SpdPassiveModule {

public:

    SpdHybMagnet();
    SpdHybMagnet(const char* Name, const char* Prefix = "SPDHYBMAGNET");
    
    virtual ~SpdHybMagnet();
    
    virtual void ConstructGeometry();
    
    /* ----- setters ------ */
  
    virtual void SetGeometryType(Int_t type);
     
    virtual void UnsetMaterials(TString media = "vacuum2");
    virtual void ResetMaterials(/*set dafault materials*/);
    
    void  SetCoilsMaterial(TString material);             // (all coils)
    
    void  SetRingCoilWidth(Double_t width /*cm*/);        // width(1) >= 1 cm
    void  SetRingCoilThickness(Double_t thickness/*cm*/); // thickness(1) >= 0.1 cm
    void  SetRingCoilAxialDistanceL1(Double_t d /*cm*/);  // DISTANSE TO VERTEX: d >= width(1)/2
    void  SetRingCoilAxialDistanceL2(Double_t d /*cm*/);  // DISTANSE TO VERTEX: d >= AxialDistance1 + width(1)
    void  SetRingCoilAxialDistanceC(Double_t d /*cm*/);   // DISTANSE TO VERTEX: d >= width(1)/2
   
    void  SetLoopCoilBaseMaterial(TString material);     
    void  SetLoopCoilAxialDistance(Double_t d /*cm*/);    // DISTANSE TO CENTRAL AXIS: d >= 10 cm 
    void  SetLoopCoilWidth(Double_t width /*cm*/);        // width(0) >= 1 cm && (width(0) >= size(X,Y)/2 - ROUNDING)
    void  SetLoopCoilThickness(Double_t thickness/*cm*/); // thickness(0) >= 0.1 cm 
    void  SetLoopCoilInnerRounding(Double_t r /*cm*/);    // COIL ROUNDING: r <= size(X,Y)/2 - width(0)  && r > 0 
    void  StretchLoopCoilAlongR(Double_t dr /*cm*/);      // dr: > or < 0; new R_size >= width(0) + ROUNDING
    void  PurseLoopCoilAlongZ(Double_t dz /*cm*/);        // dz -> |dz|;   new Z_size >= width(0) + ROUNDING
    
    /* ----- getters ------ */
    
    virtual Bool_t IsGeometryLocked() const { return fLockGeom; }
    
    virtual Double_t GetCapacity()   const;  // cm^3
    virtual Double_t GetMass()       const;  // g
    
    Double_t GetCapacity(Int_t ncoil) const;  // cm^3
    Double_t GetMass(Int_t ncoil)     const;  // g
    
    inline Double_t GetLoopCoilAxialDistance()    const { return fLoopCoilAxialDistance;   }
    inline Double_t GetRingCoilAxialDistanceL1()  const { return fRingCoilAxialDistanceL1; }
    inline Double_t GetRingCoilAxialDistanceL2()  const { return fRingCoilAxialDistanceL2; }
    inline Double_t GetRingCoilAxialDistanceC()   const { return fRingCoilAxialDistanceC;  }
    
    inline const TGeoVolume* GetCoil(Int_t ncoil) const;
 
    inline TString   GetCoilMaterial(Int_t ncoil) const;
    inline Double_t  GetCoilCapacity(Int_t ncoil) const;
    inline Double_t  GetCoilMass(Int_t ncoil)     const;
    inline Double_t  GetCoilDensity(Int_t ncoil)  const;
    inline Double_t  GetCoilWidth(Int_t ncoil)    const;
 
    inline Int_t     GetCoilCounts(Int_t ncoil)   const;
    
    inline Double_t  GetLoopCoilInnerRounding() const { return fLoopCoil.inner_rounding_; }
    inline Double_t  GetLoopCoilOuterRounding() const { return fLoopCoil.inner_rounding_ + fLoopCoil.coil_width_; }
    
    inline Double_t  GetLoopCoilHXsize() const { return fLoopCoil.hxcoil_; }
    inline Double_t  GetLoopCoilHYsize() const { return fLoopCoil.hycoil_; }
    inline Double_t  GetLoopCoilHZsize() const { return fLoopCoil.hzcoil_; }
     
    virtual void Print(Option_t*) const; 
    
    //______________________________________________________________//
    struct RingCoil : public TObject {
      
           RingCoil(); 
          ~RingCoil() {}
          
           void SetFillPars(Int_t line, Int_t fill, Int_t transp);
           void SetFillPars();
           
           TGeoVolume* tube_;
          
           TString  coil_material_;
           Double_t coil_max_radius_; // cm
           Double_t coil_width_;      // cm
           Double_t coil_thickness_;  // cm
        
           Double_t capacity_;   // cm^3
           Double_t mass_;       // g
           Double_t density_;    // g/(cm^3)       
           
           Int_t    fill_color_;
           Int_t    line_color_;
           Char_t   transparency_;  
           
           Int_t    counts_;
           
           ClassDef(SpdHybMagnet::RingCoil,1)
    };
    
    //______________________________________________________________//
    struct LoopCoil : public TObject {
      
           LoopCoil(); 
          ~LoopCoil() {}
          
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
           
           ClassDef(SpdHybMagnet::LoopCoil,1)
    };
    
    virtual Bool_t FillParsIn(SpdPassiveGeoParSet* params);
    virtual Bool_t LoadParsFrom(SpdPassiveGeoParSet* params);

    FairModule* CloneModule() const { return new SpdHybMagnet(*this); }
       
private:
  
    Bool_t   fLockGeom; //! if true, prevent geometry modifications
    
    Double_t fLoopCoilAxialDistance;     // mm
    Double_t fRingCoilAxialDistanceL1;   // mm
    Double_t fRingCoilAxialDistanceL2;   // mm
    Double_t fRingCoilAxialDistanceC;    // mm
    
    LoopCoil fLoopCoil;
    RingCoil fRingCoil;

    void SetGeoPars(Int_t geotype);
    void SetDefaultMaterials(Int_t geotype);
    
    Bool_t ConstructGeometry_1();
    Bool_t ConstructGeometry_2();
    
    Bool_t BuildLoopCoil(LoopCoil& coil);
    Bool_t BuildRingCoil(RingCoil& coil);
    
    ClassDef(SpdHybMagnet,1)
};

//_____________________________________________________________________________
inline TString SpdHybMagnet::GetCoilMaterial(Int_t ncoil) const
{
   return (ncoil == 0) ? fLoopCoil.coil_material_ : fRingCoil.coil_material_;
}

//_____________________________________________________________________________
inline Double_t SpdHybMagnet::GetCoilWidth(Int_t ncoil) const
{
   return (ncoil == 0) ? fLoopCoil.coil_width_ : fRingCoil.coil_width_;
}

//_____________________________________________________________________________
inline Double_t SpdHybMagnet::GetCoilCapacity(Int_t ncoil) const
{
   return (ncoil == 0) ? fLoopCoil.capacity_ : fRingCoil.capacity_;
}

//_____________________________________________________________________________
inline Double_t SpdHybMagnet::GetCoilMass(Int_t ncoil) const
{
   return (ncoil == 0) ? fLoopCoil.mass_ : fRingCoil.mass_;
}

//_____________________________________________________________________________
inline Double_t SpdHybMagnet::GetCoilDensity(Int_t ncoil) const
{
   return (ncoil == 0) ? fLoopCoil.density_ : fRingCoil.density_;
}

//_____________________________________________________________________________
inline const TGeoVolume* SpdHybMagnet::GetCoil(Int_t ncoil) const 
{ 
   return (ncoil == 0) ? fLoopCoil.box_ : fRingCoil.tube_; 
}
 
//_____________________________________________________________________________
inline Int_t SpdHybMagnet::GetCoilCounts(Int_t ncoil) const 
{  
   return (ncoil == 0) ? fLoopCoil.counts_ : fRingCoil.counts_; 
}


#endif  /* __SPDHYBMAGNET_H__ */

