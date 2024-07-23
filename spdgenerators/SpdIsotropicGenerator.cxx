// $Id$
// Author: artur   2017/11/15


//_____________________________________________________________________________
//
// SpdIsotropicGenerator
//_____________________________________________________________________________

#include "SpdIsotropicGenerator.h"

#include "SpdPrimaryGenerator.h"
#include "SpdMCEventHeader.h"
#include "SpdPrimGenParSet.h"

#include <TMath.h>
#include <TMCParticle.h> 
#include <TDatabasePDG.h>
#include <TParticle.h>

#include <iostream>

using std::cout;
using std::endl;
using std::map;
using std::pair;

#define DP_MIN 1e-9  // eV/c
#define MAX_DENS_LEVEL 7

using namespace TMath;

ClassImp(SpdIsotropicGenerator)

//_____________________________________________________________________________
SpdIsotropicGenerator::SpdIsotropicGenerator():SpdGenerator("SPD Isotropic Generator"),
fParticles(0)
{
    Reset();
}

//_____________________________________________________________________________
SpdIsotropicGenerator::SpdIsotropicGenerator(const Char_t* Name):SpdGenerator(Name),
fParticles(0)
{
    Reset();
}

//_____________________________________________________________________________
SpdIsotropicGenerator::~SpdIsotropicGenerator() 
{
    if (fParticles) {
        fParticles->Delete();
        delete fParticles;
    }
}

//_____________________________________________________________________________
void SpdIsotropicGenerator::Reset()
{
    fVgenopt = 2;        // Generator option
    fKeepEvent = kFALSE; // Particles storing 
    
    fPdg = 0;            // particle pdg-number
    fMomentum = 1.;      // particle momentum
    fNPL = 1;            // particles per event
    
    fTmin =  1;          // cos(theta_min)
    fTmax = -1;          // cos(theta_max)
    fPhimin = 0;         // phi_min, rad
    fPhimax = TwoPi();   // phi_max, rad
    
    fXTmin  = 1;         // cos(theta_min)
    fXTmax  = -1;        // cos(theta_max)
    fXPhi   = 0;         // phi(solid angle), rad
    fXTheta = 0;         // theta(solid angle), rad
    
    fDP = 0;             // momentum step
    fNP = 1;             // number of steps by momentum 
   
    SetSeed();
       
    fInit = kFALSE;      // Init mark
    
    fECounter = 0;       // event counter (<= fNP)
   
    ResetEvent();
}

//_____________________________________________________________________________
void SpdIsotropicGenerator::ResetEvent()
{
    fNParticles = 0;
    if (fParticles) fParticles->Clear();
}

//_____________________________________________________________________________
void SpdIsotropicGenerator::SetSeed(UInt_t seed)
{
    fGen->SetSeed(seed);
    fSeed = fGen->GetSeed();
    //cout << "-I- <SpdIsotropicGenerator::SetSeed> Seed: " << fSeed << endl;
}

//_____________________________________________________________________________
void SpdIsotropicGenerator::SetVGenOpt(Int_t opt)
{
    if (fVgenopt < 0) fVgenopt = 0;
    else if (fVgenopt > 2) fVgenopt = 2;
    else fVgenopt = opt;
    //cout << "-I- <SpdIsotropicGenerator::SetVGenOpt> Option: " << fVgenopt << endl;
}

//_____________________________________________________________________________
void SpdIsotropicGenerator::SetKeepEvent(Bool_t v) 
{ 
    if (v) {
        if (!fParticles) fParticles = new TClonesArray("TParticle");
        else fParticles->Delete();
    }
    else {
        if (fParticles) {
            fParticles->Delete();
            delete fParticles;
            fParticles = 0;
        }
    }
    
    fKeepEvent = v;  
}
    
//_____________________________________________________________________________   
void SpdIsotropicGenerator::SetPgid(Double_t dp, Int_t nstep)
{
    if (dp < DP_MIN) {
        fDP = 0;
        fNP = 1;
        return;
    }
    
    fDP = dp;
    (nstep < 1) ? fNP = 1 : fNP = nstep;
    
    fECounter = 0;
}

