// $Id$
// Author: artur   2015/09/26

#ifndef __SPDSTTGEOMAPPER_H__
#define __SPDSTTGEOMAPPER_H__

#include <TObject.h>
#include <TObjArray.h>
#include <vector>

#include "SpdSttStrawTube.h"
#include "SpdParameter.h"

class TString;
class SpdParameter;

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdSttGeoMapper                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdSttGeoMapper: public TObject {

public:

    SpdSttGeoMapper(Int_t GeoType = 0);
    virtual ~SpdSttGeoMapper();
    
    static SpdSttGeoMapper* Instance()  { return (gInstance) ? gInstance : new SpdSttGeoMapper(); }
    
    virtual void ClearGeometry();
    virtual void ClearParameters();
    virtual void ClearComplexGeom();
    
    virtual void InitGeometry(Int_t gtype, Bool_t reinit = kFALSE);
    
    /* Set-methods */
    
    void SetScale(Double_t scale);    // set "n_tube_sections"
    void SetTubeStep(Double_t step);  // set "scale"
    void SetTubeSections(Int_t nsec); // set "tube_step"
    
    /* Get-methods */
    
    virtual Bool_t IsInit() { return GeoType_; }
     
    virtual SpdParameter* AddParameter(TString par_name);
    virtual SpdParameter* GetParameter(TString par_name);
    virtual TObjArray*    GetParameters();
    
    virtual Double_t GetDigitPar(const Text_t* par_name);
      
    virtual SpdSttStrawTube* GetTube() { return Tube_; }
    virtual SpdSttStrawTube* GetTube(Int_t id, Int_t option = 0);   
    
    inline  Int_t GetGeoType()    const { return GeoType_;     } 
    inline  Int_t GetTotalTubes() const { return NTotalTubes_; }  
    inline  Int_t GetNSections()  const { return NSections_;   } 
   
    /* methods for complex geometry */
    inline  Bool_t IsComplexGeom() { return ComplexGeom_; }
    
    inline  Int_t GetNBlockTypes() { return Blocks_.size();     }
    inline  Int_t GetNBlocks()     { return BlockLocs_.size();  }
    inline  Int_t GetNBlockPars()  { return (BlockPars_) ? sizeof(BlockPars_) : 0; }
    
    virtual SpdSttGeoMapper* GetBlockMapper(Int_t block_type);
    virtual Double_t*        GetBlockLocation(Int_t block_num);
    virtual Double_t*        GetBlockParameters();
    
    virtual void print(Int_t opt = 0);
   
private:
  
    /* data members */
    static SpdSttGeoMapper* gInstance;
    
    Int_t                         GeoType_;
    std::vector<Int_t>            GeoId_; 
    std::vector<SpdSttStrawTube*> Tubes_; 
    SpdSttStrawTube*              Tube_;
     
    /* parameters */ 
    TObjArray* Params_;
    
    Int_t    NTotalTubes_;       // total number of tubes    
    Int_t    NTubesPerSection_;  // number of tubes per section
    Int_t    NSections_;         // number of sections
            
    /* GEOMETRY OF TYPE 1 PARAMETERS & METHODS */
    void SetParametersOfGeoType1(Bool_t reinit_default = 0);
    bool InitGeometryOfType1();
    bool SetTubeOfType1(Int_t id, Int_t option); 
    /*-------------- GEOMETRY 1 -------------- */
              
    
    /* GEOMETRY OF TYPE 2 PARAMETERS & METHODS */
    void SetParametersOfGeoType2(Bool_t reinit_default = 0);
    bool InitGeometryOfType2();
    bool SetTubeOfType2(Int_t id, Int_t option);
    /*-------------- GEOMETRY 2 -------------- */
    
    
    /*** DEFINITIONS FOR COMPLEX GEOMETRY ***/
    
    Bool_t ComplexGeom_;
    std::vector<SpdSttGeoMapper*>  Blocks_;     //! list of different blocks
    std::vector<Double_t*>         BlockLocs_;  //! list of the block locations
    Double_t*                      BlockPars_;  //! the block parameters
    
    ClassDef(SpdSttGeoMapper,1)
};

#endif  /* __SPDSTTGEOMAPPER_H__ */

