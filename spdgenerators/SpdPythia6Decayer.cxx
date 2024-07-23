// $Id$
// Author: artur   2018/08/13

//_____________________________________________________________________________
//
// SpdPythia6Decayer
//_____________________________________________________________________________

#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TParticle.h>
#include <TRandom3.h>
#include <TMath.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>

#include <TPythia6.h>

#include "SpdDecayer.h"
#include "SpdPrimGenParSet.h"
#include "SpdPythia6Decayer.h"

#include <iostream>
using std::cout;
using std::endl;

#define PYTHIA6_RANMAX  200000000

ClassImp(SpdPythia6Decayer)

//_____________________________________________________________________________
SpdPythia6Decayer::SpdPythia6Decayer():TPythia6Decayer(),
fParticlePdg(-1),fParticles(0),fSeed(0),fInit(kFALSE),fDecayer(0)
{
    fParticles = new TClonesArray("TParticle",1000);
}

//_____________________________________________________________________________
SpdPythia6Decayer::SpdPythia6Decayer(SpdDecayer* decayer):TPythia6Decayer(),
fParticlePdg(-1),fParticles(0),fSeed(0),fInit(kFALSE),fDecayer(decayer)
{
    fParticles = new TClonesArray("TParticle",1000);
}

//_____________________________________________________________________________
SpdPythia6Decayer::~SpdPythia6Decayer() 
{
    if (fParticles) { 
        fParticles->Delete(); 
        delete fParticles; 
        fParticles = 0; 
    }
    
    if (fgInstance == this) fgInstance = 0;
}

//_____________________________________________________________________________
void SpdPythia6Decayer::SetSeed(UInt_t seed) 
{
    cout << "-I- <SpdPythia6Decayer::SetSeed> This method is not implemented here " << endl;
}

//_____________________________________________________________________________
void SpdPythia6Decayer::Init()
{
    if (fInit) return;
    
    TPythia6Decayer::Init();

    /*!WARNING! Random seed should be initalized only from SpdPythia6Generator !WARNING! */
    
    fInit = kTRUE;
}

//______________________________________________________________________________
void SpdPythia6Decayer::Decay(Int_t idpart, TLorentzVector* p)
{
   // Decay a particle of type IDPART (PDG code) and momentum P.
    
   //cout << "-I- <SpdPythia6Decayer::Decay> Particle (pdg): " 
   //     << TDatabasePDG::Instance()->GetParticle(idpart)->GetName()  
   //     << " (" << idpart << ")" << endl;
   
   if (!fInit) Init();
    
   fParticlePdg = -1;
    
   if (!p) return;
   
   TPythia6::Instance()->Py1ent(0, idpart, p->Energy(), p->Theta(), p->Phi());
   TPythia6::Instance()->GetPrimaries();
   
   fParticlePdg = idpart;
   
   if (fDecayer) fDecayer->FillDecay(idpart,p);
}

//______________________________________________________________________________
Int_t SpdPythia6Decayer::ImportParticles(TClonesArray *particles)
{
   // Get the decay products into the passed PARTICLES TClonesArray of TParticles
   
   Int_t np = TPythia6::Instance()->ImportParticles(fParticles,"All"); 

   TParticle* p;
   
   TClonesArray &parts = *particles;
   parts.Clear();
   
   Int_t npp = 0;
   for (Int_t i(0); i<np; i++) {
        p = (TParticle*)fParticles->At(i);
        if (p->GetFirstMother() == 1) 
        {
            p = new(parts[npp++]) TParticle(*p);     
            p->SetStatusCode(1);
        }
   }
   
   fParticles->Clear();
   
//    cout << "-I- <SpdPythia6Decayer::ImportParticles> Particle (pdg): " << fParticlePdg  
//         << " add: " << npp << "/" << np << "; particles: " << endl;
//    
//    for (Int_t i(0); i<npp; i++) { 
//        p = (TParticle*)parts.At(i);
//        p->Print();
//    }
   
   if (fDecayer) fDecayer->FillParticles(&parts);
        
   return npp; 
}

//_____________________________________________________________________________
TPythia6* SpdPythia6Decayer::GetPythia()
{
   return TPythia6::Instance();
}