//_____________________________________________________________________________
void SpdIsotropicGenerator::SetSpherical(Double_t tmin, Double_t tmax, 
                                         Double_t pmin, Double_t pmax)
{
    if (tmin < 0)   tmin = 0;
    if (tmax > 180) tmax = 180;
    
    if (!(tmax > tmin)) {
        tmin = 0;
        tmax = 180;
        cout << "-W- <SpdIsotropicGenerator::SetSpherical> Bad settings. Set default: "
             << "Theta [min,max] = " << tmin << " " <<tmax << endl; 
    }
    
    fTmin = Cos(tmin*DegToRad());
    fTmax = Cos(tmax*DegToRad()); 

    if (pmin < 0)   pmin = 0;
    if (pmax > 360) pmax = 360;
    
    if (!(pmax > pmin)) {
        pmin = 0;
        pmax = 180;
        cout << "-W- <SpdIsotropicGenerator::SetSpherical> Bad settings. Set default: "
             << "Phi [min,max] = " << pmin << " " <<pmax << endl; 
    }
    
    fPhimin = DegToRad()*pmin; 
    fPhimax = DegToRad()*pmax;     
}

//_____________________________________________________________________________
void SpdIsotropicGenerator::SetSolid(Double_t tmin, Double_t tmax, 
                                     Double_t phi, Double_t theta)
{
    if (tmin < 0)   tmin = 0;
    if (tmax > 180) tmax = 180;
    
    if (!(tmax > tmin)) {
        tmin = 0;
        tmax = 180;
        cout << "-W- <SpdIsotropicGenerator::SetSolid> Bad settings. Set default: "
             << "Theta [min,max] = " << tmin << " " <<tmax << endl; 
    }
    
    fXTmin = Cos(tmin*DegToRad());
    fXTmax = Cos(tmax*DegToRad()); 

    if (phi < 0)   phi = 0;
    if (phi > 360) phi = 360;
    
    if (theta < 0)   theta = 0;
    if (theta > 180) theta = 180;
   
    fXPhi = DegToRad()*phi; 
    fXTheta = DegToRad()*theta;     
}

//_____________________________________________________________________________
void SpdIsotropicGenerator::SetSeeds(const std::vector<UInt_t>& seeds)
{
    if (seeds.size() < 1) return;
    SetSeed(seeds[0]);
} 

//_____________________________________________________________________________
void SpdIsotropicGenerator::GetSeeds(std::vector<UInt_t>& seeds) 
{ 
    seeds.clear(); 
    seeds.push_back(fSeed);
} 

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t SpdIsotropicGenerator::Init()
{
   if (fInit) return kTRUE;
     
   fInit = kFALSE;
   
   ResetEvent();
   
   if (!(fMomentum > 0)) return kFALSE;
  
   TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(fPdg);
   if (!part) {
        cout << "-E- <SpdIsotropicGenerator::Init> Unknown particle: " << fPdg << endl;
        return kFALSE;
   }
   
   if (fNPL < 1) fNPL = 1;
   
   fInit = kTRUE; 
   
   return kTRUE;
}

//_____________________________________________________________________________
void SpdIsotropicGenerator::Initialize(Int_t pdg, Double_t momentum, Int_t np)
{
    fInit = kFALSE; 
 
    ResetEvent();
    
    if (!(momentum > 0)) return;
  
    TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(pdg);
    if (!part) {
        cout << "-E- <SpdIsotropicGenerator::Initialize> Unknown particle: " << pdg << endl;
        return;
    }
       
    fPdg = pdg;
    fMomentum = momentum;
    
    (np < 1) ? fNPL = 1 : fNPL = np;
    
    fInit = kTRUE; 
}

