// $Id$
// Author: artur   2018/12/05


//_____________________________________________________________________________
//
// SpdGenerator
//_____________________________________________________________________________

#include "SpdGenerator.h"
#include "SpdPrimGenData.h"
#include "SpdPrimGenParSet.h"
#include "SpdPrimaryGenerator.h"
#include <TSystem.h>
#include <TParticle.h>
#include <TRandom3.h>
#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdGenerator)

Int_t SpdGenerator::fNTracks = 0;

//_____________________________________________________________________________
SpdGenerator::SpdGenerator():
FairGenerator(),
fGenIndex(0),fGenData(0),fVerboseLevel(1),
fGen(new TRandom3()),fEvent(0)
{
}

//_____________________________________________________________________________
SpdGenerator::SpdGenerator(const char* Name):
FairGenerator(Name,"Spd Primary Generator"),
fGenIndex(0),fGenData(0),fVerboseLevel(1),
fGen(new TRandom3()),fEvent(0)
{
}

//_____________________________________________________________________________
SpdGenerator::~SpdGenerator() 
{
   if (fGen) delete fGen;
   if (fEvent) {
       fEvent->Delete();
       delete fEvent;
   }
}

//_____________________________________________________________________________
void SpdGenerator::Reset()
{
   fGenIndex = 0;
   fGenData = 0; 
}

//_____________________________________________________________________________
Bool_t SpdGenerator::ReadEvent(FairPrimaryGenerator* primGen) 
{
   return kFALSE;  
}

//_____________________________________________________________________________
void SpdGenerator::RemoveGenData()
{
   if (!fGenData) return;
   
   Int_t ntracks = fGenData->GetNParticles();
   
   if (fNTracks > 0) {
       fNTracks -= ntracks;
       if (fNTracks < 0) fNTracks = 0;
   }
      
   fGenData->Clear();
}

//_____________________________________________________________________________
Bool_t SpdGenerator::AddTrack(FairPrimaryGenerator* primGen, Bool_t AddToStack, 
                              Int_t pdg, Int_t num, Int_t parent_num, 
                              Double_t px, Double_t py, Double_t pz, /* momentum */
                              Double_t vx, Double_t vy, Double_t vz, /* vertex */
                              Bool_t wanttracking,
                              Double_t e, Double_t time, Double_t weight)
{
   if (fEvent != 0) {
       
       Int_t n = fEvent->GetEntriesFast();
       TParticle* part = new ((*fEvent)[n]) TParticle(pdg, num, parent_num, AddToStack, 
                                    -1, -1, px, py, pz, e, vx, vy, vz, time); 
       part->SetWeight(weight);
       part->SetUniqueID(IsDecayerEnable());
       return kTRUE; 
   }
     
   if (!primGen && !fGenData) return kFALSE;
       
   TParticle* particle = 0;
   
   Int_t StackTrackId = -1; 
   Int_t DataTrackId  = -1;
     
   if (fGenData) {

       if (!primGen) StackTrackId = AddToStack;
         
       TParticle* part = new TParticle(pdg, num, parent_num, StackTrackId, 
                                      -1, -1, px, py, pz, e, vx, vy, vz, time);
   
       part->SetWeight(weight);
       part->SetUniqueID(kPPrimary);
       
       part->SetFirstDaughter(fGenIndex);
       part->SetLastDaughter(fNTracks);
       
       DataTrackId = fGenData->GetNParticles();
       
       particle = fGenData->AddParticle(part);
      
       GetSeeds(fGenData->GetGenSeeds());
       
       fNTracks++;
       
       delete part;
   }
   
   if (primGen && AddToStack) {
    
       SpdPrimaryGenerator* gen = dynamic_cast<SpdPrimaryGenerator*>(primGen);
  
       if (gen) StackTrackId = gen->GetCurrentTrackId();
       
       primGen->AddTrack(pdg, px, py, pz, vx, vy, vz, DataTrackId, 
                         wanttracking, e, time, weight);

       if (gen && particle && StackTrackId < gen->GetCurrentTrackId()) {
           particle->SetLastMother(gen->GetCurrentTrackId());
       }
       
       return kTRUE;
   }
   
   return kFALSE;
}

//_____________________________________________________________________________
FairGenerator* SpdGenerator::CloneGenerator() const
{
   return FairGenerator::CloneGenerator();
}

