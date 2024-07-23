// $Id$
// Author: artur   2019/02/04

//_____________________________________________________________________________
//
// SpdEvtBaseGenerator
//_____________________________________________________________________________

#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TDatabasePDG.h>
#include <TObjArray.h>
#include <TParticle.h>

#include "FairRunSim.h"
#include "FairIon.h"

#include "SpdEvtBaseGenerator.h"
#include "SpdPrimGenParSet.h"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;

ClassImp(SpdEvtBaseGenerator)

//_____________________________________________________________________________
SpdEvtBaseGenerator::SpdEvtBaseGenerator():SpdGenerator("SPD Event Base Generator"),
fPathToData(""),fFileName(""),fFileFormat('n'),
fNEvents(0),fNSkipEvents(0),
fInit(kFALSE),
fDataType("std"),fGenerator(""),fFileEvents(0),fEnableDecayer(kFALSE),
fRootFile(0),fTree(0),fData(0)          
{
}

//_____________________________________________________________________________
SpdEvtBaseGenerator::SpdEvtBaseGenerator(const char* Name):SpdGenerator(Name),
fPathToData(""),fFileName(""),fFileFormat('n'),
fNEvents(0),fNSkipEvents(0),
fInit(kFALSE),
fDataType("std"),fGenerator(""),fFileEvents(0),fEnableDecayer(kFALSE),
fRootFile(0),fTree(0),fData(0)           
{
}

//_____________________________________________________________________________
SpdEvtBaseGenerator::~SpdEvtBaseGenerator() 
{
   CloseFile();
}

//_____________________________________________________________________________
void SpdEvtBaseGenerator::Reset()
{
   fInit = kFALSE;
   
   fPathToData = ""; 
   fFileName = "";   
   fFileFormat = 'n';
   
   fNEvents = 0;     
   fNSkipEvents = 0; 
   
   fInit = kFALSE;   
   
   fDataType = "std";
   fGenerator = "";  
   fFileEvents = 0;  
   fEnableDecayer = kFALSE; 
   
   if (fRootFile) CloseFile();
   if (fAsciiFile.is_open()) fAsciiFile.close();
}

//_____________________________________________________________________________
Bool_t SpdEvtBaseGenerator::EnableExternalDecayer()
{
   FairRunSim* run = FairRunSim::Instance();
   if (run) {
       run->SetPythiaDecayer(kTRUE);
       fEnableDecayer = kTRUE;
   }
   
   if (fVerboseLevel > -2) {
       cout << "-I- <SpdEvtBaseGenerator::EnableExternalDecayer> "
            << "Activate external decayer: " << fEnableDecayer << endl;
   }
         
   return fEnableDecayer;
}

//_____________________________________________________________________________
void SpdEvtBaseGenerator::SetFile(TString filename, TString datapath)
{
   if (filename.EndsWith(".root")) fFileFormat = 'r';
   else if (filename.EndsWith(".txt")) fFileFormat = 'a';
   else {
       cout << "-E- <SpdEvtBaseGenerator::GetDataPath> Unknown data file type" 
            << " (it should be *.root or *.txt) "<< endl;
       fFileFormat = 'n';         
       fFileName = "";
       fPathToData = "";       
       return;
   }
   
   fFileName  = filename;
   fPathToData = datapath;   
  
   TString fullpath = GetFullDataPath();
  
   if (!fullpath.IsWhitespace()) {
       if (fVerboseLevel > -1) {
           cout << "-I- <SpdEvtBaseGenerator::SetFile> Full path to data: " << fullpath << endl;
       }
       return;
   }
  
   cout << "-E- <SpdEvtBaseGenerator::SetFile> Bad path to data: " << fullpath << endl;
        
   fFileName = "";
   fPathToData = "";      
}

//_____________________________________________________________________________
TString SpdEvtBaseGenerator::GetFullDataPath() const
{
   if (fFileName.IsWhitespace()) {
       cout << "-E- <SpdEvtBaseGenerator::GetDataPath> File name is undefined " << endl;
       return "";
   }
   
   if (fFileFormat != 'a' && fFileFormat != 'r') {
       cout << "-E- <SpdEvtBaseGenerator::GetDataPath> File format is undefined " << endl;
       return "";
   }
     
   /* try to create the file */     
   
   TString filepath;
   
   if (fPathToData.IsWhitespace()) {
       filepath = gSystem->Getenv("VMCWORKDIR");
       if (!filepath.EndsWith("/")) filepath += "/";
       filepath += "input/";
   }
   else {
       filepath = fPathToData;
       if (!filepath.EndsWith("/")) filepath += "/";
   }
   
   filepath += fFileName;
 
   return filepath;
}

