// $Id$
// Author: artur   2018/12/25


//_____________________________________________________________________________
//
// SpdFtfGenerator
//_____________________________________________________________________________

#include "SpdFtfGenerator.h"
#include "SpdPrimGenParSet.h"

#include <TRandom.h>
#include <TString.h>
#include <TSystem.h>

#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TParticle.h>
#include <TDatabasePDG.h>

#include <iostream>

using std::cout;
using std::endl;

#define FTF_MAX_RANDOM 2147483647

ClassImp(SpdFtfGenerator)

//_____________________________________________________________________________
SpdFtfGenerator::SpdFtfGenerator():
SpdGenerator("SPD FTF primary generator"),
fMode(-10),fInitEvents(-1),fInitEpar(-1.0),fConfig(""),fFilePath(""),
fSeed(0),fSeedCycle(0),
fGenPath(""),fInit(kFALSE),fInitSeeds(kFALSE),
fInputFile(0),fInputTree(0),fParticles(0),fNEvents(0),
fRemoveDataFile(kTRUE)
{
}

//_____________________________________________________________________________
SpdFtfGenerator::SpdFtfGenerator(const char* Name):
SpdGenerator(Name),
fMode(0),fInitEvents(-1),fInitEpar(-1.0),fConfig(""),fFilePath(""),
fSeed(0),fSeedCycle(0),
fGenPath(""),fInit(kFALSE),fInitSeeds(kFALSE),
fInputFile(0),fInputTree(0),fParticles(0),fNEvents(0),
fRemoveDataFile(kTRUE)
{ 
}

//_____________________________________________________________________________
SpdFtfGenerator::SpdFtfGenerator(Int_t mode):
SpdGenerator("SPD FTF primary generator"),
fMode(0),fInitEvents(-1),fInitEpar(-1.0),fConfig(""),fFilePath(""),
fSeed(0),fSeedCycle(0),
fGenPath(""),fInit(kFALSE),fInitSeeds(kFALSE),
fInputFile(0),fInputTree(0),fParticles(0),fNEvents(0),
fRemoveDataFile(kTRUE)
{
   Build(mode);    
}

//_____________________________________________________________________________
SpdFtfGenerator::SpdFtfGenerator(const char* Name, Int_t mode):
SpdGenerator(Name),
fMode(0),fInitEvents(-1),fInitEpar(-1.0),fConfig(""),fFilePath(""),
fSeed(0),fSeedCycle(0),
fGenPath(""),fInit(kFALSE),fInitSeeds(kFALSE),
fInputFile(0),fInputTree(0),fParticles(0),fNEvents(0),
fRemoveDataFile(kTRUE)
{
   Build(mode);  
}

//_____________________________________________________________________________
SpdFtfGenerator::~SpdFtfGenerator() 
{
   CloseInput();
   RemoveDataFile();
}

//_____________________________________________________________________________
void SpdFtfGenerator::RemoveDataFile()
{
   if (!fRemoveDataFile) return;
    
   if (fFilePath.IsWhitespace()) return;
   
   if (!gSystem->AccessPathName(fFilePath)) {
       gSystem->Exec(Form("rm %s",fFilePath.Data())); 
   }
}

//_____________________________________________________________________________
void SpdFtfGenerator::Build(Int_t mode) { // -> protected
    
   if (mode < -1 || mode > 1) mode = 0;
 
   if (mode < 0)  {
       fMode = mode;
       fRemoveDataFile = kFALSE;
       return;
   }

   /* ===== Build FTF generator ===== */
   
   TString pwd = gSystem->pwd();
   //cout << "-I- <SpdFtfGenerator::SpdFtfGenerator> Current path: " << pwd << endl;
   
   fGenPath = gSystem->Getenv("VMCWORKDIR");
   fGenPath += "/spdgenerators/FtfEvtGen/";
   
   if (!gSystem->cd(fGenPath)) {
       cout << "-F- <SpdFtfGenerator::SpdFtfGenerator> No FTF directory: " << fGenPath << endl;
       exit(1);
   }
     
   Int_t is_ok = kTRUE;
 
   TString s = fGenPath + "FTFGen";
   if (gSystem->AccessPathName(s)) {
       is_ok = !gSystem->Exec("make -f FTFmake");
   }
   else {
       if (mode == 1) {
           gSystem->Exec("rm -r tmp/ lib/ FTFGen"); 
           is_ok = !gSystem->Exec("make -f FTFmake");
       }
   }
  
   if (is_ok) {
       if (fVerboseLevel > -1) {
           cout << "-I- <SpdFtfGenerator::SpdFtfGenerator> Create FTF generator: ... ok " << endl;
       }
   }
   else {
       cout << "-F- <SpdFtfGenerator::SpdFtfGenerator> Create FTF generator: ... fail " << endl;
       exit(1);
   }
 
   fMode = mode;
   fRemoveDataFile = kTRUE;
      
   gSystem->cd(pwd);
}

