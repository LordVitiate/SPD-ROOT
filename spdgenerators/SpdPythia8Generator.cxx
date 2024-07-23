// $Id$
// Author: artur   2021/10/23

#include <TString.h>
#include <TParticle.h>
#include <TDatabasePDG.h>

#include "FairPrimaryGenerator.h"

#include "SpdParticlesAndProcesses.h"
#include "SpdPythia8Generator.h"

#include <iostream>

using std::cout;
using std::endl;

#define PYTHIA8_RANMAX  900000000

ClassImp(SpdPythia8Generator)

//_____________________________________________________________________________________
SpdPythia8Generator::SpdPythia8Generator():SpdGenerator("SPD Pythia8 Generator"),
fInit(false),fMaxDecayD(1e-10),
fSeed(0),fUseExtGen(false),
fBeamA(0),fBeamB(0),fEnergy(-1.)
{
   //cout << "-I- <SpdPythia8Generator::SpdPythia8Generator>" << endl;
   
   fPythia = new SpdPythia8();

   SpdParticlesAndProcesses::AddParticlesToDatabasePDG();
}

//_____________________________________________________________________________________
SpdPythia8Generator::SpdPythia8Generator(const Char_t* Name):SpdGenerator(Name),
fInit(false),fMaxDecayD(1e-10),
fSeed(0),fUseExtGen(false),
fBeamA(0),fBeamB(0),fEnergy(-1.)
{
   //cout << "-I- <SpdPythia8Generator::SpdPythia8Generator>" << endl;
   
   fPythia = new SpdPythia8();

   SpdParticlesAndProcesses::AddParticlesToDatabasePDG();
}

//_____________________________________________________________________________________
SpdPythia8Generator::~SpdPythia8Generator()
{
   if (fPythia) {
       delete fPythia;
       fPythia = 0;
   }

   if (event_filter) {
       delete event_filter;
   }
}

//_____________________________________________________________________________
SpdPythia8* SpdPythia8Generator::GetGenerator() 
{ 
    fInit = false;
    if (fPythia) return fPythia;
    fPythia = new SpdPythia8();
    return fPythia;  
}

//_____________________________________________________________________________
void SpdPythia8Generator::SetSeeds(const std::vector<UInt_t>& seeds)
{
    if (seeds.size() < 2) return;
    SetSeed(seeds[0],seeds[1]);
} 

//_____________________________________________________________________________
void SpdPythia8Generator::GetSeeds(std::vector<UInt_t>& seeds) 
{ 
    seeds.clear(); 
    seeds.push_back(fSeed);
    seeds.push_back(UInt_t(fUseExtGen));
} 

//_____________________________________________________________________________________
void SpdPythia8Generator::SetParameters(const Char_t* par)
{
    if (!fPythia) return;
    fPythia->SetParameter(par);
    fInit = false;
    
    //cout << "-I- <SpdPythia8Generator::SetParameters> (\"" <<par << "\")" << endl;
}

//_____________________________________________________________________________________
void SpdPythia8Generator::SetParameter(const Char_t* par)
{
    if (!fPythia) return;
    fPythia->SetParameter(par);
    fInit = false;
    
    //cout << "-I- <SpdPythia8Generator::SetParameter> (\"" <<par << "\")" << endl;
}

//_____________________________________________________________________________
void SpdPythia8Generator::SetExternalGenerator(Bool_t UseExternalGenerator)
{
    fUseExtGen = UseExternalGenerator;
    fInit = false;  
}

//_____________________________________________________________________________
void SpdPythia8Generator::SetGenSeed(UInt_t seed) 
{
    if (seed > 0 && seed < PYTHIA8_RANMAX) fSeed = seed;
    else {   
        TRandom3* gen = new TRandom3();
        gen->SetSeed(0);   
        fSeed = gen->Integer(PYTHIA8_RANMAX);
        delete gen;
    }
    
    fInit = false;

    cout << "-I- <SpdPythia8Generator::SetSeed> Seed (ini/actual):  "
         << seed << "/" << fSeed << endl;
}

