// $Id$
// Author: artur   2019/07/04

#ifndef __SPDTSTECGEOBUILDER_H__
#define __SPDTSTECGEOBUILDER_H__

#include "SpdGeoBuilder.h"
#include "SpdGeoMapper.h"

#include <TGeoMatrix.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdTsTECGeoBuilder                                                         //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdTsTECGeoBuilder: public SpdGeoBuilder {

public:

    SpdTsTECGeoBuilder();
    virtual ~SpdTsTECGeoBuilder();
    
    static SpdTsTECGeoBuilder* Instance();
    static SpdTsTECGeoBuilder* GetBuilder();
    
    virtual Bool_t ConstructGeometry();
    
    virtual void SetMapper(SpdGeoMapper* mapper);
    
    virtual SpdGeoMapper* GetMapper() const { return fGeoMapper; }
    virtual SpdGeoTable*  GetTable()  const;
    
protected:
 
    /* data members */
    static SpdTsTECGeoBuilder* fInstance;
    
    SpdGeoMapper* fGeoMapper; //!
    
    virtual TGeoVolume* BuildVolume(const SpdGeoVolPars* vpars);
         
private:

    ClassDef(SpdTsTECGeoBuilder,1)
};

#endif  /* __SPDTSTECGEOBUILDER_H__ */

