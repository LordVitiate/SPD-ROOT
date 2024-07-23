// $Id$
// Author: artur   2021/10/06


//_____________________________________________________________________________
//
// SpdPythia8Decayer
//_____________________________________________________________________________

#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TParticle.h>
#include <TRandom3.h>
#include <TMath.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>

#include "SpdDecayer.h"
#include "SpdPrimGenParSet.h"
#include "SpdPythia8Decayer.h"

#include <Pythia8/ParticleData.h>

using Pythia8::ParticleDataEntry;
using Pythia8::DecayChannel;

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdPythia8Decayer)

// 2147483647 = 0x7FFFFFFF
//#define PYTHIA8_RANMAX  0x7FFFFFFF

// 0x80000000 = 0x7FFFFFFF+1 (= 2147483647+1)
//#define PYTHIA8_RANMAX  0x80000000

#define PYTHIA8_RANMAX  900000000

//_____________________________________________________________________________
SpdPythia8Decayer::SpdPythia8Decayer():
fParticlePdg(-1),fParticles(0),fSeed(0),fInit(false),fDecayer(0),fPythia(0)
{
   SetSeed();
}

//_____________________________________________________________________________
SpdPythia8Decayer::SpdPythia8Decayer(SpdDecayer* decayer):
fParticlePdg(-1),fParticles(0),fSeed(0),fInit(false),fDecayer(decayer),fPythia(0)
{
   SetSeed();
}

//_____________________________________________________________________________
SpdPythia8Decayer::~SpdPythia8Decayer() 
{
}

//_____________________________________________________________________________
void SpdPythia8Decayer::Init()
{
   if (fInit) return;
        
   if (!fPythia) fPythia = new SpdPythia8("../share/Pythia8/xmldoc",false);
   
   if ((fSeed < 1) || (fSeed >= PYTHIA8_RANMAX)) 
   {
       TRandom3* gen = new TRandom3();
       gen->SetSeed(0);   
       fSeed = gen->Integer(PYTHIA8_RANMAX);
       delete gen;
   }
   
   fPythia->SetSeed(fSeed,fUseExtGen);
   
   fPythia->SetParameter("ParticleDecays:limitRadius = on");
   fPythia->SetParameter("ParticleDecays:rMax = 1e-9"); // mm
   fPythia->SetParameter("SoftQCD:elastic = on");
        
   fInit = fPythia->Init();
   
   if (fInit) cout << "-I- <SpdPythia8Decayer::Init> Intialization successfull " << endl;
   else cout << "-E- <SpdPythia8Decayer::Init> Intialization not successfull " << endl;
}

//_____________________________________________________________________________
void SpdPythia8Decayer::SetSeed(UInt_t seed, Bool_t UseExternalGenerator) 
{
    if (seed > 0 && seed < PYTHIA8_RANMAX) fSeed = seed;
    else {   
        TRandom3* gen = new TRandom3();
        gen->SetSeed(0);   
        fSeed = gen->Integer(PYTHIA8_RANMAX);
        delete gen;
    }
    
    fUseExtGen = UseExternalGenerator;
    
    fInit = false;

    cout << "-I- <SpdPythia8Decayer::SetSeed> Seed (ini/actual):  "
         << seed << "/" << fSeed << endl;
}

//_____________________________________________________________________________
Int_t SpdPythia8Decayer::ImportParticles(TClonesArray *particles)
{  
   if (!fInit) return 0;
    
   // import the decay products into particles array

   Int_t np = fPythia->ImportParticles(particles,"Final");
   
//    cout << "-I- <SpdPythia8Decayer::ImportParticles> Particle (pdg): " << fParticlePdg  
//         << " add: " << np << "; particles: " << endl;
//    
//    TParticle* p;
//    for (Int_t i(0); i<np; i++) { 
//        p = (TParticle*)particles->At(i);
//        p->Print();
//    }
   
   if (fDecayer) fDecayer->FillParticles(particles);
   
   return np;
}

//_____________________________________________________________________________
void SpdPythia8Decayer::Decay(Int_t pdg, TLorentzVector* p)
{  
   if (!fInit) return;
   
   // decay a single particle
    
//    cout << "-I- <SpdPythia8Decayer::Decay> Particle (pdg): " 
//         << TDatabasePDG::Instance()->GetParticle(pdg)->GetName()  
//         << " (" << pdg << ")" << endl;  

   // clear the event stack
   Pythia()->event.clear();
    
   if (!fInit) Init();
    
   fParticlePdg = -1;
    
   if (!p) return;
   
   fParticlePdg = pdg;

   // append a particle to the Pythia8 stack
   Pythia()->event.append(pdg, 11, 0, 0, p->Px(), p->Py(), p->Pz(), p->E(), p->M());
   
   Int_t idPart = Pythia()->event[0].id();
   Pythia()->particleData.mayDecay(idPart,kTRUE);
   Pythia()->moreDecays();
   
   if (fDecayer) fDecayer->FillDecay(pdg,p);
   
   //fPythia->EventListing();
}

//_____________________________________________________________________________
Float_t SpdPythia8Decayer::GetLifetime(Int_t pdg)
{
   if (!fPythia) return -1.;
    
   // lifetime in seconds of the particle with PDG number pdg
    
   Float_t t = Pythia()->particleData.tau0(pdg) * 3.3356410e-12; // [mm/c] -> [s]  
   
   //cout << "pdg = " << pdg << " t = " << t << endl;
   
   return t;
}

