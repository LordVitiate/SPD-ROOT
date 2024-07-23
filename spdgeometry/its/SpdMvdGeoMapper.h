// $Id$
// Author: artur   2021/11/12

#ifndef __SPDMVDGEOMAPPER_H__
#define __SPDMVDGEOMAPPER_H__

#include <TObjArray.h>

#include "SpdGeoMapper.h"
#include <vector>

class TString;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdMvdGeoMapper                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdMvdGeoMapper: public SpdGeoMapper {

public:
  
    SpdMvdGeoMapper();
    SpdMvdGeoMapper(TString prefix);
    
    virtual ~SpdMvdGeoMapper();
    
    static SpdMvdGeoMapper* Instance();
    static SpdMvdGeoMapper* GetMapper();
    
    virtual Bool_t InitGeometry(Int_t gtype = -1, Bool_t reinit = false);
    
    virtual void   UnsetMaterials(Bool_t precise = kTRUE, TString option = "base");
    
    virtual Bool_t IsGeoTypeDefined(Int_t gtype) const;
    
    void       SetActiveMaterial(TString material);
   
    TString    GetActiveMaterial() const;
   
    TString    GetBaseMaterial()   const;
    Double_t   GetMaxLength()      const;
    Double_t   GetMinRadius()      const;
    Double_t   GetMaxRadius()      const;
    
    Int_t      GetNLayers() const;
    Int_t      GetNSublayers(Int_t nlayer) const;
    
    // nlayer = 1, nlayers
    Double_t   GetLayerLength(Int_t nlayer) const;
    Double_t   GetLayerRmin(Int_t nlayer) const;
    Double_t   GetLayerRmax(Int_t nlayer) const;
    TString    GetLayerBaseMaterial(Int_t nlayer) const;
    Bool_t     IsLayerActive(Int_t nlayer) const;   
    
    Double_t   GetLayerPhi(Int_t nlayer) const;    
    Double_t   GetLayerZpos(Int_t nlayer) const;
    
    // nlayer = 1, nlayers;  nsublayer = 1, nsublayers
    Double_t   GetSublayerRmin(Int_t nlayer, Int_t nsublayer) const;
    Double_t   GetSublayerWidth(Int_t nlayer, Int_t nsublayer) const;
    TString    GetSublayerMaterial(Int_t nlayer, Int_t nsublayer) const;
    
    /* >>>>>>>>>>>>>> Methods to build detector geometry <<<<<<<<<<<<<<< */
    
    // Remove all layers
    void     ClearGeometry();
    
    //----------------------------------------------------------------------------------
    // Create a new layer;
    // returns (temporary) layer label (-1 if layer has not been added).
    // The new layer is not a detector, so set its activity manually (if necessary) 
    // using SetLayerActivity method (see below)
    
    Int_t    DefineLayer(Double_t Rmin /*cm*/, Double_t Length /*cm*/, 
                         Double_t Phi = 0./*deg*/, Double_t Zpos = 0./*cm*/, 
                         Double_t dead_zone = 6. /*cm*/, TString dzone_material = "carbon",
                         TString base_material = "air");
    
    //----------------------------------------------------------------------------------
    // Create a new layer using an existed one (tagged by layer_label) and redefine some of its parameters;
    // returns (temporary) layer label (-1 if layer has not been added).
    // The new layer is not a detector, so set its activity manually (if necessary) 
    // using SetLayerActivity method (see below)
    
    Int_t    DefineLayerCopy(Int_t layer_label, Double_t Rmin /*cm*/, 
                             Double_t Phi = 0./*deg*/, Double_t Zpos = 0./*cm*/);
    
    Int_t    DefineLayerCopy2(Int_t layer_label, Double_t Rmin /*cm*/, Double_t Length /*cm*/, 
                              Double_t Phi = 0./*deg*/, Double_t Zpos = 0./*cm*/);
    
    //----------------------------------------------------------------------------------
    // Mark layer as detector. 
    
    void     SetLayerActivity(Int_t layer_label, Bool_t is_active = true);
    