//_____________________________________________________________________________
void SpdFtfGenerator::Reset() 
{
   fMode = 0;
   
   fInitEvents = -1;
   fInitEpar = -1.0;
   fConfig = "";
   fFilePath = "";
   
   fSeed = 0;
   fSeedCycle = 0;
   
   fGenPath = "";
   
   fInit = kFALSE;
   fInitSeeds = kFALSE;
   
   fNEvents = 0;
   
   if (fInputFile) CloseInput();
}

//_____________________________________________________________________________
void SpdFtfGenerator::SetSeed(UInt_t seed, UInt_t ncycle) 
{
   fGen->SetSeed(0); 
    
   if (seed >= FTF_MAX_RANDOM) {
       cout << "-W- <SpdFtfGenerator::SetSeed> Large seed number value:  "
            << seed << ", initialization will be performed random" << endl;
       seed = 0; 
   }
   
   if (seed == 0) {   
       fSeed = fGen->Integer(FTF_MAX_RANDOM);
       fSeedCycle = 0;
   }
   else {
       fSeed = seed;
       (ncycle == 0) ? fSeedCycle = 0 : fSeedCycle = ncycle-1;
   }
   
   if (fMode < 0) fSeed = 0;
 
   fInitSeeds = kFALSE;    
   
   cout << "-I- <SpdFtfGenerator::SetSeed> Seed (ini/actual) "
        << "[ seed: " << seed << " cycle: " << ncycle
        << " ] -> [ seed: " << fSeed << " cycle: " << fSeedCycle << " ]" << endl;
}

//_____________________________________________________________________________
void SpdFtfGenerator::SetConfig(TString config) 
{
   fInit = kFALSE;  
    
   if (config.IsWhitespace()) fConfig = "PP.mac";
   else fConfig = config;
}

