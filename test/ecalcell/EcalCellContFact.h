
#ifndef __ECALCELLCONTFACT_H__
#define __ECALCELLCONTFACT_H__

#include "FairContFact.h"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// EcalCellContFact                                                           //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class FairContainer;

class EcalCellContFact : public FairContFact {
  
public:

    EcalCellContFact();
   ~EcalCellContFact() {}
   
    FairParSet* createContainer(FairContainer*);
  
private:

    void setAllContainers();
    
    ClassDef(EcalCellContFact,0) 
};

#endif /* __ECALCELLCONTFACT_H__ */
