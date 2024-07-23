// $Id$
// Author: artur   2018/11/29

#ifndef __SPDRUNSIM_H__
#define __SPDRUNSIM_H__

#include "FairRunSim.h"
#include <map>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdRunSim                                                                  //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class SpdRunSim: public FairRunSim {

public:

    SpdRunSim();
    virtual ~SpdRunSim();
    
    static SpdRunSim* Instance();
    
    virtual void  Init();
    virtual void  Run(Int_t NEvents = 0, Int_t NotUsed = 0);
    
    virtual void  ForceParticleLifetime(Int_t pdg /*pdg number*/, Double_t t /*life time [ns]*/ );
     
protected:
  
    virtual void  PrevIniActions();
    virtual void  PostIniActions();
  
    std::map<Int_t,Double_t> fForcedPartsLifetime;
    
    ClassDef(SpdRunSim,1)
};

#endif  /* __SPDRUNSIM_H__ */

