// $Id$
// Author: artur   2018/02/13

#ifndef __SPDTORBASKET_H__
#define __SPDTORBASKET_H__

#include "SpdPassiveModule.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTorBasket                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTorBasket: public SpdPassiveModule {

public:

    SpdTorBasket();
    SpdTorBasket(const char* Name, const char* Prefix = "SPDTORBASKET");
     
    virtual ~SpdTorBasket();
    
    virtual void ConstructGeometry();
    
    virtual Bool_t   IsGeometryLocked() const { return fLockGeom; }
  
    virtual Double_t GetCapacity() const;  // cm^3
    virtual Double_t GetMass()     const;  // g
  
    virtual void UnsetMaterials(TString media = "vacuum2");
    virtual void ResetMaterials(/*set dafault materials*/);
    
    virtual void Print(Option_t*) const; 
    
    virtual Bool_t FillParsIn(SpdPassiveGeoParSet* params);
    virtual Bool_t LoadParsFrom(SpdPassiveGeoParSet* params);

   FairModule* CloneModule() const { return new SpdTorBasket(*this); }

private:
  
    Bool_t   fLockGeom; //! if true, prevent geometry modifications
    
    TGeoVolume* fLeafBox;    //!
    TGeoVolume* fCasingWall; //!
    
    TString  fBaseMaterial;
    TString  fLeafMaterial;
    TString  fCasingMaterial;

    Double_t fLeafBoxAxialDistance;    
    Double_t fLeafBoxHalfX;
    Double_t fLeafBoxHalfY;
    Double_t fLeafBoxHalfZ;
    
    Int_t    fLeafBoxNNodes;
    Double_t fLeafBoxCapacity;
    Double_t fLeafBoxMass;
    
    Double_t fCasingWallHmin;
    Double_t fCasingWallHmax;
    Double_t fCasingWallHalfLmin;
    Double_t fCasingWallHalfLmax;
    Double_t fCasingWallHalfLen;
    
    Int_t    fCasingWallNNodes;
    Double_t fCasingWallCapacity;
    Double_t fCasingWallMass;   
    
    void     BuildSection(Int_t ns, Double_t zshift); 
    
    Bool_t   BuildLeaf(); 
    Bool_t   BuildCasingWall(); 
    
    void SetGeoPars(Int_t geotype);
    void SetDefaultMaterials(Int_t geotype);
    
    TGeoVolume* BuildArb(TString volname, TString matname, Double_t HalfLen,
                         Double_t Hmin, Double_t Hmax, 
                         Double_t Lmin, Double_t Lmax);
    
    Double_t GetLeafDensity() const;
    Double_t GetCasingWallDensity()const;

    ClassDef(SpdTorBasket,1)
};

#endif  /* __SPDTORBASKET_H__ */

