
#include "FairRuntimeDb.h"
#include "SpdBbcContFact.h"
#include "SpdBbcParSet.h"

#include <iostream>

ClassImp(SpdBbcContFact)

static SpdBbcContFact gSpdBbcContFact;

//_____________________________________________________________________________________
SpdBbcContFact::SpdBbcContFact():FairContFact()
{
  fName  = "SpdBbcContFact";
  fTitle = "Factory for parameter containers in SpdBbc";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdBbcContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdBbc library.
  */
  FairContainer* p;
  
  p = new FairContainer("BbcParSet",
                        "SpdBbc parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("SpdBbcParSet",
                        "SpdBbc parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdBbcContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"BbcParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"SpdBbcParSet") == 0) {
      p = new SpdBbcParSet(c->getConcatName().Data(),
                            c->GetTitle(),
                            c->getContext());
  }
  
  return p;
}


