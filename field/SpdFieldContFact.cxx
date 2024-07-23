// $Id$
// Author: artur   2017/11/29

#include "FairRuntimeDb.h"
#include "SpdFieldContFact.h"

#include "SpdFieldPar.h"

ClassImp(SpdFieldContFact)

static SpdFieldContFact gSpdFieldContFact;

//_____________________________________________________________________________
SpdFieldContFact::SpdFieldContFact() 
{
  fName  = "SpdFieldContFact";
  fTitle = "Factory for parameter containers in SpdField";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdFieldContFact::setAllContainers()
{
  FairContainer* p = new FairContainer("SpdFieldPar",
                                       "Spd Field Parameters",
                                       "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdFieldContFact::createContainer(FairContainer* c)
{
  const char* name = c->GetName();
  
  FairParSet* p = NULL;
  
  if (strcmp(name,"SpdFieldPar") == 0) {
      p = new SpdFieldPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  
  return p;
}

