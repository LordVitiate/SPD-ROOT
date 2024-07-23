// $Id$
// Author: artur   2021/01/20

#ifndef __SPDPARTICLESANDPROCESSES_H__
#define __SPDPARTICLESANDPROCESSES_H__

#include <TObject.h>
#include <set>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SpdParticlesAndProcesses                                                   //
//                                                                            //
// <brief class description>                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

enum SpdPhysProbeId
{
   kSpdUnknown,
   kSpdPromtPhoton,
   kSpdCharmonium,
   kSpdCharmedMeson,
   kSpdCharmedBaryon
};

struct SpdParticlesAndProcesses 
{
    // check for charmonium, open charm or charmed baryon
    static Int_t  VerifyPhysProbe(Int_t pdg);
    
    static Bool_t VerifyCharmonium(Int_t pdg);
    static Bool_t VerifyCharmedMeson(Int_t pdg);
    static Bool_t VerifyCharmedBaryon(Int_t pdg);
    
    static Bool_t IsQuarkOrGluon(Int_t pdg);
 
    static void AddParticlesToDatabasePDG();
       
private:
    
    SpdParticlesAndProcesses() {}
   ~SpdParticlesAndProcesses() {}
    
    static Bool_t IsParticlesAddedToDB;
 
    static Bool_t IsCharmoniumInit;
    static Bool_t IsCharmedMesonInit;
    static Bool_t IsCharmedBaryonInit;
 
    static std::set<Int_t> Charmonium_;
    static std::set<Int_t> CharmedMeson_;
    static std::set<Int_t> CharmedBaryons_;
 
    ClassDef(SpdParticlesAndProcesses,1)
};

#endif  /* __SPDPARTICLESANDPROCESSES_H__ */

