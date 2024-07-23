// $Id$
// Author: artur   2021/03/16


//_____________________________________________________________________________
//
// SpdMCDataIterator
//_____________________________________________________________________________

#include "SpdMCDataIterator.h"
#include "SpdRunAna.h"

#include "FairRunIdGenerator.h" 
#include "FairFileSource.h"
#include "SpdRootFileSink.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"

#include <TSystem.h>

#include <iostream>

using std::cout;
using std::endl;

ClassImp(SpdMCDataIterator)

SpdMCDataIterTask::SpdMCDataIterTask():FairTask("Iterator task"),fIter(0) { }
SpdMCDataIterTask::~SpdMCDataIterTask() {}

InitStatus SpdMCDataIterTask::Init() 
{
    if (!fIter) return kFATAL;
    fIter->ActivateDataBranches();
    return kSUCCESS;
}

void SpdMCDataIterTask::Exec(Option_t* opt) 
{
    //cout <<"-I- <SpdMCDataIterTask::Exec> " << endl;    
}

//_____________________________________________________________________________
SpdMCDataIterator::SpdMCDataIterator():
fRun(0),fTask(0),fSource(0),fSink(0),
fInit(false),fRunId(0),fNFiles(0),fNEvents(-1),
fCurrentEntry(-1),fLastSavedEntry(-1),
fSaveEvent(false),
// data
fActivateEvent(false),fEvent(0),
fActivateParticles(false),fParticles(0),
fActivateVertices(false),fVertices(0),
fActivateItsHits(false),fItsHits(0),         
fActivateTsHits(false),fTsHits(0),    
fActivateTofHits(false),fTofHits(0),    
fActivateEcalHits(false),fEcalHits(0),        
fActivateRsHits(false),fRsHits(0),
fActivateBbcHits(false),fBbcHits(0),
fActivateAegHits(false),fAegHits(0),
fActivateZdcHits(false),fZdcHits(0),
fActivateTracks(false),fTracks(0),         
fActivateVerticesRC(false),fVerticesRC(0),  
fActivateTracksRC(false),fTracksRC(0),           
fActivateTrackSeedsRC(false),fTrackSeedsRC(0),        
fActivateEcalClustersRC(false),fEcalClustersRC(0),     
fActivateEcalParticlesRC(false),fEcalParticlesRC(0),  
fActivateEcalClustersMCInfo(false),fEcalClustersMCInfo(0),
fActivateEcalParticlesMC(false),fEcalParticlesMC(0),
fActivateRsClustersMC(false),fRsClustersMC(0),
fActivateRsParticlesMC(false),fRsParticlesMC(0),
fActivateTsParticles(false),fTsParticles(0),
fActivateTofParticles(false),fTofParticles(0),
fActivateAegParticles(false),fAegParticles(0)
{                                      
    fRun = new SpdRunAna();
    fRun->SetContainerStatic(true);
    
    fTask = new SpdMCDataIterTask();
    fTask->SetIter(this);
    fRun->AddTask(fTask);
}

//_____________________________________________________________________________
SpdMCDataIterator::~SpdMCDataIterator() 
{
    //cout << "-I- <SpdMCDataIterator::~SpdMCDataIterator>" <<endl;
    
    if (fSink && fSaveEvent) Finish();
    
    if (fRun) delete fRun;
    if (fSource) delete fSource;
    if (fSink) delete fSink;
}

//_____________________________________________________________________________
const SpdSetParSet* SpdMCDataIterator::GetParameters() const
{
    if (!fRun) return 0;
    return dynamic_cast<SpdSetParSet*>(fRun->GetParameters("SpdSetParSet"));
}

//=============================================================================
//  SOURCE/SINK
//

//_____________________________________________________________________________
void SpdMCDataIterator::AddSourceFile(TString fname)
{
    if (fInit) return;
    
    fname = fCurrentDataDir + fname;
    
    if (!fname.EndsWith(".root")) fname += ".root";
    
    //cout << "add: " << fname << endl;
    
    if (!fSource) {
        fSource = new FairFileSource(fname);
        fRun->SetSource(fSource);
        fNFiles++;
        return;
    }
    
    fSource->AddFile(fname);
    fNFiles++;
}

