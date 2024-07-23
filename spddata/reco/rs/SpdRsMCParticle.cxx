// $Id$
// Author: artur   2021/07/25


//_____________________________________________________________________________
//
// SpdRsMCParticle
//_____________________________________________________________________________

#include "SpdRsMCParticle.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRsMCParticle)

//_____________________________________________________________________________
SpdRsMCParticle::SpdRsMCParticle() 
{
 
}

//_____________________________________________________________________________
SpdRsMCParticle::~SpdRsMCParticle() 
{
 
}

//_____________________________________________________________________________
void SpdRsMCParticle::Reset()
{
   fId = -1;
   fMotherId = -1;
   
   fParticleId = -1;
   fParticlePdg = 0;
  
   fFirstState = -1;
   fLastState = -1;
   
   fInfo.clear();   
}

//_____________________________________________________________________________
void SpdRsMCParticle::SetNClusters(Int_t n) 
{ 
   fInfo.clear();
   fInfo.resize(n);
}

//_____________________________________________________________________________
void SpdRsMCParticle::SetClusterInfo(Int_t loc_id, Int_t cluster_id, Int_t cluster_part_id)
{
   if (loc_id < 0 || !(loc_id < fInfo.size())) return;
   
   fInfo[loc_id].first   = cluster_id;
   fInfo[loc_id].second  = cluster_part_id;
}

//_____________________________________________________________________________
void SpdRsMCParticle::Print(Option_t* option) const
{
   cout << "\n<SpdRsMCParticle::Print> " << "\n\n";
   cout << "ID:                       " << fId << endl;
   cout << "Mother ID:                " << fMotherId << endl;
   cout << "MC-particle ID:           " << fParticleId << endl;
   cout << "MC-particle PDG code:     " << fParticlePdg << endl;
   cout << "First state region:       " << fFirstState << endl;
   cout << "Last state region:        " << fLastState << endl; 
   cout << endl;
   
   if (fInfo.size() < 1) return;
       
   printf("\t%3s %12s %12s\n\n","i","cluster_id","cluster_pid");
   Int_t loc_id(0);
   for (auto p : fInfo) printf("\t%3d %12d %12d \n",loc_id++, p.first, p.second);
   cout << endl;
}

