// $Id$
// Author: artur   2019/02/01


//_____________________________________________________________________________
//
// SpdParticleGenerator
//_____________________________________________________________________________

#include "SpdParticleGenerator.h"
#include "SpdPrimGenParSet.h"

#include "FairRunSim.h"
#include "FairIon.h"

#include <TDatabasePDG.h>
#include <TObjArray.h>
#include <TMath.h>
#include <TRandom3.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdParticleGenerator)

//_____________________________________________________________________________
SpdParticleGenerator::SpdParticleGenerator():SpdGenerator("SPD Particle Generator"),
fPdg(-1),fNP(1),fPx(0.),fPy(0.),fPz(0.),fVx(0.),fVy(0.),fVz(0.),fTime(0.),
fIonQ(0),fIonExcEnergy(0.),
fEnableDecayer(kFALSE),fInit(kFALSE)
{
}

//_____________________________________________________________________________
SpdParticleGenerator::SpdParticleGenerator(const char* Name):SpdGenerator(Name),
fPdg(-1),fNP(1),fPx(0.),fPy(0.),fPz(0.),fVx(0.),fVy(0.),fVz(0.),fTime(0.),
fIonQ(0),fIonExcEnergy(0.),
fEnableDecayer(kFALSE),fInit(kFALSE)
{
}

//_____________________________________________________________________________
SpdParticleGenerator::SpdParticleGenerator(Int_t pdg, 
                                           Double_t px, Double_t py, Double_t pz,
                                           const char* Name, Int_t np,
                                           Double_t vx, Double_t vy, Double_t vz,
                                           Double_t t
                                          ):
SpdGenerator(Name),fPdg(pdg),fNP(np),fPx(px),fPy(py),fPz(pz),
fVx(vx),fVy(vy),fVz(vz),fTime(t),
fIonQ(0),fIonExcEnergy(0.),
fEnableDecayer(kFALSE),fInit(kFALSE)
{
  if (IsIon(fPdg)) fIonQ = IonZ(pdg);
}

//_____________________________________________________________________________
SpdParticleGenerator::SpdParticleGenerator(Int_t Z, Int_t A,  
                         Double_t px, Double_t py, Double_t pz,     
                         const char* Name, Int_t np, 
                         Double_t vx , Double_t vy, Double_t vz, 
                         Double_t t
                        ):
SpdGenerator(Name),fNP(np),fPx(px),fPy(py),fPz(pz),
fVx(vx),fVy(vy),fVz(vz),fTime(t),fEnableDecayer(kFALSE),fInit(kFALSE)
{
   fPdg = IonPdg(Z,A);
   fIonQ = Z;
}

//_____________________________________________________________________________
SpdParticleGenerator::~SpdParticleGenerator() 
{
}

//_____________________________________________________________________________
void SpdParticleGenerator::Reset()
{
   fPdg = -1;
  
   fNP = 1;
  
   fPx = 0.; fPy = 0.; fPz = 0.;
   fVx = 0.; fVy = 0.; fVz = 0.;
   
   fTime = 0.;
   
   fIonQ = 0;
   fIonExcEnergy = 0.;
   
   fInit = kFALSE;
}

//_____________________________________________________________________________
Bool_t SpdParticleGenerator::EnableExternalDecayer()
{
   FairRunSim* run = FairRunSim::Instance();
   if (run) {
       run->SetPythiaDecayer(kTRUE);
       fEnableDecayer = kTRUE;
   }
   
   if (fVerboseLevel > -2) {
       cout << "-I- <SpdParticleGenerator::EnableExternalDecayer> "
            << "Activate external decayer: " << fEnableDecayer << endl;
   }
       
   return fEnableDecayer;
}

//_____________________________________________________________________________
void SpdParticleGenerator::SetPdg(Int_t pdg) 
{ 
   if (fInit) {
       cout << "-W- <SpdParticleGenerator::SetPdg> "
            << "This parameter is already initialized" << endl;
       return;
   }
     
   fPdg = -1;
   
   if (pdg == -1) {
       cout << "-E- <SpdParticleGenerator::SetPdg> Unacceptable particle" << endl;
       return;
   }  
   
   if (IsIon(pdg)) fIonQ = IonZ(pdg);
   else {
       TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(pdg);
       if (!part) {
           cout << "-E- <SpdParticleGenerator::SetPdg> Unknown particle: " << pdg << endl;
           return;
       }  
   }
   
   fPdg = pdg;
   
   fInit = kFALSE;
}

//_____________________________________________________________________________
void SpdParticleGenerator::SetNP(Int_t np)      
{
   if (fInit) {
       cout << "-W- <SpdParticleGenerator::SetNP> "
            << "This parameter is already initialized" << endl;
       return;     
   }
 
   (np > 0) ? fNP = np : fNP = 1; 
   
   fInit = kFALSE;
}

