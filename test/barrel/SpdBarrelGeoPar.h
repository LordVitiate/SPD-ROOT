
#ifndef __SPDBARRELGEOPAR_H__
#define __SPDBARRELGEOPAR_H__

#include "FairParGenericSet.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdBarrelGeoPar                                                            //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class TObjArray;
class FairParamList;

class SpdBarrelGeoPar : public FairParGenericSet {
  
public:

    SpdBarrelGeoPar(const char* name = "SpdBarrelGeoPar",
                    const char* title = "SpdBarrel Geometry Parameters",
                    const char* context = "TestDefaultContext");
   ~SpdBarrelGeoPar();
    
    void clear();
    
    void   putParams(FairParamList*);
    Bool_t getParams(FairParamList*);
    
    TObjArray* GetGeoSensitiveNodes() const { return fGeoSensNodes; }
    TObjArray* GetGeoPassiveNodes()   const { return fGeoPassNodes; }

private:

    SpdBarrelGeoPar(const SpdBarrelGeoPar&);
    SpdBarrelGeoPar& operator=(const SpdBarrelGeoPar&);

    TObjArray*  fGeoSensNodes;
    TObjArray*  fGeoPassNodes;
    
    ClassDef(SpdBarrelGeoPar,1)
};

#endif /* __SPDBARRELGEOPAR_H__ */