//_____________________________________________________________________________
Bool_t SpdEvtBaseGenerator::Init()
{
   if (fInit) return kTRUE;
   
   SetFile(fFileName,fPathToData);
   Initialize(fNEvents,fNSkipEvents);
   
   return fInit;
}

//_____________________________________________________________________________
void SpdEvtBaseGenerator::Initialize(Int_t nevents, Int_t nskipevents)
{
   fInit = kFALSE;
  
   if (nevents < 1) {
       cout << "-E- <SpdEvtBaseGenerator::GetDataPath> Bad number of events: " << nevents << endl;
       return;
   }
   
   fNEvents = nevents;
   fNSkipEvents = (nskipevents < 0)? 0 : nskipevents; 
   
   fInit = InitFile();
}
 
//_____________________________________________________________________________
Bool_t SpdEvtBaseGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
   if (!fInit) {
       cout << "-E- <SpdEvtBaseGenerator::ReadEvent> [root] "
            << "Parameters were not initialized properly " << endl;
       return kFALSE;
   }
  
   if (fFileFormat == 'r') 
   {    
       if (!fRootFile) {
           cout << "-E- <SpdEvtBaseGenerator::ReadEvent> [root] "
                << "Data were not initialized properly (no input file)" << endl;
           return kFALSE;
       }
    
       Int_t np = fData->GetEntries();
       
       if (np < 1) {
           cout << "-E- <SpdEvtBaseGenerator::ReadEvent> [root] No particles in the event " 
                << fTree->GetReadEntry() << endl;
           return kFALSE;
       }
       
       TParticle* part;
       Int_t pdg, n(0);
   
       Bool_t addTrack = kTRUE;
       
       Double_t px, py, pz, e;
       Double_t vx, vy, vz, t;
     
       for (Int_t i(0); i < np; i++) {
         
           part = (TParticle*)fData->At(i);
           if (!part) continue;
             
           pdg = part->GetPdgCode();
  
           if (IsIon(pdg) && !IsIonDefined(pdg)) {
               cout << "-W <SpdEvtBaseGenerator::ReadEvent> [root] The ion should be defined previously: " 
                    << pdg << ", skip it." << endl;
               continue;     
           }
           else {
               if (!TDatabasePDG::Instance()->GetParticle(pdg)) {
                   cout << "-W <SpdEvtBaseGenerator::ReadEvent> [root] Unknown particle type " 
                       << pdg << ", skip it." << endl;
                   continue;
               }
           }

           addTrack = part->GetSecondMother(); 
           
           px = part->Px();
           py = part->Py();
           pz = part->Pz();
           e  = part->Energy();

           vx = part->Vx();
           vy = part->Vy();
           vz = part->Vz();
           t  = part->T();
 
           AddTrack(primGen, addTrack, pdg, n, -1, px, py, pz, vx, vy, vz, kTRUE, e, t);
           n++;
       }
       
       Int_t entry = fTree->GetReadEntry()+1;
       
       if (entry >= fTree->GetEntries()) {
           cout << "-W- <SpdEvtBaseGenerator::ReadEvent> [root] No more events in the file: " 
                << entry << "/" << fTree->GetEntries() << endl;
           CloseFile();
           return kFALSE;
       }
       
       /* read next event */ 
       TFile* g = gFile;
   
       fRootFile->cd();
       fTree->GetEntry(entry);
       
       if (g) g->cd();
   }
   else if (fFileFormat == 'a') 
   {
       if (!fAsciiFile.good()) {
           cout << "-E- <SpdEvtBaseGenerator::ReadEvent> [acsii] "
                << "Data were not initialized properly (no input file)" << endl;
           return kFALSE;
       }
       
       std::stringstream sdata;
       
       const Int_t DSIZE = 10000;
       Char_t cstr[DSIZE], cc;
       
       Bool_t is_ok = kFALSE;
       
       while (fAsciiFile.getline(cstr,DSIZE)) 
       {
          if (!strlen(cstr) || TString(cstr).IsWhitespace()) continue;
          
          sdata.clear();
          sdata.str(cstr);
          sdata >> cc;
          
          if (cc != '#') {                
              is_ok = kTRUE; 
              break; 
          }
         //cout << cstr << endl;
       }
       
       if (!is_ok) {
           cout << "-W- <SpdEvtBaseGenerator::ReadEvent> "
                << "[acsii]  No more events in the file " << endl;
           CloseFile();
           return kFALSE;
       }
       
       sdata.clear();
       sdata.str(cstr);
       
       Int_t ievent;
      
       sdata >> ievent;
       
       Int_t i(0), n(0);
     
       Int_t nn, addTrack, pdg, mother, fd, ld;
     
       Double_t px, py, pz, e;
       Double_t vx, vy, vz, t;
       
       while (kTRUE) {
         
          if (!fAsciiFile.getline(cstr,DSIZE)) {
              cout << "-E- <SpdEvtBaseGenerator::ReadEvent> "
                   << "[ascii] End of file (unexpected) " << endl; 
              return kFALSE;
          }
          
          i++;
          
          if (!strlen(cstr)|| TString(cstr).IsWhitespace()) continue;
         
          sdata.clear();
          sdata.str(cstr);
          sdata >> cc;
         
          if (cc == '%') break;
          if (cc == '#') continue;
          
          sdata.clear();
          sdata.str(cstr);
          
          sdata >> nn >> addTrack >> pdg;
          
          //cout << "["<< ievent << "] line/particle: " 
          //     << i-1 << "/" << n << " num = " << nn << " pdg = " << pdg << endl;
               
          if (IsIon(pdg) && !IsIonDefined(pdg)) {
              cout << "-W <SpdEvtBaseGenerator::ReadEvent> [ascii] The ion should be defined previously: " 
                   << pdg << ", skip it." << endl;
              continue;     
          }
          else {
              if (!TDatabasePDG::Instance()->GetParticle(pdg)) {
                  cout << "-W <SpdEvtBaseGenerator::ReadEvent> [ascii] Unknown particle type " 
                       << pdg << ", skip it." << endl;
                  continue;
              }
          }      
               
          sdata >> mother >> fd >> ld; 
          sdata >> px >> py >> pz >> e;
          sdata >> vx >> vy >> vz >> t;
          
          if (primGen != (FairPrimaryGenerator*)0x1) {
              AddTrack(primGen, addTrack, pdg, n, -1, px, py, pz, vx, vy, vz, kTRUE, e, t);
          }
       
          n++;
       }

       cout << endl;
   }
   else 
   {
       cout << "-E- <SpdEvtBaseGenerator::ReadEvent> Bad file format: " << fFileFormat << endl;
       return kFALSE;
   }
 
   if (!primGen) return kFALSE;
   
   return kTRUE; 
}