//_____________________________________________________________________________
void SpdParticleGenerator::SetMomentum(Double_t px, Double_t py, Double_t pz) 
{ 
   if (fInit) {
       cout << "-W- <SpdParticleGenerator::SetMomentum> "
            << "This parameter is already initialized" << endl;
       return;      
   }
   
   fPx = px; fPy = py; fPz = pz; 
   
   fInit = kFALSE;
}

//_____________________________________________________________________________
void SpdParticleGenerator::SetMomThetaPhi(Double_t p, Double_t theta, Double_t phi)
{
   if (fInit) {
       cout << "-W- <SpdParticleGenerator::SetMomThetaPhi> "
            << "This parameter is already initialized" << endl;
       return;      
   }
   
   p = TMath::Abs(p);
   
   theta *= TMath::DegToRad();
   phi   *= TMath::DegToRad();
    
   fPx = p * TMath::Sin(theta) * TMath::Cos(phi);
   fPy = p * TMath::Sin(theta) * TMath::Sin(phi);
   fPz = p * TMath::Cos(theta);
   
   fInit = kFALSE;
}

//_____________________________________________________________________________
void SpdParticleGenerator::SetVertex(Double_t vx, Double_t vy, Double_t vz)   
{  
   if (fInit) {
       cout << "-W- <SpdParticleGenerator::SetVertex> "
            << "This parameter is already initialized" << endl;
       return;      
   }
   
   fVx = vx; fVy = vy; fVz = vz; 
   
   fInit = kFALSE;
}

//_____________________________________________________________________________                 
void SpdParticleGenerator::SetTime(Double_t t)
{
   if (fInit) {
       cout << "-W- <SpdParticleGenerator::SetTime> "
            << "This parameter is already initialized" << endl;
       return;      
   }
   
   fTime = t;  
   
   fInit = kFALSE;
}

//_____________________________________________________________________________ 
void SpdParticleGenerator::SetIonQ(Int_t Q)
{
   if (fInit) {
       cout << "-W- <SpdParticleGenerator::SetIonQ> "
            << "This parameter is already initialized" << endl;
       return;      
   }
   
   fIonQ = Q;  
   
   fInit = kFALSE;
}
  
//_____________________________________________________________________________   
void SpdParticleGenerator::SetIonExcEnergy(Int_t ee)
{
   if (fInit) {
       cout << "-W- <SpdParticleGenerator::SetExcEnergy> "
            << "This parameter is already initialized" << endl;
       return;      
   }
   
   fIonExcEnergy = ee;
   
   fInit = kFALSE; 
}

//_____________________________________________________________________________
Bool_t SpdParticleGenerator::Init()
{
   if (fInit) return kTRUE;
     
   fInit = kFALSE;
   
   if (fPdg == -1) {
       cout << "-F- <SpdParticleGenerator::Init> Unacceptable particle " << endl;
       exit(1);
   }
   
   if (IsIon(fPdg)) AddIon(fPdg);
  
   if (fNP < 1) {
       cout << "-E- <SpdParticleGenerator::Init> Bad multiplicity: " << fNP << endl;
       return kFALSE;
   }
   
   fInit = kTRUE;
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdParticleGenerator::Initialize(Int_t pdg,  
                           Double_t px, Double_t py, Double_t pz,               
                           Int_t np, Double_t vx, Double_t vy, Double_t vz,
                           Double_t t)
{
   Reset();
   
   if (pdg == -1) {
       cout << "-F- <SpdParticleGenerator::Initialize> Unacceptable particle " << endl;
       exit(1);
   }
   
   fPdg = pdg;
   
   if (IsIon(fPdg)) {
       fIonQ = IonZ(fPdg);
       AddIon(fPdg); 
   }
   
   if (np < 1) {
       cout << "-E- <SpdParticleGenerator::Initialize> Bad multiplicity: " 
            << np << endl;
       return;
   }
   
   fNP = np;
   
   fPx = px; fPy = py; fPz = pz;
   fVx = vx; fVy = vy; fVz = vz;
   
   fTime = t; 
    
   fInit = kTRUE;
}

//_____________________________________________________________________________
void SpdParticleGenerator::Initialize(Int_t Z, Int_t A,  
                           Double_t px, Double_t py, Double_t pz,               
                           Int_t np, Double_t vx, Double_t vy, Double_t vz,
                           Double_t t)
{
   Reset();
   
   if (Z < 1 || A < 1) {
       cout << "-E- <SpdParticleGenerator::Initialize> Bad ion definition Z/A: " 
            << Z << "/" << A << endl;
       return;
   }
   
   fPdg = IonPdg(Z,A);
  
   if (IsIon(fPdg)) {
       fIonQ = IonZ(fPdg);
       AddIon(fPdg); 
   }
  
   if (np < 1) {
       cout << "-E- <SpdParticleGenerator::Initialize> Bad multiplicity: " 
            << np << endl;
       return;
   }
   
   fNP = np;
   
   fPx = px; fPy = py; fPz = pz;
   fVx = vx; fVy = vy; fVz = vz;
   
   fTime = t; 
    
   fInit = kTRUE;
}
 
//_____________________________________________________________________________
Bool_t SpdParticleGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
   if (!fInit) {
       cout << "-E- <SpdParticleGenerator::ReadEvent> "
            << "Parameters are not initialized properly. " << endl;
       return kFALSE;
   }
   
   if (primGen && !CheckParticle(fPdg)) {
       cout << "-E- <SpdParticleGenerator::ReadEvent> "
            << "Bad definition for the particle pdg: " << fPdg << endl;
       return kFALSE;      
   }
      
   Bool_t addTrack = kTRUE;
    
   for (Int_t n(0); n < fNP; n++) {
        AddTrack(primGen, addTrack, fPdg, n, -1, 
                 fPx, fPy, fPz, fVx, fVy, fVz,
                 kTRUE, -1. , fTime);
   }
   
   if (fVerboseLevel > -1) {
       cout << "-I- <SpdParticleGenerator::ReadEvent> "
            << "Add " << fNP << " tracks (pdg: " << fPdg << ") " << endl;
   }
   
   if (!primGen) return kFALSE; 
   
   return kTRUE; 
}
  
