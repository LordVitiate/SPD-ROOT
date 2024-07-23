/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

#ifndef __SPDSTSCONTFACT_H__
#define __SPDSTSCONTFACT_H__

#include "FairContFact.h"

/* Factory for all SpdSts parameter containers */

class FairContainer;

class SpdStsContFact : public FairContFact
{
  public:
  
     SpdStsContFact();
    ~SpdStsContFact() {}
  
     FairParSet* createContainer(FairContainer*);
    
  private:
    
    void setAllContainers();
    
    ClassDef( SpdStsContFact,0) 
};

#endif