//_____________________________________________________________________________________
Bool_t SpdIsotropicGenerator::ReadEvent(FairPrimaryGenerator* primGen) 
{
    fSeed = fGen->GetSeed();
  
    if (!fInit) {
        cout << "-E- <SpdIsotropicGenerator::ReadEvent> "
             << "Parameters were not initialized properly " << endl;
        return kFALSE;
    }
    
    if (fVgenopt == 0) {
        cout << "-E- <SpdIsotropicGenerator::ReadEvent> "
             << "Generator's option is not defined (option = "
             << fVgenopt << ") " << endl;
        return kFALSE;   
    }
    
    if (fVerboseLevel > -1) {
        cout << "-I- <SpdIsotropicGenerator::ReadEvent> Seed: " << fGen->GetSeed() << endl;
    }
    
    Bool_t ok = kFALSE;
    
    switch (fVgenopt) {
      
      case -1:  { ok = GenerateIsotropic_0(primGen); break; }
      case -2:  { ok = GenerateIsotropic_0(primGen); break; }
      case  1:  { ok = GenerateIsotropic_1(primGen); break; }
      case  2:  { ok = GenerateIsotropic_2(primGen); break; }
      
      default : break;
    }
    
    if (!primGen) return kFALSE;
    
    return ok;
}

//_____________________________________________________________________________________
Bool_t SpdIsotropicGenerator::GenerateIsotropic_1(FairPrimaryGenerator* primGen) 
{
    if (!fInit) {
        cout << "-E- <SpdIsotropicGenerator::GenerateIsotropic[1]> "
             << "Parameters are not initialized properly. " << endl;
        return kFALSE;
    }
    
    if (fVerboseLevel > 0) {
        cout << "---------------------------------------------------------------------" 
             << "---------------------------------------------------------------------" 
             << endl;
    }
    
    ResetEvent();
    
    // Generate momentum
    if (fECounter == fNP) fECounter = 0;  
    Double_t p = fMomentum + fECounter*fDP;
    fECounter++;
   
    // Generate direction
    Double_t phi, cost, sint, nx, ny, nz;
  
    for (int i(0); i<fNPL; i++) {
    
         phi  = fPhimax - fGen->Rndm()*(fPhimax - fPhimin);
         cost = fTmax - fGen->Rndm()*(fTmax - fTmin);
         sint = Sqrt(Abs(1-cost*cost));
            
         nx = sint*Cos(phi);
         ny = sint*Sin(phi);
         nz = cost;   
         
         AddParticle(primGen,p*nx,p*ny,p*nz);
    }  
    
    if (fVerboseLevel > -1) {
        cout << "-I- <SpdIsotropicGenerator::GenerateIsotropic[1]> "
             << " OPTION: " << fVgenopt 
             << "; SEED: " << fSeed << endl;
        cout << "-I- <SpdIsotropicGenerator::GenerateIsotropic[1]> "
             << " Primaries: " << GetNPrimaries()
             << endl; 
    }
    
    return kTRUE;
}

//_____________________________________________________________________________________
Bool_t SpdIsotropicGenerator::GenerateIsotropic_2(FairPrimaryGenerator* primGen) 
{
    if (!fInit) {
        cout << "-E- <SpdIsotropicGenerator::GenerateIsotropic[2]> "
             << "Parameters are not initialized properly. " << endl;
        return kFALSE;
    }
    
    if (fVerboseLevel > 0) {
        cout << "---------------------------------------------------------------------" 
             << "---------------------------------------------------------------------" 
             << endl;
    }
    
    ResetEvent();
    
    // Generate momentum
    if (fECounter == fNP) fECounter = 0;  
    Double_t p = fMomentum + fECounter*fDP;
    fECounter++;
   
    // Generate direction
    Double_t phi, cost, sint, nx, ny, nz;
    
    TVector3 N0(0,0,1), N1, NN;
    N1.SetMagThetaPhi(1,fXTheta,fXPhi);
    NN = N0.Cross(N1);
    
    for (int i(0); i<fNPL; i++) {
    
         phi  = TwoPi()*fGen->Rndm();
         cost = fXTmax - fGen->Rndm()*(fXTmax - fXTmin);
         sint = Sqrt(Abs(1-cost*cost));
            
         nx = sint*Cos(phi);
         ny = sint*Sin(phi);
         nz = cost;  
         
         N0.SetXYZ(nx,ny,nz);
         N0.Rotate(fXTheta,NN);
         
         nx = N0.X();
         ny = N0.Y();
         nz = N0.Z();
         
         AddParticle(primGen,p*nx,p*ny,p*nz);
    }  
   
    if (fVerboseLevel > -1) {
        cout << "-I- <SpdIsotropicGenerator::GenerateIsotropic[2]> "
             << " OPTION: " << fVgenopt 
             << "; SEED: " << fSeed << endl;
        cout << "-I- <SpdIsotropicGenerator::GenerateIsotropic[2]> "
             << " Primaries: " << GetNPrimaries()
             << endl;            
    }
   
    return kTRUE;
}

