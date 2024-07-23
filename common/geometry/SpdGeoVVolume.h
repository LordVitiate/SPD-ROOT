// $Id$
// Author: artur   2019/04/18

#ifndef __SPDGEOVVOLUME_H__
#define __SPDGEOVVOLUME_H__

#include <TNamed.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGeoVVolume                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdGeoVVolume: public TNamed {

public:

    SpdGeoVVolume();
    SpdGeoVVolume(const Char_t* name, const Char_t* mother = "");
    
    virtual ~SpdGeoVVolume();
    
    virtual void      Clear() {}
    
    virtual void      SetMotherVolume(TString mname) { fTitle = mname; }
    
    virtual TString   GetMotherVolume() const { return fTitle; }
    
    virtual Int_t     GetCellId(Double_t x, Double_t y, Double_t z) const;
    virtual Bool_t    GetCellPos(Int_t id, Double_t& x, Double_t& y, Double_t& z) const;
    virtual Double_t  GetMaxStep() const;
    
    virtual Bool_t    MasterToLocal(Int_t id, Double_t& x, Double_t& y, Double_t& z);
    virtual Bool_t    LocalToMaster(Int_t id, Double_t& x, Double_t& y, Double_t& z);
    
    virtual void      PrintGeoVVolume() const;
    virtual void      PrintVolume() const {}
   
private:

    ClassDef(SpdGeoVVolume,1)
};

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGeoVVolumeBox2D                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdGeoVVolumeBox2D: public SpdGeoVVolume {

public:

    SpdGeoVVolumeBox2D();
    SpdGeoVVolumeBox2D(const Char_t* name, const Char_t* title = "");
    
    virtual ~SpdGeoVVolumeBox2D();
    
    void SetCells(Double_t n1, Double_t n2);
    void SetSizes(Double_t L1, Double_t L2);
    
    void SetAxes(Char_t x1 = 'x', Char_t x2 = 'y');
    
    void SetMaxStep(Double_t step)    { fMaxStep = (step > 0) ? step : 1.; }
    void SetMaxStepFactor(Double_t f) { fMaxStepFactor = (f > 0) ? f : 1;  }
      
    virtual Int_t  GetCellId(Double_t x, Double_t y, Double_t z) const;
    virtual Bool_t GetCellPos(Int_t id, Double_t& x, Double_t& y, Double_t& z) const;
    virtual Bool_t GetCellPos(Int_t id, Double_t* pos /*3-dim array*/) const;
    virtual Bool_t GetPos(Double_t id1, Double_t id2, Double_t* pos /*3-dim array*/) const;
            
    Int_t   GetCellId(Double_t x1, Double_t x2) const;
    Bool_t  GetCellPos(Int_t id, Double_t& x1, Double_t& x2) const;
  
    Bool_t  GetPos(Double_t id1, Double_t id2, Double_t& x1, Double_t& x2) const;
  
    inline Int_t CellId(Int_t id1, Int_t id2) const { return fN2 * id1 + id2;} 
    inline void  CellId(Int_t id,  Int_t& id1, Int_t& id2) const { id1 = id/fN2; id2 = id%fN2; }
    
    virtual Double_t GetMaxStep() const { return fMaxStep; }
    virtual Double_t GetMaxStepFactor() const { return fMaxStepFactor; }
    
    virtual void PrintVolume() const;
    
    const Char_t* GetAxes() const;
    Char_t GetAxis(Int_t axis /*1,2*/) const;
    
private:

    Int_t    fN1, fN2;
    Double_t fL1, fL2;
    Double_t fD1, fD2;
    
    Double_t fMaxStep;
    Double_t fMaxStepFactor;
    
    Int_t    fAxes;
    
    ClassDef(SpdGeoVVolumeBox2D,1)
};

#endif  /* __SPDGEOVVOLUME_H__ */

