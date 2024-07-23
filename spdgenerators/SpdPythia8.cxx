// $Id$
// Author: artur   2021/10/01


//_____________________________________________________________________________
//
// SpdPythia8
//_____________________________________________________________________________

#include "SpdPythia8.h"

#include "TClonesArray.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TLorentzVector.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdPythia8)

//_____________________________________________________________________________
SpdPythia8::SpdPythia8():TGenerator("SpdPythia8","SpdPythia8"),
fPythia(0),fNParticles(0),fStorageIndex(-100),fExtGenerator(0),fCurrentSeed(0)
{
   delete fParticles; // was allocated as TObjArray in TGenerator
   
   fParticles = new TClonesArray("TParticle",50);
   fPythia    = new Pythia8::Pythia();
   
   AddParticlesToPdgDataBase();
   
   fParams = new TObjArray();
}

//_____________________________________________________________________________
SpdPythia8::SpdPythia8(const char* xmlDir, bool banner):TGenerator("SpdPythia8","SpdPythia8"),
fPythia(0),fNParticles(0),fStorageIndex(-100),fExtGenerator(0),fCurrentSeed(0)
{
   delete fParticles; // was allocated as TObjArray in TGenerator
 
   fParticles = new TClonesArray("TParticle",50);
   fPythia    = new Pythia8::Pythia(xmlDir,banner);
   
   AddParticlesToPdgDataBase();
   
   fParams = new TObjArray();
}

//_____________________________________________________________________________
SpdPythia8::~SpdPythia8()
{
   if (fParams) {
       fParams->Delete();
       delete fParams;
       fParams = 0;
   }
   
   if (fParticles) {
       fParticles->Delete();
       delete fParticles;
       fParticles = 0;
   }
   
   if (fExtGenerator) {
       delete fExtGenerator;
       fExtGenerator = 0;
   }
   
   delete fPythia;
}

//_____________________________________________________________________________
Bool_t SpdPythia8::Init()
{
   Bool_t ini = fPythia->init();
   if (ini) cout << "-I- <SpdPythia8::Init> Initialization successful " << endl; 
   else     cout << "-E- <SpdPythia8::Init> Wrong initialization" << endl; 
   return ini;
}

//_____________________________________________________________________________
void SpdPythia8::GenerateEvent()
{
   // Generate the next event
    
   SearchForCurrentEventSeed();
    
   if (!fPythia->next()) {
       cout << "-E- <SpdPythia8Generator::ReadEvent> Wrong ganarator status " << endl;
       GenerateEvent();
   }
       
   ImportParticles();
}

//_____________________________________________________________________________
void SpdPythia8::SetParameter(std::string par) 
{
   fPythia->readString(par);
   //cout << "-I- <SpdPythia8::SetParameter> (\"" <<par.c_str() << "\")" << endl;
}

//_____________________________________________________________________________
void SpdPythia8::SetSeed(UInt_t seed, Bool_t UseExternalGenerator)
{
    if (!UseExternalGenerator)  {
        SetParameter("Random:setSeed = on");
        SetParameter(Form("Random:seed = %d",seed));
    }
    else {
        SetExtGenerator(seed);
        SearchForCurrentEventSeed();
        SetParameter("Random:setSeed = off");
        SetParameter(Form("Random:seed = %d",fCurrentSeed));  
    }
}

//_____________________________________________________________________________
Int_t SpdPythia8::ImportParticles(TClonesArray* particles, Option_t *option)
{
   // Import particles from Pythia stack
    
   fNParticles = 0;
   
   if (!particles) return 0;

   particles->Clear();
   
   Int_t ioff = (fPythia->event[0].id() == 90) ? -1 : 0; // the first output particle is string!
   Int_t i = (ioff < 0) ? 1 : 0; 
   
   Int_t nparts = fPythia->event.size();
 
   TClonesArray& parts = *particles;
 
   if (!strcmp(option,"") || !strcmp(option,"Final")) 
   {
      for (; i < nparts; i++) {
           // chack if particle is final 
           if (!fPythia->event[i].isFinal()) continue;
           // add particle
           new (parts[fNParticles++]) 
                TParticle(fPythia->event[i].id(), 
                          fPythia->event[i].isFinal(),
                          -1, -1,
                          -1, -1,
                          fPythia->event[i].px(),         // [GeV/c]
                          fPythia->event[i].py(),         // [GeV/c]
                          fPythia->event[i].pz(),         // [GeV/c]
                          fPythia->event[i].e(),          // [GeV]
                          fPythia->event[i].xProd()*0.1,  // [mm] -> [cm]
                          fPythia->event[i].yProd()*0.1,  // [mm] -> [cm]
                          fPythia->event[i].zProd()*0.1,  // [mm] -> [cm]
                          fPythia->event[i].tProd()*3.3356410e-12); // [mm/c] -> [s]  (t -> t/c)
      } 
      return fNParticles;
   } 
   
   if (!strcmp(option,"All")) 
   {
      for (; i < nparts; i++) {
           // add particle
           new (parts[fNParticles++]) 
                TParticle(fPythia->event[i].id(), 
                          fPythia->event[i].isFinal(),
                          fPythia->event[i].mother1()   + ioff, fPythia->event[i].mother2()   + ioff,
                          fPythia->event[i].daughter1() + ioff, fPythia->event[i].daughter2() + ioff,
                          fPythia->event[i].px(),         // [GeV/c]
                          fPythia->event[i].py(),         // [GeV/c]
                          fPythia->event[i].pz(),         // [GeV/c]
                          fPythia->event[i].e(),          // [GeV]
                          fPythia->event[i].xProd()*0.1,  // [mm] -> [cm]
                          fPythia->event[i].yProd()*0.1,  // [mm] -> [cm]
                          fPythia->event[i].zProd()*0.1,  // [mm] -> [cm]
                          fPythia->event[i].tProd()*3.3356410e-12); // [mm/c] -> [s]  (t -> t/c)
      }
      return fNParticles;
   }
}