//_____________________________________________________________________________
void SpdPythia8Generator::SetSeed(UInt_t seed, Bool_t UseExternalGenerator) 
{
    if (seed > 0 && seed < PYTHIA8_RANMAX) fSeed = seed;
    else {   
        TRandom3* gen = new TRandom3();
        gen->SetSeed(0);   
        fSeed = gen->Integer(PYTHIA8_RANMAX);
        delete gen;
    }
    
    fUseExtGen = UseExternalGenerator;
    
    fInit = false;

    cout << "-I- <SpdPythia8Generator::SetSeed> Seed (ini/actual):  "
         << seed << "/" << fSeed << endl;
}

//_____________________________________________________________________________________
void SpdPythia8Generator::SetBeam(Int_t beamA, Int_t beamB, Double_t Ecms)
{
    if (!fPythia) return;
    
    fBeamA  = beamA;
    fBeamB  = beamB;
    fEnergy = Ecms;
    
    fPythia->SetParameter(Form("Beams:idA = %d",fBeamA));
    fPythia->SetParameter(Form("Beams:idB = %d",fBeamB));
    fPythia->SetParameter(Form("Beams:eCM = %e",fEnergy));
    
    fPythia->SetParameter(Form("Beams:frameType = %d",1));
    
    fInit = false;
    
    cout << "-I- <SpdPythia8Generator::SetBeam> Beam [pdg(A),pdg(B)]:  "
         << fBeamA << ", " << fBeamB << "; E(cms) = " << fEnergy << endl;
}

//_____________________________________________________________________________________
Bool_t SpdPythia8Generator::Init()
{
    if (fInit) return true;
    
    cout << "-I- <SpdPythia8Generator::Init> " << endl;
    
    if (!fPythia) fPythia = new SpdPythia8();

    if ((fSeed < 1) || (fSeed >= PYTHIA8_RANMAX)) 
    {
        // WTF??? Just throw exception here!
        TRandom3* gen = new TRandom3();
        gen->SetSeed(0);   
        fSeed = gen->Integer(PYTHIA8_RANMAX);
        delete gen;
    }
   
    fPythia->SetSeed(fSeed,fUseExtGen);
   
    if (fMaxDecayD > 0) {
        fPythia->SetParameter("ParticleDecays:limitRadius = on"); 
        fPythia->SetParameter(Form("ParticleDecays:rMax = %f",10*fMaxDecayD)); // cm -> mm (default = 0.1 cm)
    }
    
    //fPythia->SetParameter("SoftQCD:elastic = on"); 
        
    fInit = fPythia->Init();
   
    if (fInit) cout << "-I- <SpdPythia8Generator::Init> Intialization successfull " << endl;
    else cout << "-E- <SpdPythia8Generator::Init> Intialization not successfull " << endl;
    
    return fInit;
}