//_____________________________________________________________________________
Bool_t SpdPythia8Decayer::FillParsIn(SpdPrimGenParSet* params, Int_t index)
{
   if (!params || !fPythia) return false;
   
   fPythia->SetStorageIndex(index);
   
   params->SetParameter("UseExtGenerator",fUseExtGen,index);
   
   fPythia->FillParsIn(params);
   
   return true;
}

//_____________________________________________________________________________
Bool_t SpdPythia8Decayer::LoadParsFrom(SpdPrimGenParSet* params, Int_t index)
{
   if (!params|| !fPythia) return false;
   
   fPythia->SetStorageIndex(index);
   fPythia->LoadParsFrom(params);

   params->GetParameter("UseExtGenerator",fUseExtGen,index);
   
   return true;  
}

//_____________________________________________________________________________
void SpdPythia8Decayer::PrintParticleDecayChannels(Int_t pdg)
{
   if (!fPythia) return;
   
   TDatabasePDG* db = TDatabasePDG::Instance();
   TParticlePDG* p = db->GetParticle(pdg);

   if (!p) {
       cout << "-W- <SpdPythia8Decayer::PrintParticleDecayBranching> [TDatabasePDG] unknown particle:  " 
            << pdg << endl;
       return;
   }
   
   ParticleDataEntry* partdata = Pythia()->particleData.particleDataEntryPtr(pdg);
   
   if (!partdata) {
       cout << "-W- <SpdPythia8Decayer::PrintParticleDecayChannels> Unknown particle (pdg): " 
            << pdg << endl;
       return;
   }
   
   cout << "\n-I- <SpdPythia8Decayer::PrintParticleDecayBranching> Particle (pdg): " 
        <<  p->GetName() << " (" << pdg << ")  Life time [ns]: " << GetLifetime(pdg)*1e9
        << "; may decay: " << partdata->mayDecay() << "\n\n"; 
      
   if (partdata->sizeChannels() < 1) return;
        
   Double_t brsum(0);
   Int_t ppdg;
   
   //  Loop over decay channels
   
   for (Int_t ch = 0; ch < Int_t(partdata->sizeChannels()); ch++) 
   {
        const DecayChannel& channel = partdata->channel(ch);
       
        cout << std::setprecision(7) 
             << "channel: "         << std::setw(5) << ch 
             << ";  mode(in,me): "  << std::setw(3) << channel.onMode() << "," << std::setw(3) << channel.meMode() 
             << ";  branch ratio: " << std::fixed << std::setw(12) << channel.bRatio()
             << ";  particles (" << channel.multiplicity() << "): ";
            
        brsum += channel.bRatio();
         
        for (Int_t k = 0; k < channel.multiplicity(); ++k) {   
             ppdg = channel.product(k);
             if (ppdg == 0) continue;
             p = db->GetParticle(ppdg);
             if (p) cout << p->GetName() << " (" << ppdg << ") " << " ";
             else cout << "unknown" << " (" << ppdg << ") " << "; ";
        }
        cout << endl;
   }
   
   cout << "\nBranching (sum): " << brsum  << endl;
}

//_____________________________________________________________________________
void SpdPythia8Decayer::SelectForcedDecay(Int_t pdg, Int_t channel)
{
   if (!fPythia) return;
   if (channel < 0) return;
       
   ParticleDataEntry* partdata = Pythia()->particleData.particleDataEntryPtr(pdg);
   if (!partdata) return;

   if (!(channel < partdata->sizeChannels())) return;
   
   std::map< Int_t, std::set<Int_t> >::iterator it = fSelectedDecays.find(pdg);
   if (it == fSelectedDecays.end()) {
       std::set<Int_t> channels;
       channels.insert(channel);
       fSelectedDecays[pdg] = channels;
   }
   else it->second.insert(channel);   
}

//_____________________________________________________________________________
void SpdPythia8Decayer::ForceSelectedDecays()
{
   if (!fPythia) return;
    
   if (fSelectedDecays.empty()) {
       cout << "-W- <SpdPythia8Decayer::ForceSelectedDecays> Decays list is empty, nothing has been done. " << endl;
       return;
   }
   
   Int_t pdg;
    
   for (auto it : fSelectedDecays)     
   {
       pdg = it.first;
       
       if (!TDatabasePDG::Instance()->GetParticle(pdg)) {
           cout << "-W- <SpdPythia8Decayer::ForceSelectedDecays> [TDatabasePDG] unknown particle:  " 
                << pdg << endl;
           continue;
       }
       
       cout << "-I- <SpdPythia8Decayer::ForceSelectedDecays> particle: " << pdg << " " 
            << "( " << TDatabasePDG::Instance()->GetParticle(pdg)->GetName() << " )" << endl;
       
       fPythia->SetParameter(Form("%d:onMode = off",pdg));
       for (auto its : it.second) fPythia->SetParameter(Form("%d:%d:onMode = on",pdg,its));
   }
}

//_____________________________________________________________________________
void SpdPythia8Decayer::SetMayDecay(Int_t pdg, Bool_t mayDecay)
{
    if (fPythia) Pythia()->particleData.mayDecay(pdg,mayDecay);
}