//_____________________________________________________________________________
void SpdMCDataIterator::SetOutputFile(TString fname)
{
    if (fInit || fSink) return;
    
    fname = fCurrentDataDir + fname;
    
    if (!fname.EndsWith(".root")) fname += ".root";
    
    fSink = new SpdRootFileSink(fname);
    fRun->SetSink(fSink);
}

//_____________________________________________________________________________
void SpdMCDataIterator::AddBranchToOutput(TString branch_name)
{
    if (fSink) fSink->AddBranchToOutput(branch_name);   
}

//=============================================================================
// INITIALIZATION
//

//_____________________________________________________________________________
void SpdMCDataIterator::GenerateRunId() 
{
    FairRunIdGenerator genid;
    fRunId = genid.generateId();
}

//_____________________________________________________________________________
Bool_t SpdMCDataIterator::Init() 
{
    if (!fSource || (fNFiles < 1)) return false;
    
    fRun->Initialize();
    
    fNEvents = fSource->GetEntries();
    
    if (fNFiles > 1 || fSink) GenerateRunId();
                                            
    fInit = true;
    return fInit;   
}

//_____________________________________________________________________________
Bool_t SpdMCDataIterator::Init(Int_t nentry) 
{
    if (nentry < 0) return Init();
    
    if (!fSource || (fNFiles < 1)) return false;
    
    fRun->Initialize();
    
    fNEvents = fSource->GetEntries();
    
    if (!(nentry < fNEvents)) return false;
    
    if (fNFiles > 1 || fSink) GenerateRunId();
        
    fCurrentEntry = nentry;
     
    if (fEvent) fEvent->Clear();
     
    fRun->RunEntry(fCurrentEntry);
    
    if ((fNFiles > 1 || fSink) && fEvent) {
 
        fEvent->SetMotherRunId(fEvent->GetRunId());
        fEvent->SetMotherEventId(fEvent->GetEventId()); 
        
        fEvent->SetRunId(fRunId);
        fEvent->SetEventId(fCurrentEntry); 
    }

    fInit = true;
    return fInit;   
}

//=============================================================================
// ITERATOR
//

//_____________________________________________________________________________
void SpdMCDataIterator::Reset()
{
    fCurrentEntry = -1;
    fLastSavedEntry = -1;
    fSaveEvent = false;
}

//_____________________________________________________________________________
Bool_t SpdMCDataIterator::NextEvent()
{
    if (!fInit) return false;
    if (!(fCurrentEntry < fNEvents-1)) return false;
    
    if (fSink) {
        if (fSaveEvent) SaveLastEvent();
        fSaveEvent = true;
    }
    
    fCurrentEntry++;
     
    if (fEvent) fEvent->Clear();
     
    fRun->RunEntry(fCurrentEntry);
    
    if ((fNFiles > 1 || fSink) && fEvent) {
 
        fEvent->SetMotherRunId(fEvent->GetRunId());
        fEvent->SetMotherEventId(fEvent->GetEventId()); 
        
        fEvent->SetRunId(fRunId);
        fEvent->SetEventId(fCurrentEntry); 
    }

    return true;
}

//_____________________________________________________________________________
void SpdMCDataIterator::SaveLastEvent()
{
    if (!fSink || !fSaveEvent) return;
    
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm) return;
    
    // WARNING new event number will be stored WARNING 
    fLastSavedEntry++;
    if (fEvent) fEvent->SetEventId(fLastSavedEntry); 
    
    //cout << ">>>>>>> fill event ... " << fCurrentEntry << "; event num to save: " << fLastSavedEntry << endl;

    fRun->Fill();
    rm->DeleteOldWriteoutBufferData();
    
    fSaveEvent = false;
}

//_____________________________________________________________________________
void SpdMCDataIterator::Finish()
{
    if (!fSink) return;
    
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm) return;
    
    if (fSaveEvent) SaveLastEvent();
    else rm->DeleteOldWriteoutBufferData();
    
    //cout << ">>>>>>> finish ... " << endl;
    
    rm->StoreAllWriteoutBufferData();
    rm->LastFill();
    rm->Write();
}

//=============================================================================
// DATA
//