//_____________________________________________________________________________    
Bool_t SpdParticleGenerator::FillParsIn(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::FillParsIn(params)) return kFALSE;
   
   params->SetParameter("PDG",fPdg,fGenIndex);
   params->SetParameter("NP",fNP,fGenIndex);
    
   params->SetParameter("Px",fPx,fGenIndex);
   params->SetParameter("Py",fPy,fGenIndex);
   params->SetParameter("Pz",fPz,fGenIndex);
   
   params->SetParameter("Vx",fVx,fGenIndex);
   params->SetParameter("Vy",fVy,fGenIndex);
   params->SetParameter("Vz",fVz,fGenIndex);
   
   params->SetParameter("Time",fTime,fGenIndex);
   
   params->SetParameter("EnableDecayer",fEnableDecayer,fGenIndex);
   
   if (IsIon(fPdg)) {
       params->SetParameter("Q",fIonQ,fGenIndex);
       params->SetParameter("ExcEnergy",fIonExcEnergy,fGenIndex);
   }
  
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdParticleGenerator::LoadParsFrom(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::LoadParsFrom(params)) return kFALSE;
   
   Reset();
   
   Bool_t is_ok = kTRUE;
   
   if (!params->GetParameter("PDG",fPdg,fGenIndex)) is_ok = kFALSE;
     
   if (!params->GetParameter("NP",fNP,fGenIndex)) is_ok = kFALSE;
    
   if (!params->GetParameter("Px",fPx,fGenIndex)) is_ok = kFALSE;
   if (!params->GetParameter("Py",fPy,fGenIndex)) is_ok = kFALSE;
   if (!params->GetParameter("Pz",fPz,fGenIndex)) is_ok = kFALSE;
   
   if (!params->GetParameter("Vx",fVx,fGenIndex)) is_ok = kFALSE;
   if (!params->GetParameter("Vy",fVy,fGenIndex)) is_ok = kFALSE;
   if (!params->GetParameter("Vz",fVz,fGenIndex)) is_ok = kFALSE;
   
   if (!params->GetParameter("Time",fTime,fGenIndex)) is_ok = kFALSE;
   
   Bool_t enable_decayer;
   params->GetParameter("EnableDecayer",enable_decayer,fGenIndex);
   if (enable_decayer) EnableExternalDecayer();
   fEnableDecayer = enable_decayer;
   
   if (IsIon(fPdg)) {
       if (!params->GetParameter("Q",fIonQ,fGenIndex)) is_ok = kFALSE;
       if (!params->GetParameter("ExcEnergy",fIonExcEnergy,fGenIndex)) is_ok = kFALSE;
   }
   
   return is_ok;
}