//_____________________________________________________________________________
Bool_t SpdGenerator::FillParsIn(SpdPrimGenParSet* params)
{
   if (fGenIndex < 1) return kFALSE;
   if (!params) return kFALSE;
   
   params->SetParameter("VerboseLevel",fVerboseLevel,fGenIndex); 
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdGenerator::LoadParsFrom(SpdPrimGenParSet* params)
{
   if (fGenIndex < 1) return kFALSE;
   if (!params) return kFALSE;
   
   params->GetParameter("VerboseLevel",fVerboseLevel,fGenIndex);
   SetVerboseLevel(fVerboseLevel);
   
   return kTRUE;
}

//_____________________________________________________________________________
Bool_t SpdGenerator::CreateAsciiFile(Int_t nevents, TString filename, TString path, TString fmt)
{  
   if (nevents < 1) return kFALSE;
   
   if (filename.IsWhitespace()) {
       cout << "-E- <" << this->ClassName() << "::CreateAsciiFile> File name is undefined " << endl;
       return kFALSE;
   }
   
   if (fmt != "std") {
       cout << "-E- <" << this->ClassName() << "::CreateAsciiFile> Unknown file format: " << fmt << endl;
       return kFALSE;
   }
   
   /* try to create the file */
   
   TString filepath;
   
   if (path.IsWhitespace()) {
       filepath = gSystem->Getenv("VMCWORKDIR");
       if (!filepath.EndsWith("/")) filepath += "/";
       filepath += "input/";
   }
   else {
       filepath = path;
       if (!filepath.EndsWith("/")) filepath += "/";
   }
   
   if (!filename.EndsWith(".txt")) filename += ".txt";
       
   filepath += filename;
   
   FILE* ofile;
   if (!(ofile = fopen(filepath,"w"))) {
       cout << "-E- <" << this->ClassName() << "::CreateAsciiFile> File <" << filepath << ">" 
             << " cannot be created " << endl;
       return kFALSE;      
   }
   
   cout << "-I- <" << this->ClassName() << "::CreateAsciiFile> Open <" << filepath << ">" << endl;
   
   fEvent = new TClonesArray("TParticle",100);
  
   fprintf(ofile,"# FORMAT GENERATOR NEVENTS DECAYER[false,true]\n");
   fprintf(ofile,"%s %s %d %d\n",fmt.Data(),this->ClassName(),nevents,Int_t(IsDecayerEnable()));
   fprintf(ofile,"# IEVENT \n");
   fprintf(ofile,"# IPARTICLE ADDTOSTACK[false,true] PDG IPARENT FIRSTDAUGHTER LASTDAUGHTER ");
   fprintf(ofile,"(PX PY PZ)[GeV/c] ENERGY[GeV] (VX VY VZ)[cm] TIME[s]\n");
     
   /* generate events */
   Int_t np;
   TParticle* p;
   for (Int_t i(0); i < nevents; i++) {
        
        ReadEvent(0);
        np = fEvent->GetEntriesFast(); 
        
        fprintf(ofile,"%-8d \n",i);
        
        for (Int_t j(0); j < np; j++) {
             p = (TParticle*)fEvent->At(j);
             fprintf(ofile,"%-8d %2d  %12d %8d  %8d %8d  %16.8e %16.8e %16.8e %16.8e  %12.8e %12.8e %12.8e %12.8e\n",
                     p->GetStatusCode(),p->GetSecondMother(),
                     p->GetPdgCode(),p->GetFirstMother(),
                     p->GetFirstDaughter(),p->GetLastDaughter(),
                     p->Px(),p->Py(),p->Pz(),p->Energy(),
                     p->Vx(),p->Vy(),p->Vz(),p->T());
        }
        fprintf(ofile,"%%%%%% ENDOFEVENT\n");
        
        fEvent->Clear();
   }
  
   cout << "-I- <" << this->ClassName() << "::CreateAsciiFile> " << nevents 
        << " events is saved into " << filepath << endl;
   
   /* close the file */
   
   fclose(ofile);
   
   fEvent->Delete();
   delete fEvent;
   fEvent = 0;
   
   return kTRUE;  
}

//_____________________________________________________________________________
Bool_t SpdGenerator::CreateRootFile(Int_t nevents, TString filename, TString path, TString fmt)
{
   if (nevents < 1) return kFALSE;
       
   if (filename.IsWhitespace()) {
       cout << "-E- <" << this->ClassName() << "::CreateRootFile> File name is undefined " << endl;
       return kFALSE;
   }
   
   if (fmt != "std") {
       cout << "-E- <" << this->ClassName() << "::CreateRootFile> Unknown file format: " << fmt << endl;
       return kFALSE;
   }
   
   /* try to create the file */     
   
   TString filepath;
   
   if (path.IsWhitespace()) {
       filepath = gSystem->Getenv("VMCWORKDIR");
       if (!filepath.EndsWith("/")) filepath += "/";
       filepath += "input/";
   }
   else {
       filepath = path;
       if (!filepath.EndsWith("/")) filepath += "/";
   }
   
   if (!filename.EndsWith(".root")) filename += ".root";
       
   filepath += filename;
       
   TFile* g = gFile;
     
   TFile* ofile = new TFile(filepath,"RECREATE");
   
   if (!ofile) {
       cout << "-E- <" << this->ClassName() << "::CreateRootFile> File <" << filepath << ">" 
             << " cannot be created " << endl;
       g->cd();      
       return kFALSE;      
   }
   
   cout << "-I- <" << this->ClassName() << "::CreateRootFile> Open <" << filepath << ">" << endl;
  
   TTree* Tree = new TTree(fmt,this->ClassName());
   
   fEvent = new TClonesArray("TParticle",100);
   
   Tree->Branch("Particles",&fEvent,32000,0);
     
   /* generate events */
   
   for (Int_t i(0); i<nevents; i++) {
        ReadEvent(0);
        Tree->Fill();
        fEvent->Clear();
   }    
   
   /* write & close the file */
   
   cout << "-I- <" << this->ClassName() << "::CreateRootFile> "<< nevents 
        << " events is saved into " << filepath << endl;
      
   ofile->Write();
   ofile->Close();
   
   if (g) g->cd();
   
   fEvent->Delete();
   delete fEvent;
   fEvent = 0;
     
   return kTRUE;
}



