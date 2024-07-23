// $Id$
// Author: artur   2017/11/29

#include "FairRuntimeDb.h"
#include "SpdParSetContFact.h"

#include "SpdBaseParSet.h"
#include "SpdParSet.h"
#include "SpdPrimGenParSet.h"
#include "SpdPassiveGeoParSet.h"

ClassImp(SpdParSetContFact)

static SpdParSetContFact gSpdParSetContFact;

//_____________________________________________________________________________
SpdParSetContFact::SpdParSetContFact() 
{
  fName  = "SpdParSetContFact";
  fTitle = "Factory for parameter containers in SpdData";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdParSetContFact::setAllContainers()
{
  FairContainer* p;
  
  p = new FairContainer("BaseParSet",
                        "Spd Parameters",
                        "TestDefaultContext");
  
  p->addContext("TestNonDefaultContext");
  
  containers->Add(p);
  
  p = new FairContainer("ParSet",
                        "Spd Parameters",
                        "TestDefaultContext");
  
  p->addContext("TestNonDefaultContext");
  
  containers->Add(p);
  
  p = new FairContainer("PassiveGeoParSet",
                        "Spd Passive Geo Parameters",
                        "TestDefaultContext");
  
  p->addContext("TestNonDefaultContext");
  
  containers->Add(p);
  
  p = new FairContainer("PrimGenParSet",
                        "Spd Prim Gen Parameters",
                        "TestDefaultContext");
  
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdParSetContFact::createContainer(FairContainer* c)
{
  const char* name = c->GetName();
  
  FairParSet* p = NULL;
  
  if (strcmp(name,"BaseParSet") == 0) {
      p = new SpdBaseParSet(c->getConcatName().Data(),
                            c->GetTitle(),
                            c->getContext());
  }
  if (strcmp(name,"ParSet") == 0) {
      p = new SpdParSet(c->getConcatName().Data(),
                        c->GetTitle(),
                        c->getContext());
  }
  else if (strcmp(name,"PassiveGeoParSet") == 0) {
      p = new SpdPassiveGeoParSet(c->getConcatName().Data(),
                                  c->GetTitle(),
                                  c->getContext());
  }
  else if (strcmp(name,"PrimGenParSet") == 0) {
      p = new SpdPrimGenParSet(c->getConcatName().Data(),
                               c->GetTitle(),
                               c->getContext());
  }
  
  return p;
}

