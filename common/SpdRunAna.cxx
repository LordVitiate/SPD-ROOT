// $Id$
// Author: artur   2018/11/29


//_____________________________________________________________________________
//
// SpdRunAna
//_____________________________________________________________________________

#include "SpdRunAna.h"

#include "FairIon.h"
#include "FairField.h"

#include "FairBaseParSet.h" 
#include "FairGeoParSet.h"    
#include "FairRuntimeDb.h"  
#include "FairEventHeader.h"
#include "FairTask.h"

#include "FairFileSource.h"
#include "FairRootFileSink.h"

#include "SpdParticlesAndProcesses.h"
#include "SpdCommonGeoMapper.h"

#include <iostream>
using std::cout;
using std::endl;

ClassImp(SpdRunAna)

//_____________________________________________________________________________
SpdRunAna::SpdRunAna():FairRunAna(),fParSet(0),
fKeepParameters(true),
fLoadGeoParSet(true),fLoadBaseParSet(true),
fGeoParSet(0),fBaseParSet(0),
fIsParsSaved(false)
{ 
    SpdParticlesAndProcesses::AddParticlesToDatabasePDG();  
   
    fgRinstance = this;
}

//_____________________________________________________________________________
SpdRunAna::~SpdRunAna() 
{
    DeleteParSet();
}

//_____________________________________________________________________________
void SpdRunAna::DeleteParSet()
{
    if (fParSet) {
        delete fParSet;
        fParSet = 0;
    }  
}
 
//_____________________________________________________________________________
SpdSetParSet* SpdRunAna::GetParSetAssembly()  
{ 
    if (fParSet) return fParSet; 
    fParSet = new SpdSetParSet();
    return fParSet;
}

//_____________________________________________________________________________
FairParGenericSet* SpdRunAna::GetParameters(TString pars_name)
{
    FairParGenericSet* parameters = 0;
    
    FairRuntimeDb* rtdb = GetRuntimeDb();
    if (rtdb && (rtdb->getFirstInput() || rtdb->getSecondInput())) {
        if (pars_name == "FairGeoParSet" || pars_name == "FairBaseParSet") return parameters;
        parameters = dynamic_cast<FairParGenericSet*>(rtdb->getContainer(pars_name));
        if (parameters) ((FairParSet*)parameters)->init();
    }
    else {
        FairRootManager* rm = FairRootManager::Instance();
        FairFileSource* source = dynamic_cast<FairFileSource*>(rm->GetSource());
        if (!source) return 0;
        parameters = dynamic_cast<FairParGenericSet*>(((TFile*)source->GetRootFile())->Get(pars_name));
        if (pars_name == "FairGeoParSet")  fGeoParSet  = parameters;
        if (pars_name == "FairBaseParSet") fBaseParSet = parameters;
    }
    
    return parameters;
}

//_____________________________________________________________________________
void SpdRunAna::Initialize()
{
    cout << "-I- <SpdRunAna::Initialize>(1) >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " << endl; 
    
    SpdParticlesAndProcesses::AddParticlesToDatabasePDG();
    
    LoadParameters();
    
    Init();
    
    if (fKeepParameters) SaveParameters();
   
    cout << "-I- <SpdRunAna::Initialize>(2) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << "\n\n"; 
}

//_____________________________________________________________________________
void SpdRunAna::LoadParameters()
{
    // try loading base parameters & geometry from Runtime Database (from separate file)
    FairRuntimeDb* rtdb = GetRuntimeDb();
    if (!rtdb) return;
    
    if (rtdb->getFirstInput() || rtdb->getSecondInput()) 
    {
        if (!fLoadGeoParSet) {
            FairParSet* parset1 = GetRuntimeDb()->getContainer("FairGeoParSet");
            if (parset1) parset1->setStatic(true);
        }
        if (!fLoadBaseParSet) {
            FairParSet* parset2 = GetRuntimeDb()->getContainer("FairBaseParSet");
            if (parset2) parset2->setStatic(true);
        }    
    }
    else {
        fLoadGeoParSet  = false;
        fLoadBaseParSet = false;
        FairParSet* parset1 = GetRuntimeDb()->getContainer("FairGeoParSet");
        if (parset1) parset1->setStatic(true);
        FairParSet* parset2 = GetRuntimeDb()->getContainer("FairBaseParSet");
        if (parset2) parset2->setStatic(true);
    }
    
    FairRootManager* rm = FairRootManager::Instance();
    FairFileSource* source = dynamic_cast<FairFileSource*>(rm->GetSource());
    if (!source) return;
    fParSet = dynamic_cast<SpdSetParSet*>(((TFile*)source->GetRootFile())->Get("SpdSetParSet"));
}

//_____________________________________________________________________________
void SpdRunAna::SaveParameters()
{
    if (fIsParsSaved) return;
    
    cout << "-I- <SpdRunAna::SaveParameters> ... " << endl;
    
    fIsParsSaved = true;
    
    FairRootManager* ioman = FairRootManager::Instance();
    FairRootFileSink* sink = (FairRootFileSink*)ioman->GetSink(); 
    
    if (!sink) return;
    
    FairParSet *geoparset(0), *baseparset(0);
    
    FairRuntimeDb* rtdb = GetRuntimeDb();
    if (rtdb && (rtdb->getFirstInput() || rtdb->getSecondInput())) {
        geoparset  = GetRuntimeDb()->getContainer("FairGeoParSet");
        baseparset = GetRuntimeDb()->getContainer("FairBaseParSet"); 
    }
    else {
        FairRootManager* rm = FairRootManager::Instance();
        FairFileSource* source = dynamic_cast<FairFileSource*>(rm->GetSource());
        if (source) {
            if (fGeoParSet) geoparset = fGeoParSet;
            else geoparset  = (FairParSet*)((TFile*)source->GetRootFile())->Get("FairGeoParSet");
            if (fBaseParSet) baseparset = fBaseParSet;
            else baseparset = (FairParSet*)((TFile*)source->GetRootFile())->Get("FairBaseParSet");
        }
    }
    
    if (geoparset)  sink->WriteObject(geoparset,"FairGeoParSet",TObject::kOverwrite);
    if (baseparset) sink->WriteObject(baseparset,"FairBaseParSet",TObject::kOverwrite);
    
    if (fParSet) sink->WriteObject(fParSet,"SpdSetParSet",TObject::kOverwrite);
    
    //if (geoparset) geoparset->setStatic(false);
    //if (baseparset) baseparset->setStatic(false); 
   
    cout << "\t spdpars: " << fParSet 
         << "; geopars: " << geoparset 
         << "; basepars: " << baseparset << endl; 
}
  
//_____________________________________________________________________________
void SpdRunAna::FinishTask()
{
    cout << "\n-I- <SpdRunAna::FinishTask> " << endl;    
}

//_____________________________________________________________________________
void SpdRunAna::RunEntry(Int_t entry)
{
   fRootManager->ReadEvent(entry);
   
   UInt_t tmpId = fEvtHeader->GetRunId();
   
   if (tmpId != fRunId) 
   {
       fRunId = tmpId;
       if (!fStatic) {
           Reinit( fRunId );
           fTask->ReInitTask();
       }
   }
   
   fRootManager->StoreWriteoutBufferData(fRootManager->GetEventTime());
   fTask->ExecuteTask("");
}





