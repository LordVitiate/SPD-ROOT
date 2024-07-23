
#ifndef __SPDCAVE_H__
#define __SPDCAVE_H__

#include "FairModule.h"            
#include "SpdPassiveGeoParSet.h"

/////////////////////////////////////////////////////////////////////////////////
//
// SpdCave
//
// CAVE volume
//
/////////////////////////////////////////////////////////////////////////////////

class SpdCave : public FairModule {
  
public:
   
    SpdCave();
    SpdCave(const char* name, const char* title = "Cave");  
    
    virtual ~SpdCave() {}
    
    virtual void ConstructGeometry();
    
    virtual TString GetGeometryFileName();
    virtual void SetGeometryFileName(TString fname, TString geoVer = "0");

    SpdPassiveGeoParSet* GetParameters();
    TString GetActualGeometryFileName() const { return fgeoName; }
    
    Bool_t FillParsIn(SpdPassiveGeoParSet* params);
    Bool_t LoadParsFrom(SpdPassiveGeoParSet* params);

    FairModule* CloneModule() const { return new SpdCave(*this); }
    
private:
    
    ClassDef(SpdCave,1) 
};

#endif /* __SPDCAVE_H__ */