//_____________________________________________________________________________
TObjArray* SpdPythia8::ImportParticles(Option_t* /*option*/)
{
   // Import particles from Pythia stack
    
   fNParticles = 0;
   
   fParticles->Clear();
   
   Int_t ioff = (fPythia->event[0].id() == 90) ? -1 : 0; // the first output particle is string!
   Int_t i = (ioff < 0) ? 1 : 0; 
   
   Int_t nparts = fPythia->event.size();
 
   TClonesArray &parts = *((TClonesArray*)fParticles);
   
   for (; i < nparts; i++) {
        // add particle
        new (parts[fNParticles++]) 
             TParticle(fPythia->event[i].id(), 
                       fPythia->event[i].isFinal(),
                       fPythia->event[i].mother1()   + ioff, fPythia->event[i].mother2()   + ioff,
                       fPythia->event[i].daughter1() + ioff, fPythia->event[i].daughter2() + ioff,
                       fPythia->event[i].px(),         // [GeV/c]
                       fPythia->event[i].py(),         // [GeV/c]
                       fPythia->event[i].pz(),         // [GeV/c]
                       fPythia->event[i].e(),          // [GeV]
                       fPythia->event[i].xProd()*0.1,  // [mm] -> [cm]
                       fPythia->event[i].yProd()*0.1,  // [mm] -> [cm]
                       fPythia->event[i].zProd()*0.1,  // [mm] -> [cm]
                       fPythia->event[i].tProd()*3.3356410e-12); // [mm/c] -> [s]  (t -> t/c)
   }
   
   return fParticles;
}

//_____________________________________________________________________________
void SpdPythia8::AddParticlesToPdgDataBase()
{
   // Add some pythia specific particle code to the data base
    
   TDatabasePDG *pdgDB = TDatabasePDG::Instance();
   
   if (!pdgDB->GetParticle(90))      pdgDB->AddParticle("string",    "string",    0, kTRUE, 0, 0, "QCD string",       90);
   if (!pdgDB->GetParticle(9900110)) pdgDB->AddParticle("rho_diff0", "rho_diff0", 0, kTRUE, 0, 0, "QCD diffr. state", 9900110);
   if (!pdgDB->GetParticle(9900210)) pdgDB->AddParticle("pi_diffr+", "pi_diffr+", 0, kTRUE, 0, 1, "QCD diffr. state", 9900210);
   if (!pdgDB->GetParticle(9900220)) pdgDB->AddParticle("omega_di",  "omega_di",  0, kTRUE, 0, 0, "QCD diffr. state", 9900220);
   if (!pdgDB->GetParticle(9900330)) pdgDB->AddParticle("phi_diff",  "phi_diff",  0, kTRUE, 0, 0, "QCD diffr. state", 9900330);
   if (!pdgDB->GetParticle(9900440)) pdgDB->AddParticle("J/psi_di",  "J/psi_di",  0, kTRUE, 0, 0, "QCD diffr. state", 9900440);
   if (!pdgDB->GetParticle(9902110)) pdgDB->AddParticle("n_diffr0",  "n_diffr0",  0, kTRUE, 0, 0, "QCD diffr. state", 9902110);
   if (!pdgDB->GetParticle(9902210)) pdgDB->AddParticle("p_diffr+",  "p_diffr+",  0, kTRUE, 0, 1, "QCD diffr. state", 9902210);
}

