
#include "FairRuntimeDb.h"
#include "SpdZdcContFact.h"
#include "SpdZdcParSet.h"

#include <iostream>

ClassImp(SpdZdcContFact)

static SpdZdcContFact gSpdZdcContFact;

//_____________________________________________________________________________________
SpdZdcContFact::SpdZdcContFact():FairContFact()
{
  fName  = "SpdZdcContFact";
  fTitle = "Factory for parameter containers in SpdZdc";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdZdcContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdZdc library.
  */
  FairContainer* p;
  
  p = new FairContainer("ZdcParSet",
                        "SpdZdc parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("SpdZdcParSet",
                        "SpdZdc parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdZdcContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"ZdcParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"SpdZdcParSet") == 0) {
      p = new SpdZdcParSet(c->getConcatName().Data(),
                            c->GetTitle(),
                            c->getContext());
  }
  
  return p;
}