//_____________________________________________________________________________________
void SpdIsotropicGenerator::AddParticle(FairPrimaryGenerator* primGen, 
                            Double_t px, Double_t py, Double_t pz)
{ 
   Bool_t add = AddTrack(primGen, 1, fPdg , fNParticles, -1, px, py, pz); 
   
   if (!fKeepEvent && fVerboseLevel > 0) {
     
       TParticlePDG *part;
       const Char_t* ptype = "Undefined";
       Double_t te = -1;
   
       TDatabasePDG *db = TDatabasePDG::Instance(); 
       if (db) {
           part = db->GetParticle(fPdg);
           if (part) {
               ptype = part->ParticleClass();
               //Double_t mass = part->Mass();
               //te =  TMath::Sqrt(e*e + mass*mass);   
           }
       }
     
       Char_t c;
       (add) ? c = '+' : c= '-';
       
       printf("%-5d [%c] %8d %12d %10.6f %10.6f %10.6f %12.6f %12.6f %12.6f %12.6f %8d   %s\n",
                    fNParticles, c, 1/*status*/, fPdg, px, py, pz, 
                    0., 0., 0., 0., -1 , ptype); 
   }
 
   if (fKeepEvent && fParticles) {
     
       TParticle* particle = new ((*fParticles)[fNParticles]) TParticle();
      
       particle->SetPdgCode(fPdg);
       particle->SetFirstMother(-1);
       particle->SetStatusCode(1);
       particle->SetProductionVertex(0.,0.,0.,0.); 
       particle->SetMomentum(px, py, pz, -1);  
       
       //TDatabasePDG *db = TDatabasePDG::Instance();
       //TParticlePDG *part = db->GetParticle(fPdg);
       //Double_t mass = part->Mass();
       //Double_t te =  TMath::Sqrt(e*e + mass*mass);   
       //particle->SetMomentum(e*nx, e*ny, e*nz, te);
       
       if (fVerboseLevel > 0) PrintParticle(particle,fNParticles,add);
   }
   
   fNParticles++;
}

