// $Id$
// Author: artur   2020/09/22

//_____________________________________________________________________________
//
// SpdMCParticle
//_____________________________________________________________________________

#include "SpdMCParticle.h"
#include "SpdDetectorList.h"

#include <TDatabasePDG.h>  
#include <TParticle.h>          
#include <TParticlePDG.h>
#include <TMCProcess.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMCParticleInfo)
ClassImp(SpdMCParticle)

//_____________________________________________________________________________
SpdMCParticle::SpdMCParticle():
fMCTrackId(-1),fMCGeneratorId(-1),
fId(-1),fMotherId(-1),
fVertexG1MotherId(-1),fVertexG1MotherPdg(0),
fGeneration(-1),fPdgCode(0),
fVProcess(kPNoProcess),fVModule(kSTOPHERE),
fStartTime(0),fChar('-'),
fInfo(0)
{
  fSpecialMotherId[0] = -1; fSpecialMotherId[1] = 0;
}

//_____________________________________________________________________________
SpdMCParticle::~SpdMCParticle() 
{
   if (fInfo) { delete fInfo; fInfo = 0; }
}

//_____________________________________________________________________________
void SpdMCParticle::DeleteInfo()
{
   if (fInfo) { delete fInfo; fInfo = 0; }
}

//_____________________________________________________________________________
void SpdMCParticle::Clear(Option_t* option)
{
   DeleteInfo();
}

//_____________________________________________________________________________________
Bool_t SpdMCParticle::IsInDatabase() const
{
   TDatabasePDG* db = TDatabasePDG::Instance();
   if (!db) return false;
   TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(fPdgCode); 
   return (particle) ? true : false;
}

//_____________________________________________________________________________________
Double_t SpdMCParticle::GetMass() const
{
   if (TDatabasePDG::Instance()) {
       TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(fPdgCode);
       if (particle) return particle->Mass();
       else return 0.;
   }
   return 0.;
}

//_____________________________________________________________________
Double_t SpdMCParticle::GetCharge() const
{
   if (TDatabasePDG::Instance()) {
       TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(fPdgCode);
       if (particle) return particle->Charge()/3.;
       else return 0.;
   }
   return 0.;
}
 
//_____________________________________________________________________
Double_t SpdMCParticle::GetStartEnergy() const 
{  
   Double_t mass = GetMass();
   return TMath::Sqrt(mass*mass + fStartMom.Mag2());
}

//_____________________________________________________________________
Double_t SpdMCParticle::GetStartBetaGamma() const
{
   Double_t mass = GetMass();
   return (mass > 0.) ? fStartMom.Mag()/mass : 0;
}

//_____________________________________________________________________
TString SpdMCParticle::GetParticleName() const 
{
   if (fPdgCode > Int_t(1e9)) return "nucleus";  
   TParticlePDG* pdgpart = TDatabasePDG::Instance()->GetParticle(fPdgCode);
   return (pdgpart) ? pdgpart->GetName() : "unknown";
}

//_____________________________________________________________________
TString SpdMCParticle::GetParticleType() const 
{
   if (fPdgCode > Int_t(1e9)) return "nucleus";  
   TParticlePDG* pdgpart = TDatabasePDG::Instance()->GetParticle(fPdgCode);
   return (pdgpart) ? pdgpart->ParticleClass() : "unknown";
}

//_____________________________________________________________________
TString SpdMCParticle::GetVProcessName(Int_t proc_i) const
{
   if (proc_i < 0 || proc_i >= kMaxMCProcess) return TMCProcessName[kPNoProcess];
   return TMCProcessName[proc_i];
}

//_____________________________________________________________________________
void SpdMCParticle::Print(Option_t* option) const
{
    cout << "<SpdMCParticle::Print>\n\n";
    
    cout << "MC track id:             " << fMCTrackId << endl;
    cout << "MC generator id:         " << fMCGeneratorId << endl;
    cout << endl;
    cout << "Id:                      " << fId << endl;
    cout << "Mother id:               " << fMotherId << endl;
    cout << "G1 mother id/pdg:        " << fVertexG1MotherId << "/" << fVertexG1MotherPdg << endl;
    cout << "Special mother id/sid:   " << fSpecialMotherId[0] << "/" << fSpecialMotherId[1] << endl;
    cout << "Generation:              " << fGeneration << endl;
    cout << "Char.:                   " << fChar << endl;
    cout << endl;
    
    TParticlePDG* pdgpart = TDatabasePDG::Instance()->GetParticle(fPdgCode);
    TString ptype, pname;
    if (pdgpart) {
        ptype = pdgpart->ParticleClass();
        pname = pdgpart->GetName();
    }
    else {
        ptype = "undefined";
        pname = "undefined";
    }
    
    printf("Pdg number/type/name:    %d/%s/%s\n",fPdgCode,ptype.Data(),pname.Data()); 
    printf("Mass [GeV]:              %-12.6f [GeV]\n",GetMass()); 
    printf("Charge:                  %-2.0f\n",GetCharge());  
    printf("Init. process:           %d [%s]\n",fVProcess,TMCProcessName[fVProcess]);
    printf("Init. module:            %d \n",fVModule);
    printf("Start position:          (%f,%f,%f) [cm]\n", fStartPos.X(),fStartPos.Y(),fStartPos.Z());
    printf("Start momentum:          (%f,%f,%f) p = %f [GeV], theta = %6.3f [deg]\n",
                                     fStartMom.X(),fStartMom.Y(),fStartMom.Z(),
                                     fStartMom.Mag(),GetStartPtheta());
    printf("Start time [ns]:         %f [ns]\n",fStartTime);
    printf("\n");
    
    if (!strcmp(option,"short")) return;
    
    if (!fInfo) return;
    
    cout << "First vertex (id):       " << GetFirstVertexId() << endl;
    cout << "Last vertex (id):        " << GetLastVertexId() << endl;
    cout << "Track (id):              " << GetTrackId() << endl;
    cout << "TS   partcile (id):      " << GetTsParticleId() << endl;
    cout << "TOF  partcile (id):      " << GetTofParticleId() << endl;
    cout << "ECAL particle (id):      " << GetEcalParticleId() << endl;
    cout << "RS   particle (id):      " << GetRsParticleId() << endl;
    cout << endl;
}



