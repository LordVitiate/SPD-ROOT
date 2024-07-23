// $Id$
// Author: artur   2019/07/04

#ifndef __SPDTSTECGEOMAPPER_H__
#define __SPDTSTECGEOMAPPER_H__

#include "SpdGeoMapper.h"
#include <map>
#include <vector>

class TString;
class SpdTsECStrawModule;
class SpdTsECStrawLayer;


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsTECGeoMapper                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsTECGeoMapper: public SpdGeoMapper {

public:
    
    SpdTsTECGeoMapper();
    SpdTsTECGeoMapper(TString prefix);
    
    virtual ~SpdTsTECGeoMapper();
    
    static SpdTsTECGeoMapper* Instance();
    static SpdTsTECGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = kFALSE);
    
    virtual void   UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
   
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
     
    virtual void Print(Option_t*) const;
    
    Int_t      GetNSectors() const;
    Double_t   GetSecAngle() const; // deg
    Double_t   GetSecAngleOver2() const; // deg
   
    TString    GetBaseMaterial() const;
    TString    GetLayerMaterial() const;
    TString    GetBaseStrawMaterial() const;
    TString    GetStrawShellMaterial() const;
    TString    GetStrawWireMaterial() const;
    
    Int_t      GetNModules(Int_t opt /*0,1,-1*/) const;  // hybdid geometry: total(0), forward(1), backward(-1) 
    Int_t      GetNModules() const;                      // toroidal geometry
    
    Double_t   GetSize() const;
    Double_t   GetWidth() const;
    Double_t   GetLength() const;
    Double_t   GetMaxR() const;
    Double_t   GetMinDistance() const;
    Double_t   GetMaxDistance() const;
    
    Int_t      GetModuleType(Int_t opt /*1,-1*/, Int_t nmodule) const;
    Double_t   GetModuleLength(Int_t opt /*1,-1*/, Int_t nmodule);
    Double_t   GetModuleDist(Int_t opt /*1,-1*/, Int_t nmodule) const;
    Double_t   GetModulePhi(Int_t opt /*1,-1*/, Int_t nmodule) const;
    
    Double_t   GetModuleLength(Int_t mtype);
    Double_t   GetLayerLength(Int_t ltype);
    Double_t   GetStrawRadius(Int_t stype) const;
    void       GetStrawInners(Int_t stype, Double_t& wshell, Double_t& rwire) const;
    
    Int_t      GetLayerType(Int_t mtype) const;
    Int_t      GetStrawType(Int_t ltype) const;
      
    Double_t   GetModuleSizePar(Int_t opt /*1,-1*/, Int_t nmodule) const;
    Double_t   GetModuleLengthPar(Int_t opt /*1,-1*/, Int_t nmodule) const;
    Double_t   GetModulePos(Int_t opt /*1,-1*/, Int_t nmodule) const;
      
    virtual TGeoMatrix* GetNodeMatrix(const SpdGeoTableRec* rec, Int_t loc_i) const;
    
    /* =========================== SET METHODS =========================== */
    
    /* hybrid geometry */
   
    void  AddModules(Int_t mtype, Double_t distance = 0 /* > 0 cm*/, Double_t angle = 0 /* deg */);
    void  AddForwardModule(Int_t mtype, Double_t distance = 0/* > 0 cm*/, Double_t angle = 0 /* deg */);
    void  AddBackwardModule(Int_t mtype, Double_t distance = 0/* > 0 cm*/, Double_t angle = 0 /* deg */);
    
    void  AddIdentModules(Int_t opt /*0,1,-1*/, Int_t mtype, Int_t nmodules, Double_t angle = 0 /* deg */);
    
    void  SetParameters(Int_t module_type, Int_t layer_type = 0, Int_t straw_type = 0); 
    void  SetGeometryPars(Int_t g); 
    
    void  RecalculateDistance(Int_t opt = 0 /*0,1,-1*/);
    
    //--------------------------------------------------------------------------------------
    // List of default module parameters (module = [1,10]):
    //--------------------------------------------------------------------------------------
    // par[0] = Ltype     : type of LAYER (integer)
    // par[1] = NLayers   : number of LAYERS inside the MODULE (integer)
    // par[2] = Lgapsize  : clearance between two nearest LAYERS inside the MODULE (>=0)
    // par[3] = Dphi      : local rotation angle step for LAYERS (integer i, dphi = i*45)
    //--------------------------------------------------------------------------------------
    static void SetDefaultModulePars(Int_t module, Int_t par, Double_t value);
    
    //--------------------------------------------------------------------------------------
    // List of default layer parameters (layer = [1,4]):
    //--------------------------------------------------------------------------------------
    // par[0] = Stype     : type of STRAW (integer i)
    // par[1] = Zgapsize  : z-clearance between two nearest STRAW inside the LAYER 
    // par[2] = Rgapsize  : r-clearance between two nearest STRAW inside the LAYER (>=0)
    // par[3] = Poption   : LAYER packing option (integer: 0 = "eo", 1 = "oe")
    // par[4] = DRadius   : LAYER radial offset inside the layer
    // par[5] = DBoresize : LAYER bore offset inside the layer
    // par[6] = Radius    : LAYER max. size (circumscribed circle radius); R < 0 => R = MODULE_MAX_RADIUS
    // par[7] = Boresize  : LAYER bore size (inscribed circle radius); B < 0 => B = MODULE_BORE_SIZE
    //--------------------------------------------------------------------------------------
    static void SetDefaultLayerPars(Int_t layer, Int_t par, Double_t value);
    
    //---------------------------------------------------
    // List of default STRAW parameters (straw = [1,2]):
    //---------------------------------------------------
    // par[0] = Rstraw    : straw max. radius
    // par[1] = Wshell    : shell width
    // par[2] = Rwire     : wire radius
    //---------------------------------------------------
    static void SetDefaultStrawPars(Int_t straw, Int_t par, Double_t value);
    
    //-------------------------------------------------------------------------------------
    
    // set module parameters 
    void SetModulePars(Int_t mtype, Int_t ltype, Int_t nlayers = -1,
                       Double_t lgapsize = 0./*cm*/, Int_t dphi = 1);
    
    // set layer parameters 
    void SetLayerPars(Int_t ltype, Int_t stype, 
                      Double_t zgapsize = 0./*cm*/, Double_t rgapsize = 0./*cm*/,
                      Int_t popt = 0 /*0 = "eo", 1 = "oe"*/,
                      Double_t dR = 0./*cm*/, Double_t dB = 0./*cm*/,
                      Double_t R = 0./*cm*/, Double_t B = 0./*cm*/);
    
    // set straw parameters 
    void SetStrawPars(Int_t stype, Double_t rstraw /*cm*/, 
                      Double_t wshell /*cm*/, Double_t rwire /*cm*/);
         
