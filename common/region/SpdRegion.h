// $Id$
// Author: artur   2017/11/28

#ifndef __SPDREGION_H__
#define __SPDREGION_H__

#include <TNamed.h>
#include <TGeoNavigator.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRegion                                                                  //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRegion: public TNamed {

public:

    SpdRegion();
    SpdRegion(const Char_t* name, const Char_t* type);
    virtual ~SpdRegion() {}
    
    virtual TString GetRegionType() const { return fTitle; } 
    
    virtual Bool_t IsInside(Double_t x, Double_t y, Double_t z) const
    { /* is not implemented here */ return kFALSE; }
    
    virtual Bool_t IsInside(Double_t r, Double_t z) const
    { /* is not implemented here */ return kFALSE; }
    
    /* ----- init region ----- */
   
    virtual Bool_t SetBoxRegion(Double_t xmin, Double_t xmax, Double_t ymin, 
                                Double_t ymax, Double_t zmin, Double_t zmax) 
                  { /* method is not implemented here */ return kFALSE; }
   
    virtual Bool_t SetTubeRegion(Double_t rmin, Double_t rmax, 
                                 Double_t zmin, Double_t zmax) 
                  { /* method is not implemented here */ return kFALSE; }
                  
    virtual Bool_t SetPhysicalRegion(TString region, TString type)
                  { /* method is not implemented here */ return kFALSE; } 
                  
    /* ----- getters ----- */
    
    virtual Bool_t GetBoxRegion(Double_t& /*xmin*/, Double_t& /*xmax*/, 
                                Double_t& /*ymin*/, Double_t& /*ymax*/, 
                                Double_t& /*zmin*/, Double_t& /*zmax*/) const 
                  { /*  method is not implemented here */ return kFALSE; }
                           
    virtual Bool_t GetTubeRegion(Double_t& /*rmin*/, Double_t& /*rmax*/, 
                                 Double_t& /*zmin*/, Double_t& /*zmax*/) const 
                  { /*  method is not implemented here */ return kFALSE; } 
                  
    virtual Bool_t GetPhysicalRegion(TString& region, TString& type) const
                  { /*  method is not implemented here */ return kFALSE; }
                  
    virtual Double_t GetXmin() const { return 0; }
    virtual Double_t GetXmax() const { return 0; }
    virtual Double_t GetYmin() const { return 0; }
    virtual Double_t GetYmax() const { return 0; }
    virtual Double_t GetZmin() const { return 0; }
    virtual Double_t GetZmax() const { return 0; }
    virtual Double_t GetRmin() const { return 0; }
    virtual Double_t GetRmax() const { return 0; }
    
    virtual Double_t GetLx()   const { return 0; }
    virtual Double_t GetLy()   const { return 0; }
    virtual Double_t GetLz()   const { return 0; }
    virtual Double_t GetDR()   const { return 0; }
    
    virtual TString  GetRegion() const { return "unknown"; }
    virtual TString  GetType() const { return "unknown"; }
    
    virtual Bool_t IsRegionInit() const { return fIsRegionInit; }
    
    virtual TString GetMedium(Double_t x, Double_t y, Double_t z) const;
    virtual TString GetGeoPath(Double_t x, Double_t y, Double_t z) const;
    
    /* ------ print ------ */

    virtual void Print(Option_t* option = "") {}

protected:

    Bool_t fIsRegionInit;
    
    TGeoNavigator* fNavigator;
    
    ClassDef(SpdRegion,1)
};

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdBoxRegion                                                               //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdBoxRegion: public SpdRegion {
  
