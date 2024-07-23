
#include "FairRuntimeDb.h"

#include "SpdItsContFact.h"
#include "SpdItsParSet.h"

#include <iostream>

ClassImp(SpdItsContFact)

static SpdItsContFact gSpdItsContFact;

//_____________________________________________________________________________________
SpdItsContFact::SpdItsContFact():FairContFact()
{
  fName  = "SpdItsContFact";
  fTitle = "Factory for parameter containers in SpdIts";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdItsContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdIts library.
  */
  FairContainer* p;
  
  p = new FairContainer("ItsParSet",
                        "Its parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");
  
  containers->Add(p);
  
  p = new FairContainer("SpdItsParSet",
                        "Its parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);

}

//_____________________________________________________________________________________
FairParSet* SpdItsContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"ItsParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"SpdItsParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  
  return p;
}
