/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#include "SpdHmrsContFact.h"

#include "SpdHmrsGeoPar.h"

#include "FairRuntimeDb.h"

#include <iostream>

ClassImp(SpdHmrsContFact)

static SpdHmrsContFact gSpdHmrsContFact;

SpdHmrsContFact::SpdHmrsContFact()
  : FairContFact()
{
  /** Constructor (called when the library is loaded) */
  fName="SpdHmrsContFact";
  fTitle="Factory for parameter containers in libSpdHmrs";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void SpdHmrsContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the SpdHmrs library.
  */

  FairContainer* p= new FairContainer("SpdHmrsGeoPar",
                                      "SpdHmrs Geometry Parameters",
                                      "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

FairParSet* SpdHmrsContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  const char* name=c->GetName();
  FairParSet* p=NULL;
  if (strcmp(name,"SpdHmrsGeoPar")==0) {
    p=new SpdHmrsGeoPar(c->getConcatName().Data(),
                            c->GetTitle(),c->getContext());
  }
  return p;
}