//_____________________________________________________________________________
Bool_t SpdPythia6Decayer::FillParsIn(SpdPrimGenParSet* params, Int_t index)
{
   if (!params) return kFALSE;
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdPythia6Decayer::LoadParsFrom(SpdPrimGenParSet* params, Int_t index)
{
   if (!params) return kFALSE;
   
   return kTRUE;  
}

//______________________________________________________________________________
Float_t SpdPythia6Decayer::GetLifetime(Int_t pdg)
{
   Int_t  kc = TPythia6::Instance()->Pycomp(TMath::Abs(pdg));
   Float_t t = TPythia6::Instance()->GetPMAS(kc,4) * 3.3333e-12;
   
   //cout << "pdg = " << pdg << " t = " << t << endl;
   
   return t;
}

//______________________________________________________________________________
void SpdPythia6Decayer::SelectForcedDecay(Int_t particle, Int_t channel)
{
   std::map< Int_t, std::set<Int_t> >::iterator it = fSelectedDecays.find(particle);
   if (it == fSelectedDecays.end()) {
       std::set<Int_t> channels;
       channels.insert(channel);
       fSelectedDecays[particle] = channels;
   }
   else it->second.insert(channel);   
}
  
//______________________________________________________________________________  
void SpdPythia6Decayer::ForceSelectedDecays()
{
   if (fSelectedDecays.empty()) {
       cout << "-W- <SpdPythia6Decayer::ForceSelectedDecays> Decays list is empty, nothing has been done. " << endl;
       return;
   }
   
   std::map< Int_t, std::set<Int_t> >::const_iterator it = fSelectedDecays.begin();
   std::set< Int_t >::const_iterator its; 
   
   TPythia6* pythia = TPythia6::Instance();
   
   Int_t pdg, channel;
   Int_t kc, ifirst, ilast;
   
   for (; it != fSelectedDecays.end(); it++) 
   {
       pdg = it->first;
       
       if (!TDatabasePDG::Instance()->GetParticle(pdg)) {
           cout << "-W- <SpdPythia6Decayer::ForceSelectedDecays> [TDatabasePDG] unknown particle:  " 
                << pdg << endl;
           continue;
       }
       
       kc = pythia->Pycomp(pdg);
       ifirst = pythia->GetMDCY(kc,2);
       ilast  = ifirst + pythia->GetMDCY(kc,3)-1;
       
       its = it->second.begin();
       
       fBraPart[kc] = 1.;
       
       cout << "\n-I- <SpdPythia6Decayer::ForceSelectedDecays> particle: " << pdg << " " << endl;
       channel = 0;
       for (Int_t ch = ifirst; ch <= ilast; ch++) {
            channel++;
            if (channel == *its) {
                pythia->SetMDME(ch,1,1);
                cout << "\t force channel: " << channel << endl;
                its++;
             
            }
            else {
                pythia->SetMDME(ch,1,0);
                fBraPart[kc] -= pythia->GetBRAT(ch);
            }
        }
   }
}

//______________________________________________________________________________
void SpdPythia6Decayer::PrintParticleDecayChannels(Int_t particle /*pdg number*/)
{
   TPythia6* pythia = TPythia6::Instance();

   Int_t kc = pythia->Pycomp(particle);
   
   pythia->SetMDCY(kc,1,1);

   Int_t ifirst = pythia->GetMDCY(kc,2);
   Int_t ilast  = ifirst + pythia->GetMDCY(kc,3)-1;
   
   TDatabasePDG* db = TDatabasePDG::Instance();
   TParticlePDG* p = db->GetParticle(particle);

   if (!p) {
       cout << "-W- <SpdPythia6Decayer::PrintParticleDecayBranching> [TDatabasePDG] unknown particle:  " 
            << particle << endl;
       return;
   }
   
   cout << "\n-I- <SpdPythia6Decayer::PrintParticleDecayBranching> Particle (pdg): " 
        <<  p->GetName() << " (" << particle << ")  Life time [ns]: " << GetLifetime(particle)*1e9 << endl;
        
   Double_t brsum(0);
   
   //  Loop over decay channels
   Int_t n(0), pdg;
   for (Int_t channel = ifirst; channel <= ilast; channel++) {
        cout << "channel: " << ++n << "; particles: ";
        for (Int_t i = 1; i <= 5; i++) {
             pdg = pythia->GetKFDP(channel,i);
             if (pdg == 0) continue;
             p = db->GetParticle(pdg);
             if (p) cout << p->GetName() << " (" << pdg << ") " << " ";
             else cout << "unknown" << " (" << pdg << ") " << "; ";
        }
        cout << " branch ratio: " << pythia->GetBRAT(channel) << endl;
        brsum += pythia->GetBRAT(channel);
   }
   
   cout << "Branching (sum): " << brsum  << endl;
}