//_____________________________________________________________________________
Bool_t SpdEvtBaseGenerator::InitFile() // protected
{ 
   if (fFileFormat == 'r') 
   {  
       if (fRootFile) CloseFile();
       
       TString filepath = GetFullDataPath();
       
       fRootFile = new TFile(filepath,"READ");
       
       if (!fRootFile) {
           cout << "-E- <SpdEvtBaseGenerator::InitFile> [root] No file: " << filepath << endl;
           return kFALSE;
       }
     
       fTree = (TTree*)fRootFile->Get("std");
       if (!fTree) {
           cout << "-E- <SpdEvtBaseGenerator::InitFile> [root] Unknown format " << endl;
           return kFALSE;
       }
  
       TBranch* branch = fTree->GetBranch("Particles");
       if (!branch) {
           cout << "-E- <SpdEvtBaseGenerator::InitFile> [root] No branch \"Particles\" " << endl;
           return kFALSE;
       }
    
       fDataType = fTree->GetName();
       fGenerator = fTree->GetTitle(); 
    
       fFileEvents = fTree->GetEntries();       
           
       if ((fNEvents+fNSkipEvents) > fFileEvents) {
           cout << "-E- <SpdEvtBaseGenerator::InitFile> [root] There are not enough events in the file " 
                << "(needed/avilable): " << fNEvents+fNSkipEvents << "/" << fFileEvents << endl;            
           return kFALSE;
       }
       
       if (fFileEvents == 0) {
           cout << "-E- <SpdEvtBaseGenerator::InitFile> [root] No events in the file " << endl; 
           return kFALSE;
       }
       
       branch->SetAddress(&fData); 
     
       TFile* g = gFile;
       
       fRootFile->cd();
  
       fTree->GetEntry(0);
       TParticle* part = (TParticle*)fData->At(0);
     
       if (!part) {
           cout << "-F- <SpdEvtBaseGenerator::InitFile> [root] Fatal error: empty event was found " << endl; 
           exit(1);
       }
       
       if (part->GetUniqueID() == 1) EnableExternalDecayer();
              
       if (fNSkipEvents > 0) fTree->GetEntry(fNSkipEvents); 
       
       if (g) g->cd();
       
       if (fVerboseLevel > -2) {
           cout << "-I- <SpdEvtBaseGenerator::InitFile> [root] Data/Generator/Events/Entry: "
                << fDataType << "/" << fGenerator << "/" << fFileEvents << "/" 
                << fNSkipEvents << endl;
       }
       
       return kTRUE;  
   }
   else if (fFileFormat == 'a') 
   {  
       if (fAsciiFile) CloseFile();
       
       TString filepath = GetFullDataPath();
     
       fAsciiFile.open(filepath.Data());
       
       if (!fAsciiFile.good()) {
           cout << "-E- <SpdEvtBaseGenerator::InitFile> [acsii] No file: " << filepath << endl;
           return kFALSE;
       }
       
       std::stringstream sdata;
       
       const Int_t DSIZE = 10000;
       Char_t cstr[DSIZE], cc;
       
       Bool_t is_ok = kFALSE;
       
       while (fAsciiFile.getline(cstr,DSIZE)) 
       {
          if (!strlen(cstr) || TString(cstr).IsWhitespace()) continue;
          
          sdata.clear();
          sdata.str(cstr);
          sdata >> cc;
          
          if (cc != '#') {                
              is_ok = kTRUE; 
              break; 
          }
       }
       
       if (!is_ok) {
           cout << "-E- <SpdEvtBaseGenerator::InitFile> [acsii] File is empty: " << filepath << endl;
           return kFALSE;
       }
       
       std::string str;
       Bool_t enable_decayer = kFALSE;
       
       sdata.clear();
       sdata.str(cstr);
       
       sdata >> str;
       fDataType = str.data();
       
       sdata >> str;
       fGenerator = str.data();
          
       sdata >> fFileEvents;
       
       if ((fNEvents+fNSkipEvents) > fFileEvents) {
           cout << "-E- <SpdEvtBaseGenerator::InitFile> [ascii] There are not enough events in the file " 
                << "(needed/avilable): " << fNEvents+fNSkipEvents << "/" << fFileEvents << endl;            
           return kFALSE;
       }
       
       if (fFileEvents == 0) {
           cout << "-E- <SpdEvtBaseGenerator::InitFile> [ascii] No events is defined " << endl; 
           return kFALSE;
       }
       
       sdata >> enable_decayer;
       
       if (enable_decayer) EnableExternalDecayer();
           
       fInit = kTRUE;
       
       if (fNSkipEvents > 0) {
           for (Int_t i(0); i<fNSkipEvents; i++) {
                if (!ReadEvent((FairPrimaryGenerator*)0x1)) {
                   fInit = kFALSE;
                   return kFALSE;
                }
           }
       }
       
       if (fVerboseLevel > -2) {
           cout << "-I- <SpdEvtBaseGenerator::InitFile> [acsii] Data/Generator/Events/Entry: "
                << fDataType << "/" << fGenerator << "/" << fFileEvents << "/" << fNSkipEvents
                << endl;
       }
       
       return kTRUE; 
   }
  
   cout << "-E- <SpdEvtBaseGenerator::InitFile> Bad file format: " << fFileFormat << endl;
   
   return kFALSE;  
}

