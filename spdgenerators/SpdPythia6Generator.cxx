// $Id$
// Author: artur   2015/03/12

#include "SpdPythia6Generator.h"

#include "SpdPrimaryGenerator.h"
#include "SpdPrimGenParSet.h"
#include "SpdMCEventHeader.h"

#include "FairRunSim.h"

#include <SpdPythia6.h>
#include <TMath.h>
#include <TString.h>
#include <TMCParticle.h> 
#include <TClonesArray.h> 
#include <TDatabasePDG.h>
#include <TParticle.h>

#include <iostream>
#include <cstdio>
#include <set>
#include <vector>

using std::cout;
using std::endl;
using std::max;
using std::abs;

ClassImp(SpdPythia6Generator)

#define PYTHIA6_RANMAX  200000000

SpdPythia6Generator::Py6TestData::Py6TestData():
fIden(0),fN(0),fPsum(0),fPmin(1e20),fPmax(0) {}

SpdPythia6Generator::Py6TestData::Py6TestData(Double_t p, Int_t i):
fIden(i),fN(1),fPsum(0),fPmin(1e20),fPmax(0) { FillP(p);}
      
//_____________________________________________________________________________________
SpdPythia6Generator::SpdPythia6Generator():SpdGenerator("SPD Pythia6 Generator"),
fFrame(""),fBeam(""),fTarget(""),fEnergy(0.),
fVgenopt(2),fKeepEvent(kFALSE),
fSeed(0),fSeedCycle(0),
fInit(kFALSE),fInitSeeds(kFALSE),
fNP(0),fParticles(0),
fTestModeOpt(0),
fMaxDecayD(1e-10),
fPythia(0)
{
    fTestNAttempts = 0;  
    fTestNGenEvents = 0; 
}

//_____________________________________________________________________________________
SpdPythia6Generator::SpdPythia6Generator(const char* Name):SpdGenerator(Name),
fFrame(""),fBeam(""),fTarget(""),fEnergy(0.),
fVgenopt(2),fKeepEvent(kFALSE),
fSeed(0),fSeedCycle(0),
fInit(kFALSE),fInitSeeds(kFALSE),
fNP(0),fParticles(0),
fTestModeOpt(0),
fMaxDecayD(1e-10),
fPythia(0)
{
    fTestNAttempts = 0;  
    fTestNGenEvents = 0; 
}

//_____________________________________________________________________________________
SpdPythia6Generator::~SpdPythia6Generator() 
{
    if (fParticles) {
        fParticles->Delete();
        delete fParticles;
    }
      
    if (fPythia) delete fPythia;
     
    ClearTest();
}

//_____________________________________________________________________________
void SpdPythia6Generator::ResetEvent()
{
    fNP = 0;
    if (fParticles) fParticles->Clear();
}

//_____________________________________________________________________________
void SpdPythia6Generator::SetSeed(UInt_t seed, UInt_t ncycle) 
{
    fGen->SetSeed(0); 
    
    if (seed >= PYTHIA6_RANMAX) {
        cout << "-I- <SpdPythia6Generator::SetSeed> Large seed number value:  "
             << seed << ", initialization will be performed random" << endl;
        seed = 0; 
    }
    
    if (seed == 0) {   
        fSeed = fGen->Integer(PYTHIA6_RANMAX);
        fSeedCycle = 0;
    }
    else {
        fSeed = seed;
        (ncycle == 0) ? fSeedCycle = 0 : fSeedCycle = ncycle-1;
    }
    
    fInitSeeds = kFALSE;
    
    cout << "-I- <SpdPythia6Generator::SetSeed> Seed (ini/actual):  "
         << " seed: " << seed << " cycle: " << ncycle
         << " / seed " << fSeed << " cycle: " << fSeedCycle << endl;
}

//_____________________________________________________________________________
void SpdPythia6Generator::SetVGenOpt(Int_t opt, Double_t vd)  
{ 
    if (opt < 0) fVgenopt = 1;
    else if (opt > 2) fVgenopt = 2;
    else fVgenopt = opt;
   
    // Set  Max. decay vertex distance
    if (fVgenopt == 2) {
        if (vd > 0) fMaxDecayD = vd;    
    }
    
    // ATTENTION in this case set external decayer anyway ATTENTION
    if (fVgenopt == 0) { 
        FairRunSim* run = FairRunSim::Instance();
        Bool_t act = kFALSE;
        if (run) {
            run->SetPythiaDecayer(kTRUE);
            act = kTRUE;
        }
        
        if (fVerboseLevel > -2) {
            cout << "-I- <SpdPythia6Generator::SetVGenOpt> "
                 << "Activate external decayer: " << act << endl;
        }
    }
      
    //cout << "-I- <SpdPythia6Generator::SetVGenOpt> Option: " << fVgenopt << endl;
}