protected:

    /* data members */
       
    static SpdTsTECGeoMapper* fInstance;
    
    /* methods */ 
     
    Bool_t GetModulePackingPars(Int_t mtype, Int_t& ltype, Int_t& nlayers, 
                                Double_t& lgap, Double_t& ldphi, 
                                Double_t& length, Double_t& lradius, Double_t& lbsize);
    
    Bool_t GetLayerPackingPars(Int_t ltype, Int_t& stype, Int_t& popt,
                               Double_t& sradius, Double_t& lzgap, Double_t& lrgap,
                               Double_t& ldr, Double_t& ldb);
    
    SpdTsECStrawModule* InitStrawModulePacking(Int_t opt /*1,-1*/, Int_t nmodule);
                 Bool_t InitStrawLayerPacking(SpdTsECStrawLayer* layer);
    
    void SetParameters_3(Bool_t reinit);  // main build method
    
    Int_t CheckGeometry(Int_t& nfmods, Int_t& nbmods);
    
    Int_t GetNMaxLayers(Int_t ltype, Double_t lgapsize = 0);
        
    virtual TString GetPrintVolParsCap() const;
    
private:
   
    /* data members */
    
    static const Int_t NDEFMODULES = 2;
    static const Int_t NDEFLAYERS  = 2;
    static const Int_t NDEFSTRAWS  = 1;
   
    static Double_t theModulePars[NDEFMODULES][4];
    static Double_t theLayerPars[NDEFLAYERS][8];
    static Double_t theStrawPars[NDEFSTRAWS][3];
  
    static Int_t    Lt_(Int_t nmodule);
    static Int_t    Nl_(Int_t nmodule);
    static Double_t Lg_(Int_t nmodule);
    static Int_t    dphi_(Int_t nmodule);
    
    static Int_t    St_(Int_t nlayer);
    static Double_t Zg_(Int_t nlayer);
    static Double_t Rg_(Int_t nlayer);
    static Int_t    opt_(Int_t nlayer);
    static Double_t dR_(Int_t nlayer);
    static Double_t dB_(Int_t nlayer);
    static Double_t R_(Int_t nlayer);
    static Double_t B_(Int_t nlayer);
    
    static Double_t Rs_(Int_t nstraw);
    static Double_t Ws_(Int_t nstraw);
    static Double_t Rw_(Int_t nstraw);
    
    /* modules map */
    
    std::map < TString, std::vector<Int_t> > fVolumesMap;
    
    void  AddVolumeModule(TString volname, Int_t nmodule);
    Int_t GetVolumeNModules(TString volname) const;
    Int_t GetVolumeNModule(TString volname, Int_t nmodule) const;
   
    /* methods */
    
    Bool_t CheckLayerZgapsize(Int_t ltype, Double_t& zgapsize);
      
    ClassDef(SpdTsTECGeoMapper,1)
};

#endif  /* __SPDTSTECGEOMAPPER_H__ */

