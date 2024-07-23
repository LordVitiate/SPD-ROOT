// $Id$
// Author: artur   2021/02/01


//_____________________________________________________________________________
//
// SpdMCDataReaderSimple
//_____________________________________________________________________________

#include "SpdMCDataReaderSimple.h"

#include "SpdRunAna.h"
#include "SpdCommonGeoMapper.h"

#include "FairGeoParSet.h"
#include "FairBaseParSet.h"
#include "SpdBaseParSet.h"
#include "SpdSetParSet.h"

#include "SpdMCEvent.h"
#include "SpdMCParticle.h"
#include "SpdVertexMC.h"

#include "SpdMCEventHelper.h"
#include "SpdParticlesAndProcesses.h"

#include <TTree.h>
#include <TBranch.h>

#include <map>
#include <set>
#include <iostream>

using std::cout;
using std::endl;

using std::pair;

ClassImp(SpdMCDataReaderSimple)

//_____________________________________________________________________________
SpdMCDataReaderSimple::SpdMCDataReaderSimple():
fParameters(0),
fEvent(0),fParticles(0),fVertices(0),
fItsHits(0),fTsHits(0),fTofHits(0),fEcalHits(0),fRsHits(0),
fBbcHits(0),fAegHits(0),fZdcHits(0),
fTracks(0),fVerticesRC(0),
fTracksRC(0),fTrackSeedsRC(0),
fEcalClustersRC(0),fEcalParticlesRC(0),
fEcalClustersMCInfo(0),fEcalParticlesMC(0),
fRsClustersMC(0),fRsParticlesMC(0),
fTsParticles(0),fTofParticles(0), fAegParticles(0)
{
   fPrintEvent     = true;
   
   fPrintParticles = 0;
   fPrintVertices  = 0;
   
   fPrintItsHits   = 0;
   fPrintTsHits    = 0;
   fPrintTofHits   = 0;
   fPrintEcalHits  = 0;
   fPrintRsHits    = 0;
   
   fPrintBbcHits   = 0;
   fPrintAegHits   = 0;
   fPrintZdcHits   = 0;
    
   fPrintTracks     = 0;
   fPrintVerticesRC = 0;
   
   fPrintTracksRC     = 0;
   fPrintTrackSeedsRC = 0;
   
   fPrintEcalClustersRC  = 0;
   fPrintEcalParticlesRC = 0;
   fPrintEcalClustersMCInfo = 0;
   fPrintEcalParticlesMC = 0;
   
   fPrintRsClustersMC = 0;
   fPrintRsParticlesMC = 0;
   
   fPrintTsParticles = 0;
   fPrintTofParticles = 0;
   fPrintAegParticles = 0;

}

//_____________________________________________________________________________
SpdMCDataReaderSimple::~SpdMCDataReaderSimple() 
{
 
}

//_____________________________________________________________________________
void SpdMCDataReaderSimple::LoadGeometry()
{
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance(); 
   
   run->GetParameters("FairGeoParSet");
   run->GetParameters("FairBaseParSet");
   
   //FairBaseParSet* base_pars = (FairBaseParSet*)run->GetParameters("FairBaseParSet");
   //SpdCommonGeoMapper::Instance()->PrintGeometryList();
}