//_____________________________________________________________________________
void SpdPythia6Generator::SetKeepEvent(Bool_t v) 
{ 
    if (v) {
        if (!fParticles) fParticles = new TClonesArray("TParticle");
        else fParticles->Delete();
    }
    else {
        if (fParticles) {
            fParticles->Delete();
            delete fParticles;
            fParticles = 0;
        }
    }
    
    fKeepEvent = v;  
}

//_____________________________________________________________________________
void SpdPythia6Generator::SetSeeds(const std::vector<UInt_t>& seeds)
{
    if (seeds.size() < 2) return;
    SetSeed(seeds[0],seeds[1]);
} 

//_____________________________________________________________________________
void SpdPythia6Generator::GetSeeds(std::vector<UInt_t>& seeds) 
{ 
    seeds.clear(); 
    seeds.push_back(fSeed);
    seeds.push_back(fSeedCycle);
} 

//_____________________________________________________________________________
SpdPythia6* SpdPythia6Generator::GetGenerator() 
{ 
    fInit = kFALSE;
    if (fPythia) return fPythia;
    fPythia = new SpdPythia6();
    return fPythia;  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdPythia6Generator::InitSeeds() //-> private
{   
    //cout << "-I- <SpdPythia6Generator::InitSeeds> " << endl;
 
    if (!fPythia) return;
    
    if (fSeed == 0) {
        fGen->SetSeed(0);   
        fSeed = fGen->Integer(PYTHIA6_RANMAX);
        fSeedCycle = 0;
    }

    fPythia->SetMRPY(1,fSeed);
    fPythia->SetMRPY(2,0);
     
    if (fSeedCycle > 0) {
        for (int i(0); i<fSeedCycle; i++) {
            fPythia->GenerateEvent();
            //cout << "GENERATE : " << i << " " << fSeed << " " << fSeedCycle << endl;        
        }
    }
         
    fInitSeeds = kTRUE;  
}

//_____________________________________________________________________________
Bool_t SpdPythia6Generator::Init()
{
    if (fInit) return kTRUE;
  
    fInitSeeds = kFALSE;
    fInit = kFALSE;
   
    if (!fPythia) fPythia = new SpdPythia6();
    
    //fPythia->SetMSEL(1); // set miminum bias 
  
    fPythia->Initialize(fFrame,fBeam,fTarget,fEnergy); 
    
    InitSeeds();
                
    fInit = kTRUE;
    
    return kTRUE;
}
    
//_____________________________________________________________________________
void SpdPythia6Generator::Initialize(const char *frame, const char *beam, const char *target, Float_t win) 
{ 
    fFrame  = frame;   
    fBeam   = beam;      
    fTarget = target;     
    fEnergy = win;
    
    fInitSeeds = kFALSE;
    fInit = kFALSE; 
  
    if (!fPythia) fPythia = new SpdPythia6();
   
    //fPythia->SetMSEL(1); // set miminum bias 
    
    if (fVgenopt == 2 && fMaxDecayD > 0) {
        fPythia->SetPARJ(72,10*fMaxDecayD); // cm -> mm
        fPythia->SetMSTJ(22,3);     
    }
  
    fPythia->Initialize(frame,beam,target,win); 
    
    InitSeeds();
               
    fInit = kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdPythia6Generator::ReadEvent(FairPrimaryGenerator* primGen) 
{                 
    if (!fPythia) {
        cout << "-E- <SpdPythia6Generator::ReadEvent> "
             << "Main generator is not initialized " << endl;
        return kFALSE;    
    }
    
    if (!fInit) {
        cout << "-E- <SpdPythia6Generator::ReadEvent> "
             << "Parameters were not initialized properly " << endl;
        return kFALSE;
    }
    
    if (!fInitSeeds) {
        cout << "-E- <SpdPythia6Generator::ReadEvent> "
             << "Seeds are not initialized properly " << endl;
        return kFALSE;
    }
    
    Bool_t ok = GenerateDirectly(primGen); 
    
    if (!primGen) return kFALSE;
      
    return ok;
}

//_____________________________________________________________________________
Bool_t SpdPythia6Generator::GenerateStandard(FairPrimaryGenerator* primGen) 
{ 
    static const Int_t Nattempt_max = 100000;
    
    TClonesArray* ps;
    TMCParticle *part;
    
    Int_t ne, n_out, status;
    Double_t px, py, pz, vx, vy, vz, vt;
    Bool_t is_ok, addpart;
    
    Int_t pdg;
    TParticlePDG* pdgpart;
    TString ptype, partname;
    
    Int_t natt = 0;
    
    ResetEvent();
    
    if (fPythia->GetMRPY(1) != fSeed) {
        fSeed = fPythia->GetMRPY(1);
        fSeedCycle = 0;
    }
    
    while (true) {
               
       if (natt == Nattempt_max) {
           if (primGen) {
               cout << "-F- <SpdPythia6Generator::GenerateStandard> Generation failed: " 
               << " number of attempts = " << natt << endl;
               return kFALSE;
           }
           else {
               cout << "-W- <SpdPythia6Generator::GenerateStandard> Max. number of attempts "
                    << "(" << Nattempt_max << ") has been reached for the event" << endl; 
               return kTRUE;
           }                             
       }
       
       fSeedCycle++;
       
       //cout << "-I- <SpdPythia6Generator::GenerateDirectly> CURRENT SEED/CYCLE: " 
       //     << fSeed << "/" << fSeedCycle << " " << fPythia->GetMRPY(1)  << endl;
       
       if (fTestModeOpt > 0) fTestNAttempts++;
       
       natt++;
                      
       fPythia->GenerateEvent();   
              
       ps = (TClonesArray*)fPythia->GetListOfParticles();
       ne = ps->GetEntriesFast();
       
       if (ne == 0) {
           cout << "-E- <SpdPythia6Generator::GenerateStandard> no particles" << endl;
           continue; // try to generate another event
       }
       
       is_ok = true;
       
       n_out = 0;
       for (int i(0); i<ne; i++) 
       {
            part = (TMCParticle*)ps->At(i);
            
            status = part->GetKS();
            
            if (status == 1 || status == 4) {
                if (!IsAcceptableParticle(part->GetKF(),kFALSE)) { 
                    is_ok = false; 
                    break; 
                }
                n_out++;
            }
       }
       
       /* <<<<<<<<<< <<<<<<<<<< <<<<<<<<<< */    

       if (!is_ok) continue; // try to generate another event
       
       if (n_out == 0) {
           cout << "-W- <SpdPythia6Generator::GenerateStandard> no particles were accepted" << endl;
           continue; // try to generate another event
       }
       
       if (fVerboseLevel > -2) {
           cout << "-I- <SpdPythia6Generator::GenerateStandard> Generate event: ok "
                << " (attempts: " << natt << ") " << endl;
       }
           
       /* ADD PARTICLES */
       
       if (fTestModeOpt > 0) fTestNGenEvents++;
       
    
       for (int i(0); i<ne; i++) {
           
            part = (TMCParticle*)ps->At(i);
            
            Int_t xid  = i;
            Int_t xpid = part->GetParent()-1; 
            
            px = part->GetPx(); // GeV/c
            py = part->GetPy(); // GeV/c
            pz = part->GetPz(); // GeV/c
          
            vx = part->GetVx()*0.1; // mm -> cm ATTENTION
            vy = part->GetVy()*0.1; // mm -> cm ATTENTION
            vz = part->GetVz()*0.1; // mm -> cm ATTENTION
            vt = part->GetTime()*(1e-3/TMath::C()); // mm/c -> s ATTENTION
            
            pdg = part->GetKF();
            
            pdgpart = 0;
            
            ptype = "undefined";
            partname = "undefined";
            
            if (pdg > 1e10) {
                ptype = "Nuclei";
                partname = "Nuclei";
            }
            else {
               pdgpart = TDatabasePDG::Instance()->GetParticle(pdg);
               if (pdgpart) {
                   ptype = pdgpart->ParticleClass();
                   partname = pdgpart->GetName();
               }
            }
            
            /* ++++++++++++++++++++ ADD TRACKS +++++++++++++++++++ */
   
            status = part->GetKS();
            (status == 1 || status == 4) ? addpart = true :  addpart = false;
            
            if (addpart) {
                
                if (fTestModeOpt > 0) AddTestParticle(part,1);
                                           
                if (fKeepEvent && fParticles) {
                  
                    Int_t np = fParticles->GetEntriesFast();
                    TParticle* particle = new ((*fParticles)[np]) TParticle();
         
                    particle->SetPdgCode(pdg);
                    particle->SetFirstMother(xpid);
                    particle->SetStatusCode(status);
                    particle->SetProductionVertex(vx, vy, vz, vt); 
                    
                    pdgpart = TDatabasePDG::Instance()->GetParticle(pdg); 
                    if (pdgpart) {
                        Double_t mass = pdgpart->Mass();
                        Double_t te = TMath::Sqrt(px*px + py*py + pz*pz + mass*mass);   
                        particle->SetMomentum(px, py, pz, te);
                    }
                    else {
                        particle->SetMomentum(px, py, pz, -1);    
                    }
                }
                
                if (AddTrack(primGen, 1, pdg, xid, xpid, px, py, pz, vx, vy, vz, true, -9.0e9, vt)) fNP++;  
       
                if (fVerboseLevel > 0) {

                    printf("%-5d [+] %8d %12d %10.6f %10.6f %10.6f %14.8f %14.8f %14.8f %12.6e %8d   %s %s\n",
                           xid, part->GetKS()/*status*/, pdg, px, py, pz, 
                           vx, vy, vz, vt, xpid, ptype.Data(), partname.Data()); 
                }
         
            }
            else {
                
                AddTrack(primGen, 0, pdg, xid, xpid,  px, py, pz, vx, vy, vz, true, -9.0e9, vt);
                       
                if (fVerboseLevel > 0) {

                    printf("%-5d  -  %8d %12d %10.6f %10.6f %10.6f %14.8f %14.8f %14.8f %12.6e %8d   %s %s\n",
                           xid, part->GetKS()/*status*/, pdg, px, py, pz, 
                           vx, vy, vz, vt, xpid, ptype.Data(), partname.Data()); 
                }
            } 
            
            /* ++++++++++++++++++++ ADD TRACKS (FINISH) +++++++++++++++++++ */
       }
       
       if (fVerboseLevel > -1) {
           cout << "-I- <SpdPythia6Generator::GenerateStandard> "
                << " OPTION: " << fVgenopt 
                << "; SEED, CYCLE: [" << fSeed << ", " << fSeedCycle << "] " 
                << " Natt = " << natt  
                << endl;
       }
       
       if (fNP == 0) {
           RemoveGenData();
           continue;
       }
       
       return kTRUE;        
    }
    
    return kFALSE;
}

//_____________________________________________________________________________
Bool_t SpdPythia6Generator::GenerateDirectly(FairPrimaryGenerator* primGen) 
{  
    if (fVerboseLevel > 0) {
        cout << "-------------------------------------------------------------------------" 
             << "-------------------------------------------------------------------------" 
             << endl;
    }
    
    if (fVgenopt == 2) return GenerateStandard(primGen); 
    
    static const Float_t tmin = 1e-20; // mm/c
    static const Float_t kmin = 1e-9;  // mm
    
    static const Int_t Nattempt_max = 100000;
    
    TClonesArray* ps;
    TMCParticle *part;
    
    Int_t ne, pid;
    Double_t px, py, pz, vx, vy, vz, vt;
    Bool_t is_ok, addpart;
    
    Int_t pdg;
    TParticlePDG* pdgpart;
    TString ptype, partname;
    
    std::set<Int_t> pn, pnn; 
    std::set<Int_t>::const_iterator it, itx;
    
    Int_t natt = 0;
    
    ResetEvent();
    
    if (fPythia->GetMRPY(1) != fSeed) {
        fSeed = fPythia->GetMRPY(1);
        fSeedCycle = 0;
    }
       
    while (true) {
               
           if (natt == Nattempt_max) {
               if (primGen) {
                   cout << "-F- <SpdPythia6Generator::GenerateDirectly> Generation failed: " 
                   << " number of attempts = " << natt << endl;
                   return kFALSE;
               }
               else {
                   cout << "-W- <SpdPythia6Generator::GenerateDirectly> Max. number of attempts "
                        << "(" << Nattempt_max << ") has been reached for the event" << endl; 
                   return kTRUE;
               }                             
           }
           
           fSeedCycle++;
           
           //cout << "-I- <SpdPythia6Generator::GenerateDirectly> CURRENT SEED/CYCLE: " 
           //     << fSeed << "/" << fSeedCycle << " " << fPythia->GetMRPY(1)  << endl;
           
           if (fTestModeOpt > 0) fTestNAttempts++;
           
           natt++;
                          
           fPythia->GenerateEvent();   
                  
           ps = (TClonesArray*)fPythia->GetListOfParticles();
           ne = ps->GetEntriesFast();
           
           if (ne == 0) {
               cout << "-E- <SpdPythia6Generator::GenerateDirectly> no particles" << endl;
               continue; // try to generate another event
           }
           
           is_ok = true;
           
           pn.clear();
           pnn.clear();
           
/* >>>>>>>>>> >>>>>>>>>> >>>>>>>>>>*/ 

           for (int i(0); i<ne; i++) {
              
                part = (TMCParticle*)ps->At(i);
                               
                if (part->GetKS() != 1) continue;
                
                if (abs(part->GetTime()) > tmin ||
                    abs(part->GetVx()) > kmin ||
                    abs(part->GetVy()) > kmin ||
                    abs(part->GetVz()) > kmin ) 
                {
                     // not vertex particle
                     pid = part->GetParent();
                     
                     if (pid > 2) {
                       
                         while (true) {
                           
                            part = (TMCParticle*)ps->At(pid-1);
                           
                            if (abs(part->GetTime()) > tmin ||
                                  abs(part->GetVx()) > kmin ||
                                  abs(part->GetVy()) > kmin ||
                                  abs(part->GetVz()) > kmin )  
                            {
                              //cout << "-I- <SpdPythia6Generator::GenerateDirectly> "
                              //     << "Current mother (" << pid-1 << ") for primary " << i 
                              //     << " is not vertex particle," << endl;
                         
                              pid = part->GetParent();
                              
                              //cout << "\t\t\t\t\t let's try another one: " << pid-1 << endl;
                              
                              if (pid > 0) continue;       
                              else {
                                  cout << "-E- <SpdPythia6Generator::GenerateDirectly> "
                                       << "Deepest vertex mother (" << pid-1 << ") "
                                       << "is not vertex particle " << endl;
                              
                                  is_ok = false;
                                  break;
                              }
                            }
                            else {
                              
                               // add the particle or n-parent 
                              
                               if (fVgenopt == 0) {
                                   Int_t ppid;        
                                   
                                   while (true) {
                                                                   
                                      ppid = part->GetParent();
                                      if (ppid <= 2) break;
                                      //cout << "[1] ppid = " << ppid << endl;                                                                          
                                      part = (TMCParticle*)ps->At(ppid-1);                                                
                                      if (part->GetKS() != 1 
                                          && part->GetKS() != 11
                                          && part->GetKS() != 4) break;                                            
                                      if (!IsAcceptableParticle(part->GetKF(),kFALSE)) break;                              
                                      pid = ppid;                                                                       
                                   }
                               }
                              
                               //part = (TMCParticle*)ps->At(pid-1);
                               //cout << "-I- <SpdPythia6Generator::GenerateDirectly> "
                               //     << "Add vertex mother [1] (" << pid-1  
                               //    << "," << part->GetKF() << ") for particle " << i << endl;
                                    
                               pn.insert(pid-1); // add particle
                               
                               break;
                            }
                         }           
                     }
                     else {
                         cout << "-E- <SpdPythia6Generator::GenerateDirectly>"
                              << " unexpected error " << endl;
                         is_ok = false;
                         break;
                     }
                }
                else {
                    // vertex particle
               
                    pid = i+1;
                               
                    if (fVgenopt == 0) {
                        Int_t ppid;
                        while (true) {
                       
                           ppid = part->GetParent();
                           if (ppid <= 2) break;
                           part = (TMCParticle*)ps->At(ppid-1);                                             
                           if (part->GetKS() != 1 
                               && part->GetKS() != 11
                               && part->GetKS() != 4) break;  
                           if (!IsAcceptableParticle(part->GetKF(),kFALSE)) break;                       
                           pid = ppid;     
                        }
                    }
                    //part = (TMCParticle*)ps->At(pid-1);
                    //cout << "-I- <SpdPythia6Generator::GenerateDirectly> "
                    //     << "Add vertex mother [2] (" << pid-1 
                    //     << "," << part->GetKF() << ") for particle " << i << endl;
                         
                    pn.insert(pid-1);  // add  particle
               
                }
                 
                if (!is_ok) break;
           }
 
           if (!is_ok) continue; // try to generate another event

/* <<<<<<<<<< <<<<<<<<<< <<<<<<<<<< */    

           for (it = pn.begin(); it != pn.end(); it++) {
                pid = *it;
                addpart = true;
                while (pid != -1) {
                   part = (TMCParticle*)ps->At(pid);
                   pid = part->GetParent()-1;
                   itx = pn.find(pid);
                   if (itx != pn.end()) { 
                       addpart = false;
                       break;
                   }
                }
                if (addpart) pnn.insert(*it);
           }
           
           /* !ATTENTION! CHECK EVENT !ATTENTION!*/
           
           //if (!IsAcceptableEvent(ps,pnn,true)) continue;
           
           /* CHECK SELECTED PARTICLES */
           
           for (it = pnn.begin(); it != pnn.end(); it++) {
             
                part = (TMCParticle*)ps->At(*it);
                            
                if (abs(part->GetTime()) > tmin ||
                      abs(part->GetVx()) > kmin ||
                      abs(part->GetVy()) > kmin ||
                      abs(part->GetVz()) > kmin )  
                {
                    is_ok = false;
                    break;
                }
                
                is_ok = IsAcceptableParticle(part->GetKF(),kTRUE);  
                if (!is_ok) break;
           }
           
           if (!is_ok) {
               if (fVerboseLevel > -2) {
                   cout << "-W- <SpdPythia6Generator::GenerateDirectly> "
                        << "Reject event [SEED, CYCLE]: " 
                        << fSeed << "," << fSeedCycle << " particle (pdg): " << part->GetKF() << endl;
               }
               if (fTestModeOpt > 0) AddTestParticle(part,0);
               continue; // try to generate another event
           }
           
           if (fVerboseLevel > -2) {
               cout << "-I- <SpdPythia6Generator::GenerateDirectly> Generate event: ok "
                    << " (attempts: " << natt << ") " << endl;
           }
           
           /* ADD PARTICLES */
           
           if (fTestModeOpt > 0) fTestNGenEvents++;
           
           //--------------------------------------------------------
           // old->new id table => idx
           //--------------------------------------------------------
           std::map<int,int> idx; 
           idx.insert(std::pair<int,int>(-1,-1));
           int nx = 0;
           for (int i(0); i<ne; i++) {
                part = (TMCParticle*)ps->At(i);
                it = pnn.find(i);
                if (it != pnn.end()) idx[i] = nx++;
                else { 
                    if (pnn.find(part->GetParent()-1) == pnn.end() &&
                        idx.find(part->GetParent()-1) != idx.end()) idx[i] = nx++;
                }
           }
           
           for (int i(0); i<ne; i++) {
               
                part = (TMCParticle*)ps->At(i);
                
                //-----------------------------------------------------------------------
                Int_t xid(-10), xpid(-10);
                
                // >>>> old version 
                //xid  = i;
                //xpid = part->GetParent()-1; 
                
                // >>>> new version
                if (idx.find(i) != idx.end()) xid = idx[i]; 
                if (idx.find(part->GetParent()-1) != idx.end()) xpid = idx[part->GetParent()-1];
                                
                if (xid == -10) continue;
                //-----------------------------------------------------------------------
                
                px = part->GetPx(); // GeV/c
                py = part->GetPy(); // GeV/c
                pz = part->GetPz(); // GeV/c
              
                vx = part->GetVx()*0.1;     // mm -> cm ATTENTION
                vy = part->GetVy()*0.1;     // mm -> cm ATTENTION
                vz = part->GetVz()*0.1;     // mm -> cm ATTENTION
                vt = part->GetTime()*(1e-3/TMath::C());   // mm/c -> s ATTENTION
                
                pdg = part->GetKF();
                
                pdgpart = 0;
                
                ptype = "undefined";
                partname = "undefined";
                
                if (pdg > 1e10) {
                    ptype = "Nuclei";
                    partname = "Nuclei";
                }
                else {
                   pdgpart = TDatabasePDG::Instance()->GetParticle(pdg);
                   if (pdgpart) {
                       ptype = pdgpart->ParticleClass();
                       partname = pdgpart->GetName();
                   }
                }
                
                /* ++++++++++++++++++++ ADD TRACKS +++++++++++++++++++ */
   
                it = pnn.find(i);
                
                if (it != pnn.end()) {
                    
                    if (fTestModeOpt > 0) AddTestParticle(part,1);
                                               
                    if (fKeepEvent && fParticles) {
                      
                        Int_t np = fParticles->GetEntriesFast();
                        TParticle* particle = new ((*fParticles)[np]) TParticle();
             
                        particle->SetPdgCode(pdg);
                        particle->SetFirstMother(xpid);
                        particle->SetStatusCode(part->GetKS());
                        particle->SetProductionVertex(vx, vy, vz, vt); 
                        
                        pdgpart = TDatabasePDG::Instance()->GetParticle(pdg); 
                        if (pdgpart) {
                            Double_t mass = pdgpart->Mass();
                            Double_t te = TMath::Sqrt(px*px + py*py + pz*pz + mass*mass);   
                            particle->SetMomentum(px, py, pz, te);
                        }
                        else {
                            particle->SetMomentum(px, py, pz, -1);    
                        }
                    }
                    
                    if (AddTrack(primGen, 1, pdg, xid, xpid, px, py, pz)) fNP++;  
           
                    if (fVerboseLevel > 0) {

                        printf("%-5d [+] %8d %12d %10.6f %10.6f %10.6f %14.8f %14.8f %14.8f %12.6e %8d   %s %s\n",
                               xid, part->GetKS()/*status*/, pdg, px, py, pz, 
                               vx, vy, vz, vt, xpid, ptype.Data(), partname.Data()); 
                    }
             
                }
                else {
                    
                    AddTrack(primGen, 0, pdg, xid, xpid, px, py, pz);
                           
                    if (fVerboseLevel > 0) {

                        printf("%-5d  -  %8d %12d %10.6f %10.6f %10.6f %14.8f %14.8f %14.8f %12.6e %8d   %s %s\n",
                               xid, part->GetKS()/*status*/, pdg, px, py, pz, 
                               vx, vy, vz, vt, xpid, ptype.Data(), partname.Data()); 
                    }
                } 
                
                /* ++++++++++++++++++++ ADD TRACKS (FINISH) +++++++++++++++++++ */
           }
           
           if (fVerboseLevel > -1) {
               cout << "-I- <SpdPythia6Generator::GenerateDirectly> "
                    << " OPTION: " << fVgenopt 
                    << "; SEED, CYCLE: [" << fSeed << ", " << fSeedCycle << "] " 
                    << " Natt = " << natt  
                    << endl;
           }
           
           if (fNP == 0) {
               RemoveGenData();
               continue;
           }
           
           return kTRUE;
    }
    
    return kFALSE;
}

//_____________________________________________________________________________
bool SpdPythia6Generator::IsAcceptableParticle(Int_t pdg, Bool_t edump) const
{
   if (pdg == 22)  return kTRUE;      // photon, ok
   if (pdg > int(1e10)) return kTRUE; // nuclei, ok
   
   if (TMath::Abs(pdg) == 1114) return kTRUE; // Delta-, Delta-_bar
       
   TParticlePDG* pdgpart = TDatabasePDG::Instance()->GetParticle(pdg);
   
   if (!pdgpart) {
       // no particle in DB
       if (fVerboseLevel > -2 && fVgenopt > 0) { 
           cout << "-W- <SpdPythia6Generator::IsAcceptableParticle> "
                << "No such particle in DB: pdg = " << pdg << endl;
       }
       return kFALSE;  
   }
   
   TString ptype = pdgpart->ParticleClass();
   
   if (ptype.Contains("Unknown")) {
       if (fVerboseLevel > -2 && fVgenopt > 0) { 
           cout << "-W- <SpdPythia6Generator::IsAcceptableParticle> Unacceptable particle: "
                << pdg << " (" << ptype << ") " << endl; 
       }
       return kFALSE;
   }
   
//    if (ptype.Contains("Charmed")) {
//        if (fVerboseLevel > -2 && fVgenopt > 0) { 
//            cout << "-W- <SpdPythia6Generator::IsAcceptableParticle> Unacceptable particle: "
//                 << pdg << " (" << ptype << ") " << endl; 
//        }
//        return kFALSE; 
//    }
   
//    if (ptype.Contains("Charmed")) {
//        cout << "-I- <SpdPythia6Generator::IsAcceptableParticle> Charmed particle: " 
//             << pdgpart->GetName() << " " << pdg << " (" << ptype << ") " << endl; 
//        return kTRUE; 
//    }
     
   if (ptype.Contains("Meson"))  return kTRUE;
   if (ptype.Contains("Baryon")) return kTRUE;
   if (ptype.Contains("Lepton")) return kTRUE;
   
   if (edump) {
       cout << "-E- <SpdPythia6Generator::IsAcceptableParticle> "
            << "Bad particle in the list: pdg/type = " 
            << pdg << "/" << ptype << endl; 
   }
   
   return kFALSE;
}

//_____________________________________________________________________________
bool SpdPythia6Generator::IsAcceptableEvent(TClonesArray* plist, const std::set<Int_t>& psel, Bool_t edump) const
{
     std::set<Int_t>::const_iterator it;
     TMCParticle* part;
     TParticlePDG* pdgpart;
     Int_t pdg(0);
     TString ptype;
     
     Bool_t is_ok = kFALSE;
     
     for (it = psel.begin(); it != psel.end(); it++) {
          part = (TMCParticle*)plist->At(*it);
          pdg = part->GetKF();
          pdgpart = TDatabasePDG::Instance()->GetParticle(pdg); 
          if (!pdgpart) return kFALSE;
          ptype = pdgpart->ParticleClass();
          if (ptype.Contains("Charmed")) { is_ok = kTRUE; break; }
     }
     
     if (is_ok && edump) {
         cout << "-I- <SpdPythia6Generator::IsAcceptableEvent> Accept event with particle: " 
              << pdgpart->GetName() << " " << pdg << " (" << ptype << ") " << endl;
     }
     
     return is_ok;
}

//_____________________________________________________________________________
void SpdPythia6Generator::ClearTest()
{
   fTestNAttempts = 0;  
   fTestNGenEvents = 0; 
   
   if (!fTestParticles.empty()) {
       std::map<Int_t,Py6TestData*>::iterator it = fTestParticles.begin();
       for ( ; it != fTestParticles.end(); it++) delete it->second;
       fTestParticles.clear();  
   }
}

//_____________________________________________________________________________
void SpdPythia6Generator::AddTestParticle(TMCParticle* part, Int_t iden)
{
   Int_t pdg =  part->GetKF();
   
   Double_t mom = part->GetPx()*part->GetPx()+
                  part->GetPy()*part->GetPy()+
                  part->GetPz()*part->GetPz();
   mom = TMath::Sqrt(mom);
   
   if (fTestParticles.empty()) {
       fTestParticles.insert(std::pair<Int_t,Py6TestData*>(pdg,new Py6TestData(mom,iden)));
       return;
   }
   
   std::map<Int_t,Py6TestData*>::iterator it = fTestParticles.find(pdg);
   
   if (it == fTestParticles.end()) {
       fTestParticles.insert(std::pair<Int_t,Py6TestData*>(pdg,new Py6TestData(mom,iden)));
   }
   else {
       it->second->fN += 1.;
       it->second->FillP(mom);
   }    
}

//_____________________________________________________________________________
Bool_t SpdPythia6Generator::FillParsIn(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::FillParsIn(params)) return kFALSE;
   
   params->SetParameter("Frame",fFrame,fGenIndex);
   params->SetParameter("Beam",fBeam,fGenIndex);
   params->SetParameter("Target",fTarget,fGenIndex);
   params->SetParameter("Energy",fEnergy,fGenIndex);
   params->SetParameter("VgOption",fVgenopt,fGenIndex);
   params->SetParameter("KeepEvent",fKeepEvent,fGenIndex);
   
   if (fVgenopt == 2) params->SetParameter("MaxDecayD",fMaxDecayD,fGenIndex);
   
   if (fPythia) {
       fPythia->SetStorageIndex(fGenIndex);
       fPythia->FillParsIn(params);
   }
       
   return kTRUE;
}
 
//_____________________________________________________________________________
Bool_t SpdPythia6Generator::LoadParsFrom(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::LoadParsFrom(params)) return kFALSE;
   
   fInitSeeds = kFALSE;
   fInit = kFALSE;
   
   TString val;
   
   Bool_t is_ok = kTRUE;
   
   if (params->GetParameter("Frame",val,fGenIndex)) fFrame = val.Data(); else is_ok = kFALSE;
   if (params->GetParameter("Beam", val,fGenIndex)) fBeam = val.Data(); else is_ok = kFALSE;
   if (params->GetParameter("Target",val,fGenIndex)) fTarget = val.Data(); else is_ok = kFALSE; 
   
   if (!params->GetParameter("Energy",fEnergy,fGenIndex)) is_ok = kFALSE;
   
   Int_t vgenopt;
   if (params->GetParameter("VgOption",vgenopt,fGenIndex)) SetVGenOpt(vgenopt);
   
   if (fVgenopt == 2) {
       if (!params->GetParameter("MaxDecayD",fMaxDecayD,fGenIndex)) is_ok = kFALSE;
   }
    
   Bool_t keepevent;
   if (params->GetParameter("KeepEvent",keepevent,fGenIndex)) SetKeepEvent(keepevent);
  
   return is_ok;
}

