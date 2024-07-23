// $Id$
// Author: artur   2018/12/05


//_____________________________________________________________________________
//
// SpdPrimGenData
//_____________________________________________________________________________

#include <TClonesArray.h>
#include <TParticle.h>
#include <TDatabasePDG.h>

#include <iostream>

using std::cout;
using std::endl;

#include "SpdBaseParSet.h"
#include "SpdPrimGenData.h"

ClassImp(SpdPrimGenData)

//_____________________________________________________________________________
SpdPrimGenData::SpdPrimGenData():
TNamed("SpdGenerator","SpdPrimGenData"),
fGenIndex(0),fParticles(0),fParameters(0)
{
 
}

//_____________________________________________________________________________
SpdPrimGenData::SpdPrimGenData(const Char_t* generator):
TNamed(generator,"SpdPrimGenData"),
fGenIndex(0),fParticles(0),fParameters(0)
{
  
}

//_____________________________________________________________________________
SpdPrimGenData::~SpdPrimGenData() 
{
   if (fParticles) {
       fParticles->Delete();
       delete fParticles;
   }
   
   if (fParameters) delete fParameters;
}

//_____________________________________________________________________________
void SpdPrimGenData::Clear()
{
   fGenSeeds.clear();
   
   if (fParticles) fParticles->Delete();
   if (fParameters) fParameters->clear();
}

//_____________________________________________________________________________
TParticle* SpdPrimGenData::AddParticle(const TParticle* part)
{
   Int_t np = 0;
   
   if (!fParticles) fParticles = new TClonesArray("TParticle");
   else np = fParticles->GetEntriesFast();
  
   TParticle* p = new((*fParticles)[np]) TParticle(*part); 
   return p;
}

//_____________________________________________________________________________
TParticle* SpdPrimGenData::AddNewParticle()
{
   Int_t np = 0;
   
   if (!fParticles) fParticles = new TClonesArray("TParticle");
   else np = fParticles->GetEntriesFast();
 
   TParticle* p = new((*fParticles)[np]) TParticle();
   return p;
}

//_____________________________________________________________________________
Int_t SpdPrimGenData::GetNParticles() const 
{ 
   return (fParticles) ? fParticles->GetEntriesFast() : 0; 
}

//_____________________________________________________________________________
Int_t SpdPrimGenData::GetSeed(Int_t i)
{
   return (i < 0 || i>= fGenSeeds.size()) ? 0 : fGenSeeds[i];
}

//_____________________________________________________________________________
TParticle* SpdPrimGenData::GetParticle(Int_t i)
{
   if (!fParticles || i < 0 || i >= fParticles->GetEntriesFast()) return 0;
   return (TParticle*)fParticles->At(i);
}

//_____________________________________________________________________________
SpdBaseParSet* SpdPrimGenData::GetParameters()
{
   if (!fParameters) fParameters = new SpdBaseParSet();
   return fParameters;
}

//_____________________________________________________________________________
void SpdPrimGenData::Print(Option_t* option)
{ 
    cout << "\n";
    cout << "\t[" << option << "] Type/Index: " << GetGenType() << "/" << fGenIndex << "\n";
    
    Int_t npars = (fParameters) ? fParameters->GetNParameters() : 0 ;      
    cout << "\t[" << option << "] Parameters: " << npars << endl;
    if (npars > 0) { cout << endl; fParameters->print(1); }
    
    Int_t ns = fGenSeeds.size();
    cout << "\t[" << option << "] Seeds (" << ns << ") ";
   
    if (ns > 0) {
       cout << ": ";
       for (Int_t i(0); i<ns; i++) cout << fGenSeeds[i] << " ";
    }
    cout << "\n";
    
    Int_t np = GetNParticles();
    cout << "\t[" << option << "] Particles:  " << np << endl;
        
    if (np < 1) return;     
    
    cout << "\n";
    
    TParticle* p;
    TParticlePDG* pdgpart;
    TString ptype, partname, ss;
    Int_t stackmother;     
    
    printf("\t%-5s %3s/%-5s %5s  %12s  %14s %14s %14s  %10s %10s %10s %10s  %8s  %s \n\n",
           "I",   "GEN","PNUM", "STACK", "PDG", "Px","Py","Pz",  "Vx","Vy","Vz","T",  "MOTHER", "PTYPE/PNAME");
    
    for (Int_t i(0); i<np; i++) {
      
         p = (TParticle*)fParticles->At(i);
         
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
         
         stackmother = p->GetSecondMother();
         (stackmother < 0) ? ss = Form("%5s","") : ss = Form("[%3d]",stackmother);
           
         printf("\t%-5d %3d/%-5d %5s  %12d  %14.6f %14.6f %14.6f  %10.3e %10.3e %10.3e %10.3e  %8d  %s/%s\n",
                 i, p->GetFirstDaughter(), p->GetLastDaughter(), 
                 ss.Data() , p->GetPdgCode(), 
                 p->Px(), p->Py(), p->Pz(), 
                 p->Vx(),  p->Vy(),  p->Vz(), p->T(), 
                 p->GetFirstMother(), 
                 ptype.Data(), partname.Data()); 
         
   }
}


