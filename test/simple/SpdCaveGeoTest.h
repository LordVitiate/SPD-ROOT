
#ifndef __SPDCAVEGEOTEST_H__
#define __SPDCAVEGEOTEST_H__

#include "FairModule.h"     

class SpdCaveGeoTest : public FairModule
{
  public:
          
    SpdCaveGeoTest(const char* name = "GeoTest", const char* Title="Geo Test") { }
    virtual ~SpdCaveGeoTest() {}
    
    virtual void ConstructGeometry();

  private:
          
    ClassDef(SpdCaveGeoTest,1) 
};

#endif 