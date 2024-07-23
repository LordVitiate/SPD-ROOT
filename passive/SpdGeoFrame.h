// $Id$
// Author: artur   2018/01/30

#ifndef __SPDGEOFRAME_H__
#define __SPDGEOFRAME_H__

#include "SpdPassiveModule.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdGeoFrame                                                                //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdGeoFrame: public SpdPassiveModule {

public:

    SpdGeoFrame();
    SpdGeoFrame(const char* Name, const char* Prefix = "SPDTORFRAME");
    
    virtual ~SpdGeoFrame();
 
    virtual void ConstructGeometry();
    
    virtual Bool_t   IsGeometryLocked() const { return kFALSE; }
  
    virtual Double_t GetCapacity() const;  // cm^3
    virtual Double_t GetMass()     const;  // g
  
    virtual void UnsetMaterials(TString media = "vacuum2");
    
    virtual void Print(Option_t*) const;
    
    virtual Bool_t FillParsIn(SpdPassiveGeoParSet* params);
    virtual Bool_t LoadParsFrom(SpdPassiveGeoParSet* params);

    FairModule* CloneModule() const { return new SpdGeoFrame(*this); }
 
private:
    
    Bool_t   fLockGeom; //! if true, prevent geometry modifications

    ClassDef(SpdGeoFrame,1)
};

#endif  /* __SPDGEOFRAME_H__ */

