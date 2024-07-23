// $Id$
// Author: artur   2021/07/12


//_____________________________________________________________________________
//
// SpdEcalMCParticle
//_____________________________________________________________________________

#include "SpdEcalMCParticle.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalMCParticle)

//_____________________________________________________________________________
SpdEcalMCParticle::SpdEcalMCParticle():fId(-1),fMotherId(-1),
fParticleId(-1),fParticlePdg(0),fEdep(0),
fFirstState(-1),fLastState(-1)
{

}

//_____________________________________________________________________________
SpdEcalMCParticle::~SpdEcalMCParticle() 
{
 
}

//_____________________________________________________________________________
void SpdEcalMCParticle::Reset()
{
   fId = -1;
   fMotherId = -1;
   
   fParticleId = -1;
   fParticlePdg = 0;
   fEdep = 0;
   
   fFirstState = -1;
   fLastState = -1;
   
   fInfo.clear();   
}

//_____________________________________________________________________________
void SpdEcalMCParticle::SetNClusters(Int_t n) 
{ 
   fInfo.clear();
   fInfo.resize(n);
}

//_____________________________________________________________________________
void SpdEcalMCParticle::SetClusterInfo(Int_t loc_id, Int_t info_id, Int_t info_part_id)
{
   if (loc_id < 0 || !(loc_id < fInfo.size())) return;
   
   fInfo[loc_id].first   = info_id;
   fInfo[loc_id].second  = info_part_id;
}

//_____________________________________________________________________________
void SpdEcalMCParticle::Print(Option_t* option) const
{
   cout << "\n<SpdEcalMCParticle::Print> " << "\n\n";
   cout << "ID:                       " << fId << endl;
   cout << "Mother ID:                " << fMotherId << endl;
   cout << "MC-particle ID:           " << fParticleId << endl;
   cout << "MC-particle PDG code:     " << fParticlePdg << endl;
   cout << "Total particle deposit:   " << fEdep*1e3 << " [MeV] " << endl;
   cout << "First state region:       " << fFirstState << endl;
   cout << "Last state region:        " << fLastState << endl; 
   cout << "Number of clusters:       " << fInfo.size() << endl; 
   cout << endl;
     
   if (fInfo.size() < 1) return;
       
   printf("\t%3s %10s %10s\n\n","i","info_id","info_pid");
   Int_t loc_id(0);
   for (auto p : fInfo) printf("\t%3d %10d %10d \n",loc_id++, p.first, p.second);
   cout << endl;
}


