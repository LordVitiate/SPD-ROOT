// $Id$
// Author: artur   2018/05/29

#ifndef __SPDTSSBGEOBUILDER_H__
#define __SPDTSSBGEOBUILDER_H__

#include "SpdGeoBuilder.h"
#include "SpdGeoMapper.h"

#include <TGeoMatrix.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsSBGeoBuilder                                                          //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsSBGeoBuilder: public SpdGeoBuilder {

public:

    SpdTsSBGeoBuilder();
    virtual ~SpdTsSBGeoBuilder();
    
    static SpdTsSBGeoBuilder* Instance();
    static SpdTsSBGeoBuilder* GetBuilder();
    
    virtual Bool_t ConstructGeometry();
    
    virtual void SetMapper(SpdGeoMapper* mapper);
    
    virtual SpdGeoMapper* GetMapper() const { return fGeoMapper; }
    virtual SpdGeoTable*  GetTable()  const;
    
protected:
 
    /* data members */
    static SpdTsSBGeoBuilder* fInstance;
    
    SpdGeoMapper* fGeoMapper;
    
    virtual TGeoVolume* BuildVolume(const SpdGeoVolPars* vpars);
   
private:
    
    ClassDef(SpdTsSBGeoBuilder,1)
};

#endif  /* __SPDTSSBGEOBUILDER_H__ */