//_____________________________________________________________________________________
void SpdIsotropicGenerator::PrintParticle(TParticle* p, Int_t i, Bool_t add) 
{
   if (!p) return;
   
   TParticlePDG *part;
   const Char_t* ptype = "Undefined";
   
   TDatabasePDG *db = TDatabasePDG::Instance(); 
   if (db) {
       part = db->GetParticle(p->GetPdgCode());
       if (part) ptype = part->ParticleClass();
   }
   
   Char_t c;
   (add) ? c = '+' : c= '-';
       
   printf("%-5d [%c] %8d %12d %10.6f %10.6f %10.6f %12.6f %12.6f %12.6f %12.6f %8d   %s\n",
          i, c, p->GetStatusCode(), p->GetPdgCode(), p->Px(), p->Py(), p->Pz(), 
          p->Vx(), p->Vy(), p->Vz(), p->T(), p->GetFirstMother(), ptype); 
  
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

const static int INI_FACES[20][3] 
{
  {0,1,2},  {0,2,3},  {0,3,4},  {0,4,5},   {0,5,1},
  {1,2,6},  {2,3,7},  {3,4,8},  {4,5,9},   {5,1,10},
  {6,7,2},  {7,8,3},  {8,9,4},  {9,10,5},  {10,6,1},
  {11,6,7}, {11,7,8}, {11,8,9}, {11,9,10}, {11,10,6}
};
  
//_____________________________________________________________________________________
Bool_t SpdIsotropicGenerator::GenerateIsotropic_0(FairPrimaryGenerator* primGen)
{
    if (!fInit) {
        cout << "-E- <SpdIsotropicGenerator::GenerateIsotropic[1]> "
             << "Parameters are not initialized properly. " << endl;
        return kFALSE;
    }
    
    if (fVerboseLevel > 0) {
        cout << "---------------------------------------------------------------------" 
             << "---------------------------------------------------------------------" 
             << endl;
    }
    
    ResetEvent();
    
    fEdges.clear();
    fVertices.clear();
    
    // Generate momentum
    if (fECounter == fNP) fECounter = 0;  
    Double_t p = fMomentum + fECounter*fDP;
    fECounter++;
    
    Double_t px, py, pz;
   
    const Double_t a   = 26.56505*DegToRad();
    const Double_t a36 = 36.*DegToRad();
    const Double_t a72 = 72.*DegToRad();

    // --- Add primary (12) vertices   
    
    fVertices.push_back(TVector3(0,0,1));
    
    Double_t r = Cos(a);
    
    pz = Sin(a);
    for (Int_t i(0); i<5; i++) {
         px = r*cos(i*a72);
         py = r*sin(i*a72);
         fVertices.push_back(TVector3(px,py,pz));
    }
    
    pz = -pz; 
    for (Int_t i(0); i<5; i++) {
         px = r*cos(a36 + i*a72);
         py = r*sin(a36 + i*a72);
         fVertices.push_back(TVector3(px,py,pz));
    }
       
    fVertices.push_back(TVector3(0,0,-1));
       
    // --- Add secondary vertices
    
    Int_t Level = (fNPL > MAX_DENS_LEVEL) ? Level = MAX_DENS_LEVEL-1
                                          : Level = fNPL;
                                                 
    for (int i(0); i<20; i++) SplitFace(&INI_FACES[i][0],Level);
    
    fEdges.clear();
    
    // --- Add particles
    
    if (fVgenopt == -1) {
      
        Double_t costh, phi;
       
        for (Int_t i(0); i<fVertices.size(); i++) {
             
             costh = fVertices[i].CosTheta();      
             if (costh < fTmax || costh > fTmin) continue;
             
             phi = fVertices[i].Phi();
             if (phi < 0) phi = TwoPi()+phi;
                
             if (phi < fPhimin || phi > fPhimax) continue;
                         
             px = p*fVertices[i].Px();
             py = p*fVertices[i].Py();
             pz = p*fVertices[i].Pz();
             
             AddParticle(primGen,px,py,pz);
        }  
    }
    
    else if (fVgenopt == -2) {
      
         Double_t costh;
         
         TVector3 N0(0,0,1), N1, NN;
         N1.SetMagThetaPhi(1,fXTheta,fXPhi);
         NN = N0.Cross(N1);       
      
         for (Int_t i(0); i<fVertices.size(); i++) {

              costh = fVertices[i].CosTheta();      
              if (costh < fXTmax || costh > fXTmin) continue;
     
              fVertices[i].Rotate(fXTheta,NN);
                
              px = p*fVertices[i].Px();
              py = p*fVertices[i].Py();
              pz = p*fVertices[i].Pz();
              
              AddParticle(primGen,px,py,pz);
         }
    }
    
    fVertices.clear();
    
    if (fVerboseLevel > -1) {
        cout << "-I- <SpdIsotropicGenerator::GenerateIsotropic[0]> "
             << " OPTION: " << fVgenopt << endl;
        cout << "-I- <SpdIsotropicGenerator::GenerateIsotropic[0]> "
             << " Primaries: " << GetNPrimaries()
             << endl;
    }
                
    return kTRUE;
}

//_____________________________________________________________________________________
void SpdIsotropicGenerator::SplitFace(const Int_t face[], Int_t level)
{
    //cout << "<SpdIsotropicGenerator::SplitFace> " << " Level: " << level 
    //     << "; [ " << face[0] << " " << face[1] << " " << face[2] << " ] " 
    //     << endl;
    
    if (level == 1) return;
    
    Int_t p0, p1, p2;
    
    MakeBisection(p0,face[0],face[1]);
    MakeBisection(p1,face[1],face[2]);
    MakeBisection(p2,face[2],face[0]);
    
    Int_t newface[3]; 
    
    newface[0] = face[0]; 
    newface[1] = p0;      
    newface[2] = p2;
    SplitFace(newface,level-1);
      
    newface[0] = p0;  
    newface[1] = face[1];  
    newface[2] = p1;
    SplitFace(newface,level-1);
    
    newface[0] = p2;  
    newface[1] = p1;     
    newface[2] = face[2];
    SplitFace(newface,level-1);
     
    newface[0] = p0;  
    newface[1] = p1; 
    newface[2] = p2;
    SplitFace(newface,level-1);
}

//_____________________________________________________________________________________
void SpdIsotropicGenerator::MakeBisection(Int_t& x, const Int_t& i1, const Int_t& i2)
{
    static std::map<std::pair<Int_t,Int_t>, Int_t>::iterator it;
    static pair<Int_t,Int_t> pp;
    static TVector3 v;
   
    pp.first  = i1 + i2;
    pp.second = TMath::Abs(i1-i2);
    
    it = fEdges.find(pp);
    
    if (it != fEdges.end()) {
        x = it->second;
        return;
    }
     
    x = fVertices.size();
    v = fVertices[i1]+fVertices[i2];
    v.SetMag(1);
    fVertices.push_back(v);
    fEdges.insert(pair<pair<Int_t,Int_t>,Int_t>(pp,x));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
Bool_t SpdIsotropicGenerator::FillParsIn(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::FillParsIn(params)) return kFALSE;
   
   params->SetParameter("PDG",fPdg,fGenIndex);
   params->SetParameter("Momentum",fMomentum,fGenIndex);
   params->SetParameter("NPL",fNPL,fGenIndex);
    
   params->SetParameter("VgOption",fVgenopt,fGenIndex);
   params->SetParameter("KeepEvent",fKeepEvent,fGenIndex);
   
   params->SetParameter("MomentumStep",fDP,fGenIndex);
   params->SetParameter("MomentumNSteps",fNP,fGenIndex);
   
   if (Abs(fVgenopt) == 1) {
       params->SetParameter("Theta_min",ACos(fTmin)*RadToDeg(),fGenIndex); 
       params->SetParameter("Theta_max",ACos(fTmax)*RadToDeg(),fGenIndex);  
       params->SetParameter("Phi_min",fPhimin*RadToDeg(),fGenIndex);   
       params->SetParameter("Phi_max",fPhimax*RadToDeg(),fGenIndex);   
   }
   else if (Abs(fVgenopt) == 2) {
       params->SetParameter("Theta_min_solid",ACos(fXTmin)*RadToDeg(),fGenIndex); 
       params->SetParameter("Theta_max_solid",ACos(fXTmax)*RadToDeg(),fGenIndex);  
       params->SetParameter("Phi_solid",fXPhi*RadToDeg(),fGenIndex);   
       params->SetParameter("Theta_solid",fXTheta*RadToDeg(),fGenIndex);   
   }
       
   return kTRUE;
}
 
//_____________________________________________________________________________
Bool_t SpdIsotropicGenerator::LoadParsFrom(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::LoadParsFrom(params)) return kFALSE;
  
   Reset();
   
   fInit = kFALSE;
   
   Bool_t is_ok = kTRUE;
 
   if (!params->GetParameter("PDG",fPdg,fGenIndex)) is_ok = kFALSE;
   if (!params->GetParameter("Momentum",fMomentum,fGenIndex)) is_ok = kFALSE;
   if (!params->GetParameter("NPL",fNPL,fGenIndex)) is_ok = kFALSE;
 
   Int_t vgenopt;
   if (params->GetParameter("VgOption",vgenopt,fGenIndex)) {
       
       SetVGenOpt(vgenopt);
       
       Double_t v[4];
       
       if (Abs(fVgenopt) == 1) {
         
           params->GetParameter("Theta_min",v[0],fGenIndex); 
           params->GetParameter("Theta_max",v[1],fGenIndex);  
           params->GetParameter("Phi_min",v[2],fGenIndex);   
           params->GetParameter("Phi_max",v[3],fGenIndex);   
           
           SetSpherical(v[0],v[1],v[2],v[3]);
           
       }
       else if (Abs(fVgenopt) == 2) {
         
           params->GetParameter("Theta_min_solid",v[0],fGenIndex); 
           params->GetParameter("Theta_max_solid",v[1],fGenIndex);  
           params->GetParameter("Phi_solid",v[2],fGenIndex);   
           params->GetParameter("Theta_solid",v[3],fGenIndex);   
           
           SetSolid(v[0],v[1],v[2],v[3]);
       }
   }
   
   Bool_t keepevent;
   if (params->GetParameter("KeepEvent",keepevent,fGenIndex)) SetKeepEvent(keepevent);
 
   Double_t dp(1);
   Int_t np(0);
   if (params->GetParameter("MomentumStep",dp,fGenIndex) &&
       params->GetParameter("MomentumNSteps",np,fGenIndex)) 
   {     
       SetPgid(dp,np);
   }
  
   return is_ok;
}