//_____________________________________________________________________________
void SpdEvtBaseGenerator::CloseFile() // protected
{ 
   if (fRootFile) {
       fRootFile->Close();
       delete fRootFile;
       fRootFile = 0; 
  
       fTree = 0;
       fData = 0;
   }
    
   if (fAsciiFile.is_open()) fAsciiFile.close();
   
   fDataType = "std";
   fGenerator = "";  
   fFileEvents = 0;  
}

//_____________________________________________________________________________  
Bool_t SpdEvtBaseGenerator::IsIon(Int_t pdg) // static 
{  
   return (pdg > 1000000000 && pdg < 1099999999) ? kTRUE : kFALSE;
}

//_____________________________________________________________________________    
Int_t SpdEvtBaseGenerator::IonPdg(Int_t Z, Int_t A) // static 
{
   return (Z == 1 && A == 1) ? 2212 : 1000000000 + Z*10000 + A*10; 
}

//_____________________________________________________________________________  
void SpdEvtBaseGenerator::IonZA(Int_t pdg, Int_t& Z, Int_t& A) // static 
{  
   Z = (pdg-1000000000)/10000; 
   A = (pdg-1000000000-10000*Z)/10;
}

//_____________________________________________________________________________  
TString SpdEvtBaseGenerator::IonName(Int_t pdg) // static 
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
Bool_t SpdEvtBaseGenerator::RegisterIon(Int_t pdg) // static
{  
   if (!IsIon(pdg)) return kFALSE;
    
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
   
   ion = new FairIon(ionname,Z,A,Z,0);
   run->AddNewIon(ion);
    
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdEvtBaseGenerator::RegisterIon(Int_t Z, Int_t A) // static
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
   
   ion = new FairIon(ionname,Z,A,Z,0);
   run->AddNewIon(ion);
    
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdEvtBaseGenerator::RegisterIon(Int_t Z, Int_t A, Int_t Q, Double_t EE) // static
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
   
   ion = new FairIon(ionname,Z,A,Q,EE);
   run->AddNewIon(ion);
    
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdEvtBaseGenerator::IsIonDefined(Int_t pdg) // static
{
   FairRunSim* run = FairRunSim::Instance();
   if (!run) return kFALSE;
    
   TObjArray* ions = run->GetUserDefIons();
   if (!ions) return kFALSE; 
   
   if (!IsIon(pdg)) return kFALSE; 
   
   Int_t Z, A;
   IonZA(pdg,Z,A);
 
   TString ionname;
   
   TDatabasePDG* pdgDB = TDatabasePDG::Instance();
   
   if (pdgDB && pdgDB->GetParticle(pdg)) ionname = pdgDB->GetParticle(pdg)->GetName();
   else ionname = Form("ION_%03d_%03d",Z,A);
 
   FairIon* ion = (FairIon*)ions->FindObject(ionname);
   if (ion) return kTRUE;
   
   return kFALSE;
}
  
//_____________________________________________________________________________    
Bool_t SpdEvtBaseGenerator::FillParsIn(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::FillParsIn(params)) return kFALSE;
   
   //params->SetParameter("EnableDecayer",fEnableDecayer,fGenIndex);
   
   TString path;
   if (fPathToData.IsWhitespace()) path = "VMCWORKDIR/input";
   else path = fPathToData;
   
   params->SetParameter("FilePath",path,fGenIndex);
   params->SetParameter("FileName",fFileName,fGenIndex);
   params->SetParameter("Events(gen)",fNEvents,fGenIndex);
   params->SetParameter("Events(skip)",fNSkipEvents,fGenIndex);
   
   return kTRUE;
}

