// $Id$
// Author: artur   2021/11/12

#include "FairRuntimeDb.h"

#include "SpdMvdContFact.h"
#include "SpdMvdParSet.h"

#include <iostream>

ClassImp(SpdMvdContFact)

static SpdMvdContFact gSpdMvdContFact;

//_____________________________________________________________________________________
SpdMvdContFact::SpdMvdContFact():FairContFact()
{
  fName  = "SpdMvdContFact";
  fTitle = "Factory for parameter containers in SpdMvd";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdMvdContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdMvd library.
  */
  FairContainer* p;
  
  p = new FairContainer("MvdParSet",
                        "Mvd parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");
  
  containers->Add(p);
  
  p = new FairContainer("SpdMvdParSet",
                        "Mvd parameters",
                        "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);

}

//_____________________________________________________________________________________
FairParSet* SpdMvdContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"MvdParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"SpdMvdParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  
  return p;
}
