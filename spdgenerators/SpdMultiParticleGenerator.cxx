// $Id$
// Author: artur   2019/02/01


//_____________________________________________________________________________
//
// SpdMultiParticleGenerator
//_____________________________________________________________________________

#include "SpdMultiParticleGenerator.h"
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

ClassImp(SpdMultiParticleGenerator)

//_____________________________________________________________________________
SpdMultiParticleGenerator::SpdMultiParticleGenerator():
SpdGenerator("SPD MultiParticle Generator"),fNP(0),fNPAccepted(0),
fEnableDecayer(kFALSE),fSaveParticles(kTRUE),fInit(kFALSE)
{
}

//_____________________________________________________________________________
SpdMultiParticleGenerator::SpdMultiParticleGenerator(const char* Name):
SpdGenerator(Name),fNP(0),fNPAccepted(0),
fEnableDecayer(kFALSE),fSaveParticles(kTRUE),fInit(kFALSE) 
{
}

//_____________________________________________________________________________
SpdMultiParticleGenerator::~SpdMultiParticleGenerator() 
{
}

//_____________________________________________________________________________
void SpdMultiParticleGenerator::Reset()
{
   fNP = 0;
    
   fPdg.clear();
   fMomentum.clear();
   
   fVertex.clear();
   fTime.clear();
   
   fIonCharge.clear();
   fIonExcEnergy.clear();
   
   fNPAccepted = 0;
   fPAccepted.clear();
     
   fInit = kFALSE;
}

//_____________________________________________________________________________
Bool_t SpdMultiParticleGenerator::EnableExternalDecayer()
{
   FairRunSim* run = FairRunSim::Instance();
   if (run) {
       run->SetPythiaDecayer(kTRUE);
       fEnableDecayer = kTRUE;
   }
   
   if (fVerboseLevel > -2) {
       cout << "-I- <SpdMultiParticleGenerator::EnableExternalDecayer> "
            << "Activate external decayer: " << fEnableDecayer << endl;
   }
       
   return fEnableDecayer;
}

//_____________________________________________________________________________
void SpdMultiParticleGenerator::SetNP(Int_t np)
{
   Reset();
  
   if (np < 1) return;
   
   fNP = np;
   
   fPdg.assign(np,0);
   fMomentum.assign(np*3,0.);
}

//_____________________________________________________________________________
void SpdMultiParticleGenerator::SetParticle(Int_t i, Double_t px, Double_t py, Double_t pz, Int_t pdg)
{
   if (!CheckParticleNum("SetParticle",i)) return;
   if (!CheckInit("SetParticle")) return;
   
   fPdg[i] = -1;
   
   if (pdg == -1) {
       cout << "-E- <SpdMultiParticleGenerator::SetParticle> Unacceptable particle" << endl;
       return;
   }  
   
   if (IsIon(pdg)) {
       fIonCharge.assign(fNP,QH_);
       fIonExcEnergy.assign(fNP,0.);
       fIonCharge[i] = IonZ(pdg);
       fIonExcEnergy[i] = 0.;
   }
   else {
       TParticlePDG *part =  TDatabasePDG::Instance()->GetParticle(pdg);
       if (!part) {
           cout << "-E- <SpdMultiParticleGenerator::SetParticle> Unknown particle: " << pdg << endl;
           return;
       }  
   }
   
   fPdg[i] = pdg; 
   
   fMomentum[3*i]   = px;
   fMomentum[3*i+1] = py;
   fMomentum[3*i+2] = pz;
   
   fInit = kFALSE;
}