//_____________________________________________________________________________
void SpdPythia6Generator::Print(Option_t* opt) const
{
   cout << "\t-I- <SpdPythia6Generator::Print>\n" << endl;
   
   cout << "\tIs init:          " << fInit << endl;
   cout << "\tIs init seeds:    " << fInitSeeds << endl;
   cout << "\tIndex:            " << fGenIndex << endl;
   cout << "\tVerbose level:    " << fVerboseLevel << endl;
   cout << "\tVgOption:         " << fVgenopt << endl;
   cout << "\tKeepEvent:        " << fKeepEvent << endl;
   cout << "\tSeeds:            " << fSeed << " " << fSeedCycle << endl;
   
   cout << endl;
    
   cout << "\tFrame:            " << fFrame << endl;   
   cout << "\tBeam:             " << fBeam << endl;      
   cout << "\tTarget:           " << fTarget << endl; 
   cout << "\tEnergy (GeV):     " << fEnergy << endl; 
   
   cout << endl;
   
   if (fPythia) {
       cout << "\tPythia:           " << "yes" << endl;
       cout << "\tPythia set:       " << fPythia-> GetParSetNumber() << endl;
   }
   else cout << "\tPythia:           " << "no" << endl;
   
   cout << endl;
   
   if (fTestModeOpt < 1) return;
   
   cout << "Number of attempts:    " << fTestNAttempts << endl;
   cout << "Number of events:      " << fTestNGenEvents;
 
   printf(" (%.4f) \n", 100.*fTestNGenEvents/fTestNAttempts);
   
   if (fTestParticles.empty()) {
       cout << endl;
       return;
   }
   
   const Py6TestData* v;

   std::multimap<Double_t,Int_t> ordered;
   std::multimap<Double_t,Int_t>::const_reverse_iterator itx;
   
   std::map<Int_t,Py6TestData*>::const_iterator it = fTestParticles.begin();
 
   Double_t ntotmult = 0, nt;
   for ( ; it != fTestParticles.end(); it++) {
        nt = it->second->fN;
        ntotmult += nt;
        ordered.insert(std::pair<Double_t,Int_t>(nt,it->first));
   }
     
   cout << "Particles/Event:       " << ntotmult/fTestNGenEvents << endl;
   cout << endl;
   
   Int_t i(0);
   Double_t vv, pave;
   const Char_t *pname, *ptype;
   Char_t id;
   TParticlePDG* pdgpart;
   
   printf("[x] %3s %10s %15s %15s %10s (%7s) %12s %12s %12s\n\n",
          "I","PDG","NAME","TYPE","N","   %   ","P(min)","P(ave)","P(max)");
   
   for (itx = ordered.rbegin() ; itx != ordered.rend(); itx++) {  
     
        it = fTestParticles.find(itx->second);
        v = it->second;
        
        vv = 100.*v->fN/ntotmult;
        pave = v->fPsum/v->fN;
        
        pdgpart = TDatabasePDG::Instance()->GetParticle(it->first);
        if (pdgpart) {
            pname = pdgpart->GetName();
            ptype = pdgpart->ParticleClass();
        }
        else {
            pname = "undefined";
            ptype = "undefined";
        }
        
        if (v->fIden == 0) id = '-';
        else id = '+';
          
        printf("[%1c] %3d %10d %15s %15s %10d (%5.2e) %12.6f %12.6f %12.6f\n",
               id, ++i, it->first, pname, ptype, Int_t(v->fN), vv, v->fPmin, pave, v->fPmax);
   }
   
   cout << endl;
}




