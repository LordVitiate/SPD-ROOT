
#include "FairRuntimeDb.h"

#include "SpdRsTContFact.h"
#include "SpdRsTBParSet.h"
#include "SpdRsTECParSet.h"

#include <iostream>

ClassImp(SpdRsTContFact)

static SpdRsTContFact gSpdRsTContFact;

//_____________________________________________________________________________________
SpdRsTContFact::SpdRsTContFact():FairContFact()
{
  fName  = "SpdRsTContFact";
  fTitle = "Factory for parameter containers in SpdRsT";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdRsTContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdRsT library.
  */
  FairContainer* p;
  
  p = new FairContainer("RsTBParSet",
                        "SpdRs TB parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");
  
  containers->Add(p);
  
  p = new FairContainer("SpdRsTBParSet",
                        "SpdRs TB parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("RsTECParSet",
                        "SpdRs TEC parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);

  p = new FairContainer("SpdRsTECParSet",
                        "SpdRs TEC parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdRsTContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"RsTBParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"RsTECParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"SpdRsTBParSet") == 0) {
      p = new SpdRsTBParSet(c->getConcatName().Data(),
                            c->GetTitle(),
                            c->getContext());
  }
  else if (strcmp(name,"SpdRsTECParSet") == 0) {
      p = new SpdRsTECParSet(c->getConcatName().Data(),
                             c->GetTitle(),
                             c->getContext());
  }
  
  return p;
}
