
#include "FairRuntimeDb.h"
#include "SpdAegContFact.h"
#include "SpdAegParSet.h"

#include <iostream>

ClassImp(SpdAegContFact)

static SpdAegContFact gSpdAegContFact;

//_____________________________________________________________________________________
SpdAegContFact::SpdAegContFact():FairContFact()
{
  fName  = "SpdAegContFact";
  fTitle = "Factory for parameter containers in SpdAeg";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdAegContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdAeg library.
  */
  FairContainer* p;
  
  p = new FairContainer("AegParSet",
                        "SpdAeg parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("SpdAegParSet",
                        "SpdAeg parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdAegContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"AegParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"SpdAegParSet") == 0) {
      p = new SpdAegParSet(c->getConcatName().Data(),
                            c->GetTitle(),
                            c->getContext());
  }
  
  return p;
}


