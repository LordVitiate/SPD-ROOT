// $Id$
// Author: artur   2019/03/07

#ifndef __SPDITSGEOMAPPERX_H__
#define __SPDITSGEOMAPPERX_H__

#include "SpdGeoMapper.h"
#include <map>

class TString;
class SpdItsLayer;
class SpdItsLadder;
class SpdGeoVVolume;
class SpdGeoVVolumeBox2D;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdItsGeoMapperX                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
 
class SpdItsGeoMapperX: public SpdGeoMapper {

public:

    SpdItsGeoMapperX();
    SpdItsGeoMapperX(TString prefix);
    
    virtual ~SpdItsGeoMapperX();
    
    static SpdItsGeoMapperX* Instance();
    static SpdItsGeoMapperX* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void   UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
   
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
     
    virtual void Print(Option_t*) const;
    
    TString    GetBaseMaterial() const;
    TString    GetLadderMaterial() const;
    TString    GetSensorMaterial() const;
    Int_t      GetNLayers() const;
    Double_t   GetMinRadius() const; // cm
    Double_t   GetMaxRadius() const; // cm
    Double_t   GetMaxLength() const; // cm    
    Double_t   GetLayerLength(Int_t nlayer) const;  // cm
    Double_t   GetLayerWidth(Int_t nlayer) const;   // cm
    Double_t   GetLayerRadius(Int_t nlayer) const;  // cm
    Double_t   GetLayerRmin(Int_t nlayer) const;    // cm
    Double_t   GetLayerRmax(Int_t nlayer) const;    // cm
    Double_t   GetLayerPhi(Int_t nlayer) const;     // deg
    
    Double_t   GetMaxStepFactor();
    
    virtual SpdGeoVVolume* GetVVolume(TString chip); 
    virtual SpdGeoVVolume* GetVVolume(Int_t node_id);
        
    virtual TGeoMatrix* GetNodeMatrix(const SpdGeoTableRec* rec, Int_t loc_i) const;
    
    Bool_t  AreEndcapsEnabled() const;
    
    /* =========================== SET METHODS =========================== */
    
    void  SetNLayers(Int_t nlayers);  // force number of layers 
    
    void  SetMaxStepFactor(Double_t f);
    
    //------------------------------------------------------------------------
    // List of default layer/ladder parameters (geom = [1,5], layer = [1,5]):
    //----------------------------------------------------------------------------
    // par[0] = Rlayer    : the distance from z axis to ladder center (LAYER radius)
    // par[1] = Lz        : ladder size along z axis
    // par[2] = Lphi      : ladder size along phi axis
    // par[3] = Lr        : radial size of ladder 
    // par[4] = angle     : local rotation angle for ladder
    // par[5] = Phi       : global rotation angle for LAYER
    // par[6] = Nladders  : number of ladders inside the LAYER (will be calculated automatically for geometry = 1,2)
    //-------------------------------------------------------------------------
    static void SetDefaultLadderPars(Int_t geometry, Int_t layer, Int_t par, Double_t value);
    static void SetDefaultLadderPars(Int_t geometry, Int_t par, Double_t value); // all layers
   
    //-------------------------------------------------------------------------
    // List of default ladder/chip parameters (geom = [1,5], layer = [1,5]): 
    //-------------------------------------------------------------------------
    // par[0] = lz        : chip size along z-axis
    // par[1] = lphi      : chip phi-size  
    // par[2] = dlz       : gap size between chips along z-axis
    // par[3] = dlphi     : gap size between chips along phi-axis
    // par[4] = ncz       : number of chip cells (channels) along z-axis
    // par[5] = ncphi     : number of chip cells (channels) along phi-axis
    // par[6] = type      : chip type (0: NO INNER STRUCTURE, 1: MAPS, 2: DSSD, >2: USER DEFINED)
    //-------------------------------------------------------------------------
    static void SetDefaultChipPars(Int_t geometry, Int_t layer, Int_t par, Double_t value);
    static void SetDefaultChipPars(Int_t geometry, Int_t par, Double_t value); // all layers
    
    // set layer/ladder parameters 
    void SetLayerPackingPars2(Int_t nlayer, 
                              Int_t Nladders,   /* number of ladders */
                              Double_t Rlayer /*cm*/,  /* the distance from the ladder center to z-axis */
                              Double_t Lz /*cm*/, Double_t Lphi /*cm*/, Double_t Lr /*cm*/,  /* ladder sizes */
                              Double_t angle = 0. /*deg*/, Double_t Phi = 0. /*deg*/);
    