//_____________________________________________________________________________
void SpdIsotropicGenerator::Print(Option_t* opt) const
{
   cout << "\t-I- <SpdIsotropicGenerator::Print>\n" << endl;
   
   cout << "\tIs init:          " << fInit << endl;
   cout << "\tIndex:            " << fGenIndex << endl;
   cout << "\tVerbose level:    " << fVerboseLevel << endl;
   cout << "\tVgOption:         " << fVgenopt << endl;
   cout << "\tKeepEvent:        " << fKeepEvent << endl;
   cout << "\tSeed:             " << fSeed << endl;
    
   cout << endl;
    
   cout << "\tPDG:              " << fPdg << endl;
   cout << "\tMomentum:         " << fMomentum << endl;
   
   if (fVgenopt > 0) cout << "\t(NPL) Particles:  " << fNPL << endl;
   else cout << "\t(NPL) Level:      " << fNPL << endl;
   
   cout << "\tMomentumStep:       " << fDP << endl;
   cout << "\tMomentumNSteps:     " << fNP << endl;
   
   cout << endl;
   
   if (Abs(fVgenopt) == 1) {
       cout << "\tTheta (min,max), deg: (" << ACos(fTmin)*RadToDeg() << ", " << ACos(fTmax)*RadToDeg() << ") " << endl; 
       cout << "\tPhi (min,max), deg:   (" << fPhimin*RadToDeg() << ", " << fPhimax*RadToDeg() << ") " << endl; 
   }
   
   if (Abs(fVgenopt) == 2) {
       cout << "\tTheta (min,max), deg: (" << ACos(fXTmin)*RadToDeg() << ", " << ACos(fXTmax)*RadToDeg() << ") " << endl; 
       cout << "\tPhi, Theta  (deg):    (" << fXPhi*RadToDeg() << ", " << fXTheta*RadToDeg() << ") " << endl; 
   }    
   
   cout << endl;
}


