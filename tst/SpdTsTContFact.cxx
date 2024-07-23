
#include "FairRuntimeDb.h"
#include "SpdTsTContFact.h"

#include "SpdTsTBParSet.h"
#include "SpdTsTECParSet.h"

#include <iostream>

ClassImp(SpdTsTContFact)

static SpdTsTContFact gSpdTsTContFact;

//_____________________________________________________________________________________
SpdTsTContFact::SpdTsTContFact():FairContFact()
{
  fName  = "SpdTsTContFact";
  fTitle = "Factory for parameter containers in SpdTsT";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdTsTContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdTsT library.
  */
  FairContainer* p;
  
  p = new FairContainer("TsTBParSet",
                        "SpdTs TB parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("SpdTsTBParSet",
                        "SpdTs TB parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);

  p = new FairContainer("TsTECParSet",
                        "SpdTs TEC parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");
  
  containers->Add(p);
  
  p = new FairContainer("SpdTsTECParSet",
                        "SpdTs TEC parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdTsTContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"TsTBParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"TsTECParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }  
  else if (strcmp(name,"SpdTsTBParSet") == 0) {
      p = new SpdTsTBParSet(c->getConcatName().Data(),
                            c->GetTitle(),
                            c->getContext());
  }
  else if (strcmp(name,"SpdTsTECParSet") == 0) {
      p = new SpdTsTECParSet(c->getConcatName().Data(),
                             c->GetTitle(),
                             c->getContext());
  }
  
  return p;
}