//_____________________________________________________________________________
void SpdFtfGenerator::SetFilePath(TString fpath) 
{
   fInit = kFALSE;  
   
   if (!fpath.IsWhitespace()) {
       fFilePath = "";
       if (!fpath.EndsWith(".root")) fpath += ".root";
       fFilePath = fpath;
       fInit = kTRUE;
       return;
   }
   
   fFilePath = gSystem->Getenv("VMCWORKDIR");
   
   if (!fFilePath.IsWhitespace()) {
       if (fMode < 0) fFilePath += "/input/FTF.root";
       else fFilePath += "/input/FTF_tmp.root";
   }
   else {
       if (fMode < 0) fFilePath += "FTF.root";
       else fFilePath += "FTF_tmp.root";    
   }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//_____________________________________________________________________________
void SpdFtfGenerator::InitSeeds() //-> protected
{   
    // cout << "-I- <SpdFtfGenerator::InitSeeds> " << endl;
    
    if (fSeed == 0 && !(fMode < 0) ) {
        fGen->SetSeed(0);   
        fSeed = fGen->Integer(FTF_MAX_RANDOM);
        fSeedCycle = 0;
    }

    fInitSeeds = kTRUE;  
}

//_____________________________________________________________________________
Bool_t SpdFtfGenerator::Init()
{
   if (fInit) return kTRUE;
   
   Initialize(fInitEvents,fInitEpar);
   
   return fInit;
}

//_____________________________________________________________________________
void SpdFtfGenerator::Initialize(Int_t nevents, Double_t epar)
{
   fInit = kFALSE;
 
   if (fMode == -10) {
       cout << "-E- <SpdFtfGenerator::Initialize> Unknown generator mode = " 
            << fMode << endl;
       return;
   }
       
   InitSeeds();
   
   fInitEpar = -1.;
   
   (nevents > 0) ? fInitEvents = nevents :  fInitEvents = -1;
     
   if (fMode < 0) {
       if (!OpenInput()) {
           cout << "-E- <SpdFtfGenerator::Initialize> No input data file for mode = " 
                << fMode << endl;  
           return;
       }
       if (fSeedCycle >= fInputTree->GetEntries()) {
           cout << "-E- <SpdFtfGenerator::Initialize> Too much events will need to skip: " 
                << fSeedCycle << " (avaliable: " << fInputTree->GetEntries() << ")" << endl;  
           return;
       }
       
       Int_t nte = fInitEvents + fSeedCycle;
    
       if (nte > fInputTree->GetEntries()) {
           cout << "-E- <SpdFtfGenerator::Initialize> No such number of events in the file: " 
                << nte << " (avaliable: " << fInputTree->GetEntries() << ")" << endl;  
           return;
       }
       if (fVerboseLevel > -1) {
           cout << "-I- <SpdFtfGenerator::Initialize> Generator has been initialized for mode = " 
                << fMode << " with number of events = " << fInitEvents << endl;
       }
             
       fInit = kTRUE;
       return;
   }
   
   if (epar > 0) fInitEpar = epar;

   if (fConfig.IsWhitespace()) SetConfig();
   if (fFilePath.IsWhitespace()) SetFilePath();
   
   TString pwd = gSystem->pwd();
   
   if (!gSystem->cd(fGenPath)) {
       cout << "-E- <SpdFtfGenerator::Initialize> Bad path to the generator directory: " 
            << fGenPath << endl; 
       gSystem->cd(pwd);
       return;
   }
   
   TString ftf_command = Form("./FTFGen %s %ld %d %f %d %s",
                              fConfig.Data(), Long_t(fSeed), fInitEvents, fInitEpar, 
                              fSeedCycle, fFilePath.Data());
 
   gSystem->Exec(ftf_command.Data());
 
   gSystem->cd(pwd);
    
   if (!OpenInput()) {
       cout << "-E- <SpdFtfGenerator::Initialize> No input data file for mode = " 
            << fMode << endl;  
       return;
   }
   
   fInit = kTRUE;
   
   if (fVerboseLevel > -1) {
       cout << "-I- <SpdFtfGenerator::Initialize> Generator has been initialized in mode " 
            << fMode << " ... ok "<< endl;
   }
}

//_____________________________________________________________________________
Bool_t SpdFtfGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
   if (!fInit) {
       cout << "-E- <SpdFtfGenerator::ReadEvent> "
            << "Parameters were not initialized properly " << endl;
       return kFALSE;
   }
   
   if (!fInitSeeds) {
       cout << "-E- <SpdFftGenerator::ReadEvent> "
            << "Seeds were not initialized properly " << endl;
       return kFALSE;
   }
   
   if (!fInputFile) {
       if (!OpenInput()) {
           cout << "-E- <SpdFftGenerator::ReadEvent> "
                << "Data were not initialized properly " << endl;
           return kFALSE;
       }
   }
   
   if (fNEvents > fInputTree->GetEntries()) {
       cout << "-I- <SpdFtfGenerator::ReadEvent> No more events in the file!" << endl;
       CloseInput();
       return kFALSE;
   }
 
   TFile* g = gFile;
   
   fInputFile->cd();
 
   // skip events for mode < 0
   if (fNEvents == 0 && fMode < 0 && fSeedCycle > 0) {
       for (Int_t i(0); i<fSeedCycle; i++) {
            fInputTree->GetEntry(fNEvents++);
       }
   }
   
   fInputTree->GetEntry(fNEvents++);
   
   if (g) g->cd();
   
   fSeedCycle++;
   
   Int_t np = fParticles->GetEntries();
   
   if (np < 1) {   
       cout << "-E- <SpdFtfGenerator::ReadEvent> No particles in the event " << fNEvents-1 << endl;
       return kFALSE;
   }
   
   // Loop over particles in TClonesArray
   
   TParticle* part;
   Int_t pdg, n(0);
   
   Bool_t addTrack = kTRUE;
   
   Double_t px, py, pz;
   Double_t vx, vy, vz;
     
   for (Int_t i(0); i < np; i++) {
     
       part = (TParticle*)fParticles->At(i);
       if (!part) continue;
         
       pdg = part->GetPdgCode();

       // Check if particle type is known
       
       if (!TDatabasePDG::Instance()->GetParticle(pdg)) {
           cout << "-W <SpdFtfGenerator::ReadEvent> Unknown particle type " 
                << pdg << ", skip it." << endl;
           continue;
       }

       px = part->Px();
       py = part->Py();
       pz = part->Pz();

       vx = part->Vx();
       vy = part->Vy();
       vz = part->Vz();
 
       AddTrack(primGen, addTrack, pdg, n, -1, px, py, pz, vx, vy, vz);
       n++;
  }  
   
  if (!primGen) return kFALSE;
   
  return kTRUE; 
}

