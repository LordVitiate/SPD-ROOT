/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#include "SpdStsContFact.h"

#include "SpdStsGeoPar.h"

#include "FairRuntimeDb.h"

#include <iostream>

ClassImp(SpdStsContFact)

static SpdStsContFact gSpdStsContFact;

//_____________________________________________________________________________________
SpdStsContFact::SpdStsContFact() : FairContFact()
{
  /** Constructor (called when the library is loaded) */
  fName  = "SpdStsContFact";
  fTitle = "Factory for parameter containers in libSpdSts";
  
  setAllContainers();
  
  FairRuntimeDb::instance()->addContFactory(this);
}

//_____________________________________________________________________________________
void SpdStsContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdSts library.
  */

  FairContainer* p = new FairContainer("SpdStsGeoPar",
                                       "SpdSts Geometry Parameters",
                                       "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

//_____________________________________________________________________________________
FairParSet* SpdStsContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  const char* name = c->GetName();
  FairParSet* p = NULL;
  if (strcmp(name,"SpdStsGeoPar") == 0) {
      p = new SpdStsGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
