// $Id$
// Author: artur   2018/11/29


//_____________________________________________________________________________
//
// SpdRunSim
//_____________________________________________________________________________

#include "SpdRunSim.h"
#include "G4ParticleTable.hh"

#include "SpdCommonGeoMapper.h"
#include "FairPrimaryGenerator.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRunSim)

SpdRunSim* SpdRunSim::Instance() 
{ 
   if (dynamic_cast<SpdRunSim*>(fginstance)) return (SpdRunSim*)fginstance; 
   return 0;
}

//_____________________________________________________________________________
SpdRunSim::SpdRunSim():FairRunSim(kTRUE) 
{
   fginstance = this;
}

//_____________________________________________________________________________
SpdRunSim::~SpdRunSim() 
{
   cout << "-I- <SpdRunSim::~SpdRunSim>" << endl;
   
   fginstance = 0;
   fRunInstance = 0;
}

//_____________________________________________________________________________
void SpdRunSim::ForceParticleLifetime(Int_t pdg, Double_t t)
{
   if (t < 0) t = 0.;
   
   std::map<Int_t,Double_t>::iterator it = fForcedPartsLifetime.find(pdg);
   
   if (it != fForcedPartsLifetime.end()) {
       cout << "-W- <SpdRunSim::ForceParticleLifetime> The particle " << pdg 
            << " lifetime is already defined as: " << it->second << " ns."
            << " Define new lifetime value: " << t << endl;
       it->second = t;
       return;
   }
   
   //cout << "-I- <SpdRunSim::ForceParticleLifetime> Force life time " 
   //     << t << " [ns] for the particle " << pdg << endl;
        
   fForcedPartsLifetime.insert(std::pair<Int_t,Double_t>(pdg,t));
}

//_____________________________________________________________________________
void SpdRunSim::Init()
{
   SpdCommonGeoMapper::Instance()->FillModulesMap();
   
   PrevIniActions();
    
   FairRunSim::Init();
   
   PostIniActions(); 
}

//_____________________________________________________________________________
void SpdRunSim::Run(Int_t NEvents, Int_t NotUsed)
{
   FairRunSim::Run(NEvents,NotUsed);
    
   if (fGen) {
       TObjArray *genlist = fGen->GetListOfGenerators();
       if (genlist) genlist->Delete();
   }
}

//_____________________________________________________________________________
void SpdRunSim::PrevIniActions()
{
 
}
  
//_____________________________________________________________________________
void SpdRunSim::PostIniActions()
{
   if (!fForcedPartsLifetime.empty()) {
     
       G4ParticleTable* ptable = G4ParticleTable::GetParticleTable(); 
       G4ParticleDefinition* pdef;
       std::map<Int_t,Double_t>::const_iterator it = fForcedPartsLifetime.begin();
 
       for ( ; it != fForcedPartsLifetime.end(); it++) {
         
            pdef = ptable->FindParticle(it->first);
            if (!pdef) {
                cout << "-W- <SpdRunSim> (Force particle life time) Unknown particle: " 
                     << it->first << endl;
                continue;
            }

            pdef->SetPDGLifeTime(it->second);
            //pdef->DumpTable();
            
            cout << "-I- <SpdRunSim> (Force particle life time) " 
                 << " t = " << it->second << " [ns]; "
                 << " particle = " << it->first << endl;
       }    
   }
}