//_____________________________________________________________________________    
void SpdParticleGenerator::Print(Option_t* opt) const
{
   cout << "\t-I- <SpdParticleGenerator::Print>\n" << endl;
   
   cout << "\tIs init:          " << fInit << endl;  
   cout << "\tEnable decayer:   " << fEnableDecayer << endl;
   cout << "\tPDG:              " << fPdg << endl;
   cout << "\tNP:               " << fNP << endl;
   cout << "\tMomentum [GeV/c]: " << fPx << ", " << fPy << ", " << fPz << endl;
   cout << "\tVertex [cm]:      " << fVx << ", " << fVy << ", " << fVz << endl;
   cout << "\tTime [s]:         " << fTime << endl; 
   
   if (IsIon(fPdg)) {
       cout << "\tIon charge [e]:   " << fIonQ << endl;
       cout << "\tIon E(exc) [GeV]: " << fIonExcEnergy << endl;
   }
   
   cout << endl;
}

//_____________________________________________________________________________    
Bool_t SpdParticleGenerator::AddIon(Int_t pdg) // protected
{  
   //if (!IsIon(pdg)) return kTRUE;
  
   FairRunSim* run = FairRunSim::Instance();
   if (!run) return kFALSE;
    
   TObjArray* ions = run->GetUserDefIons();
   if (!ions) return kFALSE; 
 
   Int_t Z, A;
   IonZA(pdg,Z,A);
   
   TString ionname;
   
   TDatabasePDG* pdgDB = TDatabasePDG::Instance();
   
   if (pdgDB && pdgDB->GetParticle(pdg)) ionname = pdgDB->GetParticle(pdg)->GetName();
   else ionname = Form("ION_%03d_%03d",Z,A);
 
   FairIon* ion = (FairIon*)ions->FindObject(ionname);
   if (ion) return kTRUE;
   
   ion = new FairIon(ionname,Z,A,fIonQ,fIonExcEnergy);
   run->AddNewIon(ion);
    
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdParticleGenerator::CheckParticle(Int_t pdg) // protected
{ 
   if (!IsIon(pdg)) return kTRUE;
   
   FairRunSim* run = FairRunSim::Instance();
   if (!run) return kFALSE;
   
   TObjArray* ions = run->GetUserDefIons();
   if (!ions) return kFALSE; 
   
   FairIon* ion = (FairIon*)ions->FindObject(IonName(pdg));
   
   return (ion) ? kTRUE : kFALSE;
}

//_____________________________________________________________________________    
void SpdParticleGenerator::Momentum(Double_t p, Double_t theta, Double_t phi,
                                    Double_t& px, Double_t& py, Double_t& pz)
{ // static
   p = TMath::Abs(p);
   
   theta *= TMath::DegToRad();
   phi   *= TMath::DegToRad();
   
   px = p * TMath::Sin(theta) * TMath::Cos(phi);
   py = p * TMath::Sin(theta) * TMath::Sin(phi);
   pz = p * TMath::Cos(theta);
}

//_____________________________________________________________________________    
void SpdParticleGenerator::Momentum(Double_t p, 
                                    Double_t& px, Double_t& py, Double_t& pz)
{ // static
   p = TMath::Abs(p);
   
   TRandom3* gen = new TRandom3(0);
   gen->Sphere(px,py,pz,p);
   
   delete gen;
}

//_____________________________________________________________________________    
Int_t SpdParticleGenerator::IonPdg(Int_t Z, Int_t A)  // static 
{  
   return (Z == 1 && A == 1) ? 2212 : 1000000000 + Z*10000 + A*10; 
}

//_____________________________________________________________________________  
Bool_t SpdParticleGenerator::IsIon(Int_t pdg) // static 
{  
   return (pdg > 1000000000 && pdg < 1099999999) ? kTRUE : kFALSE;
}

//_____________________________________________________________________________  
Int_t SpdParticleGenerator::IonZ(Int_t pdg) // static 
{  
   return (pdg-1000000000)/10000; 
}

//_____________________________________________________________________________ 
Int_t SpdParticleGenerator::IonA(Int_t pdg) // static 
{  
   return (pdg-1000000000-10000*IonZ(pdg))/10;
}

//_____________________________________________________________________________  
void SpdParticleGenerator::IonZA(Int_t pdg, Int_t& Z, Int_t& A)  // static 
{  
   Z = (pdg-1000000000)/10000; 
   A = (pdg-1000000000-10000*Z)/10;
}

//_____________________________________________________________________________  
TString SpdParticleGenerator::IonName(Int_t pdg)  // static 
{
   TString ionname;
    
   TDatabasePDG* pdgDB = TDatabasePDG::Instance();
   
   if (pdgDB && pdgDB->GetParticle(pdg)) {
       ionname = pdgDB->GetParticle(pdg)->GetName();
   }
   else {
       Int_t Z, A;
       IonZA(pdg,Z,A);
       ionname = Form("ION_%03d_%03d",Z,A);
   }
   
   return ionname;
}

