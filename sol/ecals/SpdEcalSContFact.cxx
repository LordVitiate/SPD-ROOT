
#include "FairRuntimeDb.h"

#include "SpdEcalSContFact.h"

#include "SpdEcalSBParSet.h"
#include "SpdEcalSECParSet.h"

#include <iostream>

ClassImp(SpdEcalSContFact)

static SpdEcalSContFact gSpdEcalSContFact;

//_____________________________________________________________________________________
SpdEcalSContFact::SpdEcalSContFact():FairContFact()
{
  fName  = "SpdEcalSContFact";
  fTitle = "Factory for parameter containers in SpdEcals";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdEcalSContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdEcalS library.
  */
  FairContainer* p;
  
  p = new FairContainer("EcalSBParSet",
                        "SpdEcal TB parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("SpdEcalSBParSet",
                        "SpdEcal TB parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("EcalSECParSet",
                        "SpdEcal TEC parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);

  p = new FairContainer("SpdEcalSECParSet",
                        "SpdEcal TEC parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdEcalSContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"EcalSBParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"EcalSECParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"SpdEcalSBParSet") == 0) {
      p = new SpdEcalSBParSet(c->getConcatName().Data(),
                              c->GetTitle(),
                              c->getContext());
  }
  else if (strcmp(name,"SpdEcalSECParSet") == 0) {
      p = new SpdEcalSECParSet(c->getConcatName().Data(),
                               c->GetTitle(),
                               c->getContext());
  }
  
  return p;
}