//_____________________________________________________________________________
void SpdMultiParticleGenerator::SetParticle(Int_t i, Double_t px, Double_t py, Double_t pz, 
                                Int_t Z, Int_t A, Int_t Q, Double_t EE)
{
   if (!CheckParticleNum("SetParticle",i)) return;
   if (!CheckInit("SetParticle")) return;
   
   fPdg[i] = IonPdg(Z,A);
   
   fMomentum[3*i]   = px;
   fMomentum[3*i+1] = py;
   fMomentum[3*i+2] = pz;
   
   if (Q != QH_ && fIonCharge.empty()) fIonCharge.assign(fNP,QH_);
   if (EE > EECUT_ && fIonExcEnergy.empty()) fIonExcEnergy.assign(fNP,0.);
      
   if (!fIonCharge.empty()) fIonCharge[i] = Q;
   if (!fIonExcEnergy.empty()) fIonExcEnergy[i] = EE;
  
   fInit = kFALSE;
}

//_____________________________________________________________________________
void SpdMultiParticleGenerator::SetVertex(Int_t i, Double_t vx, Double_t vy, Double_t vz)
{
   if (!CheckParticleNum("SetVertex",i)) return;
   if (!CheckInit("SetVertex")) return;
   
   if (fVertex.empty()) fVertex.assign(fNP*3,0.);
   
   fVertex[3*i]   = vx;
   fVertex[3*i+1] = vy;
   fVertex[3*i+2] = vz;
   
   fInit = kFALSE;
}

//_____________________________________________________________________________
void SpdMultiParticleGenerator::SetTime(Int_t i, Double_t t /*s*/)
{
   if (!CheckParticleNum("SetTime",i)) return;
   if (!CheckInit("SetTime")) return;
   
   if (fTime.empty()) fTime.assign(fNP,0.);
   
   fTime[i] = t;
   
   fInit = kFALSE;
}

//_____________________________________________________________________________
Bool_t SpdMultiParticleGenerator::Init()
{ 
   if (fInit) return kTRUE;
   
   fInit = kFALSE;
   
   fNPAccepted = 0;
   fPAccepted.clear();
   
   if (fNP < 1) return kFALSE;
     
   fPAccepted.assign(fNP,kFALSE);
   
   Int_t Z, A;
   for (Int_t i(0); i<fNP; i++) 
   {
       if (fPdg[i] == -1) continue;
       
       if (IsIon(fPdg[i])) {
          
           Double_t EE = 0.;
           if (!fIonExcEnergy.empty()) EE = fIonExcEnergy[i];  
           
           Int_t Q;
           if (!fIonCharge.empty()) {
               if (fIonCharge[i] != QH_) Q = fIonCharge[i];
               else {
                 Q = IonZ(fPdg[i]);
                 fIonCharge[i] = Q;
               }
           }
           else Q = IonZ(fPdg[i]);
           
           IonZA(fPdg[i],Z,A);
           AddIon(Z,A,Q,EE);         
       }
       else {
           if (!fIonExcEnergy.empty()) fIonExcEnergy[i] = 0.;
           if (!fIonCharge.empty()) fIonCharge[i] = 0.;
       }
       
       fPAccepted[i] = kTRUE;
       fNPAccepted++;
   }
   
   if (fNPAccepted < 1) {
       cout << "-F- <SpdMultiParticleGenerator::Init> No particles is accepted " << endl;
       exit(1);
   }
   
   if (fVerboseLevel > -1) {
       cout << "-I- <SpdMultiParticleGenerator::Init> Register particles: " << fNPAccepted << endl;
   }
  
   fInit = kTRUE;
   
   return kTRUE;
}
 
