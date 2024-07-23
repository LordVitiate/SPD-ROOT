

#include "EcalCellContFact.h"
#include "FairRuntimeDb.h"
#include "SpdParSet.h"

#include <iostream>

ClassImp(EcalCellContFact)

static EcalCellContFact gEcalCellContFact;

//_____________________________________________________________________________
EcalCellContFact::EcalCellContFact():FairContFact()
{
  /* Constructor (called when the library is loaded) */
  fName = "EcalCellContFact";
  fTitle = "Factory for parameter containers in libEcalCell";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________
FairParSet* EcalCellContFact::createContainer(FairContainer* c)
{
  /*
      Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = NULL;
  
  if (strcmp(name,"EcalCellParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  
  return p;
}

//_____________________________________________________________________________
void EcalCellContFact::setAllContainers()
{
  /* 
      Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the EcalCell library.
  */

  FairContainer* p = new FairContainer("EcalCellParSet",
                                       "EcalCell Geometry Parameters",
                                       "TestDefaultContext");
  p->addContext("TestNonDefaultContext");
  containers->Add(p);
}