//_____________________________________________________________________________
void SpdMCDataIterator::ActivateDataBranches()
{
   cout << "-I- <SpdMCDataIterator::ActivateDataBranches> " << endl;

   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-F- <SpdMCDataIterator::ActivateDataBranches> RootManager not instantiated " << endl;
       exit(1);
   }
  
   // get Event 
   if (fActivateEvent) fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   
   // ----- get MCParticles array -----
   
   if (fActivateParticles) fParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCParticles));
   
   // ----- get MCVertices array -----
   
   if (fActivateVertices) fVertices = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCVertices));

   // ----- get ItsHits array -----
    
   if (fActivateItsHits) fItsHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCItsHits));
   
   // ----- get TsHits array -----
    
   if (fActivateTsHits) fTsHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTsHits));
   
   // ----- get TofHits array -----
    
   if (fActivateTofHits) fTofHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTofHits));
   
   // ----- get EcalHits array -----
  
   if (fActivateEcalHits) fEcalHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCEcalHits));
   
   // ----- get RsHits array -----
   
   if (fActivateRsHits) fRsHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCRsHits));
   
   // ----- get BbcHits array -----
   
   if (fActivateBbcHits) fBbcHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCBbcHits));
   
   // ----- get AegHits array -----
   
   if (fActivateAegHits) fAegHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCAegHits));
   
   // ----- get ZdcHits array -----
   
   if (fActivateZdcHits) fZdcHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCZdcHits));
   
   // ----- get Tracks array -----
    
   if (fActivateTracks) fTracks = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTracks));
   
   // ----- get RC-Vertices array -----
   
   if (fActivateVerticesRC) fVerticesRC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCVertices));
   
   // ----- get RC-Tracks array -----
    
   if (fActivateTracksRC) fTracksRC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCTracks));
   
   // ----- get RC-Track seeds array -----
    
   if (fActivateTrackSeedsRC) fTrackSeedsRC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCTrackSeeds));
      
   // ----- get Ecal RC-Clusters array -----
   
   if (fActivateEcalClustersRC) fEcalClustersRC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCEcalClusters));
   
   // ----- get Ecal RC-Particles array -----
   
   if (fActivateEcalParticlesRC) fEcalParticlesRC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCEcalParticles));
   
   // ----- get Ecal RC-Clusters MC-Info array -----
   
   if (fActivateEcalClustersMCInfo) fEcalClustersMCInfo = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCEcalClustersInfo));
   
   // ----- get Ecal MC-Particles array -----
   
   if (fActivateEcalParticlesMC) fEcalParticlesMC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCEcalParticles));
   
   // ----- get Rs MC-Clusters array -----
   
   if (fActivateRsClustersMC) fRsClustersMC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCRsClusters));
   
   // ----- get Rs MC-Particles array -----
   
   if (fActivateRsParticlesMC) fRsParticlesMC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCRsParticles));
   
   // ----- get Ts Particles array -----
   
   if (fActivateTsParticles) fTsParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kTsParticles));
   
   // ----- get Tof Particles array -----
   
   if (fActivateTofParticles) fTofParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kTofParticles));
   
   // ----- get Tof Particles array -----
   
   if (fActivateAegParticles) fAegParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kAegParticles));

   //============== PASS DATA TO EVENTHELPER ================================/   
   
   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
   // base objects
   if (fEvent)     helper->SetEvent(fEvent);
   if (fParticles) helper->SetParticles(fParticles);
   if (fVertices)  helper->SetVertices(fVertices);
   
   // hits
   if (fItsHits)   helper->SetItsHits(fItsHits);
   if (fTsHits)    helper->SetTsHits(fTsHits);
   if (fTofHits)   helper->SetTofHits(fTofHits);
   if (fEcalHits)  helper->SetEcalHits(fEcalHits);
   if (fRsHits)    helper->SetRsHits(fRsHits);
   if (fBbcHits)   helper->SetBbcHits(fBbcHits);
   if (fAegHits)   helper->SetAegHits(fAegHits);
   if (fZdcHits)   helper->SetZdcHits(fZdcHits);
   
   // objects
   if (fTracks)             helper->SetTracks(fTracks);
   if (fVerticesRC)         helper->SetVerticesRC(fVerticesRC);
   
   if (fTracksRC)           helper->SetTracksRC(fTracksRC);
   if (fTrackSeedsRC)       helper->SetTrackSeedsRC(fTrackSeedsRC);
   
   if (fEcalClustersRC)     helper->SetEcalClustersRC(fEcalClustersRC);
   if (fEcalParticlesRC)    helper->SetEcalParticlesRC(fEcalParticlesRC);
   if (fEcalClustersMCInfo) helper->SetEcalClustersMCInfo(fEcalClustersMCInfo);
   if (fEcalParticlesMC)    helper->SetEcalParticlesMC(fEcalParticlesMC);
   
   if (fRsClustersMC)       helper->SetRsClustersMC(fRsClustersMC);
   if (fRsParticlesMC)      helper->SetRsParticlesMC(fRsParticlesMC);
   
   if (fTsParticles)        helper->SetTsParticles(fTsParticles);
   if (fTofParticles)       helper->SetTofParticles(fTofParticles);
   if (fAegParticles)       helper->SetAegParticles(fAegParticles);

}