//_____________________________________________________________________________
Bool_t SpdMultiParticleGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
   if (!fInit) {
       cout << "-E- <SpdMultiParticleGenerator::ReadEvent> "
            << "Parameters are not initialized properly. " << endl;
       return kFALSE;
   }
   
   if (fNPAccepted == 0) {
       cout << "-E- <SpdMultiParticleGenerator::ReadEvent> "
            << "The list of particles is empty" << endl;
       return kFALSE;
   }

   Bool_t addTrack = kTRUE;
   
   Double_t vx, vy, vz, t;
   
   Int_t i(0);
   for (Int_t n(0); n < fNP; n++) {
     
        if (!fPAccepted[n]) continue;
        
        if (fVertex.empty()) { vx = 0; vy = 0; vz =0; }
        else { vx = fVertex[3*n]; vy = fVertex[3*n+1]; vz = fVertex[3*n+2]; }
          
        t = (fTime.empty()) ? 0. : fTime[n];
        
        AddTrack(primGen, addTrack, fPdg[n], i, -1, 
                 fMomentum[3*n], fMomentum[3*n+1], fMomentum[3*n+2],
                 vx, vy, vz, kTRUE, -1., t);
        i++;
   }
   
   if (fVerboseLevel > -1) {
       cout << "-I- <SpdMultiParticleGenerator::ReadEvent> "
            << "Add " << fNPAccepted << "/" << fNP << " tracks " << endl;
   }
   
   if (!primGen) return kFALSE; 
   
   return kTRUE; 
}
  
//_____________________________________________________________________________    
Bool_t SpdMultiParticleGenerator::FillParsIn(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::FillParsIn(params)) return kFALSE;
   
   params->SetParameter("SaveParticles",fSaveParticles,fGenIndex);
         
   if (!fSaveParticles) {
       if (fNPAccepted > 0) params->SetParameter("NP(a)",1,fGenIndex);
       else {
           params->SetParameter("NP(a)",0,fGenIndex);
           return kTRUE;
       }
   }
   else params->SetParameter("NP(a)",fNPAccepted,fGenIndex);
   
   params->SetParameter("EnableDecayer",fEnableDecayer,fGenIndex);
   
   Int_t i(0);
   for (Int_t n(0); n < fNP; n++) {
     
        if (!fPAccepted[n]) continue;
        
        params->SetParameter(Form("PDG(%d)",i),fPdg[n],fGenIndex);
        params->SetParameter(Form("Px(%d)",i),fMomentum[3*n],fGenIndex);
        params->SetParameter(Form("Py(%d)",i),fMomentum[3*n+1],fGenIndex);
        params->SetParameter(Form("Pz(%d)",i),fMomentum[3*n+2],fGenIndex);
        
        if (!fVertex.empty()) {
            params->SetParameter(Form("Vx(%d)",i),fVertex[3*n],fGenIndex);
            params->SetParameter(Form("Vy(%d)",i),fVertex[3*n+1],fGenIndex);
            params->SetParameter(Form("Vz(%d)",i),fVertex[3*n+2],fGenIndex);
        }
        
        if (!fTime.empty()) {
            params->SetParameter(Form("Time(%d)",i),fTime[n],fGenIndex);
        }
        
        if (IsIon(fPdg[n])) {
           if (!fIonCharge.empty()) params->SetParameter(Form("Q(%d)",i),fIonCharge[n],fGenIndex);
           if (!fIonExcEnergy.empty()) params->SetParameter(Form("ExcEnergy(%d)",i),fIonExcEnergy[n],fGenIndex);
        }
        
        if (!fSaveParticles) return kTRUE;
      
        i++; 
   }
  
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdMultiParticleGenerator::LoadParsFrom(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::LoadParsFrom(params)) return kFALSE;
  
   Reset();
   
   if (!params->GetParameter("SaveParticles",fSaveParticles,fGenIndex)) return kFALSE;
   if (!params->GetParameter("NP(a)",fNP,fGenIndex)) return kFALSE; 
   
   if (fNP < 1) {
       cout << "-W- <SpdMultiParticleGenerator::ReadEvent> Empty list of particles" << endl;
       return kFALSE; 
   }
     
   Bool_t enable_decayer;
   params->GetParameter("EnableDecayer",enable_decayer,fGenIndex);
   if (enable_decayer) EnableExternalDecayer();
   fEnableDecayer = enable_decayer;
   
   fPdg.assign(fNP,0);
   fMomentum.assign(fNP*3,0);
  
   Double_t val;
   Int_t ival= 55;
 
   if (params->GetParameter("Vx(0)",val,fGenIndex)) fVertex.assign(fNP*3,0);
   if (params->GetParameter("Time(0)",val,fGenIndex)) fTime.assign(fNP,0);
        
   Bool_t is_ok = kTRUE;
   
   for (Int_t i(0); i < fNP; i++) {
     
        if (!params->GetParameter(Form("PDG(%d)",i),fPdg[i],fGenIndex)) return kFALSE; 
          
        if (!params->GetParameter(Form("Px(%d)",i),fMomentum[3*i],fGenIndex)) is_ok = kFALSE;
        if (!params->GetParameter(Form("Py(%d)",i),fMomentum[3*i+1],fGenIndex)) is_ok = kFALSE;
        if (!params->GetParameter(Form("Pz(%d)",i),fMomentum[3*i+2],fGenIndex)) is_ok = kFALSE;
              
        if (!fVertex.empty()) {
            if (!params->GetParameter(Form("Vx(%d)",i),fVertex[3*i],fGenIndex)) is_ok = kFALSE;
            if (!params->GetParameter(Form("Vy(%d)",i),fVertex[3*i+1],fGenIndex)) is_ok = kFALSE;
            if (!params->GetParameter(Form("Vz(%d)",i),fVertex[3*i+2],fGenIndex)) is_ok = kFALSE;
        }
        
        if (!fTime.empty()) {
            if (!params->GetParameter(Form("Time(%d)",i),fTime[i],fGenIndex)) is_ok = kFALSE;
        }
        
        if (IsIon(fPdg[i])) {
          
           fIonCharge.assign(fNP,QH_);
           fIonExcEnergy.assign(fNP,0.);
          
           if (!params->GetParameter(Form("Q(%d)",i),fIonCharge[i],fGenIndex)) is_ok = kFALSE;      
           if (!params->GetParameter(Form("ExcEnergy(%d)",i),fIonExcEnergy[i],fGenIndex)) is_ok = kFALSE;
        }   
   }
   
   return is_ok;
}

