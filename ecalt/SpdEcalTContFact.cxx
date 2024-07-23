
#include "FairRuntimeDb.h"

#include "SpdEcalTContFact.h"
#include "SpdEcalTBParSet.h"
#include "SpdEcalTECParSet.h"

#include <iostream>

ClassImp(SpdEcalTContFact)

static SpdEcalTContFact gSpdEcalTContFact;

//_____________________________________________________________________________________
SpdEcalTContFact::SpdEcalTContFact():FairContFact()
{
  fName  = "SpdEcalTContFact";
  fTitle = "Factory for parameter containers in SpdEcalt";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdEcalTContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdEcalT library.
  */
  FairContainer* p;
  
  p = new FairContainer("EcalTBParSet",
                        "SpdEcal TB parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("SpdEcalTBParSet",
                        "SpdEcal TB parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("EcalTECParSet",
                        "SpdEcal TEC parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);

  p = new FairContainer("SpdEcalTECParSet",
                        "SpdEcal TEC parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdEcalTContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"EcalTBParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"EcalTECParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"SpdEcalTBParSet") == 0) {
      p = new SpdEcalTBParSet(c->getConcatName().Data(),
                              c->GetTitle(),
                              c->getContext());
  }
  else if (strcmp(name,"SpdEcalTECParSet") == 0) {
      p = new SpdEcalTECParSet(c->getConcatName().Data(),
                               c->GetTitle(),
                               c->getContext());
  }
  
  return p;
}
