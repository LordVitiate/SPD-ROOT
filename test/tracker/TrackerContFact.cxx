
#include "TrackerContFact.h"
#include "FairRuntimeDb.h"
#include "SpdParSet.h"

#include <iostream>

ClassImp(TrackerContFact)

static TrackerContFact gTrackerContFact;

//_____________________________________________________________________________
TrackerContFact::TrackerContFact():FairContFact()
{
  /* Constructor (called when the library is loaded) */
  fName = "TrackerContFact";
  fTitle = "Factory for parameter containers in libTracker";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________
FairParSet* TrackerContFact::createContainer(FairContainer* c)
{
  /*
      Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = NULL;
  
  if (strcmp(name,"TrackerParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  
  return p;
}

//_____________________________________________________________________________
void TrackerContFact::setAllContainers()
{
  /* 
      Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the Tracker library.
  */

  FairContainer* p = new FairContainer("TrackerParSet",
                                       "Tracker Geometry Parameters",
                                       "TestDefaultContext");
  p->addContext("TestNonDefaultContext");
  containers->Add(p);
}
