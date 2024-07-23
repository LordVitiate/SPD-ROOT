// $Id$
// Author: artur   2018/05/29

#ifndef __SPDTSTBGEOBUILDER_H__
#define __SPDTSTBGEOBUILDER_H__

#include "SpdGeoBuilder.h"
#include "SpdGeoMapper.h"

#include <TGeoMatrix.h>
#include <vector>
#include <set>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsTBGeoBuilder                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsTBGeoBuilder: public SpdGeoBuilder {

public:

    SpdTsTBGeoBuilder();
    virtual ~SpdTsTBGeoBuilder();
    
    static SpdTsTBGeoBuilder* Instance();
    static SpdTsTBGeoBuilder* GetBuilder();
    
    virtual Bool_t ConstructGeometry();
    
    virtual void SetMapper(SpdGeoMapper* mapper);
    
    virtual SpdGeoMapper* GetMapper() const { return fGeoMapper; }
    virtual SpdGeoTable*  GetTable()  const;
    
    virtual void FillParametersIn(SpdParSet* params);
    virtual void LoadParametersFrom(SpdParSet* params);
    
    /* 
     * remove volumes of number nj = n + j*mod 
     * n = [vf,vl]; j = 0,1, ... ; mod >= n. 
     */
       
    Bool_t  AddRejection(TString pattern, Int_t vf, Int_t vl = 0, Int_t mod = 1);
    void    PrintRejection(Int_t opt = 0) const;
    
    Bool_t  IsGeometryChanged();
    
protected:
 
    /* data members */
    static SpdTsTBGeoBuilder* fInstance;
    
    SpdGeoMapper* fGeoMapper; //!
    
    virtual TGeoVolume* BuildVolume(const SpdGeoVolPars* vpars);
  
    Bool_t  RejectVolume(TString volname);
        
private:
  
    std::multimap< TString, std::vector<Int_t> > fRejection;
    std::map< TString, Int_t > fCheckedVolumes;
    
    ClassDef(SpdTsTBGeoBuilder,1)
};

#endif  /* __SPDTSTBGEOBUILDER_H__ */