//_____________________________________________________________________________    
void SpdMultiParticleGenerator::Print(Option_t* opt) const
{
   cout << "\t-I- <SpdMultiParticleGenerator::Print>\n" << endl;
   
   cout << "\tIs init:          " << fInit << endl; 
   cout << "\tEnable decayer:   " << fEnableDecayer << endl;
   cout << "\tNP:               " << fNP << endl;
   cout << "\tNP(accepted):     " << fNPAccepted << endl;
   
   cout << endl;
    
   if (fNPAccepted == 0) return;
   
   Double_t px, py, pz, vx, vy, vz;
   
   Int_t i(0);
   
   TParticlePDG* pdgpart;
   TString ptype, partname;
   
   printf("\t%-5s  %12s   %10s %10s %10s  %10s %10s %10s %10s  %8s %10s\n\n",
           "N","PDG","px","py","pz","vx","vy","vz","t","Q(ion)","EE(ion)");
   
   for (Int_t n(0); n < fNP; n++) {
     
        if (!fPAccepted[n]) continue;
        
        GetMomentum(n,px,py,pz);
        GetVertex(n,vx,vy,vz);
        
        pdgpart = TDatabasePDG::Instance()->GetParticle(fPdg[n]);
        
        if (pdgpart) {
            ptype = pdgpart->ParticleClass();
            partname = pdgpart->GetName();
        }
        else {
            ptype = "undefined";
            partname = "undefined";
        }
            
        i++;
        
        printf("\t%-5d  %12d   %10.3e %10.3e %10.3e  %10.3e %10.3e %10.3e %10.3e  %8d %10.3e    %s  %s\n",
                i, fPdg[n], px, py, pz, vx, vy, vz, GetTime(n), 
                GetIonQ(n), GetIonExcEnergy(n), 
                ptype.Data(), partname.Data()
                );      
   }
   
   cout << endl;
}

