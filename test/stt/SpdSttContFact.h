/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

/* Factory for all SpdStt parameter containers */

#ifndef __SPDSTTCONTFACT_H__
#define __SPDSTTCONTFACT_H__

#include "FairContFact.h"

//class FairContainer;

class SpdSttContFact : public FairContFact {
  
public:
  
    SpdSttContFact();
   ~SpdSttContFact() {}
  
    virtual FairParSet* createContainer(FairContainer*);
    
private:
    
    void setAllContainers();
    
    ClassDef( SpdSttContFact,0) 
};

#endif
