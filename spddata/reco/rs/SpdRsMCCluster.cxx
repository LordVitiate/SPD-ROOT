// $Id$
// Author: artur   2021/06/03


//_____________________________________________________________________________
//
// SpdRsMCCluster
//_____________________________________________________________________________

#include "SpdRsMCCluster.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdRsMCCluster)

//_____________________________________________________________________________
SpdRsMCCluster::SpdRsMCCluster():fId(-1),fModId(-1),fDetId(-10)
{
 
}

//_____________________________________________________________________________
SpdRsMCCluster::~SpdRsMCCluster() 
{
 
}

//_____________________________________________________________________________
void SpdRsMCCluster::Reset()
{
   fId = -1;
   fModId = -1;
   fDetId = -10;

   fHits.clear();
   fData.clear();   
}

//_____________________________________________________________________________
void SpdRsMCCluster::AddHit(Int_t id) 
{ 
    for (auto hid : fHits) if (hid == id) return;
    fHits.push_back(id); 
}

//_____________________________________________________________________________
void SpdRsMCCluster::SetNParticles(Int_t n) 
{ 
   fData.clear();
   fData.resize(n);
}
           
//_____________________________________________________________________________
void SpdRsMCCluster::SetParticle(Int_t loc_id, Int_t loc_mid, 
                                 Int_t id, Int_t pdg, 
                                 Int_t first_state, Int_t last_state)
{
   if (loc_id < 0 || !(loc_id < fData.size())) return;
   
   fData[loc_id].loc_mid = loc_mid;
   fData[loc_id].id  = id;
   fData[loc_id].pdg = pdg;
   fData[loc_id].first_state = first_state;
   fData[loc_id].last_state  = last_state;
}

//_____________________________________________________________________________ 
void SpdRsMCCluster::SetRsParticleId(Int_t loc_id, Int_t id)
{
   if (loc_id < 0 || !(loc_id < fData.size())) return; 
   fData[loc_id].rspart_id = id;
}

//_____________________________________________________________________________
void SpdRsMCCluster::Print(Option_t* option) const
{
    cout << "\n<SpdRsMCCluster::Print> " << "\n\n";
    cout << "Unique cluster ID:            " << fId << endl;
    cout << "Detector (supermodule) ID:    " << fModId << endl;
    cout << "Detector (barrel,endcap) ID:  " << fDetId << endl;
    cout << "Number of hits:               " << fHits.size();
    
    if (fHits.size() < 1) { cout << "\n\n"; return; }
    
    Int_t n = 0;
    for (const Int_t& i : fHits) {
         if (n++ % 5 == 0) cout << Form("\n %30s","[hit_id] :");
         cout << Form(" %6d",i);
    }
    cout << "\n\n";
     
    if (fData.size() < 1) return;
     
    cout << "Number of particles:          " << fData.size() << "\n\n";
    
    printf("\t%3s %3s  %7s %10s  %4s %4s  %5s\n\n",
          "i","m","id","pdg","s(1)","s(2)","rspid");
   
    Int_t loc_id(0);
    for (auto p : fData) {
         printf("\t%3d %3d  %7d %10d  %4d %4d  %5d\n",
                loc_id++, p.loc_mid, 
                p.id, p.pdg, 
                p.first_state, p.last_state,
                p.rspart_id
               );
    }
    cout << endl;
}




