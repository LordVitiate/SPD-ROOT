
#include "FairRuntimeDb.h"
#include "SpdRsSContFact.h"

#include "SpdRsSBParSet.h"
#include "SpdRsSECParSet.h"

#include <iostream>

ClassImp(SpdRsSContFact)

static SpdRsSContFact gSpdRsSContFact;

//_____________________________________________________________________________________
SpdRsSContFact::SpdRsSContFact():FairContFact()
{
  fName  = "SpdRsSContFact";
  fTitle = "Factory for parameter containers in SpdRsS";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdRsSContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdRsS library.
  */
  FairContainer* p;
  
  p = new FairContainer("RsSBParSet",
                        "SpdRs SB parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");
  
  containers->Add(p);
  
  p = new FairContainer("SpdRsSBParSet",
                        "SpdRs SB parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("RsSECParSet",
                        "SpdRs SEC parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);

  p = new FairContainer("SpdRsSECParSet",
                        "SpdRs SEC parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdRsSContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"RsSBParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"RsSECParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"SpdRsSBParSet") == 0) {
      p = new SpdRsSBParSet(c->getConcatName().Data(),
                            c->GetTitle(),
                            c->getContext());
  }
  else if (strcmp(name,"SpdRsSECParSet") == 0) {
      p = new SpdRsSECParSet(c->getConcatName().Data(),
                             c->GetTitle(),
                             c->getContext());
  }
  
  return p;
}