//_____________________________________________________________________________
void SpdPythia8::ListAll() const
{
   if (fPythia) fPythia->settings.listAll();
}

//_____________________________________________________________________________    
void SpdPythia8::ListChanged() const
{
   if (fPythia) fPythia->settings.listChanged();
}
    
//_____________________________________________________________________________   
void SpdPythia8::PartList(Int_t id) const
{
   if (fPythia) fPythia->particleData.list(id);
}
//_____________________________________________________________________________      
void SpdPythia8::PartListAll() const
{
   if (fPythia) fPythia->particleData.listAll();
}

//_____________________________________________________________________________   
void SpdPythia8::PartListChanged() const
{
   if (fPythia) fPythia->particleData.listChanged();
}

//_____________________________________________________________________________   
void SpdPythia8::PrintStatistics() const
{
   if (fPythia) fPythia->stat();
}

//_____________________________________________________________________________
void SpdPythia8::EventListing(Bool_t showScaleAndVertex, Bool_t showMothersAndDaughters) const
{
   if (fPythia) fPythia->event.list(showScaleAndVertex,showMothersAndDaughters);
}

//_____________________________________________________________________________
void SpdPythia8::SetExtGenerator(UInt_t seed) //-> protected
{
   if (!fExtGenerator) {
       fExtGenerator = new SpdPythia8Rndm();
       fPythia->setRndmEnginePtr(fExtGenerator);
   }
   ((SpdPythia8Rndm*)fExtGenerator)->SetSeed(seed);
}
  
//_____________________________________________________________________________  
void SpdPythia8::SearchForCurrentEventSeed() //-> protected
{
   if (fExtGenerator) {
       fCurrentSeed = ((SpdPythia8Rndm*)fExtGenerator)->GetSeed();
   }
}

//_____________________________________________________________________________________
void SpdPythia8::Print(Option_t* opt) const
{
   cout << "<SpdPythia8::Print> Print current event "<< "\n\n";
   cout << "Storage index:       " << fStorageIndex << endl;
   cout << "External generator:  " << HasExternalGenerator() << endl;
   cout << "Current event seed:  " << fCurrentSeed << endl;
   cout << "Number of particles: " << fNParticles << endl;
   cout << endl;
   
   TClonesArray* parts = (TClonesArray*)fParticles;
   PrintParticles(parts);
}

//_____________________________________________________________________________
void SpdPythia8::PrintParticles(const TClonesArray* particles) const
{
   if (!particles) return;
   
   Int_t np = particles->GetEntriesFast();
   
   if (np < 1) return;
 
   TParticle* p;
   TParticlePDG* pdgpart;
   TString ptype, partname, ss;
   Int_t stackmother(0);  

   printf("\t%-5s %5s  %8s %8s  %8s %8s  %12s  %14s %14s %14s  %10s %10s %10s %10s   %s \n\n",
           "I", "FINAL", "M(1)", "M(2)", "D(1)","D(2)", "PDG", "Px","Py","Pz",  "Vx","Vy","Vz","T", "PTYPE/PNAME");
        
   for (Int_t i(0); i < np; i++) {
 
         p = (TParticle*)particles->At(i);
         
         if (!p) continue;
         
         pdgpart = TDatabasePDG::Instance()->GetParticle(p->GetPdgCode());
         
         if (pdgpart) {
             ptype = pdgpart->ParticleClass();
             partname = pdgpart->GetName();
         }
         else {
             ptype = "undefined";
             partname = "undefined";
         }
         
         if (p->GetStatusCode() == 1) {
             ss = Form("[%3d]",stackmother); 
             stackmother++;
         }
         else ss = Form("%5s","");
           
         printf("\t%-5d %5s  %8d %8d  %8d %8d  %12d  %14.6f %14.6f %14.6f  %10.3e %10.3e %10.3e %10.3e  %s/%s\n",
                 i, ss.Data(), 
                 p->GetFirstMother(), p->GetSecondMother(), 
                 p->GetFirstDaughter(), p->GetLastDaughter(), 
                 p->GetPdgCode(), 
                 p->Px(), p->Py(), p->Pz(), 
                 p->Vx(), p->Vy(), p->Vz(), p->T(), 
                 ptype.Data(), partname.Data()); 
   }
   
   cout << endl;
}

//_____________________________________________________________________________
Bool_t SpdPythia8::FillParsIn(SpdPrimGenParSet* params)
{
   if (!params) return false; 
   
   return true;
}

//_____________________________________________________________________________
Bool_t SpdPythia8::LoadParsFrom(SpdPrimGenParSet* params)
{
   if (!params) return false;
   
   return true;
}



