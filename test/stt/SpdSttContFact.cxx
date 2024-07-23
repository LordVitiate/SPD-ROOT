/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#include "SpdSttContFact.h"
#include "SpdSttGeoPar.h"
#include "FairRuntimeDb.h"

#include <iostream>

ClassImp(SpdSttContFact)

static SpdSttContFact gSpdSttContFact;

//_____________________________________________________________________________________
SpdSttContFact::SpdSttContFact() : FairContFact()
{
  /** Constructor (called when the library is loaded) */
  fName  = "SpdSttContFact";
  fTitle = "Factory for parameter containers in libSpdStt";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdSttContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdStt library.
  */
  FairContainer* p = new FairContainer("SpdSttGeoPar",
                                       "SpdStt Geometry Parameters",
                                       "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdSttContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  const char* name = c->GetName();
  FairParSet* p = NULL;
  if (strcmp(name,"SpdSttGeoPar") == 0) {
      p = new SpdSttGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}