//_____________________________________________________________________________________
Bool_t SpdPythia8Generator::ReadEvent(FairPrimaryGenerator* primGen)
{
    if (!fInit || !fPythia) {
        cout << "-E- <SpdPythia8Generator::ReadEvent> "
             << "Parameters were not initialized properly " << endl;
        return false;
    }
    
    if (!event_filter) {
        do {
            fPythia->GenerateEvent();
        } while (!fPythia->GetNParticles());
    } else {
        if (!event_filter->FilterEvent(fPythia)) {
            cout << "-E- <SpdPythia8Generator::ReadEvent> EventFilter::Filter event failed" << endl;
            return (primGen) ? true : false;
        }
    }
        
    TClonesArray* parts = (TClonesArray*)fPythia->GetListOfParticles();
    TParticle* part;
    
    Int_t np = parts->GetEntriesFast();
    
    Int_t pdg, parentId;
    Double_t vx, vy, vz, px, py, pz;
    Bool_t addtostack;
    Double_t vt(0.);
    
    TParticlePDG* pdgpart;
    TString ptype, partname;
    
    for (Int_t i(0); i<np; i++) 
    {
        part = (TParticle*)parts->At(i);
        
        addtostack = part->GetStatusCode(); 
        
        pdg = part->GetPdgCode();
        parentId = part->GetFirstMother();
        
        vt = part->T();
        vx = part->Vx();
        vy = part->Vy(); 
        vz = part->Vz();
        
        px = part->Px(); 
        py = part->Py();   
        pz = part->Pz();    

        ptype = "undefined";
        partname = "undefined";
           
        if (pdg > 1e10) {
            ptype = "Nuclei";
            partname = "Nuclei";
        }
        else {
            pdgpart = TDatabasePDG::Instance()->GetParticle(pdg);
            if (pdgpart) {
                ptype = pdgpart->ParticleClass();
                partname = pdgpart->GetName();
            }
        }
        
        AddTrack(primGen, addtostack, pdg, i, parentId, px, py, pz, vx, vy, vz, true, -9.0e9, vt);
        
        if (fVerboseLevel > 0) {
            if (addtostack) {
                printf("%-5d [+] %8d %12d %10.6f %10.6f %10.6f %14.8f %14.8f %14.8f %12.6e %8d   %s %s\n",
                       i, part->GetStatusCode(), pdg, px, py, pz, 
                       vx, vy, vz, vt, parentId, ptype.Data(), partname.Data()); 
            }
            else {
                printf("%-5d  -  %8d %12d %10.6f %10.6f %10.6f %14.8f %14.8f %14.8f %12.6e %8d   %s %s\n",
                       i, part->GetStatusCode(), pdg, px, py, pz, 
                       vx, vy, vz, vt, parentId, ptype.Data(), partname.Data());
            }
        }
    }
    
    return (primGen) ? true : false;
}

//_____________________________________________________________________________
Bool_t SpdPythia8Generator::FillParsIn(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::FillParsIn(params)) return false;
   
   params->SetParameter("MaxDecayD",fMaxDecayD,fGenIndex);
   
   params->SetParameter("BeamA",fBeamA,fGenIndex);
   params->SetParameter("BeamB",fBeamB,fGenIndex);
   params->SetParameter("Energy",fEnergy,fGenIndex);
   
   if (fPythia) {
       fPythia->SetStorageIndex(fGenIndex);
       fPythia->FillParsIn(params);
   }
       
   return true;
}
 
//_____________________________________________________________________________
Bool_t SpdPythia8Generator::LoadParsFrom(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::LoadParsFrom(params)) return false;
   
   fInit = false;
   
   Bool_t is_ok = kTRUE;
   
   if (!params->GetParameter("MaxDecayD",fMaxDecayD,fGenIndex)) is_ok = false;
   
   if (!params->GetParameter("BeamA",fBeamA,fGenIndex)) is_ok = false;
   if (!params->GetParameter("BeamB",fBeamB,fGenIndex)) is_ok = false;
   if (!params->GetParameter("Energy",fEnergy,fGenIndex)) is_ok = false;
   
   return is_ok;
}

//_____________________________________________________________________________________
void SpdPythia8Generator::Print(Option_t* opt) const
{
   cout << "\t-I- <SpdPythia8Generator::Print>\n" << endl;
   
   cout << "\tIs init:               " << fInit << endl;
   cout << "\tIndex:                 " << fGenIndex << endl;
   cout << "\tDecay region (size):   " << fMaxDecayD*1e10 << " [pm] " << endl;
   cout << "\tSeed:                  " << fSeed << " (using an external generator: " << fUseExtGen << ") " << endl;
   
   cout << endl;
   
   cout << "\tBeam(A):               " << fBeamA  << endl;      
   cout << "\tBeam(B):               " << fBeamB  << endl; 
   cout << "\tEnergy (CM):           " << fEnergy << " GeV " << endl; 
   
   cout << endl;
   
   if (fPythia && !strcmp(opt,"all")) fPythia->ListAll();
}

bool SpdPythia8Generator::P8EventFilter::CheckEvent(SpdPythia8 *pythia) {
    return true;
}

bool SpdPythia8Generator::P8EventFilter::FilterEvent(SpdPythia8 *pythia) {

    do {
        pythia->GenerateEvent();
    } while (!CheckEvent(pythia));

    return true;
}