//_____________________________________________________________________________    
Bool_t SpdEvtBaseGenerator::LoadParsFrom(SpdPrimGenParSet* params)
{
   if (!SpdGenerator::LoadParsFrom(params)) return kFALSE;
   
   Reset();
   
   //Bool_t enable_decayer;
   //params->GetParameter("EnableDecayer",enable_decayer,fGenIndex);
   //if (enable_decayer) EnableExternalDecayer();
   //fEnableDecayer = enable_decayer;
  
   Bool_t is_ok = kTRUE;
  
   if (!params->GetParameter("FilePath",fPathToData,fGenIndex)) is_ok = kFALSE; 
   if (!params->GetParameter("FileName",fFileName,fGenIndex)) is_ok = kFALSE;
   if (!params->GetParameter("Events(gen)",fNEvents,fGenIndex)) is_ok = kFALSE;
   if (!params->GetParameter("Events(skip)",fNSkipEvents,fGenIndex)) is_ok = kFALSE;
   
   if (fPathToData == "VMCWORKDIR/input") fPathToData = "";  
  
   return is_ok;
}

//_____________________________________________________________________________    
void SpdEvtBaseGenerator::Print(Option_t* opt) const
{
   cout << "\t-I- <SpdEvtBaseGenerator::Print> \n" << endl;
   
   TString fmt("unknown");
   if (fFileFormat == 'r') fmt = "root";
   else if (fFileFormat == 'a') fmt = "ascii";
   
   cout << "\tIs init:          " << fInit << endl;  
   cout << "\tEnable decayer:   " << fEnableDecayer << endl;
   cout << "\tPath to data:     " << fPathToData << endl;
   cout << "\tFile:             " << fFileName << endl;
   
   cout << endl;
   
   cout << "\tFile format:      " << "\"" << fmt << "\"" << endl;
   cout << "\tData format:      " << "\"" << fDataType << "\"" << endl;
   cout << "\tGenerator:        " << fGenerator << endl;  
   cout << "\tEvents(file):     " << fFileEvents << endl;
   
   cout << endl;
   
   cout << "\tEvents(generate): " << fNEvents << endl;
   cout << "\tEvents(skip):     " << fNSkipEvents << endl;
 
   cout << endl;
}