//_____________________________________________________________________________
InitStatus SpdMCDataReaderSimple::Init() 
{   
   // get RootManager
   
   FairRootManager* ioman = FairRootManager::Instance();
   if (!ioman) {
       cout << "-E- <SpdMCDataReaderSimple::Init> "
            << "RootManager not instantiated!" << endl;
       return kFATAL;
   }
   
   //========================== LOAD GEOMETRY ==============================/
   
   LoadGeometry();
   
   //========================== GET PARAMETERS =============================/
   
   SpdRunAna* run = (SpdRunAna*)FairRunAna::Instance();
   fParameters = (SpdSetParSet*)run->GetParameters("SpdSetParSet");
   
   //===================== LOAD INPUT DATA OBJECTS =========================/
   
   // ----- get Event -----
   
   fEvent = (SpdMCEvent*)ioman->GetObject(SpdDataBranchName(kMCEvent));
   
   // ----- get MCParticles array -----
   
   fParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCParticles));
   
   // ----- get MCVertices array -----
   
   fVertices = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCVertices));

   // ----- get ItsHits array -----
    
   fItsHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCItsHits));
   
   // ----- get TsHits array -----
    
   fTsHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTsHits));
   
   // ----- get TofHits array -----
    
   fTofHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTofHits));
   
   // ----- get EcalHits array -----
  
   fEcalHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCEcalHits));
   
   // ----- get RsHits array -----
   
   fRsHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCRsHits));
   
   // ----- get ZdcHits array -----
   
   fBbcHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCBbcHits));
     
   // ----- get ZdcHits array -----
   
   fAegHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCAegHits));
   
   // ----- get ZdcHits array -----
   
   fZdcHits = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCZdcHits));
   
   // ----- get Tracks array -----
    
   fTracks = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCTracks));
   
   // ----- get RC-Vertices array -----
   
   fVerticesRC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCVertices));
   
   // ----- get RC-Tracks array -----
    
   fTracksRC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCTracks));
   
   // ----- get RC-Track seeds array -----
    
   fTrackSeedsRC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCTrackSeeds));
      
   // ----- get Ecal RC-Clusters array -----
   
   fEcalClustersRC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCEcalClusters));
   
   // ----- get Ecal RC-Particles array -----
   
   fEcalParticlesRC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kRCEcalParticles));
   
   // ----- get Ecal RC-Clusters MC-info array -----
   
   fEcalClustersMCInfo = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCEcalClustersInfo));
   
   // ----- get Ecal MC-Particles array -----
   
   fEcalParticlesMC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCEcalParticles));
   
   // ----- get Rs MC-Clusters array -----
   
   fRsClustersMC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCRsClusters));
   
   // ----- get Rs MC-Particles array -----
   
   fRsParticlesMC = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kMCRsParticles));
   
   // ----- get Ts Particles array -----
   
   fTsParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kTsParticles));
   
   // ----- get Tof Particles array -----
   
   fTofParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kTofParticles));
   
 // ----- get Aeg Particles array -----
   
   fAegParticles = (TClonesArray*)ioman->GetObject(SpdDataBranchName(kAegParticles));

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

   cout << "-I- <SpdMCDataReaderSimple:Init> Intialization successfull " << endl;
   
   return kSUCCESS;
}

