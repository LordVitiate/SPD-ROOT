// $Id$
// Author: artur   2018/09/02

//_____________________________________________________________________________
//
// SpdEveEventViewer
//_____________________________________________________________________________

#include <TGeoManager.h>
#include <TEveGeoNode.h>

#include "SpdEveEventViewer.h"

#include <iostream>
using std::cout;
using std::endl;

SpdEveEventViewer* SpdEveEventViewer::fInstance = 0;

ClassImp(SpdEveEventViewer)

//_____________________________________________________________________________
SpdEveEventViewer::SpdEveEventViewer(): TEveEventManager("SpdEveEventViewer",""),
fRootFileManager(0),fRunAnalyzer(0),fIsInit(kFALSE),fNEvent(0)
{
    fRootFileManager = FairRootManager::Instance();
    fRunAnalyzer = FairRunAna::Instance();
    
    //cout << "-I- <SpdEveEventViewer::SpdEveEventViewer> Analyzer: " 
    //     << fRunAnalyzer->ClassName() << endl;
    
    fInstance = this;
}

//_____________________________________________________________________________
SpdEveEventViewer::~SpdEveEventViewer() 
{
    fInstance = 0;
}

//_____________________________________________________________________________
void SpdEveEventViewer::AddTask(FairTask* task) 
{
    if (fRunAnalyzer) fRunAnalyzer->AddTask(task);
}

//_____________________________________________________________________________
void SpdEveEventViewer::Open()
{
    
}

//_____________________________________________________________________________
void SpdEveEventViewer::Init(Int_t visopt, Int_t vislvl, Int_t maxvisnds)
{
    if (fIsInit) return;
   
    TEveManager::Create();
    gEve->AddEvent(this);

    fRunAnalyzer->Init();

    if (!gGeoManager) return;
   
    TEveGeoTopNode* tn = new TEveGeoTopNode(gGeoManager, gGeoManager->GetTopNode(), 
                                            visopt, vislvl, maxvisnds);
    gEve->AddGlobalElement(tn);
    
    gEve->FullRedraw3D(kTRUE);
     
    fIsInit = kTRUE;    
}

//_____________________________________________________________________________   
void SpdEveEventViewer::GotoEvent(Long64_t nevent)
{
    if (!fRunAnalyzer) return;
    
    fRunAnalyzer->Run(nevent);
    fNEvent = nevent;
}

//_____________________________________________________________________________      
void SpdEveEventViewer::NextEvent()
{
    fRunAnalyzer->Run(fNEvent+1);
    fNEvent++; 
}

//_____________________________________________________________________________      
void SpdEveEventViewer::PrevEvent()
{
    fRunAnalyzer->Run(fNEvent-1);
    fNEvent--; 
}

//_____________________________________________________________________________      
void SpdEveEventViewer::Close()
{
    
}
    
    
    
    