//_____________________________________________________________________________
void SpdMCDataIterator::ActivateBranch(const Char_t* data)
{
   if (!strcmp(data,"all") || 
       !strcmp(data,"All") || 
       !strcmp(data,"ALL"))
   {
       fActivateEvent = true;
       fActivateParticles = true;
       fActivateVertices = true;
       
       fActivateItsHits = true;
       fActivateTsHits = true;
       fActivateTofHits = true;
       fActivateEcalHits = true;
       fActivateRsHits = true;
       fActivateBbcHits = true;
       fActivateAegHits = true;
       fActivateZdcHits = true;
   
       fActivateTracks = true;
       fActivateVerticesRC = true;
       
       fActivateTracksRC = false;     //FIXME
       fActivateTrackSeedsRC = false; //FIXME
       
       fActivateEcalClustersRC = true;
       fActivateEcalParticlesRC = true;   
       fActivateEcalClustersMCInfo = true;
       fActivateEcalParticlesMC = true;
       
       fActivateRsClustersMC = true;
       fActivateRsParticlesMC = true;
       
       fActivateTsParticles = true;
       fActivateTofParticles = true;
       fActivateAegParticles = true;

       return;
   }
       
        if (!strcmp(data,SpdDataBranchName(kMCEvent)))            { fActivateEvent     = true; }
   else if (!strcmp(data,SpdDataBranchName(kMCParticles)))        { fActivateParticles = true; }
   else if (!strcmp(data,SpdDataBranchName(kMCVertices)))         { fActivateVertices  = true; } 
   
   else if (!strcmp(data,SpdDataBranchName(kMCItsHits)))          { fActivateItsHits   = true; } 
   else if (!strcmp(data,SpdDataBranchName(kMCTsHits)))           { fActivateTsHits    = true; }
   else if (!strcmp(data,SpdDataBranchName(kMCTofHits)))          { fActivateTofHits   = true; }
   else if (!strcmp(data,SpdDataBranchName(kMCEcalHits)))         { fActivateEcalHits  = true; } 
   else if (!strcmp(data,SpdDataBranchName(kMCRsHits)))           { fActivateRsHits    = true; }
   else if (!strcmp(data,SpdDataBranchName(kMCBbcHits)))          { fActivateBbcHits   = true; }
   else if (!strcmp(data,SpdDataBranchName(kMCAegHits)))          { fActivateAegHits   = true; }
   else if (!strcmp(data,SpdDataBranchName(kMCZdcHits)))          { fActivateZdcHits   = true; }
   
   else if (!strcmp(data,SpdDataBranchName(kMCTracks)))           { fActivateTracks             = true; } 
   else if (!strcmp(data,SpdDataBranchName(kRCVertices)))         { fActivateVerticesRC         = true; } 
   
   else if (!strcmp(data,SpdDataBranchName(kRCTracks)))           { fActivateTracksRC           = true; } 
   else if (!strcmp(data,SpdDataBranchName(kRCTrackSeeds)))       { fActivateTrackSeedsRC       = true; } 
    
   else if (!strcmp(data,SpdDataBranchName(kRCEcalClusters)))     { fActivateEcalClustersRC     = true; } 
   else if (!strcmp(data,SpdDataBranchName(kRCEcalParticles)))    { fActivateEcalParticlesRC    = true; } 
   else if (!strcmp(data,SpdDataBranchName(kMCEcalClustersInfo))) { fActivateEcalClustersMCInfo = true; } 
   else if (!strcmp(data,SpdDataBranchName(kMCEcalParticles)))    { fActivateEcalParticlesMC    = true; }
   
   else if (!strcmp(data,SpdDataBranchName(kMCRsClusters)))       { fActivateRsClustersMC       = true; }
   else if (!strcmp(data,SpdDataBranchName(kMCRsParticles)))      { fActivateRsParticlesMC      = true; }
   
   else if (!strcmp(data,SpdDataBranchName(kTsParticles)))        { fActivateTsParticles        = true; }
   else if (!strcmp(data,SpdDataBranchName(kTofParticles)))       { fActivateTofParticles       = true; }
   else if (!strcmp(data,SpdDataBranchName(kAegParticles)))       { fActivateAegParticles       = true; }

   else 
   {
       cout << "-W- <SpdMCDataIterator::ActivateBranch> Unknown data type: " << data << endl;
   }
}