    //----------------------------------------------------------------------------------
    // Create a sublayer for the layer tagged by layer_label
    // returns max radius of the sublayer (0.0 if there is no layer with such a label)
    
    Double_t AddSublayer(Int_t layer_label, Double_t width /*cm*/, TString material = "");
    
    // >>>> Print Geometry <<<< 
    
    virtual void Print(Option_t* opt) const;

private:
  
    /* data members */
    static SpdMvdGeoMapper* fInstance; 
    
    void  SetParameters_1(Bool_t reinit); // standard geometry (1)
    void  SetParameters_2(Bool_t reinit); // standard geometry (2)
    void  SetParameters_3(Bool_t reinit); // user-defined geometry
    
    void  FillParameters(Bool_t reinit);

    void  Print2(Option_t*) const;
    
    // barrel layer
    struct mvd_blayer 
    {
        mvd_blayer(): length(100.), phi(0.), zpos(0.), dzone(6.0), 
                      dzone_mat("carbon"), base_mat("air"), is_active (false),
                      mother_layer(0),label(-1) {}
                      
        mvd_blayer* makeCopy(Double_t rmin) const
        {
            mvd_blayer* layer = new mvd_blayer();
            
            layer->length = length;
            layer->phi = phi;
            layer->zpos = zpos;
            layer->dzone = dzone;
            layer->dzone_mat = dzone_mat;
            layer->base_mat = base_mat;
            
            layer->r = r;
            layer->mat = mat;
            
            Double_t d = rmin-r[0];
            for (Double_t& rr : layer->r) rr += d;
        
            return layer; 
        }
        
        /* ----- setters ----- */
        
        void  setRmin(Double_t rr)  
              { if (!r.empty()) r.clear(); if (rr > 0) r.push_back(rr); }
        void  addLayer(Double_t dr) 
              { if (!r.empty() && dr > 0) { r.push_back(r[r.size()-1]+dr); mat.push_back(base_mat); } }
        void  addLayer(Double_t dr, TString material) 
              { if (!r.empty() && dr > 0) { r.push_back(r[r.size()-1]+dr); mat.push_back(material); } }      
        
        /*-----  getters ----- */
        
        Int_t     getNLayers() const { return r.size()-1; }
        
        Double_t  getRmin()    const { return (r.size() > 1) ? r[0] : -1; }
        Double_t  getRmax()    const { return (r.size() > 1) ? r[r.size()-1] : -1; }
        Double_t  getWidth()   const { return (r.size() > 1) ? r[r.size()-1]-r[0] : 0; }
        
        Double_t  getRmin(Int_t i)   const { return r[i];   }
        Double_t  getRmax(Int_t i)   const { return r[i+1]; }
        Double_t  getWidth(Int_t i)  const { return r[i+1]-r[i]; }
        
        Bool_t    checkRSize() const { return (r.size() > 1) ? (r[0] < r[r.size()-1]) : false; }
        
        TString getBaseMaterial()    const { return base_mat;  }
        TString getDZoneMaterial()   const { return dzone_mat; }
        TString getMaterial(Int_t i) const { return (i < mat.size()) ? mat[i] : base_mat; }
        
        /* ----- data members ----- */
        
        Double_t length;   // cm
        Double_t phi;      // deg (rotation angle around z-axis)
        Double_t zpos;     // cm
        
        Double_t dzone;    // cm, (dead zone)
        TString dzone_mat; // dead zone material
        
        TString base_mat;  // base material

        std::vector<Double_t> r;  // radius[nlayers+1]
        std::vector<TString> mat; // materials[nlayers]
        
        Bool_t is_active;  // detector or not 
        
        mvd_blayer* mother_layer; // pointer to mother layer (if this layer is a copy)
        Int_t label;              // this layer unique number >= 1 !
        
    };
    
    std::vector<mvd_blayer*> fBLayers;  //! barrel layers
    
    Bool_t    CheckGeometry();
    Bool_t    CheckLayer(Int_t nlayer);
       
    ClassDef(SpdMvdGeoMapper,1) 
};

#endif  /* __SPDMVDGEOMAPPER_H__ */


