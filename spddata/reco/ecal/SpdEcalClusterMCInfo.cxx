// $Id$
// Author: artur   2021/05/31


//_____________________________________________________________________________
//
// SpdEcalClusterMCInfo
//_____________________________________________________________________________

#include "SpdEcalClusterMCInfo.h"

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdEcalClusterMCInfo)

//_____________________________________________________________________________
SpdEcalClusterMCInfo::SpdEcalClusterMCInfo():fId(-1),fClusterId(-1),fNoiseEdep(0)
{
    memset(fNParticles,0,3*sizeof(Int_t));
}

//_____________________________________________________________________________
SpdEcalClusterMCInfo::~SpdEcalClusterMCInfo() 
{ 
    
}

//_____________________________________________________________________________
void SpdEcalClusterMCInfo::Reset()
{
   fId = -1;
   fClusterId = -1;
   
   memset(fNParticles,0,3*sizeof(Int_t));
   
   fData.clear();   
}

//_____________________________________________________________________________
void SpdEcalClusterMCInfo::SetNParticles(Int_t n0, Int_t n1,  Int_t n2) 
{ 
   if (n0 < 0 || n1 < 0 || n2 < 0) return;
       
   fNParticles[0] = n0; 
   fNParticles[1] = n1; 
   fNParticles[2] = n2; 
    
   fData.clear();
   fData.resize(n0+n1+n2);
}
           
//_____________________________________________________________________________
void SpdEcalClusterMCInfo::SetParticle(Int_t loc_id, Int_t loc_mid, 
                                       Int_t id, Int_t pdg, 
                                       Int_t first_state, Int_t last_state,
                                       Double_t edep)
{
   if (loc_id < 0 || !(loc_id < fData.size())) return;
   
   fData[loc_id].loc_mid = loc_mid;
   fData[loc_id].id  = id;
   fData[loc_id].pdg = pdg;
   fData[loc_id].first_state = first_state;
   fData[loc_id].last_state  = last_state;
   fData[loc_id].edep = edep;
}

//_____________________________________________________________________________ 
void SpdEcalClusterMCInfo::SetEcalParticleId(Int_t loc_id, Int_t id)
{
   if (loc_id < 0 || !(loc_id < fData.size())) return; 
   fData[loc_id].ecalpart_id = id;
}

//_____________________________________________________________________________ 
Double_t SpdEcalClusterMCInfo::GetClusterEdep() const
{
    Double_t edep = 0;
    for (auto p : fData) if (p.first_state == 0) edep += p.edep;
    return edep;
}

//_____________________________________________________________________________ 
Double_t SpdEcalClusterMCInfo::GetClusterTotalEdep() const
{
    return GetClusterEdep()+fNoiseEdep;
}

//_____________________________________________________________________________
void SpdEcalClusterMCInfo::Print(Option_t* option) const
{    
   cout << "\t<SpdEcalClusterMCInfo::Print>" << "\n\n";
   cout << "\tInfo unique ID:         " << fId << endl;
   cout << "\tCluster unique ID:      " << fClusterId << endl;
   cout << "\tCluster energy, MeV:    " << GetClusterEdep()*1.e3 << endl;
   cout << "\tNoise, MeV:             " << fNoiseEdep*1.e3 << endl;
   cout << "\tParticles(0|1|2):       " << fNParticles[0] 
        << " " << fNParticles[1] << " " << fNParticles[2] << endl;
   cout << "\n";
   
   printf("\t%3s %3s  %7s %10s  %12s  %4s %4s  %4s\n\n",
          "i","m","id","pdg","edep, MeV","s(1)","s(2)","epid");
   
   Int_t loc_id(0);
   for (auto p : fData) {
        printf("\t%3d %3d  %7d %10d  %12.3f  %4d %4d  %4d\n",
               loc_id++, p.loc_mid, 
               p.id, p.pdg, 
               p.edep*1.e3,
               p.first_state, p.last_state,
               p.ecalpart_id
               );
   }
   cout << endl;
}

