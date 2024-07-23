
#include "SpdBarrelContFact.h"
#include "SpdBarrelGeoPar.h"
#include "FairRuntimeDb.h"

ClassImp(SpdBarrelContFact)

static SpdBarrelContFact gSpdBarrelContFact;

//_____________________________________________________________________________
SpdBarrelContFact::SpdBarrelContFact():FairContFact()
{
  fName = "SpdBarrelContFact";
  fTitle = "Factory for parameter containers in libSpdBarrel";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________
FairParSet* SpdBarrelContFact::createContainer(FairContainer* c)
{
  /*
      Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = NULL;
  
  if (strcmp(name,"SpdBarrelGeoPar") == 0) {
      p = new SpdBarrelGeoPar(c->getConcatName().Data(),
                              c->GetTitle(),
                              c->getContext());
  }
  
  return p;
}

//_____________________________________________________________________________
void SpdBarrelContFact::setAllContainers()
{
  /* 
      Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdBarrel library.
  */

  FairContainer* p = new FairContainer("SpdBarrelGeoPar",
                                       "SpdBarrel Geometry Parameters",
                                       "TestDefaultContext");
  p->addContext("TestNonDefaultContext");
  containers->Add(p);
}
