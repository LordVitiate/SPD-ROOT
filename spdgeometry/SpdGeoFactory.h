// $Id$
// Author: artur   2018/11/26

#ifndef __SPDGEOFACTORY_H__
#define __SPDGEOFACTORY_H__

#include <TObject.h>

#include "SpdDetectorList.h"
#include "SpdGeoMapper.h"
#include "SpdGeoBuilder.h"
#include "SpdNodesIdTable.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGeoFactory                                                              //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdGeoFactory: public TObject {

public:

    SpdGeoFactory();
    virtual ~SpdGeoFactory();
    
    static SpdGeoFactory* Instance();
    
    virtual SpdGeoMapper*  SearchForMapper(DetectorId id, TString type = ""); // search for default mapper for the detector of a such id 
    virtual SpdGeoMapper*  SearchForMapper(TString classname);  // search for mapper of a such type
    virtual SpdGeoMapper*  Mapper(TString classname);           // get mapper (or create if doesn't exist!) of a such type
    
    virtual SpdGeoBuilder* SearchForBuilder(DetectorId id, TString type = ""); // search for default builder for the detector of a such id 
    virtual SpdGeoBuilder* SearchForBuilder(TString classname); // search for builder of a such type
    virtual SpdGeoBuilder* Builder(TString classname);          // get builder (or create if doesn't exist!) of a such type
    
    virtual SpdNodesIdTable* GetNodesIdTable(TString classname);
    
private:

    static SpdGeoFactory* fInstance;

    ClassDef(SpdGeoFactory,1)
};

#endif  /* __SPDGEOFACTORY_H__ */

