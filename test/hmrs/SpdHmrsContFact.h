/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#ifndef NEWDETECTORCONTFACT_H
#define NEWDETECTORCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class SpdHmrsContFact : public FairContFact
{
  private:
    void setAllContainers();
  public:
    SpdHmrsContFact();
    ~SpdHmrsContFact() {}
    FairParSet* createContainer(FairContainer*);
    ClassDef( SpdHmrsContFact,0) // Factory for all SpdHmrs parameter containers
};

#endif