//________________________________________________________________________________________
void SpdMCDataReaderSimple::Exec(Option_t* opt) 
{
  if (fEvent) {
      cout << "\n-I- <SpdMCDataReaderSimple:Exec> Read event of number ... " 
           << fEvent->GetEventId() << "\n\n";
  }
  else cout << "\n-I- <SpdMCDataReaderSimple:Exec> Read event ... " << "\n\n";
  
  SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
  Int_t prn_entries = 2;
   
  if (fPrintEvent && fEvent) fEvent->PrintEvent();
  
  if (fPrintParticles > 0 && fParticles) {
      helper->PrintParticles();
      if (fPrintParticles > 1) helper->PrintParticles2(prn_entries);
  }
  
  if (fPrintVertices > 0 && fVertices) {
      helper->PrintVertices();
      if (fPrintVertices > 1) helper->PrintVertices2(prn_entries,"fitpars");
  }
  
  //---------------------------------------------------------
  
  if (fPrintItsHits > 1 && fItsHits) {
      helper->PrintItsHits2(prn_entries);
  }
  
  if (fPrintTsHits > 1 && fTsHits) {
      helper->PrintTsHits2(prn_entries);
  }
  
  if (fPrintTofHits > 1 && fTofHits) {
      helper->PrintTofHits2(prn_entries);
  }
  
  if (fPrintEcalHits > 1 && fEcalHits) {
      helper->PrintEcalHits2(prn_entries);
  }
  
  if (fPrintRsHits > 1 && fRsHits) {
      helper->PrintRsHits2(prn_entries);
  }
  
  if (fPrintBbcHits > 1 && fBbcHits) {
      helper->PrintBbcHits2(prn_entries);
  }
  
  if (fPrintAegHits > 1 && fAegHits) {
      helper->PrintAegHits2(prn_entries);
  }
  
  if (fPrintZdcHits > 1 && fZdcHits) {
      helper->PrintZdcHits2(prn_entries);
  }
  
  //---------------------------------------------------------
  
  if (fPrintTracks > 0 && fTracks) {
      helper->PrintTracks();
      if (fPrintVertices > 1) helper->PrintTracks2(prn_entries,"fitpars");
  }
  
  if (fPrintVerticesRC > 0 && fVerticesRC) {
      if (fPrintVerticesRC == 1) helper->PrintVerticesRC();
      if (fPrintVerticesRC > 1) helper->PrintVerticesRC2(prn_entries,"fitpars");
  }
  
  if (fPrintTracksRC > 0 && fTracksRC) {
      if (fPrintTracksRC == 1) helper->PrintTracksRC();
      if (fPrintTracksRC > 1) helper->PrintTracksRC2(prn_entries,"fitpars");
  }
  
  if (fPrintEcalClustersRC > 1 && fEcalClustersRC) {
      helper->PrintEcalClustersRC2(prn_entries);
  }
  
  if (fPrintEcalParticlesRC > 1 && fEcalParticlesRC) {
      helper->PrintEcalParticlesRC2(prn_entries);
  } 
  
  if (fPrintEcalClustersMCInfo > 1 && fEcalClustersMCInfo) {
      helper->PrintEcalClustersMCInfo2(prn_entries);
  }
  
  if (fPrintEcalParticlesMC > 1 && fEcalParticlesMC) {
      helper->PrintEcalParticlesMC2(prn_entries);
  } 
  
  if (fPrintRsClustersMC > 1 && fRsClustersMC) {
      helper->PrintRsClustersMC2(prn_entries);
  }
  
  if (fPrintRsParticlesMC > 1 && fRsParticlesMC) {
      helper->PrintRsParticlesMC2(prn_entries);
  }
  
  if (fPrintTsParticles > 1 && fTsParticles) {
      helper->PrintTsParticles2(prn_entries);
  } 
  
  if (fPrintTofParticles > 1 && fTofParticles) {
      helper->PrintTofParticles2(prn_entries);
  } 

  if (fPrintAegParticles > 1 && fAegParticles) {
      helper->PrintAegParticles2(prn_entries);
  } 
}

//________________________________________________________________________________________  
void SpdMCDataReaderSimple::Finish()
{
   cout << "\n-I- <SpdMCDataReaderSimple:Finish> " << endl;
   
   SpdMCEventHelper* helper = SpdMCEventHelper::Instance();
   
   helper->SetEvent(0);
   helper->SetParticles(0);
   helper->SetVertices(0);
   
   helper->SetItsHits(0);
   helper->SetTsHits(0);
   helper->SetTofHits(0);
   helper->SetEcalHits(0);
   helper->SetRsHits(0);
   helper->SetZdcHits(0);
   
   helper->SetTracks(0);
   helper->SetVerticesRC(0);
    
   helper->SetEcalClustersRC(0);
   helper->SetEcalParticlesRC(0);
   helper->SetEcalClustersMCInfo(0);
   helper->SetEcalParticlesMC(0);
   
   helper->SetRsClustersMC(0);
   helper->SetRsParticlesMC(0);
   
   helper->SetTsParticles(0);
   helper->SetTofParticles(0);
   helper->SetAegParticles(0);

}



