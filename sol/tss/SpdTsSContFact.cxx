
#include "FairRuntimeDb.h"
#include "SpdTsSContFact.h"

#include "SpdTsSBParSet.h"
#include "SpdTsSECParSet.h"

#include <iostream>

ClassImp(SpdTsSContFact)

static SpdTsSContFact gSpdTsSContFact;

//_____________________________________________________________________________________
SpdTsSContFact::SpdTsSContFact():FairContFact()
{
  fName  = "SpdTsSContFact";
  fTitle = "Factory for parameter containers in SpdTsS";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdTsSContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdTsS library.
  */
  FairContainer* p;
  
  p = new FairContainer("TsSBParSet",
                        "SpdTs TB parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("SpdTsSBParSet",
                        "SpdTs TB parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);

  p = new FairContainer("TsSECParSet",
                        "SpdTs TEC parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");
  
  containers->Add(p);
  
  p = new FairContainer("SpdTsSECParSet",
                        "SpdTs TEC parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdTsSContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"TsSBParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                               c->GetTitle(),
                               c->getContext());
  }
  else if (strcmp(name,"TsSECParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                               c->GetTitle(),
                               c->getContext());
  }  
  else if (strcmp(name,"SpdTsSBParSet") == 0) {
      p = new SpdTsSBParSet(c->getConcatName().Data(),
                            c->GetTitle(),
                            c->getContext());
  }
  else if (strcmp(name,"SpdTsSECParSet") == 0) {
      p = new SpdTsSECParSet(c->getConcatName().Data(),
                             c->GetTitle(),
                             c->getContext());
  }
  
  return p;
}

