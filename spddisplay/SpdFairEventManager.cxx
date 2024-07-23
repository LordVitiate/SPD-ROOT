// $Id$
// Author: artur   2018/08/31


//_____________________________________________________________________________
//
// SpdFairEventManager
//_____________________________________________________________________________

#include "SpdFairEventManager.h"

#include <iostream>
using std::cout;
using std::endl;

SpdFairEventManager* SpdFairEventManager::fInstance = 0;

ClassImp(SpdFairEventManager)

//_____________________________________________________________________________
SpdFairEventManager::SpdFairEventManager():FairEventManager()
{
    fInstance = this;
    fIsInit = kFALSE;
    fName  = "SpdFairEventManager";
}

//_____________________________________________________________________________
SpdFairEventManager::~SpdFairEventManager() 
{
    fInstance = 0;
    fIsInit = kFALSE;
}

//_____________________________________________________________________________
void SpdFairEventManager::Init(Int_t visopt, Int_t vislvl, Int_t maxvisnds)
{
    if (fIsInit) return;
    FairEventManager::Init(visopt,vislvl,maxvisnds);
    fIsInit = kTRUE;
    
    cout << "-I- <SpdFairEventManager::Init> Ok" << endl;
}

//_____________________________________________________________________________
void SpdFairEventManager::AddTask(FairTask* task) 
{
    FairEventManager::AddTask(task);   
}

//______________________________________________________________________________
void SpdFairEventManager::UpdateEditor()
{

}