//_____________________________________________________________________________    
Bool_t SpdMultiParticleGenerator::AddIon(Int_t Z, Int_t A, Int_t Q, Double_t EE) // protected
{  
   FairRunSim* run = FairRunSim::Instance();
   if (!run) return kFALSE;
    
   TObjArray* ions = run->GetUserDefIons();
   if (!ions) return kFALSE; 
 
   Int_t pdg = IonPdg(Z,A);
   
   TString ionname;
   
   TDatabasePDG* pdgDB = TDatabasePDG::Instance();
   
   if (pdgDB && pdgDB->GetParticle(pdg)) ionname = pdgDB->GetParticle(pdg)->GetName();
   else ionname = Form("ION_%03d_%03d",Z,A);
 
   FairIon* ion = (FairIon*)ions->FindObject(ionname);
   if (ion) return kTRUE;
   
   if (Q != QH_) ion = new FairIon(ionname,Z,A,Q,EE);
   else ion = new FairIon(ionname,Z,A,Z,EE);
   
   run->AddNewIon(ion);
    
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdMultiParticleGenerator::CheckParticle(Int_t pdg) // protected
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
void SpdMultiParticleGenerator::Momentum(Double_t p, Double_t theta, Double_t phi,
                                         Double_t& px, Double_t& py, Double_t& pz)
{  // static
   p = TMath::Abs(p);
   
   theta *= TMath::DegToRad();
   phi   *= TMath::DegToRad();
   
   px = p * TMath::Sin(theta) * TMath::Cos(phi);
   py = p * TMath::Sin(theta) * TMath::Sin(phi);
   pz = p * TMath::Cos(theta);
}

//_____________________________________________________________________________    
void SpdMultiParticleGenerator::Momentum(Double_t p, 
                                         Double_t& px, Double_t& py, Double_t& pz)
{  // static
   p = TMath::Abs(p);
   
   TRandom3* gen = new TRandom3(0);
   gen->Sphere(px,py,pz,p);
   
   delete gen;
}

//_____________________________________________________________________________    
Int_t SpdMultiParticleGenerator::IonPdg(Int_t Z, Int_t A)  // static
{  
   return (Z == 1 && A == 1) ? 2212 : 1000000000 + Z*10000 + A*10; 
}

//_____________________________________________________________________________  
Bool_t SpdMultiParticleGenerator::IsIon(Int_t pdg)  // static
{  
   return (pdg > 1000000000 && pdg < 1099999999) ? kTRUE : kFALSE;
}

//_____________________________________________________________________________  
Int_t SpdMultiParticleGenerator::IonZ(Int_t pdg)  // static
{  
   return (pdg-1000000000)/10000; 
}

//_____________________________________________________________________________ 
Int_t SpdMultiParticleGenerator::IonA(Int_t pdg)  // static
{  
   return (pdg-1000000000-10000*IonZ(pdg))/10;
}

//_____________________________________________________________________________  
void SpdMultiParticleGenerator::IonZA(Int_t pdg, Int_t& Z, Int_t& A)  // static
{  
   Z = (pdg-1000000000)/10000; 
   A = (pdg-1000000000-10000*Z)/10;
}

//_____________________________________________________________________________  
TString SpdMultiParticleGenerator::IonName(Int_t pdg)   // static
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

//_____________________________________________________________________________   
Bool_t SpdMultiParticleGenerator::CheckInit(const Char_t* func) const // protected
{  
   if (fInit) {
       cout << "-W- <SpdMultiParticleGenerator::" << func << "> "
            << "This parameter is already initialized" << endl;
       return kFALSE;
   }
   return kTRUE;
}

//_____________________________________________________________________________   
Bool_t SpdMultiParticleGenerator::CheckParticleNum(const Char_t* func, Int_t pnum) const // protected
{ 
   if (pnum < 0 || pnum >= fNP) {
       cout << "-W- < SpdMultiParticleGenerator::" << func << "> "
            << "Bad particle number: " << pnum 
            << " (it must be < " << fNP << ") " << endl;
       return kFALSE;
   }
   return kTRUE; 
}


