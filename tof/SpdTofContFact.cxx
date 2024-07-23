
#include "FairRuntimeDb.h"
#include "SpdTofContFact.h"

#include "SpdTofBParSet.h"
#include "SpdTofECParSet.h"

#include <iostream>

ClassImp(SpdTofContFact)

static SpdTofContFact gSpdTofContFact;

//_____________________________________________________________________________________
SpdTofContFact::SpdTofContFact():FairContFact()
{
  fName  = "SpdTofContFact";
  fTitle = "Factory for parameter containers in SpdTof";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdTofContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdTof library.
  */
  FairContainer* p;
  
  p = new FairContainer("TofBParSet",
                        "SpdTof Barrel parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
  
  p = new FairContainer("SpdTofBParSet",
                        "SpdTof Barrel parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);

  p = new FairContainer("TofECParSet",
                        "SpdTof EC parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");
  
  containers->Add(p);
  
  p = new FairContainer("SpdTofECParSet",
                        "SpdTof EC parameters",
                        "DefaultContext");
  p->addContext("NonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdTofContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  
  const char* name = c->GetName();
  FairParSet* p = 0;
  
  if (strcmp(name,"TofBParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"TofECParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }  
  else if (strcmp(name,"SpdTofBParSet") == 0) {
      p = new SpdTofBParSet(c->getConcatName().Data(),
                            c->GetTitle(),
                            c->getContext());
  }
  else if (strcmp(name,"SpdTofECParSet") == 0) {
      p = new SpdTofECParSet(c->getConcatName().Data(),
                             c->GetTitle(),
                             c->getContext());
  }
  
  return p;
}