public:
  
   SpdBoxRegion();
   virtual ~SpdBoxRegion() {}
   
   virtual Bool_t IsInside(Double_t x, Double_t y, Double_t z) const;
   virtual Bool_t IsInside(Double_t r, Double_t z) const;
   
   /* init region */
   
   virtual Bool_t SetBoxRegion(Double_t xmin, Double_t xmax, Double_t ymin, 
                               Double_t ymax, Double_t zmin, Double_t zmax);
   
   /* getters */
   
   virtual Bool_t GetBoxRegion(Double_t& xmin, Double_t& xmax, 
                               Double_t& ymin, Double_t& ymax, 
                               Double_t& zmin, Double_t& zmax) const;
   
   virtual Double_t GetXmin() const { return fXmin; }
   virtual Double_t GetXmax() const { return fXmax; }
   virtual Double_t GetYmin() const { return fYmin; }
   virtual Double_t GetYmax() const { return fYmax; }
   virtual Double_t GetZmin() const { return fZmin; }
   virtual Double_t GetZmax() const { return fZmax; }
   
   virtual Double_t GetLx()   const { return fXmax - fXmin; }
   virtual Double_t GetLy()   const { return fYmax - fYmin; }
   virtual Double_t GetLz()   const { return fZmax - fZmin; }
   
   /* ------ print ------ */

   virtual void Print(Option_t* option = "");
  
protected:
  
   Double_t fXmin;   
   Double_t fXmax;
   Double_t fYmin;
   Double_t fYmax;
   Double_t fZmin;
   Double_t fZmax;
    
   ClassDef(SpdBoxRegion,1)
};

//________________________________________________________________________
inline Bool_t SpdBoxRegion::GetBoxRegion(Double_t& xmin, Double_t& xmax, 
                                         Double_t& ymin, Double_t& ymax, 
                                         Double_t& zmin, Double_t& zmax) const
{ 
   xmin = fXmin; xmax = fXmax; 
   ymin = fYmin; ymax = fYmax; 
   zmin = fZmin; zmax = fZmax;
    
   return kTRUE;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTubeRegion                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTubeRegion: public SpdRegion {

public:
  
   SpdTubeRegion();
   virtual ~SpdTubeRegion() {}
   
   virtual Bool_t IsInside(Double_t x, Double_t y, Double_t z) const;
   virtual Bool_t IsInside(Double_t r, Double_t z) const;
   
   /* init region */
   
   virtual Bool_t SetTubeRegion(Double_t rmin, Double_t rmax, 
                                Double_t zmin, Double_t zmax); 
   
   /* getters */
   
   virtual Bool_t GetTubeRegion(Double_t& rmin, Double_t& rmax,
                                Double_t& zmin, Double_t& zmax) const;
    
   virtual Double_t GetRmin() const { return fRmin; }
   virtual Double_t GetRmax() const { return fRmax; }
   virtual Double_t GetZmin() const { return fZmin; }
   virtual Double_t GetZmax() const { return fZmax; }
   
   virtual Double_t GetLz()   const { return fZmax - fZmin; }
   virtual Double_t GetDR()   const { return fRmax - fRmin; }

   /* ------ print ------ */
  
   virtual void Print(Option_t* option = "");
   
protected:
  
   Double_t fRmin;
   Double_t fRmax;
   Double_t fZmin;
   Double_t fZmax;
  
   ClassDef(SpdTubeRegion,1)
};

//_____________________________________________________________________________
inline Bool_t SpdTubeRegion::GetTubeRegion(Double_t& rmin, Double_t& rmax,
                                           Double_t& zmin, Double_t& zmax) const
{ 
   rmin = fRmin; rmax = fRmax;
   zmin = fZmin; zmax = fZmax;
     
   return kTRUE;
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdPhysicalRegion                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdPhysicalRegion: public SpdRegion {

public:
  
   SpdPhysicalRegion();
   virtual ~SpdPhysicalRegion() {}
   
   virtual Bool_t IsInside(Double_t x, Double_t y, Double_t z) const;
   virtual Bool_t IsInside(Double_t r, Double_t z) const { return kFALSE; }
   
   /* init region */
   
   virtual Bool_t SetPhysicalRegion(TString region, TString type); 

   /* getters */
   
   virtual Bool_t GetPhysicalRegion(TString& region, TString& type) const; 
   
   virtual TString  GetRegion() const { return fRegion; }
   virtual TString  GetType() const;

   /* ------ print ------ */
  
   virtual void Print(Option_t* option = "");
   
protected:
  
   TString fRegion;
   Int_t   fType;
  
   ClassDef(SpdPhysicalRegion,1)
};

//_____________________________________________________________________________
inline Bool_t SpdPhysicalRegion::GetPhysicalRegion(TString& region, TString& type) const 
{ 
   region = fRegion;
   type = GetType();
   
   return kTRUE;
}

#endif  /* __SPDREGION_H__ */

