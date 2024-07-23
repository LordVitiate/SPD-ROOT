
#include "SpdBarrelGeoPar.h"
#include "FairParamList.h"
#include <TObjArray.h>

ClassImp(SpdBarrelGeoPar)

//_____________________________________________________________________________
SpdBarrelGeoPar::SpdBarrelGeoPar(const char* name,
                                   const char* title,
                                   const char* context)
  : FairParGenericSet(name,title,context),
    fGeoSensNodes(new TObjArray()),
    fGeoPassNodes(new TObjArray())
{
  //std::cout << "<SpdBarrelGeoPar::SpdBarrelGeoPar> CREATE CONTAINER" << std::endl;  
}

//_____________________________________________________________________________
SpdBarrelGeoPar::~SpdBarrelGeoPar()
{
}

//_____________________________________________________________________________
void SpdBarrelGeoPar::clear()
{
  if(fGeoSensNodes) { delete fGeoSensNodes; }
  if(fGeoPassNodes) { delete fGeoPassNodes; }
}

//_____________________________________________________________________________
void SpdBarrelGeoPar::putParams(FairParamList* l)
{
  if (!l) { return; }
  
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

//_____________________________________________________________________________
Bool_t SpdBarrelGeoPar::getParams(FairParamList* l)
{
  if (!l) { return kFALSE; }
  
  if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) { return kFALSE; }
  if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes))   { return kFALSE; }
  
  return kTRUE;
}