//_____________________________________________________________________________
Bool_t SpdFtfGenerator::OpenInput()
{
   fNEvents = 0;
   
   if (fInputFile) CloseInput();
     
   if (fFilePath.IsWhitespace()) {
       cout << "-E- <SpdFtfGenerator::OpenInput> File path is undefined " << endl;
       return kFALSE;
   }
   
   fInputFile = new TFile(fFilePath);
   if (!fInputFile) {
       cout << "-E- <SpdFtfGenerator::OpenInput> No file: " << fFilePath << endl;
       return kFALSE;
   }
     
   fInputTree = (TTree*)fInputFile->Get("data");
   if (!fInputTree) {
       cout << "-E- <SpdFtfGenerator::OpenInput> No tree: " << "data" << endl;
       return kFALSE;
   }
   
   TBranch* branch = fInputTree->GetBranch("Particles");
   if (!branch) {
       cout << "-E- <SpdFtfGenerator::OpenInput> No branch: " << "Particles" << endl;
       return kFALSE;
   }
    
   branch->SetAddress(&fParticles);
   return kTRUE;
}

//_____________________________________________________________________________ 
void SpdFtfGenerator::CloseInput()
{
   fNEvents = 0;
  
   if (!fInputFile) return;
    
   fInputFile->Close();
   delete fInputFile; 
   fInputFile = 0; 
   
   fInputTree = 0;
   fParticles = 0;
}

//_____________________________________________________________________________
void SpdFtfGenerator::SetSeeds(const std::vector<UInt_t>& seeds)
{
   if (seeds.size() < 2) return;
   SetSeed(seeds[0],seeds[1]);
}

//_____________________________________________________________________________
void SpdFtfGenerator::GetSeeds(std::vector<UInt_t>& seeds) 
{  
   seeds.clear(); 
   seeds.push_back(fSeed);
   seeds.push_back(fSeedCycle);
}  

//_____________________________________________________________________________
Bool_t SpdFtfGenerator::FillParsIn(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::FillParsIn(params)) return kFALSE;
   
   params->SetParameter("Mode",fMode,fGenIndex);
   params->SetParameter("Events(init)",fInitEvents,fGenIndex);
  
   if (fMode < 0) {
       params->SetParameter("File(input)",fFilePath,fGenIndex);
   }
   else {
       params->SetParameter("Epar(init)",fInitEpar,fGenIndex);
       params->SetParameter("Config",fConfig,fGenIndex);
   }
  
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdFtfGenerator::LoadParsFrom(SpdPrimGenParSet* params)
{  
   if (!SpdGenerator::LoadParsFrom(params)) return kFALSE;
   
   Reset();
  
//    Int_t mode;
//    if (!params->GetParameter("Mode",mode,fGenIndex)) return kFALSE; 
//    if (fMode != mode) Build(mode);
   
   if (!params->GetParameter("Mode",fMode,fGenIndex)) return kFALSE;
   Build(fMode);
   
   Bool_t is_ok = kTRUE;
   
   if (!params->GetParameter("Events(init)",fInitEvents,fGenIndex)) is_ok = kFALSE;
   
   if (fMode < 0) {
       TString fname;
       if (params->GetParameter("File(input)",fname,fGenIndex)) SetFilePath(fname);
       else is_ok = kFALSE;
   }
   else {   
       if (!params->GetParameter("Epar(init)",fInitEpar,fGenIndex)) is_ok = kFALSE;
       if (!params->GetParameter("Config",fConfig,fGenIndex)) is_ok = kFALSE;
   }
   
   return is_ok;
}

//_____________________________________________________________________________
void SpdFtfGenerator::Print(Option_t* opt) const
{
  cout << "\t-I- <SpdFtfGenerator::Print> \n" << endl;
  
  Int_t tevents = 0;
  if (fInputTree) tevents = fInputTree->GetEntries();
   
  cout << "\tMode:             " << fMode << endl;
  cout << "\tIs init:          " << fInit << endl;
  cout << "\tIs init seeds:    " << fInitSeeds << endl;
  cout << "\tVerbose level:    " << fVerboseLevel << endl;
  cout << "\tGenerator(path):  " << fGenPath << endl;
  cout << "\tConfig(name):     " << fConfig << endl;
  cout << "\tData(path):       " << fFilePath << endl;
  if (tevents > 0) cout << "\tData(events):     " << tevents << endl;
  cout << "\tSeeds:            " << fSeed << " " << fSeedCycle << endl;
   
  cout << endl;
   
  cout << "\tEvents, init.     " << fInitEvents << endl;   
  cout << "\tEpar, init.       " << fInitEpar << " GeV " << endl;      
  
  cout << endl;
   
  cout << "\tTotal events:     " << fNEvents << endl;   
   
  cout << endl;
}