//_____________________________________________________________________________
void SpdMCDataIterator::InactivateBranch(const Char_t* data)
{
        if (!strcmp(data,SpdDataBranchName(kMCEvent)))            { fActivateEvent     = false; }
   else if (!strcmp(data,SpdDataBranchName(kMCParticles)))        { fActivateParticles = false; }
   else if (!strcmp(data,SpdDataBranchName(kMCVertices)))         { fActivateVertices  = false; } 
   
   else if (!strcmp(data,SpdDataBranchName(kMCItsHits)))          { fActivateItsHits   = false; } 
   else if (!strcmp(data,SpdDataBranchName(kMCTsHits)))           { fActivateTsHits    = false; }
   else if (!strcmp(data,SpdDataBranchName(kMCTofHits)))          { fActivateTofHits   = false; }
   else if (!strcmp(data,SpdDataBranchName(kMCEcalHits)))         { fActivateEcalHits  = false; } 
   else if (!strcmp(data,SpdDataBranchName(kMCRsHits)))           { fActivateRsHits    = false; }
   else if (!strcmp(data,SpdDataBranchName(kMCBbcHits)))          { fActivateBbcHits   = false; }
   else if (!strcmp(data,SpdDataBranchName(kMCAegHits)))          { fActivateAegHits   = false; }
   else if (!strcmp(data,SpdDataBranchName(kMCZdcHits)))          { fActivateZdcHits   = false; }
   
   else if (!strcmp(data,SpdDataBranchName(kMCTracks)))           { fActivateTracks             = false; } 
   else if (!strcmp(data,SpdDataBranchName(kRCVertices)))         { fActivateVerticesRC         = false; } 
   
   else if (!strcmp(data,SpdDataBranchName(kRCTracks)))           { fActivateTracksRC           = false; } 
   else if (!strcmp(data,SpdDataBranchName(kRCTrackSeeds)))       { fActivateTrackSeedsRC       = false; } 
    
   else if (!strcmp(data,SpdDataBranchName(kRCEcalClusters)))     { fActivateEcalClustersRC     = false; } 
   else if (!strcmp(data,SpdDataBranchName(kRCEcalParticles)))    { fActivateEcalParticlesRC    = false; } 
   else if (!strcmp(data,SpdDataBranchName(kMCEcalClustersInfo))) { fActivateEcalClustersMCInfo = false; } 
   else if (!strcmp(data,SpdDataBranchName(kMCEcalParticles)))    { fActivateEcalParticlesMC    = false; }
   
   else if (!strcmp(data,SpdDataBranchName(kMCRsClusters)))       { fActivateRsClustersMC       = false; }
   else if (!strcmp(data,SpdDataBranchName(kMCRsParticles)))      { fActivateRsParticlesMC      = false; }
   
   else if (!strcmp(data,SpdDataBranchName(kTsParticles)))        { fActivateTsParticles        = false; }
   else if (!strcmp(data,SpdDataBranchName(kTofParticles)))       { fActivateTofParticles       = false; }
   else if (!strcmp(data,SpdDataBranchName(kAegParticles)))       { fActivateAegParticles       = false; }

   else 
   {
       cout << "-W- <SpdMCDataIterator::InactivateBranch> Unknown data type: " << data << endl;
   }  
}