    // set layer/ladder parameters (in this case number of ladders will be calculated automatically)
    void SetLayerPackingPars3(Int_t nlayer, 
                              Double_t Rlayer /*cm*/, /* the distance from the ladder center to z-axis */
                              Double_t Lz /*cm*/, Double_t Lphi /*cm*/, Double_t Lr /*cm*/,  /* ladder sizes */
                              Double_t angle = 0. /*deg*/, Double_t Phi = 0. /*deg*/);
      
     // set ladder/chip parameters
    void SetLadderPackingPars(Int_t nlayer, 
                              Int_t chiptype,                            /* type of the chip */
                              Int_t ncz, Int_t ncphi,                    /* number of cells (channels) in the chip */
                              Double_t lz /*cm*/, Double_t lphi /*cm*/,  /* chip sizes */
                              Double_t dlz = 0. /*cm*/, Double_t dlphi = 0. /*cm*/);
    
    // set parameters
    void SetGeometryPars(Int_t g1 /*layer geometry*/, Int_t g2 = 0 /*ladder geometry*/);
    
    void EnableEndcaps(Bool_t on = kTRUE);
          
protected:
    
    /* data members */
    static SpdItsGeoMapperX* fInstance;
    
    /* methods */ 
    void   SetLayerPackingPars(Int_t nlayer, Int_t nladders, 
                               Double_t Rlayer /*cm*/, Double_t& Rmin /*cm*/,  Double_t& Rmax /*cm*/,
                               Double_t Lz /*cm*/, Double_t Lphi /*cm*/, Double_t Lr /*cm*/, 
                               Double_t angle = 0. /*deg*/, Double_t Phi = 0. /*deg*/);
    
    Bool_t GetLayerPackingPars(Int_t nlayer, Int_t& nladders, 
                               Double_t& Rlayer /*cm*/, Double_t& Rmin /*cm*/,  Double_t& Rmax /*cm*/,
                               Double_t& Lz /*cm*/, Double_t& Lphi /*cm*/, Double_t& Lr /*cm*/, 
                               Double_t& angle /*deg*/, Double_t& Phi /*deg*/);
    
    Bool_t GetLadderPackingPars(Int_t nlayer, Int_t& nz, Int_t& nphi,
                                Double_t& lz /*cm*/, Double_t& lphi  /*cm*/, 
                                Double_t& dlz /*cm*/, Double_t& dlphi /*cm*/);
    
    Bool_t GetChipPackingPars(Int_t nlayer, TString& cname);
 
    
    SpdItsLayer* InitLayer(Int_t nlayer);
    
    void InitLadderPacking(Int_t nlayer, SpdItsLadder* ladder);
    
    void SetParameters(Bool_t reinit);  // main build method
    void SetEndcaps(); // build method for ITS endcaps
    
    virtual TString ChipName(Int_t type, Int_t ncz, Int_t ncphi) const;
    virtual TString GetPrintVolParsCap() const;
           
private:
  
    static Double_t thePars[5][5][7];
    static Double_t theCPars[5][5][7];
    
    static Double_t Rl_(Int_t g, Int_t nlayer);
    static Double_t Lz_(Int_t g, Int_t nlayer);
    static Double_t Lphi_(Int_t g, Int_t nlayer);
    static Double_t Lr_(Int_t g, Int_t nlayer);
    static Double_t angle_(Int_t g, Int_t nlayer);
    static Double_t Phi_(Int_t g, Int_t nlayer);
    static Int_t    Nl_(Int_t g, Int_t nlayer);
    
    static Double_t lz_(Int_t g, Int_t nlayer);    
    static Double_t lphi_(Int_t g, Int_t nlayer); 
    static Double_t dlz_(Int_t g, Int_t nlayer); 
    static Double_t dlphi_(Int_t g, Int_t nlayer);
    static Int_t    ncz_(Int_t g, Int_t nlayer); 
    static Int_t    ncphi_(Int_t g, Int_t nlayer); 
    static Int_t    ctype_(Int_t g, Int_t nlayer); 
    
    // virtual volume table
    
    typedef std::map<TString, SpdGeoVVolume*> VVolumesTable;
    typedef std::map<TString, SpdGeoVVolume*>::iterator VVolumesTableIter;
    typedef std::map<TString, SpdGeoVVolume*>::const_iterator VVolumesTableConstIter;    
    
    VVolumesTable* fVVolumes;
    
    void DeleteVVolumes();
    void PrintVVolumes() const;
    
    SpdGeoVVolumeBox2D* AddNewVolume(TString volname, VVolumesTable* table) const;
    SpdGeoVVolumeBox2D* FindVolume(TString volname, VVolumesTable* table) const;
    
    ClassDef(SpdItsGeoMapperX,1)
};

#endif  /* __SPDITSGEOMAPPERX_H__ */

